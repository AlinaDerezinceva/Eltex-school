#ifndef LIST_H
#define LIST_H

#include "structures.h"
#include <stdbool.h>
#include <stdarg.h>

void initList(ContactList* list);
void update(Contact* c, int n, ...);
bool add(ContactList* list, Contact contact);
bool del(ContactList* list, const char* fio);
Contact* findByName(ContactList* list, const char* fio);
void show(ContactList* list);
void freeList(ContactList* list);

#endif