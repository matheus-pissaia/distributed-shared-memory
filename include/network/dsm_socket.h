#ifndef DSM_SOCKET_H
#define DSM_SOCKET_H

/**
 * @brief Initializes the socket environment to listen for external requests.
 *
 * NOTE: This function should only be called after `process_init()`
 */
int dsm_socket_init();

/**
 * @brief Processes client requests received through the socket.
 */
void dsm_socket_process_requests();

#endif
