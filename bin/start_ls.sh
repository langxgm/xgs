#! /bin/sh

ulimit -c unlimited
ulimit -n 102400

if [ "`pidof login`" == "" ]; then
	nohup ./login >login.log 2>&1 &
	echo "login `pidof login`"
else
	echo "login running"
fi

