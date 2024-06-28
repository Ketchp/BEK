#define _XOPEN_SOURCE 700

#include <time.h>
#include <wait.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/inotify.h>
#include <netinet/in.h>

#include "cap_drop.h"


#define MIN_READ_LEN 128  // we don't want to read byte by byte as it would be slow
#define FORMAT_BUFFER_LEN 256  // must accommodate formatted time from any strftime specifier

#define LOG(...) fprintf(stderr, __VA_ARGS__)
#define SIZEOF_ARR(arr) (sizeof((arr)) / sizeof((arr)[0]))
#define IGNORE(value) ((void)(value))   // some return values marked as nodiscard may be ignored


/*-------------------  Global constants  -------------------*/
const char CMD_GET[] = "get:";
const char CMD_SET[] = "set:";
const char REPLY_UNKNOWN[] = "Unknown command.\n";
const char REPLY_PRIVILEGE[] = "Low privilege for set command.\n";
const char REPLY_PARSE_FAILED[] = "Parsing time failed.\n";
const char REPLY_TIME_NO_REPR[] = "Time can not be represented.\n";
const char REPLY_CHANGE_ERR[] = "Time could not be changed.\n";

// Files starting with '/' are absolute.
// Files not starting with '/' are relative to "$HOME" environment variable NOT to current working directory.
// If "$HOME" is not found, configs are skipped.
// Path specified in "$NET_CLOCK_CONF" is used "as is" in fopen().
const char *CONFIG_SEARCH_LIST[] = {
    ".config/net_clock.conf",
    "net_clock.conf",
    ".net_clock.conf",
    "/etc/net_clock.conf",
    // path specified in environment variable NET_CLOCK_CONF
};

const char *ENV_CONF_NAME = "NET_CLOCK_CONF";


/*-------------------  Typedefs  -------------------*/
typedef int fd_t;

typedef struct config {
    in_port_t port;
    int max_connection;
    int server_enable;
    int changed;
} config_t;

typedef struct io_client {
    char *buffer;
    char *set_fmt;
    size_t buffer_use;
    size_t buffer_cap;
    fd_t fd;
    int is_local;

    // of course there are better data structures that linked list for this
    struct io_client *prev;
    struct io_client *next;
} io_client_t;

typedef struct state {
    fd_t epoll_fd;
    fd_t inotify_fd;
    int config_watch_id;
    int running;
    int exit_code;
    io_client_t *client_head;
    io_client_t *client_tail;

    char *config_filename;

    // server info
    fd_t server_fd;

    config_t config;
} state_t;

// for some function we need three state logic
// marked no discard, so we can not implicitly ignore result
typedef enum [[nodiscard]] result {
    RES_OK,
    RES_FAIL,
    RES_FATAL,
} result_t;

typedef struct formatter {
    char buffer[FORMAT_BUFFER_LEN];
    struct tm tm_time;
} formatter_t;


/*-------------------  Global variables  -------------------*/
state_t g_state = {
    .epoll_fd = -1,
    .inotify_fd = -1,
    .config_watch_id = -1,
    .running = 1,
    .exit_code = EXIT_SUCCESS,
    .client_head = NULL,
    .client_tail = NULL,
    .config_filename = NULL,
    .server_fd = -1,
    .config = {}  // config is initialised in code
};

/*-------------------  Function declarations  -------------------*/
/*-------------------  Main control flow  -------------------*/
/// Initialises config and important structures like epoll/inotify.
result_t init(state_t *state);

/// Wait for event to occur and handles it.
result_t loop(state_t *state);

/// free/closes all allocated memory, file descriptors. Valid to run in half initialised state.
void clean_up(state_t *state);


/*-------------------  Protocol implementation  -------------------*/
/// Handles communication with client.
result_t communicate(io_client_t *client);

/**
 * Searches for new line character in last `new_bytes` of client buffer and
 * if '\n' is found. Replies to received message.
 * @param client
 * @param new_bytes number of bytes received since last try_reply call / number of bytes to check
 * @return RES_FATAL if unrecoverable error occurred, else RES_OK.
 */
result_t try_reply(io_client_t *client, size_t new_bytes);

/**
 * Replies to 'GET' command.
 * @param client
 * @param msg_len Length of message including starting 'get:' and ending '\n'.
 * @return RES_FATAL if unrecoverable error occurred, else RES_OK.
 */
result_t cmd_get_handler(io_client_t *client, size_t msg_len);

/**
 * Handles 'SET' command, by setting `client->set_fmt` format.
 * Also check if client has SET time privilege.
 * @param client
 * @param msg_len Length of message including starting 'get:' and ending '\n'.
 * @return RES_FATAL if unrecoverable error occurred, else RES_OK.
 * @note client->set_fmt is malloced and must be freed after next message.
 */
result_t cmd_set_fmt_handler(io_client_t *client, size_t msg_len);

/**
 * Handles parsing and setting time.
 * @param client
 * @param msg_len Length of message including ending '\n'.
 * @return RES_FATAL if unrecoverable error occurred, RES_OK if time was successfully changed, else RES_FAIL.
 */
