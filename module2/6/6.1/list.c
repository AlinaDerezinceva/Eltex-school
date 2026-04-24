#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initList(ContactList* list) {
    if (list) {
        list->head = NULL;
        list->tail = NULL;
        list->size = 0;
    }
}

static Node* createNode(Contact contact) {
    Node* node = malloc(sizeof(Node));
    if (!node) return NULL;
    
    node->data = contact;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

static int compareFio(const char* a, const char* b) {
    return strcmp(a, b);
}

void update(Contact* c, int n, ...) {
    va_list args; 
    va_start(args, n);
    for (int i = 0; i < n; i++) {
        Field f = va_arg(args, Field);
        char* v = va_arg(args, char*);
        if (!v || !*v) continue;
        switch (f) {
            case FIELD_JOB:
                strcpy(c->job, v);
                break;
            case FIELD_PHONE:
                strcpy(c->phone, v);
                break;
            case FIELD_EMAIL:
                strcpy(c->email, v);
                break;
            case FIELD_LINKS:
                strcpy(c->links, v);
                break;
        }
    }
    va_end(args);
}

bool add(ContactList* list, Contact contact) {
    if (!list || !*contact.fio) return false;
    
    Node* newNode = createNode(contact);
    if (!newNode) return false;
    
    if (!list->head) {
        list->head = newNode;
        list->tail = newNode;
        list->size = 1;
        return true;
    }
    
    if (compareFio(contact.fio, list->head->data.fio) <= 0) {
        newNode->next = list->head;
        list->head->prev = newNode;
        list->head = newNode;
        list->size++;
        return true;
    }
    
    if (compareFio(contact.fio, list->tail->data.fio) >= 0) {
        newNode->prev = list->tail;
        list->tail->next = newNode;
        list->tail = newNode;
        list->size++;
        return true;
    }
    
    Node* current = list->head;
    while (current && compareFio(contact.fio, current->data.fio) > 0) {
        current = current->next;
    }
    
    newNode->next = current;
    newNode->prev = current->prev;
    current->prev->next = newNode;
    current->prev = newNode;
    list->size++;
    
    return true;
}

bool del(ContactList* list, const char* fio) {
    if (!list || !list->head) return false;
    
    Node* current = list->head;
    
    while (current) {
        if (strstr(current->data.fio, fio) != NULL) {
            break;
        }
        current = current->next;
    }
    
    if (!current) return false;
    
    if (current->prev) {
        current->prev->next = current->next;
    } else {
        list->head = current->next;
    }
    
    if (current->next) {
        current->next->prev = current->prev;
    } else {
        list->tail = current->prev;
    }
    
    free(current);
    list->size--;
    
    return true;
}

Contact* findByName(ContactList* list, const char* fio) {
    if (!list || !list->head) return NULL;
    
    Node* current = list->head;
    while (current) {
        if (strstr(current->data.fio, fio) != NULL) {
            return &current->data;
        }
        current = current->next;
    }
    return NULL;
}

void show(ContactList* list) {
    if (!list || !list->head) { 
        printf("Список пуст\n"); 
        return; 
    }
    printf("Контакты\n");
    Node* current = list->head;
    while (current) {
        printf("[%s]\n", current->data.fio);
        printf("Работа: %s\n", current->data.job);
        printf("Телефон: %s\n", current->data.phone);
        printf("Email: %s\n", current->data.email);
        printf("Ссылки: %s\n", current->data.links);
        current = current->next;
    }
    printf("Всего: %d\n\n", list->size);
}

void freeList(ContactList* list) {
    if (!list) return;
    
    Node* current = list->head;
    while (current) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}