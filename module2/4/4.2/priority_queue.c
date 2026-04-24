#include "priority_queue.h"
#include <stdlib.h>
#include <string.h>

static int bin_pop(Bin* bkt, Msg* out) {
    if (!bkt->head) return Q_ERR;
    Node* tmp = bkt->head;
    *out = tmp->msg;
    bkt->head = bkt->head->next;
    if (!bkt->head) bkt->tail = NULL;
    free(tmp);
    return Q_OK;
}

void pq_init(PriQueue* q) {
    if (!q) return;
    for (int i = 0; i <= PRI_MAX; i++) {
        q->bins[i].head = NULL;
        q->bins[i].tail = NULL;
    }
}

int pq_push(PriQueue* q, int pri, const char* data) {
    if (!q || !data || pri < 0 || pri > PRI_MAX) return Q_ERR;
    Node* n = malloc(sizeof(Node));
    if (!n) return Q_ERR;

    n->msg.pri = pri;
    strncpy(n->msg.data, data, MSG_MAX - 1);
    n->msg.data[MSG_MAX - 1] = '\0';
    n->next = NULL;

    Bin* bkt = &q->bins[pri];
    if (!bkt->head) bkt->head = bkt->tail = n;
    else { bkt->tail->next = n; bkt->tail = n; }
    return Q_OK;
}

int pq_pop(PriQueue* q, Msg* out) {
    if (!q || !out) return Q_ERR;
    for (int p = PRI_MAX; p >= 0; p--)
        if (q->bins[p].head) return bin_pop(&q->bins[p], out);
    return Q_ERR;
}

int pq_pop_pri(PriQueue* q, int pri, Msg* out) {
    if (!q || !out || pri < 0 || pri > PRI_MAX) return Q_ERR;
    return bin_pop(&q->bins[pri], out);
}

int pq_pop_ge(PriQueue* q, int min_pri, Msg* out) {
    if (!q || !out || min_pri < 0 || min_pri > PRI_MAX) return Q_ERR;
    for (int p = PRI_MAX; p >= min_pri; p--)
        if (q->bins[p].head) return bin_pop(&q->bins[p], out);
    return Q_ERR;
}

void pq_free(PriQueue* q) {
    if (!q) return;
    for (int i = 0; i <= PRI_MAX; i++) {
        Node* cur = q->bins[i].head;
        while (cur) {
            Node* nxt = cur->next;
            free(cur);
            cur = nxt;
        }
        q->bins[i].head = q->bins[i].tail = NULL;
    }
}