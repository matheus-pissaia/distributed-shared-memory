#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "api.h"
#include "network.h"

// Initialize the MPI communication by connecting to the DSM service
static int api_comm_init(MPI_Comm *comm)
{
    MPI_Init(NULL, NULL);

    char port_name;

    if (MPI_Lookup_name("dsm_service", MPI_INFO_NULL, &port_name) != MPI_SUCCESS)
    {
        fprintf(stderr, "Failed to lookup service name\n");
        return -1;
    }

    return MPI_Comm_connect(&port_name, MPI_INFO_NULL, 0, MPI_COMM_WORLD, comm);
}

// Ends the MPI communication by disconnecting and finalizing MPI
static void api_comm_finalize(MPI_Comm *comm)
{
    MPI_Comm_disconnect(comm);
    MPI_Finalize();
}

int dsm_read(int position, char *buffer, int size)
{
    MPI_Comm intercomm;

    if (api_comm_init(&intercomm) != MPI_SUCCESS)
    {
        fprintf(stderr, "Failed to initialize communication\n");
        return -1;
    }

    DsmMsg msg = {.opcode = OP_READ_REQ, .position = position, .size = size};

    MPI_Send(&msg, sizeof(DsmMsg), MPI_BYTE, 0, OP_READ_REQ, MPI_COMM_WORLD);
    MPI_Recv(buffer, size, MPI_BYTE, 0, OP_READ_RESP, intercomm, MPI_STATUS_IGNORE);

    api_comm_finalize(&intercomm);

    return 0;
}

int dsm_write(int position, char *buffer, int size)
{
    MPI_Comm intercomm;

    if (api_comm_init(&intercomm) != MPI_SUCCESS)
    {
        fprintf(stderr, "Failed to initialize communication\n");
        return -1;
    }

    DsmMsg msg;
    msg.opcode = OP_WRITE_REQ;
    msg.position = position;
    msg.size = size;
    memcpy(msg.data, buffer, size);

    MPI_Send(&msg, sizeof(DsmMsg), MPI_BYTE, 0, OP_WRITE_REQ, MPI_COMM_WORLD);
    // MPI_Recv to check write status?

    api_comm_finalize(&intercomm);

    return 0;
}

const char *api_error_str(ApiErrorCode err)
{
    switch (err)
    {
    case API_SUCCESS:
        return "Success";
    case API_ERR_INVALID_PARAMS:
        return "Invalid parameters (position < 0 or size <= 0)";
    case API_ERR_BLOCK_BOUNDARY:
        return "Access crosses block boundary";
    case API_ERR_REMOTE_READ:
        return "Failed to read from remote block";
    case API_ERR_CACHE_MISS:
        return "Block not found in cache";
    case API_ERR_BLOCK_NOT_LOCAL:
        return "Block is not owned by this process";
    default:
        return "Unknown error";
    }
}
