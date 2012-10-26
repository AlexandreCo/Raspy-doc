#!/bin/sh

### BEGIN INIT INFO
# Provides:        custom-x-startup
# Required-Start:  $network $syslog $all
# Required-Stop:
# Default-Start:   2 3 4 5
# Default-Stop:
# Short-Description: Boot into X
### END INIT INFO

PATH=/sbin:/bin:/usr/sbin:/usr/bin

. /lib/lsb/init-functions

test -x /usr/bin/startx || exit 5

case "$1" in
    start)
        log_daemon_msg "Starting X" "custom-x-startup"
        /bin/su - pi --command /usr/bin/startx
        ;;
    *)
        echo "Usage: {start}"
        exit 2
        ;;
esac
