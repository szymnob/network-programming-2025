
#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


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

    sock = socket(AF_INET, SOCK_STREAM, 0);
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

    rc = connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (rc == -1) {
        perror("connect");
        close(sock);
        exit(1);
    }

    char buffer[1024];
    ssize_t received = read(sock, buffer, sizeof(buffer));

    if (received == -1) {
        perror("read");
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