#include <mpi.h>
#include <stdlib.h>

#include "../../include/common.h"
#include "../../include/memory/process.h"

static Process *process; //  Internal variable to store the local process

// Initializes the vector of memory blocks for the local process.
static MemoryBlock *process_blocks_init()
{
    int blocks_per_process = DSM_BLOCK_COUNT / DSM_PROCESS_COUNT;
    int block_id_offset = process->rank_id * blocks_per_process;

    MemoryBlock *blocks = malloc(sizeof(MemoryBlock) * blocks_per_process);

    for (int i = 0; i < blocks_per_process; i++)
    {
        blocks[i] = *memory_block_init(i + block_id_offset, NULL);
    }

    return blocks;
}

int get_owner_from_block_id(int *block_id)
{
    return *block_id / (DSM_BLOCK_COUNT / DSM_PROCESS_COUNT);
}

Process *process_get()
{
    return process;
}

Process *process_init()
{
    process = malloc(sizeof(Process));

    MPI_Comm_rank(MPI_COMM_WORLD, &process->rank_id);

    process->cache = cache_init();
    process->blocks = process_blocks_init();

    return process;
}

MemoryBlock *process_block_get(int block_id)
{
    int blocks_per_process = DSM_BLOCK_COUNT / DSM_PROCESS_COUNT;

    for (int i = 0; i < blocks_per_process; i++)
    {
        if (process->blocks[i].id == block_id)
            return &process->blocks[i];
    }

    return NULL;
}
