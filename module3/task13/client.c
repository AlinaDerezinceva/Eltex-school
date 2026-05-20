#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024
#define FILE_BUFFER_SIZE 4096

void error(const char *msg) {
    perror(msg);
    exit(0);
}

void handle_file_transfer(int my_sock, char *filename, int mode) {
    char buffer[FILE_BUFFER_SIZE];
    char response[20];
    int file_fd, bytes_read, bytes_recv;
    long file_size, total_sent;
    
    if(mode == 1) {
        file_fd = open(filename, O_RDONLY);
        if(file_fd < 0) {
            printf("Error: Cannot open file %s\n", filename);
            return;
        }
        
        struct stat file_stat;
        fstat(file_fd, &file_stat);
        file_size = file_stat.st_size;
        send(my_sock, &file_size, sizeof(file_size), 0);
        
        total_sent = 0;
        while((bytes_read = read(file_fd, buffer, FILE_BUFFER_SIZE)) > 0) {
            send(my_sock, buffer, bytes_read, 0);
            total_sent += bytes_read;
        }
        
        close(file_fd);
        recv(my_sock, response, sizeof(response), 0);
        printf("Server response: %s\n", response);
        
    } else {
        recv(my_sock, response, sizeof(response), 0);
        response[19] = '\0';
        
        if(strcmp(response, "FILE_NOT_FOUND") == 0) {
            printf("Error: File not found on server\n");
            return;
        }
        
        recv(my_sock, &file_size, sizeof(file_size), 0);
        
        file_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(file_fd < 0) {
            error("ERROR opening file for writing");
        }
        
        total_sent = 0;
        while(total_sent < file_size) {
            bytes_recv = recv(my_sock, buffer, FILE_BUFFER_SIZE, 0);
            if(bytes_recv <= 0) break;
            if(write(file_fd, buffer, bytes_recv) < 0) {
            perror("ERROR writing to file");
        }
            total_sent += bytes_recv;
        }
        
        close(file_fd);
        printf("File %s received successfully (%ld bytes)\n", filename, file_size);
    }
}

int main(int argc, char *argv[]) {
    int my_sock, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];
    
    printf("TCP DEMO CLIENT\n");
    
    if(argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    
    portno = atoi(argv[2]);
    
    my_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(my_sock < 0) 
        error("ERROR opening socket");
    
    server = gethostbyname(argv[1]);
    if(server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if(connect(my_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    printf("Connected to server %s:%d\n", argv[1], portno);
    
    while(1) {
        memset(buffer, 0, BUFFER_SIZE);
        n = recv(my_sock, buffer, BUFFER_SIZE - 1, 0);
        
        if(n <= 0) {
            if(n == 0) {
                printf("Server disconnected\n");
            } else {
                printf("Recv error\n");
            }
            break;
        }
        
        printf("%s", buffer);
        
        printf("> ");
        memset(buffer, 0, BUFFER_SIZE);
        if(fgets(buffer, BUFFER_SIZE - 1, stdin) == NULL) {
            break;
        }
        
        buffer[strcspn(buffer, "\n")] = 0;
        
        if(strcmp(buffer, "quit") == 0 || strcmp(buffer, "QUIT") == 0) {
            send(my_sock, "QUIT", 4, 0);
            printf("Exit...\n");
            close(my_sock);
            return 0;
        }
        
        if(strncmp(buffer, "FILE_SEND", 9) == 0) {
            char *filename = buffer + 10;
            send(my_sock, buffer, strlen(buffer), 0);
            handle_file_transfer(my_sock, filename, 1);
            continue;
        }
        
        if(strncmp(buffer, "FILE_RECV", 9) == 0) {
            char *filename = buffer + 10;
            send(my_sock, buffer, strlen(buffer), 0);
            handle_file_transfer(my_sock, filename, 2);
            continue;
        }
        
        send(my_sock, buffer, strlen(buffer), 0);
    }
    
    close(my_sock);
    return 0;
}