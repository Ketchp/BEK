# Network clock

Application consists of two programs, `net_clock` and `change_time`.
`net_clock` is user interface application that accepts messages
from terminal or TCP connection and replies by formatted time or
changes time.
`change_time` is responsible for actually changing system time and is 
really minimalistic.

Both programs run without root privileges.
`change_time` program binary has `cap_sys_time` capability set
to be able to change system time.

## Format
All messages are separated by newline character `\n`.

Get message starts with `get:` and continues by format specifier.
Format specifier is same as for `strftime` C function.
Response to get command is formatted time.

Set message is allowed only for local editing from console.
Message starts with `set:` followed by format specifier as in
get command.
On next line are values for new time supplied in previous format.

## Config
Configuration file is searched for at:
1. `~/.config/net_clock.conf`
2. `~/net_clock.conf`
3. `~/.net_clock.conf`
4. `/etc/net_clock.conf`
5. path specified in environment variable `NET_CLOCK_CONF`

First configuration file from list that is found is used as configuration.
If no configuration file is found, default values are used.
If configuration file changes during program run,
configuration is also updated in running program.

Configurable items:
- `server_enable`: `"true" | "false"`
- `port`: `int` `1024 <= port <= 65535`
- `max_connections`: `int` `1 <= max_conn <= INT32_MAX`
