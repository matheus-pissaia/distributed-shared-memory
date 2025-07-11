#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "memory.h"
#include "network.h"

int main(int argc, char **argv)
{
    comm_init(&argc, &argv);
    Process *local_process = process_init();

    if (local_process->rank_id == 0)
    {
        const char *text = "Hello World!";
        int len = strlen(text) + 1; // Include '\0'

        DsmMsg msg;
        msg.opcode = OP_WRITE_REQ;
        msg.position = 32;
        msg.size = len;
        memcpy(msg.data, text, len);

        MPI_Send(&msg, sizeof(DsmMsg), MPI_BYTE, 1, OP_WRITE_REQ, MPI_COMM_WORLD);

        sleep(4);

        char resp_data[msg.size];
        msg.opcode = OP_READ_REQ;

        puts("Sending read request...");

        MPI_Send(&msg, sizeof(DsmMsg), MPI_BYTE, 1, OP_READ_REQ, MPI_COMM_WORLD);

        MPI_Status status;
        MPI_Recv(&resp_data, msg.size, MPI_BYTE, 1, OP_READ_RESP, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == DSM_INVALID_PARAMS)
            printf("Invalid position or size sended to read data\n");

        printf("Received: %s\n", resp_data);
    }
    else
    {
        while (1)
        {
            comm_process_requests();
            sleep(1);
        }
    }

    comm_finalize();

    return 0;
}
