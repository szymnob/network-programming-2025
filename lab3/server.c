
#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "palindrome.h"

#define PORT 2020
#define BUFFER_SIZE 1025 // 1024 + 1 for null terminator

int has_nulls(const char *buffer, ssize_t received){
    for (ssize_t i = 0; i < received; i++) {
        if (buffer[i] == '\0') {
            return 1;
        }
    }
    return 0;
}

int main()
{

    int sock;
    int rc;
    char buffer[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock == -1)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr = {.s_addr = htonl(INADDR_ANY)},
        .sin_port = htons(PORT)};

    rc = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (rc == -1)
    {
        perror("bind");
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    char response[RESPONSE_SIZE];

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        memset(response, 0, sizeof(response));

        ssize_t received = recvfrom(sock, buffer, sizeof(buffer)-1, 0,
                                    (struct sockaddr *)&client_addr, &addr_len);

        if (received < 0)
        {
            perror("recvfrom");
            break;
        }

        if(has_nulls(buffer, received)){
            //\0 in request
            strncpy(response, "ERROR", sizeof(response));
        }else
        {
            //clean text in buffer and checks for spaces at the end
            size_t len = strlen(buffer);

            if(len>0 && buffer[len-1] == '\n'){
                buffer[len-1] = '\0';
                len--;
            }

            //space at the end or beginning
            if ((len > 0 && buffer[len-1] == ' ') || (len > 0 && buffer[0] == ' '))
            {
                strncpy(response, "ERROR", sizeof(response));
            }
            else{
                count_palindromes(buffer, response);
            }

            printf("Received: %s\n", buffer);
            
        }

        ssize_t send = sendto(sock, response, strlen(response), 0, (struct sockaddr *)&client_addr, addr_len);
        if (send < 0)
        {
            perror("sendto");
            continue;
        }
        printf("Sent: %s\n", response);
    }

    rc = close(sock);
    if (rc == -1)
    {
        perror("close");
        return 1;
    }

    return 0;
}