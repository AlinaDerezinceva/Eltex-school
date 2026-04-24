#ifndef STRUCTURES_H
#define STRUCTURES_H

#define PRI_MAX  255   
#define MSG_MAX  256   

typedef struct {
    int  pri;
    char data[MSG_MAX];
} Msg;

typedef struct Node {
    Msg msg;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
} Bin;               
typedef struct {
    Bin bins[PRI_MAX + 1];
} PriQueue;          

#endif