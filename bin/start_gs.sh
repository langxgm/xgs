#! /bin/sh

ulimit -c unlimited
ulimit -n 102400

if [ "`pidof game`" == "" ]; then
	nohup ./game >game.log 2>&1 &
	echo "game `pidof game`"
else
	echo "game running"
fi

