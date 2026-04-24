#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <errno.h>
#include <bits/pthreadtypes.h>
#include <signal.h>

#include "chat_common.h"

int msgid;
int active_clients[MAX_CLIENTS];
int running = 1;

void cleanup(int sig) {
    running = 0;
}

void add_client(int prio) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (active_clients[i] == 0) {
            active_clients[i] = prio;
            printf("[Сервер] Клиент с приоритетом %d подключен\n", prio);
            return;
        }
    }
    printf("[Сервер] Ошибка: достигнуто максимальное количество клиентов\n");
}

void remove_client(int prio) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (active_clients[i] == prio) {
            active_clients[i] = 0;
            printf("[Сервер] Клиент с приоритетом %d отключен\n", prio);
            return;
        }
    }
}

int is_client_active(int prio) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (active_clients[i] == prio) {
            return 1;
        }
    }
    return 0;
}


int main() {
    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);
    
    msgid = msgget(QUEUE_KEY, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }
    
    memset(active_clients, 0, sizeof(active_clients));
    
    printf("[Сервер] Запущен. Ожидание сообщений...\n");
    printf("[Сервер] Для остановки нажмите Ctrl+C\n\n");
    
    struct msgbuf msg;
    
    while (running) {
        
    printf("[Сервер] Остановлен. Очередь удалена.\n");
    
    return 0;
}