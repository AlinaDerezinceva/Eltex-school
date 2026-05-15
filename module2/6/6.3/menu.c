#include <stdio.h>
#include "menu.h"

void print_menu(const Plugin* plugins, size_t count) {
    printf("Калькулятор\n");
    for (size_t i = 0; i < count; i++) {
        printf("%zu. %s\n", i + 1, plugins[i].name);
    }
    printf("0. Выход\n");
}

int get_choice() {
    int c;
    printf("Выберите действие: ");
    if (scanf("%d", &c) == 1) return c;
    return -1;
}

int get_numbers(double *a, double *b) {
    printf("Введите два числа: ");
    return (scanf("%lf %lf", a, b) == 2);
}