result_t cmd_set_time_handler(io_client_t *client, size_t msg_len);


/*-------------------  epoll wrappers  -------------------*/
/// Wrapper for creating epoll object.
result_t create_epoll(state_t *state);

/// Closes epoll object. No call to `epoll_add`/`epoll_del` can be made afterwards.
/// Valid tu call if epoll is uninitialised in `state`.
void close_epoll(state_t *state);

/// Wrapper for adding file descriptor to epoll.
result_t epoll_add(state_t *state, fd_t fd);

/// Wrapper for removing file descriptor from epoll object.
/// Valid to run if epoll is not initialised or with fd < 0 (invalid fd).
void epoll_del(state_t *state, fd_t fd);

/// Handles file descriptors returned by epoll, we have no record of.
result_t epoll_unknown_fd(fd_t fd);


/*-------------------  inotify wrappers  -------------------*/
/// Creates inotify object.
result_t create_inotify(state_t *state, const char *filename);

/// Closes inotify object and removes it from epoll.
void close_inotify(state_t *state);


/*-------------------  io_client  -------------------*/
/**
 * Sets appropriate `fcntl` flags on `fd`, adds `fd` to epoll and
 * insert newly created client to list of io_clients in `state`.
 * @param state
 * @param fd file descriptor of io_client
 * @param is_local 0 for network client, else local client.
 * @return RES_FATAL if client could not be created, else RES_OK.
 * @note If RES_FATAL is returned, fd is closed using close_io_client.
 */
result_t create_io_client(state_t *state, fd_t fd, int is_local);

/// Closes io_client, and frees all memory allocated by client. Valid to call on half initialised client.
void close_io_client(state_t *state, io_client_t *client);

/**
 * Resizes internal client buffer to size `size`.
 * @param client
 * @param size size of new buffer
 * @return RES_FATAL if realloc failed,
 * RES_FAIL if size is smaller than current usage of buffer
 * RES_OK if buffer was successfully resized.
 * If RES_FATAL/RES_FAIL is returned original buffer is left untouched.
 */
result_t io_client_resize(io_client_t *client, size_t size);

/**
 * Reads at least MIN_READ_LEN bytes from client if available.
 * If less data is available, reads less.
 * @param client
 * @return Number of read bytes. 0 if client disconnected.
 * -1 on read error or EOF and errno is set appropriately.
 */
[[nodiscard]] ssize_t io_client_read(io_client_t *client);

/**
 * Writes `write_size` bytes from buffer to client.
 * For network clients send to client->fd is used.
 * For local client write to STDOUT is used.
 * @param client
 * @param buffer
 * @param write_size
 * @return RES_OK on successful write, else RES_FATAL.
 */
result_t io_client_write(io_client_t *client, const char *buffer, size_t write_size);


/*-------------------  link_client: linked list  -------------------*/
/// Inserts fd to io_client linked list.
[[nodiscard]] io_client_t *link_client_create(state_t *state, fd_t fd, int is_local);

/// Finds client with `fd` in liked list. If not found return NULL.
[[nodiscard]] io_client_t *link_client_find(state_t *state, fd_t fd);

/// Removes client from linked list and calls close_io_client on it.
void link_client_remove(state_t *state, io_client_t *client);

/// Convenience wrapper for link_client_find and link_client_remove.
result_t link_client_remove_fd(state_t *state, fd_t fd);


/*-------------------  server wrappers  -------------------*/
/// Creates listening TCP server according to config.
result_t create_server(state_t *state);

/// Closes server. Valid to call on half initialised state.
void close_server(state_t *state);

/**
 * @param state
 * @return RES_FATAL if io_client can not be created. RES_FAIL if accept failed.
 * RES_OK if new io_client was successfully added to state.
 */
result_t accept_connection(state_t *state);

/**
 * Handles server creation / refresh / close when config has changed.
 * @param state
 * @return RES_OK if everything went OK.
 * RES_FAIL if server could not be established but should be.
 * RES_FATAL on non-recoverable error.
 */
result_t handle_server(state_t *state);


/*-------------------  config wrappers  -------------------*/
/**
 * Initialises configuration: finds configuration file or loads default values.
 * @param state
 * @return RES_FATAL on non-recoverable error.
 * RES_FAIL if no configuration file was found and default config is used.
 * RES_OK if configuration was found and parsed.
 */
result_t load_config(state_t *state);

/**
 * Tries opening configuration file.
 * On successful open, file is used for configuration and registered to inotify.
 * @param state
 * @param filename name of configuration file
 * @return RES_OK if configuration was loaded and inotify registered.
 * RES_FAIL if filename could not be opened.
 * RES_FATAL on non-recoverable error.
 */
result_t try_hook_up_conf(state_t *state, const char *filename);

/**
 * Reloads config from configuration file.
 * If some configuration changed, also sets state->config.changed to 1.
 * @param state
 */
void reload_config(state_t *state);

/**
 * Sets default config for configuration.
 * @param config
 */
void set_default_config(config_t *config);


