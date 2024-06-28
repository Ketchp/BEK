#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <stdlib.h>

#include "cap_drop.h"


int main(int argc, char *argv[]) {
    if(argc != 2)
        return EXIT_FAILURE;

    cap_value_t cap_sys_time = CAP_SYS_TIME;
    if(cap_drop(&cap_sys_time, 1) != 0)
        return EXIT_FAILURE;

    char *end;
    time_t time = strtoll(argv[1], &end, 10);

    if(argv[1] == end || *end != '\0')
        return EXIT_FAILURE;

    struct timespec tspec = {.tv_sec = time, .tv_nsec = 0};

    if(clock_settime(CLOCK_REALTIME, &tspec) != 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
