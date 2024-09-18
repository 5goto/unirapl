#!/bin/bash
# Проверка первого аргумента
if [[ "$1" == "p" || "$1" == "c" || "$1" == "a" ]]; then
    # Имя файла с логами (зашито в скрипте)
    LOG_FILE="/var/log/unirapl.log"

    # Проверка, передан ли номер сессии в качестве аргумента
    if [ -z "$2" ]; then
    echo "./write_result_by_sessionID [p - PKG | c - single core | a - all cores] [sessionID]"
    exit 1
    fi

    SESSION_ID="$2"

    # Имя выходного CSV файла
    OUTPUT_FILE="${SESSION_ID}.csv"
  # Если аргумент соответствует одному из значений, продолжаем
  case "$1" in
    p)
      grep $SESSION_ID $LOG_FILE | grep 'result' | cut -d ' ' -f6 | awk '{print NR "," $0}' > $OUTPUT_FILE
      ;;
    c)
      grep $SESSION_ID $LOG_FILE | grep 'result' | cut -d ' ' -f6 | awk '{print NR "," $0}' > $OUTPUT_FILE
      ;;
    a)
      grep $SESSION_ID $LOG_FILE | grep 'cores' | cut -d ' ' -f6 | awk '{print NR "," $0}' > $OUTPUT_FILE
  esac
else
  # Если аргумент не соответствует, показываем подсказку
  echo "./write_result_by_sessionID [p - PKG | c - single core | a - all cores] [sessionID]"
fi