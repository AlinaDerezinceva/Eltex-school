#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define SEM_NAME "/task09_file_sem"
#define FILE_NAME "data.txt"
#define TOTAL_LINES 10

int main() {
    sem_unlink(SEM_NAME);
    remove(FILE_NAME);

    sem_t *sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        int processed_lines = 0;
        int done = 0;

        while (!done) {
            sem_wait(sem); 

            FILE *fp = fopen(FILE_NAME, "r");
            if (fp != NULL) {
                char buffer[2048];
                int current_line = 0;

                while (fgets(buffer, sizeof(buffer), fp)) {
                    current_line++;
                    
                    if (current_line <= processed_lines) continue;
                    if (strncmp(buffer, "EXIT", 4) == 0) {
                        done = 1;
                        break;
                    }

                    int min = INT_MAX, max = INT_MIN;
                    int nums_found = 0;
                    char *token = strtok(buffer, " \t\n\r");
                    while (token != NULL) {
                        int val = atoi(token);
                        if (val < min) min = val;
                        if (val > max) max = val;
                        nums_found++;
                        token = strtok(NULL, " \t\n\r");
                    }

                    if (nums_found > 0) {
                        printf("[Дочерний] Строка %d: min = %d, max = %d\n", current_line, min, max);
                        fflush(stdout); 
                    }
                }
                processed_lines = current_line;
                fclose(fp);
            }
            sem_post(sem); 
            if (!done) sleep(1);
        }
        sem_close(sem);
        exit(0);
    } else {
        srand((unsigned)time(NULL));

        for (int i = 0; i < TOTAL_LINES; i++) {
            int count = rand() % 8 + 3;
            char line[1024] = "";
            for (int j = 0; j < count; j++) {
                char num[20];
                sprintf(num, "%d ", rand() % 200 - 100); 
                strcat(line, num);
            }

            sem_wait(sem); 
            FILE *fp = fopen(FILE_NAME, "a");
            if (fp != NULL) {
                fprintf(fp, "%s\n", line);
                fclose(fp);
            }
            sem_post(sem); 

            printf("[Родительский] Записана строка %d (%d чисел)\n", i + 1, count);
            sleep(1); 
        }

        sem_wait(sem);
        FILE *fp = fopen(FILE_NAME, "a");
        if (fp != NULL) {
            fprintf(fp, "EXIT\n");
            fclose(fp);
        }
        sem_post(sem);

        wait(NULL); 
        sem_close(sem);
        sem_unlink(SEM_NAME);
        printf("[Родительский] Все процессы завершены.\n");
    }

    return 0;
}