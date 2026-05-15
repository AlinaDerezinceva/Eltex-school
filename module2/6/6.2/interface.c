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
    printf("1-Добавить 2-Редактировать 3-Удалить 4-Показать 0-Выход\n");
    printf("Выбор: ");
    int action;
    scanf("%d", &action);
    clearInput();
    return action;
}

Contact printAddMenu(void) {
    Contact c = {0};
    printf("Добавление контакта \nФИО*: ");
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