/*-------------------  Helpers  -------------------*/
/// Marks global state to stop application.
void sigint_handler(int);

/// Joins absolute/relative path1 with relative path2, result must be freed by caller.
[[nodiscard]] char *path_join(const char *path1, const char *path2);

/// Sets current time in formatter.
result_t formatter_get_time(formatter_t *formatter);

/**
 * Reads *data and places result in formatter buffer. Sets *data to new start of data
 * after parsing part of it.
 * @param formatter
 * @param data
 * @return Number of characters in formatter after formatting.
 * If formatting can not succeed data is set to NULL and zero is returned.
 * @note Return value zero is not always bad. In fact we should use format until zero is returned.
 */
[[nodiscard]] size_t format(formatter_t *formatter, const char **data);

/**
 * Parses strftime conversion specification from `data` and places it to `dest` buffer.
 * If conversion specification does not fit in `dest` buffer with '\0' 0 is returned and
 * and `dest` is kept untouched.
 *
 * @param dest specifier is placed to dest
 * @param dest_size size of dest array
 * @param data specifier is parsed from here
 * @return length of format specifier place in dest (not counting trailing '\0'), or 0 on failure.
 * @note Caller must ensure that parsing format from `data` won't result in read past end of `data`.
 *       This can be ensured by using terminating character for data like '\0' or '\n'.
 */
[[nodiscard]] int get_conversion_spec(char *dest, size_t dest_size, const char *data);

/// Check if string contains only white space characters.
[[nodiscard]] int is_empty(const char *str);


/*-------------------  Main  -------------------*/
int main() {
    if(cap_drop(NULL, 0) != 0)
        return EXIT_FAILURE;

    if(init(&g_state) != RES_OK) {
        g_state.exit_code = EXIT_FAILURE;
    }

    while(g_state.running && g_state.exit_code == EXIT_SUCCESS)
        if(loop(&g_state) != RES_OK)
            g_state.exit_code = EXIT_FAILURE;

    clean_up(&g_state);

    return g_state.exit_code;
}


/*-------------------  Main control flow  -------------------*/
result_t init(state_t *state) {
    if(signal(SIGINT, sigint_handler) == SIG_ERR)
        return RES_FATAL;

    if(create_epoll(state) != RES_OK)
        return RES_FATAL;

    if(create_io_client(state, STDIN_FILENO, 1) != RES_OK)
        return RES_FATAL;

    if(load_config(state) == RES_FATAL)
        return RES_FATAL;

    return RES_OK;
}

result_t loop(state_t *state) {
    if(state->config.changed) {
        result_t res = handle_server(state);

        if(res == RES_FATAL)
            return RES_FATAL;

        // if we could not establish listen server we still may continue on stdin
        if(res == RES_FAIL) {
            LOG("Listen server could not be established: disabling.\n");
            state->config.server_enable = 0;
        }

        state->config.changed = 0;
    }

    struct epoll_event new_event;
    int ev_cnt = epoll_wait(state->epoll_fd,
                            &new_event,
                            1,     // read only one event at a time
                            -1);   // wait indefinitely

    if(ev_cnt < 0) {
        if(errno == EINTR)
            return RES_OK;  // signal received... need to check is state.running is still set.

        perror("epoll failed");
        return RES_FATAL;
    }

    if(ev_cnt == 0) {
        LOG("Epoll returned no event.\n");
        return RES_FATAL;
    }

    /* Epoll waits for:
     *  - stdin
     *  - client sockets
     *  - server socket
     *  - inotify object
     *
     *  Only first two are in io_client linked-list.
     */

    // Hangup or error happened on fd/socket.
    // This usually means that client has closed other end of socket.
    // Need to clear fd from io_clients.
    if(new_event.events & (EPOLLRDHUP | EPOLLERR | EPOLLHUP)) {
        // Unlikely for listen server or inotify to hangup or error out,
        // but we check it just to be safe.
        if(new_event.data.fd == state->server_fd) {
            LOG("Server error/hangup (%d)\n", new_event.events);
            state->config.changed = 1;  // force server reload
            return RES_OK;
        }

        if(new_event.data.fd == state->inotify_fd) {
            // we don't even bother recover from this
            // there is something seriously wrong if inotify hangup's or error's
            LOG("Inotify error/hangup (%d)\n", new_event.events);
            return RES_FATAL;
        }

        // error on some client -> we remove it from list
        if(link_client_remove_fd(state, new_event.data.fd) != RES_OK) {
            // link_client_remove_fd fails only if fd was not found and it should not happen
            // epoll should not give us fd after we closed it, there is something wrong.
            return RES_FATAL;
        }

        return RES_OK;
    }

    // event EPOLLIN

    if(new_event.data.fd == state->server_fd) {
        // even if accept connection fails we continue to listen on already established,
        // we may decide to reopen listen server if this happens but for now we IGNORE
        IGNORE(accept_connection(state));
        return RES_OK;
    }

    if(new_event.data.fd == state->inotify_fd) {
        struct inotify_event i_event;

        ssize_t i_len = read(state->inotify_fd, &i_event, sizeof(i_event));
        if(i_len < 0) {
            perror("inotify read failed");
            return RES_FATAL;
        }

        if(i_len != sizeof(i_event)) {
            LOG("Inotify read did not read whole event.\n");
            return RES_FATAL;
        }

        if(i_event.wd != state->config_watch_id) {
            LOG("Inotify watch id does not match.");
            return RES_FATAL;
        }

        // issue: inotify event fires twice if config file is saved by IDE.
        //        It is not big problem because second reload should see
        //        same data and shouldn't set config.changed leading only
        //        to little wasted time for reading file twice.
        //        Only problem is when config is invalid, we emit duplicate LOG messages.
        //        Possible solution: check modify time of file,
        //                           or read whole file and compare to old version before parsing
        reload_config(state);
        return RES_OK;
    }

    io_client_t *client = link_client_find(state, new_event.data.fd);

    if(client == NULL)
        return epoll_unknown_fd(new_event.data.fd);

    result_t result = communicate(client);

    if(result != RES_OK)
        link_client_remove(state, client);

    return RES_OK;
}

