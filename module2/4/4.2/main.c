#include "priority_queue.h"
#include <stdio.h>

int main() {
    PriQueue q;
    pq_init(&q);
    Msg out;
    int rc;

    puts("Генерация сообщений");
    pq_push(&q, 50,  "Msg_Normal_1");
    pq_push(&q, 200, "Msg_High_1");
    pq_push(&q, 50,  "Msg_Normal_2");
    pq_push(&q, 255, "Msg_Critical_1");
    pq_push(&q, 150, "Msg_Medium_1");
    pq_push(&q, 255, "Msg_Critical_2");
    pq_push(&q, 10,  "Msg_Low_1");
    puts("Добавлено 7 сообщений.\n");

    puts("1. Извлечение первого (макс. приоритет)");
    for (int i = 0; i < 2; i++) {
        rc = pq_pop(&q, &out);
        if (rc == Q_OK) printf("  [%3d] %s\n", out.pri, out.data);
    }

    puts("2. Извлечение с приоритетом 50");
    rc = pq_pop_pri(&q, 50, &out);
    if (rc == Q_OK) printf("  [%3d] %s\n", out.pri, out.data);
    else puts("Нет сообщений с приоритетом 50");

    puts("3. Извлечение с приоритетом >= 100");
    rc = pq_pop_ge(&q, 100, &out);
    if (rc == Q_OK) printf("  [%3d] %s\n", out.pri, out.data);
    else puts("  Нет сообщений с приоритетом >= 100");

    puts("Остаток очереди");
    while (pq_pop(&q, &out) == Q_OK)
        printf("  [%3d] %s\n", out.pri, out.data);

    pq_free(&q);
    puts("Готово!");
    return 0;
}