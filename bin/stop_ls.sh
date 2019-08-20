#! /bin/sh
USER=`whoami`
echo $USER
PID=`pidof login`
echo $PID
kill -15 $PID >/dev/null 2>&1

