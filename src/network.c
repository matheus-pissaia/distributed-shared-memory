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

/**
 * @brief Sets the data of a block memory (local or remote).
 *
 * @param position The absolute position in the shared memory.
 * @param offset The offset inside the block.
 * @param data The data to be set.
 * @param size The size of the data to be set.
 */
static void memory_block_set(int position, int offset, const char *data, int size)
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
        if (req.size <= 0 || req.position < 0)
        {
            MPI_Send(NULL, 0, MPI_BYTE, status.MPI_SOURCE, DSM_INVALID_PARAMS, MPI_COMM_WORLD);
            return;
        }

        char resp_data[req.size]; // Response data buffer
        int processed = 0;

        while (processed < req.size)
        {
            int abs_pos = req.position + processed;
            int block_id = abs_pos / DSM_BLOCK_SIZE;
            int block_offset = abs_pos % DSM_BLOCK_SIZE;
            int chunk = DSM_BLOCK_SIZE - block_offset;

            if (chunk > req.size - processed)
                chunk = req.size - processed;

            MemoryBlock *block = memory_block_get(&block_id);

            memcpy(resp_data + processed, block->data + block_offset, chunk);
            processed += chunk;
        }

        MPI_Send(resp_data, req.size, MPI_BYTE, status.MPI_SOURCE, OP_READ_RESP, MPI_COMM_WORLD);
    }
    else if (req.opcode == OP_WRITE_REQ)
    {
        if (req.size <= 0 || req.position < 0)
        {
            MPI_Send(NULL, 0, MPI_BYTE, status.MPI_SOURCE, DSM_INVALID_PARAMS, MPI_COMM_WORLD);
            return;
        }

        int processed = 0;

        while (processed < req.size)
        {
            int abs_position = req.position + processed;
            int block_offset = abs_position % DSM_BLOCK_SIZE;
            int chunk_size = DSM_BLOCK_SIZE - block_offset;

            if (chunk_size > req.size - processed)
                chunk_size = req.size - processed;

            memory_block_set(abs_position, block_offset, req.data + processed, chunk_size);
            processed += chunk_size;
        }
    }
    else if (req.opcode == OP_INVALIDATE)
    {
        int block_id = req.position / DSM_BLOCK_SIZE;

        cache_invalidate(process_get()->cache, block_id);
    }
}

void comm_init(int *argc, char ***argv)
{
    // Initialize MPI environment
    MPI_Init(argc, argv);
    MPI_Comm_size(MPI_COMM_WORLD, &DSM_PROCESS_COUNT);
}

void comm_finalize()
{
    // Finalize MPI environment
    MPI_Finalize();
}