void clean_up(state_t *state) {
    close_server(state);
    close_inotify(state);

    while(state->client_head)
        link_client_remove(state, state->client_head);

    close_epoll(state);

    free(state->config_filename);
    state->config_filename = NULL;

    LOG("Closing application.\n");
}


/*-------------------  Protocol implementation  -------------------*/
result_t communicate(io_client_t *client) {
    while(1) {
        ssize_t n_read = io_client_read(client);

        if(n_read < 0) {
            // we exhausted all data from socket / stdin
            if(errno == EWOULDBLOCK || errno == EAGAIN)
                return RES_OK;

            perror("read returned error");
            return RES_FATAL;
        }

        // socket has disconnected
        if(n_read == 0) {
            return RES_FAIL;
        }

        client->buffer_use += n_read;

        if(try_reply(client, n_read) != RES_OK)
            return RES_FATAL;
    }
}

result_t try_reply(io_client_t *client, size_t new_bytes) {
    // we want to iterate over newly added bytes to find end of message('\n')
    for(size_t offset = 0; offset < new_bytes; ++offset) {
        size_t buffer_position = client->buffer_use - new_bytes + offset;
        if(client->buffer[buffer_position] != '\n')
            continue;

        size_t msg_len = buffer_position + 1;  // we include '\n' in message

        // formatting string for parsing time is set
        if(client->set_fmt != NULL) {
            result_t result = cmd_set_time_handler(client, msg_len);
            free(client->set_fmt);
            client->set_fmt = NULL;

            // we do not end if RES_FAIL; it just means time was not parsed correctly
            if(result == RES_FATAL)
                return RES_FATAL;
        }
        else if(strncmp(client->buffer, CMD_GET, strlen(CMD_GET)) == 0) {
            if(cmd_get_handler(client, msg_len) != RES_OK)
                return RES_FATAL;
        }
        else if(strncmp(client->buffer, CMD_SET, strlen(CMD_SET)) == 0) {
            if(cmd_set_fmt_handler(client, msg_len) != RES_OK)
                return RES_FATAL;
        }
        else {
            if(io_client_write(client,
                               REPLY_UNKNOWN,
                               strlen(REPLY_UNKNOWN)) != RES_OK)
                return RES_FATAL;
        }

        /*
         * <-------------------buffer_cap------------------->
         * <-----------buffer_use---------->
         *                   <--new_bytes-->
         *                   <-offset->
         * ##################*********\n****.................
         */
        memmove(client->buffer,
                client->buffer + msg_len,
                new_bytes - offset - 1);
        client->buffer_use -= msg_len;

        // we may have received more '\n' in one messages...
        // we need to continue for loop
        new_bytes = new_bytes - offset - 1;
        offset = 0;
    }

    return RES_OK;
}

result_t cmd_get_handler(io_client_t *client, size_t msg_len) {
    formatter_t formatter;

    if(formatter_get_time(&formatter) != RES_OK)
        return RES_FATAL;

    const char *data = client->buffer + strlen(CMD_GET);  // skip "get:" command
    size_t f_count;
    while(data < client->buffer + msg_len &&
          (f_count = format(&formatter, &data)) > 0)
        if(io_client_write(client, formatter.buffer, f_count) != RES_OK)
            return RES_FATAL;

    if(data == NULL)
        return RES_FATAL;
    return RES_OK;
}

result_t cmd_set_fmt_handler(io_client_t *client, size_t msg_len) {
    if(!client->is_local)
        return io_client_write(client,
                               REPLY_PRIVILEGE,
                               strlen(REPLY_PRIVILEGE));

    // +1 for '\0'
    client->set_fmt = (char *)malloc(msg_len + 1 - strlen(CMD_SET));

    if(client->set_fmt == NULL)
        return RES_FATAL;

    memcpy(client->set_fmt, client->buffer + strlen(CMD_SET), msg_len - strlen(CMD_SET));
    client->set_fmt[msg_len - strlen(CMD_SET)] = '\0';
    return RES_OK;
}

