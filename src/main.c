#include <stdio.h>
#include <unistd.h>

#include "../include/memory/process.h"
#include "../include/network/dsm_mpi.h"
#include "../include/network/dsm_socket.h"

int main(int argc, char **argv)
{
    dsm_mpi_init(&argc, argv);
    process_init();

    dsm_socket_init();

    while (1)
    {
        dsm_mpi_process_requests();
        dsm_socket_process_requests();
        sleep(1);
    }

    return 0;
}
