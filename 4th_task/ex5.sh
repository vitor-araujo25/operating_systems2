#!/bin/sh

if [ "$#" -eq 0 ]; then
	echo "Preciso de 2 argumentos!"
	exit -1
fi

echo "$2" | grep -q "$1"

if [ "$?" -eq 0 ]; then
	echo "$1 está contida em $2"
fi

