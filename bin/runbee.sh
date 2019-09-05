#!/usr/bin/env bash

#killall bee

#rm -rf bee*.log

# set the running environment
path=$(pwd)
export LD_LIBRARY_PATH=$path:$path/lib

#sleep 1

sleep 1

./bee test.ini

sleep 1

./bee test2.ini

ps -aux | grep bee
