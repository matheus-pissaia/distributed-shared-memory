#ifndef NETWORK_H
#define NETWORK_H

typedef enum
{
    OP_READ_REQ = 100,
    OP_READ_RESP = 101,
    OP_WRITE_REQ = 200,
    OP_WRITE_RESP = 201,
    OP_INVALIDATE = 300,
} DsmOpCode;

typedef struct
{
    DsmOpCode opcode; // Operation code indicating the type
    int position;     // Absolute position in the memory
    int size;         // Size of the data to be read or written in bytes
    char data[];      // Data to be sent or received
} DsmMsg;

/**
 * @brief Initializes the communication layer.
 *
 * This function sets up the MPI environment and prepares the local memory.
 * It should be called before any other communication functions.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 */
void comm_init(int *argc, char ***argv);

/**
 * @brief Processes pending requests from other processes.
 */
void comm_process_requests();

/**
 * @brief Finalizes the communication layer.
 *
 * This function cleans up the MPI environment and releases resources.
 * It should be called at the end of the program.
 */
void comm_finalize();

#endif
