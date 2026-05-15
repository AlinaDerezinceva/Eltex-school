#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <limits.h>
#include <errno.h>


#define SEM_MUTEX 0
#define SEM_ITEMS 1

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <файл_данных>\n", argv[0]);
        return 1;
    }
    char *filename = argv[1];
    char posname[256];
    snprintf(posname, sizeof(posname), "%s.pos", filename);

    key_t key = ftok(filename, 'S');
    if (key == -1) { perror("ftok"); return 1; }

    int semid = semget(key, 2, IPC_CREAT | 0666);
    if (semid == -1) { perror("semget"); return 1; }

    FILE *posf = fopen(posname, "r+");
    if (!posf) { 
        posf = fopen(posname, "w+"); 
        fprintf(posf, "0\n"); 
        fflush(posf); 
    }

    FILE *dataf = fopen(filename, "r");
    if (!dataf) { perror("fopen data"); return 1; }

    printf("[CONS] Потребитель (PID %d) запущен. Ожидание данных...\n", getpid());

    while (1) {
        struct sembuf wait_item = {SEM_ITEMS, -1, 0};
        if (semop(semid, &wait_item, 1) == -1) {
            if (errno == EINTR) continue;
            perror("semop wait"); break;
        }

        struct sembuf lock = {SEM_MUTEX, -1, 0};
        semop(semid, &lock, 1);

        long offset = 0;
        rewind(posf);
        fscanf(posf, "%ld", &offset);

        fseek(dataf, offset, SEEK_SET);
        char line[256];
        if (fgets(line, sizeof(line), dataf)) {
            int min = INT_MAX, max = INT_MIN, val;
            char *tok = strtok(line, " \n");
            while (tok) {
                val = atoi(tok);
                if (val < min) min = val;
                if (val > max) max = val;
                tok = strtok(NULL, " \n");
            }

            printf("[CONS PID=%d] Строка: min=%d, max=%d\n", getpid(), min, max);

            offset = ftell(dataf);
            rewind(posf);
            fprintf(posf, "%ld\n", offset);
            ftruncate(fileno(posf), ftell(posf));
            fflush(posf);
        }

        struct sembuf unlock = {SEM_MUTEX, 1, 0};
        semop(semid, &unlock, 1);
    }

    fclose(dataf);
    fclose(posf);
    return 0;
}