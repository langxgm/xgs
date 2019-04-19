#! /bin/sh
USER=`whoami`
echo $USER
PID=`pgrep -u $USER game`
echo $PID
kill -15 $PID >/dev/null 2>&1

