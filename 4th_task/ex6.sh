#!/bin/sh

cat /etc/passwd | cut -d':' --fields=1,5 | sort | tr ':' "\t"
