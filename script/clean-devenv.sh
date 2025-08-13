#!/bin/sh

# Usage: $ ./clean_devenv.sh <docker command> <image name> <container name>
# This program removes docker image "hariboslinux" and container "hariboslinux".
# This program should be called from "../Makefile" with command "make clean-devenv".

if [ $# -eq 3 ]; then
	docker=$1
	image=$2
	container=$3
	if [ -n "$($docker ps -a | grep $container)" ]; then
		$docker rm $container
	fi
	if [ -n "$($docker images | grep $image)" ]; then
		$docker rmi $image
	fi
else
	echo There should be 3 arguments.
fi

