#!/usr/bin/env bash

for file in *.proto
do
    if test -f $file
    then
        #echo $file 是文件
        file_name=$(basename $file .proto)
        pb_file=${file_name}".pb"
        echo $pb_file
        protoc $file -o ${pb_file}
        mv $pb_file ../script/pb/
    fi
    if test -d $file
    then
        echo $file 是目录
    fi
done

