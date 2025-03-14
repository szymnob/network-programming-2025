
#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>

#define BUFFER_SIZE 1024
#define MSG "Hello, world!\r\n"

int main(int argc, char *argv[])
{
    if(argc != 3){
        printf("Required address and port\n");
        exit(1);
    }

    const char *serv_addr = argv[1];
    int port = atoi(argv[2]);

    int sock;
    int rc;
    ssize_t cnt;


    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port)
    };

    if (inet_pton(AF_INET, serv_addr, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);

    if (sendto(sock, NULL, 0, 0, (struct sockaddr *)&server_addr, addr_len) < 0) {
        perror("sendto");
        close(sock);
        exit(1);
    }

    printf("Sent empty datagram\n");

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))){
        perror("Error");
        close(sock);
        exit(1);
    }    

    ssize_t received = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);

    if(received < 0){
        perror("recvfrom");
        close(sock);
        exit(1);
    }

    for (ssize_t i = 0; i < received-1; i++) {
        if (isprint(buffer[i]) || buffer[i] == '\n' || buffer[i] == '\r' || buffer[i] == '\t') {
            putchar(buffer[i]);
        } else {
            putchar('?');
        }
    }
    printf("\n");

    close(sock);

    exit(0);

}