result_t cmd_set_time_handler(io_client_t *client, size_t msg_len) {
    formatter_t parser;

    // first fill fields with values of current time,
    // if format does not specify certain fields we want to keep old ones
    if(formatter_get_time(&parser) != RES_OK)
        return RES_FATAL;

    char *next = strptime(client->buffer, client->set_fmt, &parser.tm_time);

    // check if whole message was parsed
    if(client->buffer + msg_len != next) {
        if(io_client_write(client,
                           REPLY_PARSE_FAILED,
                           strlen(REPLY_PARSE_FAILED)) != RES_OK)
            return RES_FATAL;
        return RES_FAIL;
    }

    time_t t_epoch = mktime(&parser.tm_time);

    // specified time could not be represented
    if(t_epoch == (time_t)(-1)) {
        perror("time can not be represented");
        if(io_client_write(client,
                           REPLY_TIME_NO_REPR,
                           strlen(REPLY_TIME_NO_REPR)) != RES_OK)
            return RES_FATAL;
        return RES_FAIL;
    }

    char argv_time[32];  // uint64_t max value is 20 decimal digits long
    int t_len = snprintf(argv_time, sizeof(argv_time), "%ld", t_epoch);

    if(t_len <= 0 || (unsigned int)t_len >= sizeof(argv_time))
        return RES_FATAL;

    char CHANGE_TIME_BIN[] = "/usr/local/bin/change_time";
    char *CHANGE_TIME_ARGV[] = {CHANGE_TIME_BIN, argv_time, NULL};

    pid_t pid = fork();

    // if fork(), fails we might as well kill client
    if(pid == -1)
        return RES_FATAL;

    if(pid == 0) {
        execv(CHANGE_TIME_BIN, CHANGE_TIME_ARGV);
        perror("execv failed");

        // under normal circumstances, this never runs
        // if './change_time' binary is not found for example, we want to exit with EXIT_FAILURE,
        // so that parent sees exit code EXIT_FAILURE
        exit(EXIT_FAILURE);
    }

    int status;
    if(waitpid(pid, &status, 0) < 0) {
        perror("waitpid failed");
        return RES_FATAL;
    }

    if(!WIFEXITED(status))
        return RES_FATAL;

    if(WEXITSTATUS(status) != EXIT_SUCCESS) {
        if(io_client_write(client,
                           REPLY_CHANGE_ERR,
                           strlen(REPLY_CHANGE_ERR)) != RES_OK)
            return RES_FATAL;
        return RES_FAIL;
    }

    return RES_OK;
}


/*-------------------  epoll wrappers  -------------------*/
result_t create_epoll(state_t *state) {
    state->epoll_fd = epoll_create1(EPOLL_CLOEXEC);

    if(state->epoll_fd < 0) {
        perror("Epoll could not be created");
        return RES_FATAL;
    }
    return RES_OK;
}

void close_epoll(state_t *state) {
    if(state->epoll_fd >= 0)
        close(state->epoll_fd);
    state->epoll_fd = -1;
}

result_t epoll_add(state_t *state, fd_t fd) {
    struct epoll_event ev = {
        .events = EPOLLIN | EPOLLHUP,
        .data.fd = fd
    };

    if(epoll_ctl(state->epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev) < 0) {
        perror("Could not register to epoll");
        return RES_FATAL;
    }

    return RES_OK;
}

