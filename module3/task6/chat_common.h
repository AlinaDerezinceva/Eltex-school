#ifndef CHAT_COMMON_H
#define CHAT_COMMON_H

#define MAX_MSG_SIZE 256
#define SERVER_PRIO 10
#define MAX_CLIENTS 100

struct msgbuf {
    long mtype;         
    int sender;          
    char mtext[MAX_MSG_SIZE]; 
};

#define QUEUE_KEY 12345

#endif