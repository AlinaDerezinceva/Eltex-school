#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>


#define SEM_MUTEX 0
#define SEM_ITEMS 1

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <файл_данных>\n", argv[0]);
        return 1;
    }
    char *filename = argv[1];

    int tmp_fd = open(filename, O_CREAT | O_RDWR, 0666);
    if (tmp_fd == -1) { perror("open"); return 1; }
    close(tmp_fd);

    key_t key = ftok(filename, 'S');
    if (key == -1) { perror("ftok"); return 1; }

    int semid = semget(key, 2, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1) {
        if (errno == EEXIST) {
            semid = semget(key, 2, 0666);
        } else {
            perror("semget"); return 1;
        }
    } else {
        union semun arg;
        arg.val = 1; semctl(semid, SEM_MUTEX, SETVAL, arg);
        arg.val = 0; semctl(semid, SEM_ITEMS,  SETVAL, arg);
    }

    srand(time(NULL) ^ getpid());
    FILE *f = fopen(filename, "a");
    if (!f) { perror("fopen"); return 1; }

    for (int i = 0; i < 15; ++i) {
        int count = (rand() % 8) + 3; 
        char line[256] = "";
        for (int j = 0; j < count; ++j) {
            char buf[16];
            sprintf(buf, "%d ", rand() % 100);
            strcat(line, buf);
        }
        line[strlen(line) - 1] = '\n'; 
        struct sembuf lock = {SEM_MUTEX, -1, 0};
        semop(semid, &lock, 1);

        fputs(line, f);
        fflush(f);

        struct sembuf unlock = {SEM_MUTEX, 1, 0};
        semop(semid, &unlock, 1);

        struct sembuf signal = {SEM_ITEMS, 1, 0};
        semop(semid, &signal, 1);

        printf("[PROD] Добавлена строка %d: %s", i+1, line);
        usleep(500000 + rand() % 500000); 
    }

    fclose(f);
    printf("[PROD] Производитель завершил работу.\n");
    return 0;
}