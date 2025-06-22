#include <sys/socket.h>
#include "network.h"

int send_message(int socket, Message *msg)
{
    // TODO implement
    return 0;
}

int receive_message(int socket, Message *msg)
{
    // TODO implement
    return 0;
}

// ---------- Utility functions to read and write data from/to files ----------

/**
 * @brief Reads a specified number of bytes from a file descriptor into a buffer.
 *
 * @param fd File descriptor to read from
 * @param buffer Buffer to store the read data
 * @param length Number of bytes to read
 */
ssize_t read_n(int fd, void *buffer, size_t length)
{
    // TODO implement
    return 0;
}

/**
 * @brief Writes a specified number of bytes from a buffer to a file descriptor.
 *
 * @param fd File descriptor to write to
 * @param buffer Buffer to read data from
 * @param length Number of bytes to write
 */
ssize_t write_n(int fd, const void *buffer, size_t length)
{
    // TODO implement
    return 0;
}
