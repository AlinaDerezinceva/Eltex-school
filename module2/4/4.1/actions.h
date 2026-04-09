#ifndef ACTIONS_H
#define ACTIONS_H

#include "structures.h"
#include <stdbool.h>

void update(Contact* c, int n, ...);
bool add(ContactList* list, Contact contact);  
bool del(ContactList* list, const char* fio);
Contact* findByName(ContactList* list, const char* fio);
void show(ContactList* list);
void freeList(ContactList* list);  
#endif