#!/bin/sh

#command strings
date_command="date"
df_command="df -h"
w_command="w"

echo "Would you like to run?"

#confirm variable holds input value
read confirm
if [ "$confirm" = "y" ]; then
	echo -e "Running date:\n$($date_command)"
	echo -e "\n\nRunning df:\n$($df_command)"
	echo -e "\n\nRunning w:\n$($w_command)"
fi
exit 0


