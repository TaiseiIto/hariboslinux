#!/bin/sh

# Usage: ./devenv.sh <docker command> <image name> <image tag> <container name>
# This program creates and logs in to docker image "theos-devenv" and container "theos-devenv".
# This program should be called from "../Makefile" with command "make devenv".

currentdir=$(pwd)
cd $(dirname $0)
if [ $# -eq 4 ]; then
	docker=$1
	image=$2
	tag=$3
	container=$4
	if [ -z "$($docker images | grep $image)" ]; then
		$docker build --no-cache -t $image:$tag ..
	fi
	if [ -z "$($docker ps -a | grep $container)" ]; then
		$docker run --name $container -i -t $image
	else
		if [ -z "$($docker ps | grep $container)"]; then
			$docker start $container
		fi
		$docker attach $container
	fi
else
	echo There should be 4 arguments.
fi
cd $currentdir

