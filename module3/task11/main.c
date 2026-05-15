#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>
#include "structures.h"

#define SHM_NAME "/task11_posix_shm"

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

    shm_unlink(SHM_NAME);

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    if (ftruncate(shm_fd, sizeof(SharedData)) == -1) {
        perror("ftruncate");
        close(shm_fd);
        return 1;
    }

    SharedData *shm = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        return 1;
    }

    close(shm_fd);
    shm_unlink(SHM_NAME);

    shm->processed_count = 0;
    shm->ready_for_child = 0;
    shm->ready_for_parent = 0;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        munmap(shm, sizeof(SharedData));
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
        munmap(shm, sizeof(SharedData));
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

        munmap(shm, sizeof(SharedData));
    }

    return 0;
}