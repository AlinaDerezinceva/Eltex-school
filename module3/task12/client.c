#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>

#define PORT 51000
#define BUFFER_SIZE 1000

int main(int argc, char** argv) {
    int sockfd;
    int n;
    socklen_t len;
    char sendline[BUFFER_SIZE], recvline[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    fd_set readfds;
    
    if (argc != 2) {
        printf("Использование: %s <IP адрес сервера>\n", argv[0]);
        exit(1);
    }
    
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror(NULL);
        exit(1);
    }
    
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(0);
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) < 0) {
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    
    if (inet_aton(argv[1], &servaddr.sin_addr) == 0) {
        printf("Неверный IP адрес\n");
        close(sockfd);
        exit(1);
    }
    
    printf("Подключено к серверу %s:%d\n", argv[1], PORT);
    printf("Введите сообщение (или 'exit' для выхода):\n");
    
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);
        
        if (select(sockfd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }
        
        if (FD_ISSET(sockfd, &readfds)) {
            len = sizeof(servaddr);
            if ((n = recvfrom(sockfd, recvline, BUFFER_SIZE - 1, 0,
                             (struct sockaddr*)&servaddr, &len)) < 0) {
                perror(NULL);
                close(sockfd);
                exit(1);
            }
            recvline[n] = '\0';
            printf("\n[Получено]: %s\n> ", recvline);
            fflush(stdout);
        }
        
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            if (fgets(sendline, BUFFER_SIZE, stdin) == NULL) {
                break;
            }
            
            sendline[strcspn(sendline, "\n")] = 0;
            
            if (strcmp(sendline, "exit") == 0) {
                break;
            }
            
            if (sendto(sockfd, sendline, strlen(sendline), 0,
                      (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
                perror(NULL);
                close(sockfd);
                exit(1);
            }
            printf("> ");
            fflush(stdout);
        }
    }
    
    close(sockfd);
    printf("Чат завершен\n");
    return 0;
}