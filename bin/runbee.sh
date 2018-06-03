#!/usr/bin/env bash

#killall bee

#rm -rf bee*.log

# set the running environment
path=$(pwd)
export LD_LIBRARY_PATH=$path:$path/lib

#sleep 1

#nohup ./bee bee2.ini -print >> bee2.log 2>&1 &

sleep 1

#nohup ./bee bee.ini -print >> bee.log 2>&1 &
./bee bee.ini

sleep 1

ps -aux | grep bee
