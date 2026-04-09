#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct {
    char fio[100];
    char job[50];
    char phone[50];
    char email[50];
    char links[100];
} Contact;

typedef struct Node {
    Contact data;
    struct Node* prev;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    int size;
} ContactList;

typedef enum { 
    FIELD_JOB, 
    FIELD_PHONE, 
    FIELD_EMAIL, 
    FIELD_LINKS 
} Field;

#endif