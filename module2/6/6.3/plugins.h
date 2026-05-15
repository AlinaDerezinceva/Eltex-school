#ifndef PLUGINS_H
#define PLUGINS_H

#include <stddef.h>

typedef struct {
    void* handle;                          
    double (*func)(double, double);        
    char name[64];                        
} Plugin;

int load_plugins_from_dir(const char* dir_path, Plugin** out_plugins, size_t* out_count);
void free_plugins(Plugin* plugins, size_t count);

#endif