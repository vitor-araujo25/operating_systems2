#!/bin/bash
watch -n 0.5 "ps -eo pid,ppid,stat,comm | awk 'match (\$3, /Z.*/) { print }'"
