#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "api.h"
#include "../include/common.h"

// Connect to the DSM server
static int dsm_connect(int *sock_fd)
{
    struct sockaddr_in server_end;

    server_end.sin_family = AF_INET;
    server_end.sin_addr.s_addr = inet_addr(DSM_SOCKET_HOST);
    server_end.sin_port = htons(DSM_SOCKET_DEFAULT_PORT);

    if ((*sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return SOCKET_CREATION_ERROR;

    if (connect(*sock_fd, (struct sockaddr *)&server_end, sizeof(server_end)) == -1)
        return SOCKET_CONNECTION_ERROR;

    return 0;
}

int dsm_read(int position, char *buffer, int size)
{
    int sock_fd;
    char request[DSM_SOCKET_MAX_BUFFER];

    dsm_connect(&sock_fd);
    sprintf(request, "%s %d %d", READ_OP, position, size);
    write(sock_fd, &request, strlen(request));

    char response[size];
    int bytes = read(sock_fd, &response, size);
    close(sock_fd);

    // TODO handle error on read
    memcpy(buffer, response, bytes);

    return 0;
}

int dsm_write(int position, char *buffer, int size)
{
    int sock_fd;
    char request[DSM_SOCKET_MAX_BUFFER];

    dsm_connect(&sock_fd);
    sprintf(request, "%s %d %d %s", WRITE_OP, position, size, buffer);
    write(sock_fd, &request, strlen(request));

    char response[DSM_SOCKET_MAX_BUFFER];
    read(sock_fd, &response, DSM_SOCKET_MAX_BUFFER);
    close(sock_fd);

    // return get_status(response);
    return 0;
}
