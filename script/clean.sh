#!/bin/sh

for i in $*; do
	if [ -e $i ]; then
		rm $i
	fi
done

