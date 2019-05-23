#!/bin/sh

if [ "$#" -eq 0 ]; then
	exit 0
fi

echo "$2" | grep -q "$1"

if [ "$?" -eq 0 ]; then
	echo "$1 está contida em $2"
fi

