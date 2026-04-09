#ifndef ACTIONS_H
#define ACTIONS_H

#include "structures.h"
#include <stdbool.h>
#include <stdarg.h>

void update(Contact* c, int n, ...);
bool add(ContactsArray* arr, Contact contact);
bool del(ContactsArray* arr, const char* fio);
Contact* findByName(Contact* contacts, int size, const char* fio);
void show(ContactsArray* arr);

bool loadContacts(ContactsArray* arr, const char* filename);
bool saveContacts(const ContactsArray* arr, const char* filename);

#endif