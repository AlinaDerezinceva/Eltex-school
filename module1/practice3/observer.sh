#!/bin/bash

CONF_FILE="observer.conf"
LOG_FILE="observer.log"

while IFS= read -r script_path || [ -n "$script_path" ]; do
    [ -z "$script_path" ] && continue
    [[ $script_path =~ ^# ]] && continue
    
    script_name=$(basename "$script_path")
    found=false
    
    for pid_dir in /proc/[0-9]*; do
        pid=$(basename "$pid_dir")
        if [ -f "/proc/$pid/cmdline" ]; then
            cmdline=$(cat "/proc/$pid/cmdline" 2>/dev/null | tr '\0' ' ')
            if echo "$cmdline" | grep -q "$script_name"; then
                found=true
                break
            fi
        fi
    done
    
    if [ "$found" = false ]; then
        if [ -f "$script_path" ] && [ -x "$script_path" ]; then
            echo "$(date '+%Y-%m-%d %H:%M:%S') Starting $script_name" >> "$LOG_FILE"
            nohup "$script_path" >/dev/null 2>&1 &
        fi
    fi
done < "$CONF_FILE"
