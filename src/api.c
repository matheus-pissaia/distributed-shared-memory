#include "api.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int read(int position, char *buffer, int size)
{
    if (position < 0 || size <= 0 || position + size > BLOCK_COUNT * BLOCK_SIZE) {
        return -1; // Invalid parameters
    }

    int block_id = position / BLOCK_SIZE; // Calculate the block ID
    int offset = position % BLOCK_SIZE; // Calculate the offset within the block

    if (offset + size > BLOCK_SIZE) {
        return -2; // Read exceeds block size
    }

    Process* current_process = &get_processes()[get_current_process_id()];

    CacheEntry* cache_entry = cache_get(current_process->cache, block_id);
    if (cache_entry != NULL && cache_entry->valid) {
        // If the block is in the cache, copy data from the cache
        memcpy(buffer, cache_entry->data + offset, size);
        return 0; // Successful read from cache
    } 

    MemoryBlock* block = find_local_block(current_process, block_id);
    if (block != NULL) {
        // If the block is found in local memory, copy data from the block
        memcpy(buffer, block->data + offset, size);
        return 0; // Successful read from local block
    }

    return -3; // Block not found in local memory or cache
}

int write(int position, char *buffer, int size)
{
    // TODO implement
    return 0;
}
