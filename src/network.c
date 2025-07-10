#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"
#include "memory.h"

void broadcast_invalidate(int block_id)
{
    Process *local_process = process_get();

    for (int p_rank = 0; p_rank < DSM_PROCESS_COUNT; p_rank++)
    {
        if (p_rank == local_process->rank_id)
            continue;

        MPI_Send(&block_id, 1, MPI_INT, p_rank, OP_INVALIDATE, MPI_COMM_WORLD);
    }
}

static MemoryBlock *memory_block_fetch(int *block_id)
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

static MemoryBlock *memory_block_get(int *block_id)
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

static void memory_block_set(MemoryBlock *block, char *data)
{
    Process *local_process = process_get();
    int owner_rank_id = get_owner_from_block_id(&block->id);

    // If the block is owned by the local process, set it directly
    if (owner_rank_id == local_process->rank_id)
    {
        memcpy(block->data, data, DSM_BLOCK_SIZE);
        broadcast_invalidate(block->id);
        return;
    }

    // Send a write request to the owner
    DsmMsg msg;
    msg.opcode = OP_WRITE_REQ;
    msg.position = block->id * DSM_BLOCK_SIZE; // Start position from block ID
    msg.size = DSM_BLOCK_SIZE;                 // Write the entire block
    memcpy(msg.data, data, DSM_BLOCK_SIZE);    // Copy the data to be written

    MPI_Send(&msg, sizeof(DsmMsg), MPI_BYTE, owner_rank_id, OP_WRITE_REQ, MPI_COMM_WORLD);

    // TODO Handle write response status
    // MPI_Status status;
    // MPI_Recv(NULL, 0, MPI_BYTE, owner_rank_id, OP_WRITE_RESP, MPI_COMM_WORLD, &status);
}

// Process pending requests
void comm_process_requests()
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
        char resp_data[req.size];       // Response data buffer
        MemoryBlock *last_block = NULL; // Last block processed

        for (size_t i = 0; i < req.size; i++)
        {
            int block_id = (req.position + i) / DSM_BLOCK_SIZE;
            int offset = (req.position + i) % DSM_BLOCK_SIZE;

            if (!last_block || block_id != last_block->id)
                last_block = memory_block_get(&block_id);

            resp_data[i] = last_block->data[offset];
        }

        MPI_Send(resp_data, DSM_BLOCK_SIZE, MPI_BYTE, status.MPI_SOURCE, OP_READ_RESP, MPI_COMM_WORLD);
    }
    else if (req.opcode == OP_WRITE_REQ)
    {
        MemoryBlock *last_block = NULL; // Last block processed
        char *updated_data;             // Updated block data buffer

        for (size_t i = 0; i < req.size; i++)
        {
            int block_id = (req.position + i) / DSM_BLOCK_SIZE;
            int offset = (req.position + i) % DSM_BLOCK_SIZE;

            if (!last_block || block_id != last_block->id)
            {
                // Set new data for the last block before processing a new one
                if (last_block != NULL)
                    memory_block_set(&last_block, &updated_data);

                last_block = memory_block_get(&block_id);
                // Copy the block to the updated data buffer
                memcpy(updated_data, last_block->data, DSM_BLOCK_SIZE);
            }

            updated_data[offset] = req.data[i];
            memory_block_set(&last_block, &updated_data);
        }

        // Send write response status
        // MPI_Send(NULL, 0, MPI_BYTE, status.MPI_SOURCE, OP_WRITE_RESP, MPI_COMM_WORLD);
    }
    else if (req.opcode == OP_INVALIDATE)
    {
        int block_id = req.position / DSM_BLOCK_SIZE;

        cache_invalidate(process_get()->cache, block_id);
    }
}

void comm_init(int *argc, char ***argv)
{
    Process *local_process = process_get();

    // Initialize MPI environment
    MPI_Init(argc, argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &local_process->rank_id);
    MPI_Comm_size(MPI_COMM_WORLD, &DSM_PROCESS_COUNT);
}

void comm_finalize()
{
    // Finalize MPI environment
    MPI_Finalize();
}
