#include <stdio.h>
#include <stdlib.h>
#include "menu.h"
#include "plugins.h"

int main() {
    Plugin* plugins = NULL;
    size_t count = 0;

    // Загрузка библиотек из каталога ./libs
    if (load_plugins_from_dir("./libs", &plugins, &count) != 0) {
        fprintf(stderr, "Ошибка загрузки плагинов. Завершение.\n");
        return 1;
    }

    if (count == 0) {
        fprintf(stderr, "В каталоге ./libs не найдено корректных библиотек (.so).\n");
        return 1;
    }

    printf("Успешно загружено операций: %zu\n", count);

    int choice;
    double a, b, result;

    while (1) {
        print_menu(plugins, count);
        choice = get_choice();

        if (choice == 0) {
            printf("Выход\n");
            break;
        }
        if (choice < 1 || choice > (int)count) {
            printf("Неверный выбор\n");
            while(getchar()!='\n');
            continue;
        }

        if (!get_numbers(&a, &b)) {
            printf("Ошибка ввода\n");
            while(getchar()!='\n');
            continue;
        }

        // Вызов загруженной функции
        result = plugins[choice - 1].func(a, b);
        printf("Результат: %.2lf\n", result);
    }

    free_plugins(plugins, count);
    return 0;
}