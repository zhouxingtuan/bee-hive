#!/usr/bin/env bash

#killall hive

#rm -rf *.log

# set the running environment
path=$(pwd)
export LD_LIBRARY_PATH=$path:$path/lib

sleep 1

#nohup ./hive hive.ini -print >> hive.log 2>&1 &
./hive hive.ini

./hive hive2.ini

ps -aux | grep hive

