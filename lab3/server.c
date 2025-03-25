
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
#define BUFFER_SIZE 1024

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

        ssize_t received = recvfrom(sock, buffer, sizeof(buffer), 0,
                                    (struct sockaddr *)&client_addr, &addr_len);

        buffer[received] = '\0';

        if (received < 0)
        {
            perror("recvfrom");
            continue;
        }
        else if (received > BUFFER_SIZE)
        {
            printf("Received message too long\n");
            strncpy(response, "ERROR\n", sizeof(response));
        }
        else
        {
            //clean text in buffer and checks for spaces at the end
            size_t buflen = strlen(buffer);
            size_t len = strlen(buffer);
            while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r' || buffer[len - 1] == '\t'))
            {
                buffer[len - 1] = '\0';
                len--;
            }

            if (len > 0 && buffer[len-1] == ' ')
            {
                strncpy(response, "ERROR\n", sizeof(response));
            }else{
                count_palindromes(buffer, response);
            }

            printf("Received: %s\n", buffer);

            
        }

        sendto(sock, response, strlen(response), 0, (struct sockaddr *)&client_addr, addr_len);
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