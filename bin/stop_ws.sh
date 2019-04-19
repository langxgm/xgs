#! /bin/sh
USER=`whoami`
echo $USER
PID=`pgrep -u $USER world`
echo $PID
kill -15 $PID >/dev/null 2>&1

