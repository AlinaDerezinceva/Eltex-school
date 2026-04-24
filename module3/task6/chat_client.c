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

#include "chat_common.h"

int msgid;
int client_prio;
int running = 1;

void cleanup(int sig) {
    running = 0;
}

void* receive_messages(void* arg) {
    struct msgbuf msg;
    
    while (running) {
        if (msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), client_prio, 0) == -1) {
            if (running) {
                perror("msgrcv");
            }
            break;
        }
        
        printf("[Клиент %d]: %s", msg.sender, msg.mtext);
        printf("Вы: ");
        fflush(stdout);
    }
    
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <номер_клиента>\n", argv[0]);
        fprintf(stderr, "Номер клиента должен быть 20, 30, 40, ...\n");
        exit(1);
    }
    
    client_prio = atoi(argv[1]);
    if (client_prio < 20 || client_prio % 10 != 0) {
        fprintf(stderr, "Ошибка: номер клиента должен быть 20, 30, 40, ...\n");
        exit(1);
    }
    
    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);
    
    msgid = msgget(QUEUE_KEY, 0666);
    if (msgid == -1) {
        perror("msgget");
        fprintf(stderr, "Убедитесь, что сервер запущен\n");
        exit(1);
    }
    
    printf("[Клиент %d] Запущен. Для выхода введите 'shutdown'\n", client_prio);
    printf("Формат сообщений: <текст>\n");
    
    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receive_messages, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }
    
    char input[MAX_MSG_SIZE];
    struct msgbuf msg;
    msg.sender = client_prio;
    
    while (running) {
        printf("Вы: ");
        fflush(stdout);
        
       
    }
    
    return 0;
}