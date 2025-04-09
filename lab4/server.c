
#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>

#include "palindrome.h"

#define PORT 2020
#define BUFFER_SIZE 1024


//returns 1 if there is a null character in the buffer, 0 otherwise
int has_nulls(const char *buffer, ssize_t length){
    for (ssize_t i = 0; i < length; i++) {
        if (buffer[i] == '\0') {
            return 1;
        }
    }
    return 0;
}

//add '\r\n' to the response, print response and send it to the client
int send_response(int fd, char *response)
{
    strncat(response, "\r\n", sizeof(response) - strlen(response) - 1);
    printf("Response: %s", response);
    ssize_t sent = send(fd, response, strlen(response), 0);
    if (sent == -1)
    {
        perror("send");
        return -1;
    }
    return 0;
}

int main()
{

    int srv_sock;
    int rc;

    srv_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (srv_sock == -1)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr = {.s_addr = htonl(INADDR_ANY)},
        .sin_port = htons(PORT)};

    rc = bind(srv_sock, (struct sockaddr *)&addr, sizeof(addr));
    if (rc == -1)
    {
        perror("bind");
        exit(1);
    }

    if (listen(srv_sock, 10) < 0)
    {
        perror("listen");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    fd_set sock_fds, read_fds;
    FD_ZERO(&sock_fds);
    FD_SET(srv_sock, &sock_fds);
    int max_fd = srv_sock;

    char client_buffer[FD_SETSIZE][BUFFER_SIZE*2] = {{0}};
    char response[RESPONSE_SIZE];

    //data receved from client
    char temp_buf[BUFFER_SIZE] = {0};
    //buffer to store the line to be processed
    char line[BUFFER_SIZE] = {0};

    while (1)
    {
        memset(response, 0, sizeof(response));

        read_fds = sock_fds;

        int num = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (num == -1)
        {
            perror("select");
            break;
        }

        for (int fd = 0; fd <= max_fd; ++fd)
        {
            if (!FD_ISSET(fd, &read_fds))
            {
                continue;
            }

            // new client
            if (fd == srv_sock)
            {
                int client_sock = accept(srv_sock, NULL, NULL);
                if (client_sock < 0)
                {
                    perror("accept");
                    for (int fd = 0; fd <= max_fd; ++fd)
                    {
                        if (FD_ISSET(fd, &sock_fds) && fd != srv_sock)
                        {
                            close(fd);
                        }
                    }
                    exit(1);
                }
                if (client_sock >= FD_SETSIZE)
                {
                    fprintf(stderr, "Too many clients\n");
                    close(client_sock);
                    continue;
                }
                FD_SET(client_sock, &sock_fds);
                if (client_sock > max_fd)
                {
                    max_fd = client_sock;
                }
            }
            // handling connected client
            else
            {
                memset(temp_buf, 0, sizeof(temp_buf));
                char *buf = client_buffer[fd];
                ssize_t received = recv(fd, temp_buf, sizeof(temp_buf) - 1, 0);
                if (received < 0)
                {
                    perror("recv");
                    close(fd);
                    FD_CLR(fd, &sock_fds);
                    memset(buf, 0, BUFFER_SIZE);
                    continue;
                }
                else if (received == 0)
                {
                    close(fd);
                    FD_CLR(fd, &sock_fds);
                    continue;
                }
                temp_buf[received] = '\0'; // null termintator buffer

                if(has_nulls(temp_buf, received))
                {
                    strncpy(response, "ERROR", sizeof(response));
                    send_response(fd, response);
                    continue;
                }

                strncat(buf, temp_buf, BUFFER_SIZE - strlen(buf) - 1);

                char *end;
                while ((end = strstr(buf, "\r\n")) != NULL)
                {
                    memset(line, 0, sizeof(line));
                    *end = '\0';
                    strncpy(line, buf, sizeof(line) - 1); // copy part of the buffer termineted by \r\n

                    size_t len = strlen(line);
                    if ((len > 0 && line[len - 1] == ' ') || line[0] == ' ')
                    {
                        strncpy(response, "ERROR", sizeof(response));
                    }
                    else
                    {
                        // count palindromes in the line
                        count_palindromes(line, response);
                    }
                    
                    send_response(fd, response);

                    // move the rest of the buffer after processing the line
                    char *rest = end + 2;
                    int i = 0;
                    while (*rest != '\0')
                    {
                        buf[i++] = *rest++;
                    }
                    buf[i] = '\0';
                }
            }
        }
    }

    rc = close(srv_sock);
    if (rc == -1)
    {
        perror("close");
        return 1;
    }

    return 0;
}