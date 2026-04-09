#include "actions.h"
#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

void update(Contact* c, int n, ...) {
    va_list args; 
    va_start(args, n);
    for (int i = 0; i < n; i++) {
        Field f = va_arg(args, Field);
        char* v = va_arg(args, char*);
        if (!v || !*v) continue;
        switch (f) {
            case FIELD_JOB:
                strncpy(c->job, v, sizeof(c->job) - 1);
                c->job[sizeof(c->job) - 1] = '\0';
                break;
            case FIELD_PHONE:
                strncpy(c->phone, v, sizeof(c->phone) - 1);
                c->phone[sizeof(c->phone) - 1] = '\0';
                break;
            case FIELD_EMAIL:
                strncpy(c->email, v, sizeof(c->email) - 1);
                c->email[sizeof(c->email) - 1] = '\0';
                break;
            case FIELD_LINKS:
                strncpy(c->links, v, sizeof(c->links) - 1);
                c->links[sizeof(c->links) - 1] = '\0';
                break;
        }
    }
    va_end(args);
}

bool add(ContactsArray* arr, Contact contact) {
    if (arr == NULL || contact.fio[0] == '\0') {
        return false;
    }
    
    Contact* temp = realloc(arr->contacts, (arr->size + 1) * sizeof(Contact));
    if (!temp) {
        return false;
    } 
    
    arr->contacts = temp;
    arr->contacts[arr->size++] = contact;
    return true;
}

bool del(ContactsArray* arr, const char* fio) {  
    Contact* contact_it = findByName(arr->contacts, arr->size, fio);
    
    if (contact_it == NULL) {
        return false;
    }
    
    int idx = contact_it - arr->contacts;
    
    for (int i = idx; i < arr->size - 1; i++) {
        arr->contacts[i] = arr->contacts[i + 1];
    }
    
    if (arr->size > 1) {
        Contact* temp = realloc(arr->contacts, (arr->size - 1) * sizeof(Contact));
        if (temp) arr->contacts = temp;
    } else {
        free(arr->contacts);
        arr->contacts = NULL;
    }
    
    arr->size--;
    return true;
}

Contact* findByName(Contact* contacts, int size, const char* fio) {
    if (!contacts) return NULL;
    
    for (int i = 0; i < size; i++) {
        if (strstr(contacts[i].fio, fio) != NULL) {
            return &contacts[i];
        }
    }
    return NULL;
}

void show(ContactsArray* arr) {
    if (!arr || !arr->size) { 
        printf("Список пуст\n"); 
        return; 
    }
    printf("Контакты:\n");
    for (int i = 0; i < arr->size; i++) {
        printOne(&arr->contacts[i]);
        printf("\n");
    }
}

// Сохранение в файл
bool saveContacts(const ContactsArray* arr, const char* filename) {
    if (!arr || !filename) return false;
    
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return false;
    
    if (write(fd, &arr->size, sizeof(int)) != sizeof(int)) {
        close(fd);
        return false;
    }
    
    if (arr->size > 0) {
        ssize_t written = write(fd, arr->contacts, arr->size * sizeof(Contact));
        if (written != (ssize_t)(arr->size * sizeof(Contact))) {
            close(fd);
            return false;
        }
    }
    
    close(fd);
    return true;
}

// Загрузка из файла
bool loadContacts(ContactsArray* arr, const char* filename) {
    if (!arr || !filename) return false;
    
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        arr->contacts = NULL;
        arr->size = 0;
        return true;
    }
    
    int size;
    if (read(fd, &size, sizeof(int)) != sizeof(int)) {
        close(fd);
        return false;
    }
    
    arr->size = size;
    
    if (size > 0) {
        arr->contacts = malloc(size * sizeof(Contact));
        if (!arr->contacts) {
            close(fd);
            return false;
        }
        
        ssize_t bytesRead = read(fd, arr->contacts, size * sizeof(Contact));
        if (bytesRead != (ssize_t)(size * sizeof(Contact))) {
            free(arr->contacts);
            arr->contacts = NULL;
            arr->size = 0;
            close(fd);
            return false;
        }
    } else {
        arr->contacts = NULL;
    }
        
    close(fd);
    return true;
}