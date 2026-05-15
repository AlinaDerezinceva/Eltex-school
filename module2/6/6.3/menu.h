#ifndef MENU_H
#define MENU_H
#include "plugins.h"

void print_menu(const Plugin* plugins, size_t count);
int get_choice();
int get_numbers(double *a, double *b);

#endif