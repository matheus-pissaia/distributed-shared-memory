#ifndef DSM_API_H
#define DSM_API_H

typedef enum
{
    SOCKET_CREATION_ERROR = -1,
    SOCKET_CONNECTION_ERROR = -2
} ApiErrorCode;

/**
 * @brief Reads some data from the shared memory.
 *
 * First, it should try to read the block from the cache. If it fails, request
 * to the owner (process) the desired block and store it in the cache.
 *
 * @param position Initial position in memory from where to read
 * @param buffer Pointer to the variable that has the reading content
 * @param size Amount of bytes to read from the starting position
 *
 * @return
 */
int dsm_read(int position, char *buffer, int size);

/**
 * @brief Writes some data to the shared memory.
 *
 * The write operation is sent to the owner (process) of the block, which will
 * update the data and respond with a confirmation status. Then, a message is
 * sent to all processes that have the block in their cache to invalidate it.
 *
 * @param position Initial position in memory from where to write
 * @param buffer Pointer to the variable that has the writing content
 * @param size Amount of bytes to write from the starting position
 *
 * @return
 */
int dsm_write(int position, char *buffer, int size);

#endif