void epoll_del(state_t *state, fd_t fd) {
    if(state->epoll_fd >= 0 && fd >= 0)
        epoll_ctl(state->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

result_t epoll_unknown_fd(fd_t fd) {
    // this should be unreachable but better that NULL pointer dereference
    // we could try removing fd from epoll, but it is dangerous since we don't
    // know what `new_event.data.fd` refers to
    // we just return RES_FATAL to stop everything as we are in possibly compromised state
    LOG("Epoll returned unknown fd: %d\n", fd);

    return RES_FATAL;
}

/*-------------------  inotify wrappers  -------------------*/
result_t create_inotify(state_t *state, const char *filename) {
    state->inotify_fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
    if(state->inotify_fd < 0) {
        perror("inotify could not be created");
        return RES_FATAL;
    }

    if(epoll_add(state, state->inotify_fd) != RES_OK)
        return RES_FATAL;  // we rely on fact that clean_up will close inotify_fd

    state->config_watch_id = inotify_add_watch(state->inotify_fd,
                                               filename,
                                               IN_MODIFY | IN_MOVE_SELF);

    if(state->config_watch_id < 0) {
        perror("inotify watch descriptor could not be created");
        return RES_FATAL;  // we rely on fact that clean_up will close inotify_fd
    }

    return RES_OK;
}

void close_inotify(state_t *state) {
    if(state->inotify_fd < 0)
        return;

    epoll_del(state, state->inotify_fd);

    close(state->inotify_fd);
    state->inotify_fd = -1;
    state->config_watch_id = -1;
}


/*-------------------  io_client  -------------------*/
result_t create_io_client(state_t *state, fd_t fd, int is_local) {
    int flags = fcntl(fd, F_GETFL);

    if(flags < 0) {
        perror("could not read file status flags");
        return RES_FATAL;
    }

    // we want to read as much data without blocking
    // save one syscall if O_NONBLOCK already set
    if((~flags & O_NONBLOCK) && fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("could not set NONBLOCK on file");
        return RES_FATAL;
    }

    flags = fcntl(fd, F_GETFD);

    if(flags < 0) {
        perror("Could not read file descriptor flags");
        return RES_FATAL;
    }

    // set file descriptor to close on exec, we don't want file descriptors
    // to be inherited to subprocesses
    // save one syscall if FD_CLOEXEC already set
    if((~flags & FD_CLOEXEC) && fcntl(fd, F_SETFD, flags | FD_CLOEXEC) < 0) {
        perror("Could not set CLOEXEC on file");
        return RES_FATAL;
    }

    // create client record in linked list of all clients
    io_client_t *client = link_client_create(state, fd, is_local);

    if(client == NULL)
        return RES_FATAL;

    if(epoll_add(state, fd) != RES_OK) {
        // if epoll_add fails we need to remove client now
        link_client_remove(state, client);
        return RES_FATAL;
    }

    return RES_OK;
}

void close_io_client(state_t *state, io_client_t *client) {
    LOG("Closing client %d.\n", client->fd);
    if(client->fd < 0)
        return;

    epoll_del(state, client->fd);

    close(client->fd);
    client->fd = -1;

    free(client->buffer);
    client->buffer = NULL;
    client->buffer_use = 0;
    client->buffer_cap = 0;

    free(client->set_fmt);
    client->set_fmt = NULL;

    free(client);
}

void init_io_client(io_client_t *node, fd_t fd, int is_local) {
    *node = (io_client_t){
        .fd = fd,
        .is_local = is_local,
        .prev = NULL,
        .next = NULL,
        .buffer = NULL,
        .set_fmt = NULL,
        .buffer_use = 0,
        .buffer_cap = 0,
    };
}

result_t io_client_resize(io_client_t *client, size_t size) {
    if(size < client->buffer_use)
        return RES_FAIL;

    char *temp = (char *)realloc(client->buffer, size);
    if(temp == NULL)
        return RES_FATAL;

    client->buffer = temp;
    client->buffer_cap = size;
    return RES_OK;
}

[[nodiscard]]
ssize_t io_client_read(io_client_t *client) {
    size_t avail = client->buffer_cap - client->buffer_use;

    if(avail < MIN_READ_LEN) {
        size_t new_size = client->buffer_cap >= MIN_READ_LEN ? client->buffer_cap * 2 : MIN_READ_LEN;
        if(io_client_resize(client, new_size) != RES_OK)
            return RES_FATAL;
        avail = client->buffer_cap - client->buffer_use;
    }

    if(client->is_local)
        return read(client->fd, client->buffer + client->buffer_use, avail);
    else
        return recv(client->fd, client->buffer + client->buffer_use, avail, 0);
}

result_t io_client_write(io_client_t *client, const char *buffer, size_t write_size) {
    ssize_t ret;
    if(client->is_local)
        ret = write(STDOUT_FILENO, buffer, write_size);
    else
        ret = send(client->fd, buffer, write_size, 0);

    if(ret < 0 || (size_t)ret != write_size)
        return RES_FATAL;
    return RES_OK;
}


/*-------------------  link_client: linked list  -------------------*/
[[nodiscard]]
io_client_t *link_client_create(state_t *state, fd_t fd, int is_local) {
    io_client_t *new_node = (io_client_t *)malloc(sizeof(*new_node));

    if(new_node == NULL)
        return NULL;

    init_io_client(new_node, fd, is_local);

    if(state->client_tail == NULL)
        return state->client_head = state->client_tail = new_node;

    state->client_tail->next = new_node;
    new_node->prev = state->client_tail;
    state->client_tail = new_node;
    return new_node;
}

[[nodiscard]]
io_client_t *link_client_find(state_t *state, fd_t fd) {
    for(io_client_t *current = state->client_head;
        current != NULL;
        current = current->next)
    {
        if(current->fd == fd)
            return current;
    }

    return NULL;
}

void link_client_remove(state_t *state, io_client_t *client) {
    if(client->next)
        client->next->prev = client->prev;
    if(client->prev)
        client->prev->next = client->next;

    if(client == state->client_head)
        state->client_head = client->next;
    if(client == state->client_tail)
        state->client_tail = client->prev;

    close_io_client(state, client);
}

result_t link_client_remove_fd(state_t *state, fd_t fd) {
    io_client_t *node = link_client_find(state, fd);
    if(node == NULL)
        return RES_FATAL;

    link_client_remove(state, node);
    return RES_OK;
}


/*-------------------  server wrappers  -------------------*/
result_t create_server(state_t *state) {
    config_t *config = &state->config;
    LOG("Creating server at port: %hu\n", config->port);

    // create socket for listening
    state->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(state->server_fd < 0) {
        perror("could not open socket");
        return RES_FATAL;
    }

    /* setsockopt: Handy debugging trick that lets
     * us rerun the server immediately after we kill it;
     * otherwise we have to wait about 20 secs.
     * Eliminates "ERROR on binding: Address already in use" error.
     *
     * We don't check error return code, because this option is optional for us.
     */
    int option_val = 1;
    setsockopt(state->server_fd, SOL_SOCKET, SO_REUSEADDR,
               &option_val, sizeof(int));


    // set internet ip:port
    struct sockaddr_in listen_addr;
    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    listen_addr.sin_port = htons(config->port);

    // bind listen socket to ip:port
    if(bind(state->server_fd,
            (struct sockaddr *) &listen_addr,
            sizeof(listen_addr)) < 0) {
        perror("could not bind address");
        return RES_FATAL;
    }

    // start listening on socket
    if(listen(state->server_fd, config->max_connection) < 0) {
        perror("ERROR on listen");
        return RES_FATAL;
    }

    return RES_OK;
}

void close_server(state_t *state) {
    if(state->server_fd < 0)
        return;

    LOG("Closing server.\n");

    epoll_del(state, state->server_fd);

    close(state->server_fd);
    state->server_fd = -1;
}

result_t accept_connection(state_t *state) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    fd_t sock_client_fd = accept(state->server_fd, (struct sockaddr *) &client_addr, &client_len);

    if(sock_client_fd < 0) {
        perror("ERROR on accept");
        return RES_FAIL;
    }

    LOG("New connection: %d.\n", sock_client_fd);
    if(create_io_client(state, sock_client_fd, 0) != RES_OK)
        return RES_FATAL;

    return RES_OK;
}

result_t handle_server(state_t *state) {
    if(state->config.server_enable == 0) {
        if(state->server_fd >= 0)
            close_server(state);
        return RES_OK;
    }

    if(state->server_fd >= 0) {
        // first close old server
        close_server(state);
    }

    if(create_server(state) != RES_OK) {
        // failure to establish server may not result in immediate termination
        return RES_FAIL;
    }

    if(epoll_add(state, state->server_fd) != RES_OK) {
        // failure to add server to inotify means something is really wrong
        return RES_FATAL;
    }

    return RES_OK;
}


/*-------------------  config wrappers  -------------------*/
result_t load_config(state_t *state) {
    // we set default config because, no config file may not be found
    // or config file may not contain some keys
    set_default_config(&state->config);

    result_t result = RES_FAIL;
    const char *home = getenv("HOME");
    for(size_t idx = 0; idx < SIZEOF_ARR(CONFIG_SEARCH_LIST); idx++) {
        // absolute address
        if(CONFIG_SEARCH_LIST[idx][0] == '/') {
            result = try_hook_up_conf(state, CONFIG_SEARCH_LIST[idx]);
        }

        // address relative to home, if "$HOME" exists
        else if(home != NULL){
            char *path = path_join(home, CONFIG_SEARCH_LIST[idx]);

            // we could just break here, but if we don't have memory now,
            // there is no point in continuing anyway
            if(path == NULL)
                return RES_FATAL;

            result = try_hook_up_conf(state, path);
            free(path);
        }

        // we use first config we find from search list
        if(result != RES_FAIL)
            break;
    }

    const char *conf_path;
    if(result == RES_FAIL && (conf_path = getenv(ENV_CONF_NAME)) != NULL)
        result = try_hook_up_conf(state, conf_path);

    if(result == RES_FAIL)
        LOG("No configuration file found.\n");

    return result;
}

result_t try_hook_up_conf(state_t *state, const char *filename) {
    FILE *conf_file = fopen(filename, "r");

    // file does not exist, or we don't have read permission.
    if(conf_file == NULL)
        return RES_FAIL;

    fclose(conf_file);  // we only want to confirm we can open the file

    LOG("Using config from: %s\n", filename);

    if(create_inotify(state, filename) != RES_OK)
        return RES_FATAL;

    // +1 for '\0'
    state->config_filename = (char *)malloc(strlen(filename) + 1);
    if(state->config_filename == NULL)
        return RES_FATAL;
    strcpy(state->config_filename, filename);

    reload_config(state);

    return RES_OK;
}

void reload_config(state_t *state) {
    FILE *file = fopen(state->config_filename, "r");
    if(file == NULL)
        return;

    config_t *cfg = &state->config;

    char *line = NULL;
    size_t n = 0;
    while(getline(&line, &n, file) > 0) {
        char *param = NULL;
        if(is_empty(line)) {
            continue;
        }
        else if(sscanf(line, "server_enable %ms", &param) == 1) {
            int enabled = -1;
            if(strcmp(param, "true") == 0) enabled = 1;
            else if(strcmp(param, "false") == 0) enabled = 0;
            else LOG("Invalid value for 'server_enable'.\n");

            if(enabled != -1 && enabled != cfg->server_enable) {
                cfg->server_enable = enabled;
                cfg->changed = 1;
            }
        }
        else if(sscanf(line, "port %ms", &param) == 1) {
            char *end = NULL;
            long port = strtol(param, &end, 10);
            if(*end == '\0' && port >= 1024 && port <= UINT16_MAX) {
                if(port != cfg->port) {
                    cfg->port = port;
                    cfg->changed = 1;
                }
            }
            else
                LOG("Invalid port number.\n");
        }
        else if(sscanf(line, "max_connections %ms", &param) == 1) {
            char *end = NULL;
            long max_connection = strtol(param, &end, 10);
            if(*end == '\0' && max_connection >= 1 && max_connection <= INT32_MAX) {
                if(cfg->max_connection != (int)max_connection) {
                    cfg->max_connection = (int)max_connection;
                    cfg->changed = 1;
                }
            }
            else
                LOG("Invalid max_connections number.\n");
        }
        else {
            LOG("Invalid config line: %s\n", line);
        }
        free(param);
    }
    free(line);
}

void set_default_config(config_t *config) {
    config->port = 12345;
    config->server_enable = 1;
    config->max_connection = 128;
    config->changed = 1;
}


/*-------------------  Helpers  -------------------*/
void sigint_handler(int) {
    g_state.running = 0;
}

[[nodiscard]]
char *path_join(const char *path1, const char *path2) {
    if(path1 == NULL || path2 == NULL || path2[0] == '/')
        return NULL;

    // +2, for '\0' and possibly '/' between paths
    char *res = (char *) malloc(strlen(path1) + strlen(path2) + 2);
    res[0] = '\0';

    strcat(res, path1);
    if(path1[strlen(path1) - 1] != '/')
        strcat(res, "/");
    strcat(res, path2);

    return res;
}

result_t formatter_get_time(formatter_t *formatter) {
    time_t t = time(NULL);  // if tloc == NULL, the call always succeeds according to man time(2)
    struct tm *tm = localtime(&t);

    if(tm == NULL)
        return RES_FATAL;

    // we copy value of tm_time from static global data,
    // at this point we are thread safe but for future expansion...
    memcpy(&formatter->tm_time, tm, sizeof(*tm));
    return RES_OK;
}

[[nodiscard]]
size_t format(formatter_t *formatter, const char **data) {
    size_t offset_data = 0;
    size_t offset_fmt = 0;

    while(offset_fmt < sizeof(formatter->buffer)) {
        char cd = (*data)[offset_data];

        if(cd != '%') {
            // write non-format characters to buffer.
            offset_data++;
            formatter->buffer[offset_fmt++] = cd;

            // stop formatting at message end
            if(cd == '\n')
                break;
            continue;
        }

        // we found start of formatting sequence
        char fmt[4] = {0};  // buffer for formatting sequence '%' + ['0' | 'E'] + 'char' + '\0'
        if(get_conversion_spec(fmt, sizeof(fmt),
                               *data) == 0) {
            // conversion specifier starting at data is not valid,
            // we could report error, but it would cause problem
            // for long messages as we may have already sent part of message to client,
            // so we will just send ill-formatted format specifiers back as they are.

            // we copy first '%' from data to formatter->buffer,
            // so in next loop we will copy rest of format and recover
            formatter->buffer[offset_fmt++] = (*data)[offset_data++];
            continue;
        }

        size_t stored = strftime(formatter->buffer + offset_fmt,
                                 sizeof(formatter->buffer) - offset_fmt,
                                 fmt,
                                 &formatter->tm_time);

        // if strftime failed we can return only part and
        // in next call we will have whole buffer
        if(stored == 0) {
            if(offset_fmt == 0) {
                // we have empty buffer so if we failed now, we will fail again
                // report error by setting *data to NULL
                *data = NULL;
                return 0;
            }
            break;
        }

        offset_data += strlen(fmt);
        offset_fmt += stored;
    }

    *data += offset_data;
    return offset_fmt;
}

[[nodiscard]]
int get_conversion_spec(char *dest, size_t dest_size, const char *data) {
    char modifier = '\0';  // either E or O
    char csc;       // conversion specifier character

    // conversion specification starts with '%' and is always at least two characters
    if(data[0] != '%')
        return 0;

    csc = data[1];

    // optional conversion modifier
    if(csc == 'E' || csc == 'O')
        modifier = csc;

    // if modifier was present read next char as conversion specifier character
    if(modifier != '\0')
        csc = data[2];

    // neither of "%\n" | "%E\n" | "%0\n" are valid conversion specification
    if(csc == '\n')
        return 0;

    // check against all specifiers mentioned in documentation
    const char *valid_char_specifiers = "aAbBcCdDeFGghHIjklmMnpPrRsStTuUVwWxXyYzZ+%";
    if(strchr(valid_char_specifiers, csc) == NULL)
        return 0;

    // put resolved format to dest
    if(modifier != '\0') {
        if(dest_size < 4) return 0;
        dest[0] = '%';
        dest[1] = modifier;
        dest[2] = csc;
        dest[3] = '\0';
        return 3;
    }
    else {
        if(dest_size < 3) return 0;
        dest[0] = '%';
        dest[1] = csc;
        dest[2] = '\0';
        return 2;
    }
}

[[nodiscard]]
int is_empty(const char *str) {
    while(*str && isspace(*str++));

    return *str == '\0';
}
