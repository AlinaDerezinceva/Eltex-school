#include "interface.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void read(char* s, int n) {
    if (fgets(s, n, stdin)) {
        s[strcspn(s, "\n")] = 0;
    }
}

void clearInput(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int printMainMenu(void) {
    printf("1-Добавить 2-Редактировать 3-Удалить 4-Показать 0-Выход\nВыбор: ");
    int action;
    scanf("%d", &action);
    clearInput();
    return action;
}

Contact printAddMenu(void) {
    Contact c = {0};
    
    printf("Добавление\nФИО: ");
    read(c.fio, sizeof(c.fio));
    if (!*c.fio) { 
        printf("Ошибка: ФИО обязательно!\n"); 
        return c; 
    }
    
    printf("Работа: "); 
    read(c.job, sizeof(c.job));
    printf("Телефон: "); 
    read(c.phone, sizeof(c.phone));
    printf("Email: "); 
    read(c.email, sizeof(c.email));
    printf("Ссылки: "); 
    read(c.links, sizeof(c.links));
    
    printf("Добавлен!\n");
    return c;
}

void printOne(Contact* c) {
    printf("[%s]\n", c->fio);
    printf("Работа: %s\n", c->job);
    printf("Телефон: %s\n", c->phone);
    printf("Email: %s\n", c->email);
    printf("Ссылки: %s\n", c->links);
}