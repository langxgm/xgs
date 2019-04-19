#! /bin/sh

ulimit -c unlimited
ulimit -n 102400

if [ "`pidof world`" == "" ]; then
	nohup ./world >world.log 2>&1 &
	echo "world `pidof world`"
else
	echo "world running"
fi

