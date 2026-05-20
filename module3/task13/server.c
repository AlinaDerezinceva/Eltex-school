#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024
#define FILE_BUFFER_SIZE 4096

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int nclients = 0;

void printusers() {
    if(nclients) {
        printf("%d user on-line\n", nclients);
    } else {
        printf("No User on line\n");
    }
}

double calculate(double a, double b, char op) {
    switch(op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if(b != 0) return a / b;
            else return 0; 
        default: return 0;
    }
}

void handle_file_transfer(int sock, char *filename, int mode) {
    int file_fd;
    char buffer[FILE_BUFFER_SIZE];
    int bytes_read, bytes_sent;
    struct stat file_stat;
    long file_size;
    
    if(mode == 1) { 
        recv(sock, &file_size, sizeof(file_size), 0);
        
        file_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(file_fd < 0) {
            error("ERROR opening file for writing");
        }
        
        long total_received = 0;
        while(total_received < file_size) {
            bytes_read = recv(sock, buffer, FILE_BUFFER_SIZE, 0);
            if(bytes_read <= 0) break;
            if(write(file_fd, buffer, bytes_read) < 0) {
            perror("ERROR writing to file");
        }
            total_received += bytes_read;
        }
        
        close(file_fd);
        printf("File %s received successfully\n", filename);
        send(sock, "FILE_RECEIVED", strlen("FILE_RECEIVED"), 0);
        
    } else { 
        file_fd = open(filename, O_RDONLY);
        if(file_fd < 0) {
            send(sock, "FILE_NOT_FOUND", strlen("FILE_NOT_FOUND"), 0);
            return;
        }
        
        send(sock, "FILE_FOUND", strlen("FILE_FOUND"), 0);
        fstat(file_fd, &file_stat);
        file_size = file_stat.st_size;
        send(sock, &file_size, sizeof(file_size), 0);
        
        while((bytes_read = read(file_fd, buffer, FILE_BUFFER_SIZE)) > 0) {
            bytes_sent = send(sock, buffer, bytes_read, 0);
            if(bytes_sent < 0) {
                error("ERROR sending file");
            }
        }
        
        close(file_fd);
        printf("File %s sent successfully\n", filename);
    }
}

void dostuff(int sock) {
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    int bytes_recv;
    double a, b, result;
    char operation;

    const char *menu = "Operations:\n + - Addition\n - - Subtraction\n * - Multiplication\n / - Division\n FILE_SEND <filename> - Send file to server\n FILE_RECV <filename> - Receive file from server\n QUIT - Disconnect\n Enter operation: ";
    
    send(sock, menu, strlen(menu), 0);
    
    while(1) {
        memset(buffer, 0, BUFFER_SIZE);
        bytes_recv = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        
        if(bytes_recv <= 0) {
            if(bytes_recv == 0) {
                printf("Client disconnected\n");
            } else {
                error("ERROR reading from socket");
            }
            break;
        }
        
        buffer[strcspn(buffer, "\r\n")] = 0;
        if(strcmp(buffer, "QUIT") == 0) {
            send(sock, "Goodbye!", strlen("Goodbye!"), 0);
            break;
        }
        
        if(strncmp(buffer, "FILE_SEND", 9) == 0) {
            char *filename = buffer + 10;
            handle_file_transfer(sock, filename, 1);
            continue;
        }
        
        if(strncmp(buffer, "FILE_RECV", 9) == 0) {
            char *filename = buffer + 10; 
            handle_file_transfer(sock, filename, 2);
            continue;
        }
        
        if(sscanf(buffer, "%c %lf %lf", &operation, &a, &b) == 3) {
            if(operation == '/' && b == 0) {
                snprintf(response, BUFFER_SIZE, "Error: Division by zero!\n");
            } else {
                result = calculate(a, b, operation);
                snprintf(response, BUFFER_SIZE, "Result: %.2lf %c %.2lf = %.2lf\n", a, operation, b, result);
            }
        } else {
            snprintf(response, BUFFER_SIZE, "Invalid format! Use: <op> <num1> <num2>\nExample: + 5 3\n");
        }
        
        send(sock, response, strlen(response), 0);
    }
    
    nclients--;
    printf("-disconnect\n");
    printusers();
    close(sock);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd;
    int portno;
    int pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    struct hostent *hst;
    
    printf("TCP SERVER DEMO\n");
    
    if(argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) 
        error("ERROR opening socket");
        
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    
    printf("Server listening on port %d\n", portno);
    
    while(1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if(newsockfd < 0) 
            error("ERROR on accept");
        
        nclients++;
        
        hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
        printf("+%s [%s] new connect!\n", 
               (hst) ? hst->h_name : "Unknown host",
               inet_ntoa(cli_addr.sin_addr));
        printusers();
        
        pid = fork();
        if(pid < 0) 
            error("ERROR on fork");
        
        if(pid == 0) {
            close(sockfd);
            dostuff(newsockfd);
            exit(0);
        } else {    
            close(newsockfd);
        }
    }
    
    close(sockfd);
    return 0;
}