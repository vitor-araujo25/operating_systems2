#!/bin/sh

value=$1
while [ $value -ne -1 ]; do
	echo -n "$value "
	((value=$value - 1))
done
echo ""
