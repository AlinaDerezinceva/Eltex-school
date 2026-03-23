#!/bin/bash

SCRIPT_NAME=$(basename "$0")


if [ "$SCRIPT_NAME" = "template_task.sh" ]; then
    echo "я бригадир, сам не работаю"
    exit 1
fi

LOG="report_${SCRIPT_NAME}.log"

echo "$(date '+%Y-%m-%d %H:%M:%S') [$$] Скрипт запущен" >> "$LOG"


if [ -p "/tmp/run/cuckoo.pid" ]; then
    echo "${SCRIPT_NAME}[$$]: how much time do I have?" > /tmp/run/cuckoo.pid
fi


sleep 5

END_TIME=$(date +%s)
DURATION=$((END_TIME - START_TIME))

echo "$(date '+%Y-%m-%d %H:%M:%S') [$$] Скрипт завершился, работал $DURATION секунд" >> "$LOG"
