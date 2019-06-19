#!/bin/bash
kill -9 $(ps -eo stat,ppid | awk '{if($1 == "Z") { print $2 } }' | uniq)
