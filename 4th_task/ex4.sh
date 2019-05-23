#!/bin/sh

value=$1
while [ $value -ne 0 ]; do
	echo -n "$value "
	value=$(expr $value - 1)
done
echo ""
