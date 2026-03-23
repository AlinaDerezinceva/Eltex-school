#!/bin/bash

mkdir -p /tmp/run/

FIFO="/tmp/run/cuckoo.pid"
[ -p "$FIFO"] && rm -f "$FIFO"

mkfifo "$FIFO"

LOG="/tmp/run/cuckoo.log"

echo "$(date '+%Y-%m-%d %H:%M:%S') Startup!" >> "$LOG"

trup 'echo "$(date '+Y-%m-%d %H:%M:%S') Shutdown!" >> "$LOG"; rm -f "FIFO"; exit 0' SIGTERM

while true; do
	read -r line < "$FIFO"
	NAME_PID=$(echo "$line" | cut -d: -f1)
	N=$((RANDOM % 9 + 2))
	echo "$(date '+%Y-%m-%d %H:%M:%S') $NAME_PID $N" >> "$LOG"
done
