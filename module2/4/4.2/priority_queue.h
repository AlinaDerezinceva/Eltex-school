#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "structures.h"

#define Q_OK  0
#define Q_ERR -1

void pq_init(PriQueue* q);
int  pq_push(PriQueue* q, int pri, const char* data);
int  pq_pop(PriQueue* q, Msg* out);              
int  pq_pop_pri(PriQueue* q, int pri, Msg* out); 
int  pq_pop_ge(PriQueue* q, int min_pri, Msg* out); 
void pq_free(PriQueue* q);

#endif