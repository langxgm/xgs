#! /bin/sh

ulimit -c unlimited
ulimit -n 102400

if [ "`pidof gate`" == "" ]; then
	nohup ./gate >gate.log 2>&1 &
	echo "gate `pidof gate`"
else
	echo "gate running"
fi
