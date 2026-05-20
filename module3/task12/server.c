#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 51000
#define MAX_CLIENTS 2

int main() {
    int sockfd;
    socklen_t clilen;
    int n;
    char line[1000];
    struct sockaddr_in servaddr, cliaddr;
    struct sockaddr_in clients[MAX_CLIENTS];
    int client_count = 0;
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror(NULL);
        exit(1);
    }
    
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    
    printf("Сервер запущен на порту %d\n", PORT);
    
    while (1) {
        clilen = sizeof(cliaddr);
        
        if ((n = recvfrom(sockfd, line, 999, 0, 
                         (struct sockaddr*)&cliaddr, &clilen)) < 0) {
            perror(NULL);
            close(sockfd);
            exit(1);
        }
        
        line[n] = '\0';
        
        int found = 0;
        for (int i = 0; i < client_count; i++) {
            if (clients[i].sin_port == cliaddr.sin_port &&
                clients[i].sin_addr.s_addr == cliaddr.sin_addr.s_addr) {
                found = 1;
                break;
            }
        }
        
        if (!found && client_count < MAX_CLIENTS) {
            clients[client_count] = cliaddr;
            printf("Клиент %d подключился: %s:%d\n", 
                   client_count + 1, 
                   inet_ntoa(cliaddr.sin_addr), 
                   ntohs(cliaddr.sin_port));
            client_count++;
        }
        
        printf("Получено сообщение: %s\n", line);
        for (int i = 0; i < client_count; i++) {
            if (clients[i].sin_port != cliaddr.sin_port ||
                clients[i].sin_addr.s_addr != cliaddr.sin_addr.s_addr) {
                
                if (sendto(sockfd, line, strlen(line), 0,
                          (struct sockaddr*)&clients[i], sizeof(clients[i])) < 0) {
                    perror("Ошибка отправки");
                }
            }
        }
    }
    
    close(sockfd);
    return 0;
}