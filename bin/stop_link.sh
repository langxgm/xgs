#! /bin/sh
USER=`whoami`
echo $USER
PID=`pidof link`
echo $PID
kill -15 $PID >/dev/null 2>&1

