#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/utils.h"
#include "../../include/common.h"
#include "../../include/memory/process.h"
#include "../../include/network/dsm_mpi.h"

void dsm_mpi_process_requests()
{
    MPI_Status status;
    int flag = 0;

    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);

    if (!flag)
        return;

    DsmMsg req;
    MPI_Recv(&req, sizeof(DsmMsg), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    if (req.opcode == OP_READ_REQ)
    {
        char resp_data[req.size]; // Response data buffer
        memory_read(req.position, req.size, resp_data);

        MPI_Send(resp_data, req.size, MPI_BYTE, status.MPI_SOURCE, OP_READ_RESP, MPI_COMM_WORLD);
    }
    else if (req.opcode == OP_WRITE_REQ)
        memory_write(req.position, req.size, req.data);

    else if (req.opcode == OP_INVALIDATE)
        cache_invalidate(process_get()->cache, req.position / DSM_BLOCK_SIZE);
}

void dsm_mpi_init(int *argc, char **argv)
{
    MPI_Init(argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &DSM_PROCESS_COUNT);
}

void dsm_mpi_finalize()
{
    // Finalize MPI environment
    MPI_Finalize();
}
