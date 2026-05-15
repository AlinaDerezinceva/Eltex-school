#define _POSIX_C_SOURCE 200809L 
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>
#include "structures.h"

static volatile sig_atomic_t interrupted = 0;

void handle_sigint(int sig) {
    (void)sig; 
    interrupted = 1;
}

int main(void) {
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    int shmid = shmget(IPC_PRIVATE, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    SharedData *shm = (SharedData *)shmat(shmid, NULL, 0);
    if (shm == (SharedData *)-1) {
        perror("shmat");
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    shm->processed_count = 0;
    shm->ready_for_child = 0;
    shm->ready_for_parent = 0;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    if (pid == 0) {
        while (!interrupted) {
            while (!shm->ready_for_child) {
                if (interrupted) break;
                usleep(1000);
            }
            if (interrupted) break;

            int min = shm->numbers[0];
            int max = shm->numbers[0];
            for (int i = 1; i < shm->count; i++) {
                if (shm->numbers[i] < min) min = shm->numbers[i];
                if (shm->numbers[i] > max) max = shm->numbers[i];
            }

            shm->min_val = min;
            shm->max_val = max;
            shm->ready_for_parent = 1;
        }

        shmdt(shm);
        exit(0);
    } else {
        srand((unsigned int)time(NULL));

        while (!interrupted) {
            int count = rand() % 20 + 1;
            for (int i = 0; i < count; i++) {
                shm->numbers[i] = rand() % 1000; 
            }
            shm->count = count;
            shm->ready_for_child = 1;

            while (!shm->ready_for_parent) {
                if (interrupted) break;
                usleep(1000);
            }
            if (interrupted) break;

            printf("Набор #%d: количество=%d, min=%d, max=%d\n",
                   shm->processed_count + 1, count, shm->min_val, shm->max_val);

            shm->processed_count++;

            shm->ready_for_parent = 0;
            shm->ready_for_child = 0;
        }

        while (wait(NULL) == -1 && errno == EINTR);

        printf("\nВсего обработано наборов: %d\n", shm->processed_count);

        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}