#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../../include/utils.h"
#include "../../include/common.h"
#include "../../include/memory/process.h"
#include "../../include/network/dsm_socket.h"

static struct pollfd clients[DSM_SOCKET_MAX_CLIENTS];

static void dsm_socket_process_new_client()
{
    int server_fd = clients[0].fd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

    if (client_fd < 0)
    {
        printf("[PROCESS %d] Error while accepting new client\n", process_get()->rank_id);
        return;
    }

    int available_pos = -1;

    for (int i = 1; i < DSM_SOCKET_MAX_CLIENTS; i++)
    {
        if (clients[i].fd < 0)
        {
            available_pos = i;
            break;
        }
    }

    if (available_pos == -1)
    {
        char *msg = "Maximum number of clients reached!";
        write(client_fd, msg, strlen(msg));
        close(client_fd);
    }
    else
    {
        clients[available_pos].fd = client_fd;
        clients[available_pos].events = POLLIN;
    }
}

// Initialize socket environment to listen for external requests
int dsm_socket_init()
{
    int rank_id = process_get()->rank_id;

    int sock_fd;
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(DSM_SOCKET_HOST);
    server_addr.sin_port = htons(DSM_SOCKET_DEFAULT_PORT + rank_id);

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("[PROCESS %d] Error while creating socket\n", rank_id);
        exit(EXIT_FAILURE);
    }

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("[PROCESS %d] Error while binding socket\n", rank_id);
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(sock_fd, 10) < 0)
    {
        printf("[PROCESS %d] Error while listening\n", rank_id);
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < DSM_SOCKET_MAX_CLIENTS; i++)
        clients[i].fd = -1;

    clients[0].fd = sock_fd;
    clients[0].events = POLLIN;

    return 0;
}

void dsm_socket_process_requests()
{
    if (poll(clients, DSM_SOCKET_MAX_CLIENTS, -1) == -1)
    {
        printf("[PROCESS %d] Error while polling\n", process_get()->rank_id);
        close(clients[0].fd);
        exit(EXIT_FAILURE);
    }

    // Check if a new client has arrived
    if (clients[0].revents & POLLIN)
        dsm_socket_process_new_client();

    for (int i = 1; i < DSM_SOCKET_MAX_CLIENTS; i++)
    {
        int bytes_recv;
        char req_data[DSM_SOCKET_MAX_BUFFER];
        char resp_data[DSM_SOCKET_MAX_BUFFER];

        if (clients[i].fd >= 0 && (clients[i].revents & POLLIN))
        {
            bytes_recv = read(clients[i].fd, &req_data, DSM_SOCKET_MAX_BUFFER);

            if (bytes_recv == 0)
            {
                close(clients[i].fd);
                clients[i].fd = -1;
                continue;
            }

            req_data[bytes_recv] = '\0';
            printf("[PROCESS %d] Message received: %s\n", process_get()->rank_id, req_data);

            if (strstr(req_data, "READ") != NULL)
            {
                int position = atoi(strtok(req_data + strlen("READ"), " "));
                int size = atoi(strtok(NULL, " "));

                memory_read(position, size, resp_data);
                write(clients[i].fd, &resp_data, strlen(resp_data));
                continue;
            }

            if (strstr(req_data, "WRITE") != NULL)
            {
                int position = atoi(strtok(req_data + strlen("WRITE"), " "));
                int size = atoi(strtok(NULL, " "));
                char *content = strtok(NULL, "");

                memory_write(position, size, content);
                write(clients[i].fd, &resp_data, strlen(resp_data));
                continue;
            }
        }
    }
}
