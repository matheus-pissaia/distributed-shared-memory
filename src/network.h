#ifndef NETWORK_H
#define NETWORK_H

typedef enum
{
    MSG_READ_REQUEST,
    MSG_READ_RESPONSE,
    MSG_WRITE_REQUEST,
    MSG_WRITE_ACK,
    MSG_INVALIDATE,
} MessageType;

typedef struct
{
    MessageType type;   // Type of the message
    int block_id;       // ID of the block
    int position;       // Position in the block
    int size;           // Size of the data in bytes to read/write
    unsigned char data; // Data associated with the message
} Message;

/**
 * @brief Sends a message over a socket.
 *
 * @param socket Socket to send the message over
 * @param msg Pointer to the message to send
 *
 * @return 0 on success, -1 on failure
 */
int send_message(int socket, Message *msg);

/**
 * @brief Reads and handles a received message over a socket.
 *
 * The message is handled based on its type:
 * - MSG_READ_REQUEST: Request to read data from a block (read() from api)
 * - MSG_WRITE_REQUEST: Request to write data to a block (write() from api)
 * - MSG_INVALIDATE: Request to invalidate a block (invalidate cache)
 *
 * @param socket Socket to receive the message over
 * @param msg Pointer to the message to receive
 *
 * @return 0 on success, -1 on failure
 */
int receive_message(int socket, Message *msg);

#endif
