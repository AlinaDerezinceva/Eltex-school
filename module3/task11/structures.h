#ifndef STRUCTURES_H
#define STRUCTURES_H

#define MAX_NUMBERS 50

typedef struct {
    int numbers[MAX_NUMBERS];
    int count;
    int min_val;
    int max_val;
    volatile int ready_for_child;
    volatile int ready_for_parent;
    int processed_count;
} SharedData;

#endif