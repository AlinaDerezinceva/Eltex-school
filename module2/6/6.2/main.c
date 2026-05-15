#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "list.h"

int main() {
    ContactList* list = malloc(sizeof(ContactList));
    if (!list) { printf("Ошибка памяти!\n"); return 1; }
    
    initList(list);

    while (1) {   
        switch (printMainMenu()) {
            case 1: {
                Contact con = printAddMenu();
                if (*con.fio) add(list, con);
                break;
            }
            case 2: {
                show(list);
                if (!list->size) { 
                    printf("Нет контактов!\n"); 
                    break; 
                }
                
                char fio[100];
                printf("ФИО для редактирования: "); 
                read(fio, sizeof(fio));
                
                Contact* c = findByName(list, fio);
                if (!c) { 
                    printf("Не найден\n"); 
                    break; 
                }
                
                char tmp[100];
                
                printf("Новое ФИО*: "); 
                read(tmp, sizeof(tmp));
                if (!*tmp) { 
                    printf("ФИО обязательно!\n"); 
                    break; 
                }
                strcpy(c->fio, tmp);
                
                printf("Работа: "); 
                read(tmp, sizeof(tmp));
                if (*tmp) {
                    update(c, 1, FIELD_JOB, tmp);
                }
                
                printf("Телефон: "); 
                read(tmp, sizeof(tmp));
                if (*tmp) {
                    update(c, 1, FIELD_PHONE, tmp);
                }
                
                printf("Email: "); 
                read(tmp, sizeof(tmp));
                if (*tmp) {
                    update(c, 1, FIELD_EMAIL, tmp);
                }
                
                printf("Ссылки: "); 
                read(tmp, sizeof(tmp));
                if (*tmp) {
                    update(c, 1, FIELD_LINKS, tmp);
                }
                
                printf("Обновлён\n");
                break;
            }
            case 3: {
                show(list);
                if (!list->size) {
                    printf("Список пуст!\n");
                    break;
                }
                
                char fio[100];
                printf("ФИО для удаления: ");
                read(fio, sizeof(fio));
                
                if (del(list, fio)) {
                    printf("Удалён\n");
                } else {
                    printf("Не найден\n");
                }
                break;
            }
            case 4:
                show(list);
                break;
            
            case 0:
                freeList(list);
                free(list);
                printf("Выход\n");
                return 0;
            
            default:
                printf("Ошибка выбора\n");
                break;
        }
    }
}