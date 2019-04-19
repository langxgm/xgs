#! /bin/sh
USER=`whoami`
echo $USER

WS_PID=`pgrep -u $USER world`
echo world pid: $WS_PID

GS_PID=`pgrep -u $USER game`
echo game pid: $GS_PID

GWS_PID=`pgrep -u $USER gate`
echo gate pid: $GWS_PID

TOP_ARG="$WS_PID,$GS_PID,$GWS_PID"
echo top arg: $TOP_ARG

top -p $TOP_ARG
