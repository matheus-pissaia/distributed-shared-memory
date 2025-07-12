#ifndef DSM_PROCESS_H
#define DSM_PROCESS_H

#include "block.h"
#include "cache.h"

typedef struct
{
    int rank_id;         // Rank ID of the process
    MemoryBlock *blocks; // Local memory blocks for the process
    Cache *cache;        // Cache of remote blocks
} Process;

/**
 * @brief Gets the ID of the process that owns a given block ID.
 *
 * @param block_id The ID of the block to find the owner for.
 *
 * @return The rank ID of the owner process.
 */
int get_owner_from_block_id(int *block_id);

/**
 * @brief Initializes the local process struct
 *
 * @return Pointer to the initialized process struct
 */
Process *process_init();

/**
 * @brief Retrieves the local process struct
 *
 * @return Pointer to the local process struct
 */
Process *process_get();

/**
 * @brief Gets a memory block by its ID from the local process.
 *
 * @param block_id The block ID to find
 *
 * @return MemoryBlock* Pointer to the block if found, NULL otherwise
 */
MemoryBlock *process_block_get(int block_id);

#endif
