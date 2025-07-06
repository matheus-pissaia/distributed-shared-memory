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
    
    int bytes_read = 0; // Number of bytes read
    int current_pos = position; // Current position in the memory

    Process* current_process = &get_processes()[get_current_process_id()];

    while (bytes_read < size) {
        // Calculate the block ID and offset
        int block_id = current_pos / BLOCK_SIZE; // Calculate the block ID
        int offset = current_pos % BLOCK_SIZE; // Calculate the offset within the block

        int bytes_to_read = BLOCK_SIZE - offset;

        if (bytes_to_read > size - bytes_read) {
            bytes_to_read = size - bytes_read; // Adjust to read only the remaining bytes
        }

        CacheEntry* cache_entry = cache_get(current_process->cache, block_id);
        if (cache_entry != NULL && cache_entry->valid) {
            // If the block is in the cache, copy data from the cache
            memcpy(buffer + bytes_read, cache_entry->data + offset, bytes_to_read);
            bytes_read += bytes_to_read;
            current_pos += bytes_to_read; 
            continue;
        } 

        MemoryBlock* block = find_local_block(current_process, block_id);
        if (block != NULL) {
            memcpy(buffer + bytes_read, block->data + offset, bytes_to_read);
            bytes_read += bytes_to_read;
            current_pos += bytes_to_read;
            continue;
        }

        return -3;

    }

    return 0;
}

int write(int position, char *buffer, int size)
{
    // TODO implement
    return 0;
}
