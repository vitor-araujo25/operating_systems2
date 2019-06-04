#!/bin/sh

#command strings
date_command="date"
df_command="df -h"
w_command="w"


#confirm variable holds input value
read -p "Would you like to run date?" confirm
if [ "$confirm" = "y" ]; then
	echo -e "Running date:\n$($date_command)"
fi

read -p "Would you like to see disk usage?" confirm
if [ "$confirm" = "y" ]; then
	echo -e "\n\nRunning df:\n$($df_command)"
fi

read -p "Would you like to know which users are logged in?" confirm
if [ "$confirm" = "y" ]; then
	echo -e "\n\nRunning w:\n$($w_command)"
fi

exit 0


