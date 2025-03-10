
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

    int sock;
    int rc;
    char buffer[1024];


    if(argc != 2){
        printf("Port number is required\n");
        exit(1);
    }

    int port = atoi(argv[1]);

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if(sock == -1){
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr = { .s_addr = htonl(INADDR_ANY) },
        .sin_port = htons(port)
    };

    rc = bind(sock, (struct sockaddr *) & addr, sizeof(addr));
    if (rc == -1) {
        perror("bind");
        return 1;
    }

    printf("Server listening on port %d...\n", port);

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while(1){
        ssize_t received = recvfrom(sock, buffer, sizeof(buffer), 0,
                                    (struct sockaddr *)&client_addr, &addr_len);

        if (received < 0) {
            perror("recvfrom");
            continue;
        }

        printf("Otrzymano pusty datagram od %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        sendto(sock, MSG, strlen(MSG), 0, (struct sockaddr *)&client_addr, addr_len);
    
    }

    rc = close(sock);
    if (rc == -1) {
        perror("close");
        return 1;
    }

    return 0;

}