#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>
#include "plugins.h"

#define MAX_PLUGINS 64

int load_plugins_from_dir(const char* dir_path, Plugin** out_plugins, size_t* out_count) {
    DIR* dir = opendir(dir_path);
    if (!dir) {
        perror("Не удалось открыть каталог библиотек");
        return -1;
    }

    Plugin* plugins = calloc(MAX_PLUGINS, sizeof(Plugin));
    if (!plugins) {
        closedir(dir);
        return -1;
    }

    size_t count = 0;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && count < MAX_PLUGINS) {
        // Ищем только файлы с расширением .so
        if (strstr(entry->d_name, ".so") == NULL) continue;

        char path[512];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

        void* handle = dlopen(path, RTLD_LAZY);
        if (!handle) {
            fprintf(stderr, "dlopen(%s): %s\n", entry->d_name, dlerror());
            continue;
        }

        double (*func)(double, double) = (double (*)(double, double))dlsym(handle, "calc_op");
        if (!func) {
            fprintf(stderr, "dlsym(calc_op) в %s: %s\n", entry->d_name, dlerror());
            dlclose(handle);
            continue;
        }

        plugins[count].handle = handle;
        plugins[count].func = func;
        
        // Имя операции берём из имени файла без расширения
        strncpy(plugins[count].name, entry->d_name, sizeof(plugins[count].name) - 1);
        char* ext = strrchr(plugins[count].name, '.');
        if (ext) *ext = '\0';
        count++;
    }
    closedir(dir);

    *out_plugins = plugins;
    *out_count = count;
    return 0;
}

void free_plugins(Plugin* plugins, size_t count) {
    for (size_t i = 0; i < count; i++) {
        if (plugins[i].handle) dlclose(plugins[i].handle);
    }
    free(plugins);
}