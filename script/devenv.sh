#!/bin/sh

# Usage: ./devenv.sh <docker command> <image name> <image tag> <container name> <vnc port>
# This program creates and logs in to docker image "hariboslinux" and container "hariboslinux".
# This program should be called from "../Makefile" with command "make devenv".

currentdir=$(pwd)
cd $(dirname $0)
if [ $# -eq 5 ]; then
	docker=$1
	image=$2
	tag=$3
	container=$4
	vnc_port=$5
	if [ -z "$($docker images | grep $image)" ]; then
		$docker build --no-cache -t $image:$tag ..
	fi
	if [ -z "$($docker ps -a | grep $container)" ]; then
		$docker run --name $container -p $vnc_port:$vnc_port -i -t $image
	else
		if [ -z "$($docker ps | grep $container)"]; then
			$docker start $container
		fi
		$docker attach $container
	fi
else
	echo There should be 5 arguments.
fi
cd $currentdir

