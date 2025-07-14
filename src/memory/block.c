#include <mpi.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/common.h"
#include "../../include/memory/block.h"
#include "../../include/memory/process.h"
#include "../../include/network/dsm_mpi.h"

static void broadcast_invalidate(int block_id)
{
    Process *local_process = process_get();

    for (int p_rank = 0; p_rank < DSM_PROCESS_COUNT; p_rank++)
    {
        if (p_rank == local_process->rank_id)
            continue;

        DsmMsg msg = {
            .opcode = OP_INVALIDATE,
            .position = block_id * DSM_BLOCK_SIZE,
        };

        MPI_Send(&msg, sizeof(DsmMsg), MPI_BYTE, p_rank, OP_INVALIDATE, MPI_COMM_WORLD);
    }
}

MemoryBlock *memory_block_init(int block_id, char *data)
{
    MemoryBlock *block = malloc(sizeof(MemoryBlock));

    block->id = block_id;
    block->owner_id = get_owner_from_block_id(&block_id);
    block->data = calloc(DSM_BLOCK_SIZE, sizeof(char));

    if (data != NULL)
    {
        int data_len = strlen(data);
        int copy_size = (data_len < DSM_BLOCK_SIZE) ? data_len : DSM_BLOCK_SIZE - 1;
        memcpy(block->data, data, copy_size);
        block->data[copy_size] = '\0';
    }
    return block;
}

void memory_block_free(MemoryBlock *block)
{
    if (block != NULL)
    {
        if (block->data != NULL)
            free(block->data);

        free(block);
    }
}

MemoryBlock *memory_block_get(int *block_id)
{
    Process *local_process = process_get();
    int owner_rank_id = get_owner_from_block_id(block_id);

    // If the block is owned by the local process, return it
    if (owner_rank_id == local_process->rank_id)
        return process_block_get(*block_id);

    CacheEntry *cache_entry = cache_get(local_process->cache, *block_id);

    // If the block is in the cache, return it
    if (cache_entry != NULL && cache_entry->valid)
        return cache_entry->mem_block;

    // The block is not in the cache, so we must fetch it from the owner process
    return memory_block_fetch(block_id);
}

MemoryBlock *memory_block_fetch(int *block_id)
{
    Process *local_process = process_get();

    DsmMsg msg;
    msg.opcode = OP_READ_REQ;
    msg.position = *block_id * DSM_BLOCK_SIZE; // Start position from block ID
    msg.size = DSM_BLOCK_SIZE;                 // Read the entire block

    char block_data[DSM_BLOCK_SIZE]; // Buffer to hold the block data response
    int owner_rank_id = get_owner_from_block_id(block_id);

    MPI_Send(&msg, sizeof(DsmMsg), MPI_BYTE, owner_rank_id, OP_READ_REQ, MPI_COMM_WORLD);
    MPI_Recv(&block_data, DSM_BLOCK_SIZE, MPI_BYTE, owner_rank_id, OP_READ_RESP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    CacheEntry *new_entry = cache_set(local_process->cache, *block_id, block_data);

    return new_entry->mem_block;
}

void memory_block_set(int position, int offset, const char *data, int size)
{
    int block_id = position / DSM_BLOCK_SIZE;

    Process *local_process = process_get();
    int owner_rank_id = get_owner_from_block_id(&block_id);

    // If the block is owned by the local process, set it directly
    if (owner_rank_id == local_process->rank_id)
    {
        MemoryBlock *block = process_block_get(block_id);

        memcpy(block->data + offset, data, size);
        broadcast_invalidate(block->id);
        return;
    }

    // Send a write request to the owner
    DsmMsg msg;
    msg.opcode = OP_WRITE_REQ;
    msg.position = position;
    msg.size = size;              // Write the entire block
    memcpy(msg.data, data, size); // Copy the data to be written

    MPI_Send(&msg, sizeof(DsmMsg), MPI_BYTE, owner_rank_id, OP_WRITE_REQ, MPI_COMM_WORLD);
    MPI_Recv(NULL, 0, MPI_BYTE, owner_rank_id, OP_WRITE_RESP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
