
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

    int lst_sock;
    int rc;
    int clnt_sock;


    if(argc != 2){
        printf("Port number is required\n");
        exit(1);
    }

    int port = atoi(argv[1]);

    lst_sock = socket(AF_INET, SOCK_STREAM, 0);

    if(lst_sock == -1){
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr = { .s_addr = htonl(INADDR_ANY) },
        .sin_port = htons(port)
    };

    rc = bind(lst_sock, (struct sockaddr *) & addr, sizeof(addr));
    if (rc == -1) {
        perror("bind");
        return 1;
    }

    rc = listen(lst_sock, 10);
    if (rc == -1) {
        perror("listen");
        return 1;
    }

    printf("Server listening on port %d...\n", port);

    while(1){
        clnt_sock = accept(lst_sock, NULL, NULL);
        if(clnt_sock == -1){
            perror("accept");
            exit(1);
        }

        printf("Client connected\n");

        send(clnt_sock, MSG, sizeof(MSG), 0);
        rc = close(clnt_sock);

        if(rc == -1){
            perror("close");
            exit(1);
        }
        printf("Client disconnected\n");
    
    }

    rc = close(lst_sock);
    if (rc == -1) {
        perror("close");
        return 1;
    }

    return 0;

}