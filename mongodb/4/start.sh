#!/bin/sh

ulimit -f unlimited
ulimit -t unlimited
ulimit -v unlimited
ulimit -m unlimited
ulimit -n 32000
ulimit -u 32000

./mongod --config mongod.conf
