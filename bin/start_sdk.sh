#! /bin/sh

ulimit -c unlimited
ulimit -n 102400

if [ "`pidof sdk`" == "" ]; then
	nohup ./sdk >sdk.log 2>&1 &
	echo "sdk `pidof sdk`"
else
	echo "sdk running"
fi

