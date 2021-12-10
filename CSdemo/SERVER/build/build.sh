#!/bin/sh

echo "--------------------- build start --------------------"

cd ../APP
make -j clean
make -j  
if [ $? != 0 ];then
	echo "--------------------- build failed!!! ----------------"
	exit 1
fi

echo "--------------------- build complete -----------------"
