#!/bin/bash

# Проверка на права суперпользователя
if [ "$EUID" -ne 0 ]; then
  echo "You must be root to shut down unirapl"
  exit 1
fi

# Проверка существования файла с PID
if [ ! -f /var/run/unirapl.pid ]; then
  echo "Error: PID file /var/run/unirapl.pid not found"
  exit 1
fi

# Получение PID из файла
pid=$(cat /var/run/unirapl.pid)

# Проверка, является ли PID числом (исправленный вариант)
if ! [[ "$pid" =~ ^[0-9]+$ ]]; then
  echo "Error: Invalid PID in /var/run/unirapl.pid"
  exit 1
fi

# Отправка сигнала завершения процессу
kill -TERM "$pid"

echo "unirapl shut down successfully"
