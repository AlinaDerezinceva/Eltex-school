#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MQ_P1_TO_P2 "/mq_chat_p1"
#define MQ_P2_TO_P1 "/mq_chat_p2"
#define MAX_MSG_SIZE 256
#define PRI_NORMAL 1
#define PRI_QUIT 0

int main(void) {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    mqd_t mq_send_fd = mq_open(MQ_P1_TO_P2, O_CREAT | O_RDWR, 0644, &attr);
    mqd_t mq_recv_fd = mq_open(MQ_P2_TO_P1, O_CREAT | O_RDWR, 0644, &attr);

    if (mq_send_fd == (mqd_t)-1 || mq_recv_fd == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    char buf[MAX_MSG_SIZE];
    unsigned int pri;

    printf("[User 1] Чат запущен. Введите 'quit' для выхода.\n");

    while (1) {
        printf("Вы: ");
        fflush(stdout);
        if (fgets(buf, MAX_MSG_SIZE, stdin) == NULL) break;
        buf[strcspn(buf, "\n")] = '\0';

        int is_quit = (strcmp(buf, "quit") == 0);
        unsigned int send_pri;  

        if (is_quit) {
            send_pri = PRI_QUIT;     
        } else {
            send_pri = PRI_NORMAL;  
        }

        if (mq_send(mq_send_fd, buf, strlen(buf), send_pri) == -1) {
            perror("mq_send");
            break;
        }
        if (is_quit) break;

        ssize_t n = mq_receive(mq_recv_fd, buf, MAX_MSG_SIZE, &pri);
        if (n == -1) {
            perror("mq_receive");
            break;
        }
        buf[n] = '\0';

        if (pri == PRI_QUIT) {
            printf("[User 1] Собеседник завершил чат.\n");
            break;
        }
        printf("Собеседник: %s\n", buf);
    }

    mq_close(mq_send_fd);
    mq_close(mq_recv_fd);
    
    if (mq_unlink(MQ_P1_TO_P2) == -1 && errno != ENOENT) perror("mq_unlink p1");
    if (mq_unlink(MQ_P2_TO_P1) == -1 && errno != ENOENT) perror("mq_unlink p2");

    return 0;
}