#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "structures.h"
#include "actions.h"
#include "interface.h"

#define DATA_FILE "contacts.dat"

static ContactsArray* g_contacts = NULL;

void saveOnExit(void) {
    if (g_contacts) {
        printf("\nСохранение данных...\n");  
        if (saveContacts(g_contacts, DATA_FILE)) {
            printf("Данные сохранены в %s\n", DATA_FILE);  
        } else {
            printf("Ошибка сохранения!\n");  
        }
    }
}

void onSignal(int sig) {
    saveOnExit();
    exit(sig == SIGINT ? 130 : 1);
}

int main() {
    ContactsArray *contacts = malloc(sizeof(ContactsArray));
    if (!contacts) return 1;
    
    contacts->contacts = NULL;
    contacts->size = 0;
    g_contacts = contacts;
    
    // Загрузка
    if (!loadContacts(contacts, DATA_FILE)) {
        fprintf(stderr, "Ошибка загрузки!\n");
        free(contacts);
        return 1;
    }
    
    signal(SIGINT, onSignal);
    atexit(saveOnExit);
    
    while (1) {   
        switch (printMainMenu()) {
            case 1: {
                Contact con = printAddMenu();
                if (*con.fio) {
                    add(contacts, con);
                } 
                break;
            }
            case 2: {
                show(contacts);
                if (!contacts->size) {
                    printf("Нет контактов!\n");
                    break;
                }
                char fio[100];
                printf("ФИО для редактирования: "); 
                readLine(fio, sizeof(fio));
                
                Contact* c = findByName(contacts->contacts, contacts->size, fio);
                if (!c) { 
                    printf("Не найден\n"); 
                    break; 
                }
                
                char tmp[100];
                printf("Новое ФИО*: "); 
                readLine(tmp, sizeof(tmp));
                if (!*tmp) { 
                    printf("ФИО обязательно!\n"); 
                    break; 
                }
                strncpy(c->fio, tmp, sizeof(c->fio) - 1);
                c->fio[sizeof(c->fio) - 1] = '\0';
                
                printf("Работа: "); 
                readLine(tmp, sizeof(tmp)); 
                if (*tmp) update(c, 1, FIELD_JOB, tmp);
                
                printf("Телефон: "); 
                readLine(tmp, sizeof(tmp)); 
                if (*tmp) update(c, 1, FIELD_PHONE, tmp);
                
                printf("Email: "); 
                readLine(tmp, sizeof(tmp)); 
                if (*tmp) update(c, 1, FIELD_EMAIL, tmp);
                
                printf("Ссылки: "); 
                readLine(tmp, sizeof(tmp)); 
                if (*tmp) update(c, 1, FIELD_LINKS, tmp);
                
                printf("Обновлён\n");
                break;
            }
            case 3: {
                show(contacts);
                if (!contacts->size) {
                    printf("Список пуст!\n");
                    break;
                }
                char fio[100];
                printf("ФИО для удаления: "); 
                readLine(fio, sizeof(fio));
                
                if (del(contacts, fio)) {
                    printf("Удалён\n");
                } else {
                    printf("Не найден\n");
                }
                break;
            }
            case 4:
                show(contacts);
                break;
                
            case 0:
                printf("Выход...\n"); 
                saveContacts(contacts, DATA_FILE);
                // Очистка памяти
                if (contacts->contacts) free(contacts->contacts);
                free(contacts);
                g_contacts = NULL;
                return 0;
                
            default:
                printf("Ошибка выбора\n");
                break;
        }
    }
}