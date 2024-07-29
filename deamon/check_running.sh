#!/bin/bash

pid=$(cat /var/run/unirapl.pid)

ps -p $pid > /dev/null 2>&1
if [ $? -eq 0 ]; then
	echo "unirapld status: running"
else
	echo "unirapld status: shutdown"
fi
