#!/bin/bash

# Проверка на права суперпользователя
if [ "$EUID" -ne 0 ]; then
  echo "You must be root to start unirapl"
  exit 1
fi

# Проверка существования файла с PID
if [ ! -f bin/unirapl ]; then
  echo "Error: file bin/unirapl not found"
  exit 1
fi

./bin/unirapl

echo "unirapl started"
echo "see /var/log/unirapl.log"
