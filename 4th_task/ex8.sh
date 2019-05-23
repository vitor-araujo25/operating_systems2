#!/bin/sh

cat /etc/passwd | cut -d':' -f7 | sort | uniq 
