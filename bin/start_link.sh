#! /bin/sh

ulimit -c unlimited
ulimit -n 102400

if [ "`pidof link`" == "" ]; then
	nohup ./link >link.log 2>&1 &
	echo "link `pidof link`"
else
	echo "link running"
fi

