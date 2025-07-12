#include <string.h>

#include "../include/utils.h"
#include "../include/common.h"
#include "../include/memory/block.h"

void memory_read(int position, int size, char *buffer)
{
    memset(buffer, 0, size);
    int processed = 0;

    while (processed < size)
    {
        int abs_pos = position + processed;
        int block_id = abs_pos / DSM_BLOCK_SIZE;
        int block_offset = abs_pos % DSM_BLOCK_SIZE;
        int chunk = DSM_BLOCK_SIZE - block_offset;

        if (chunk > size - processed)
            chunk = size - processed;

        MemoryBlock *block = memory_block_get(&block_id);

        memcpy(buffer + processed, block->data + block_offset, chunk);
        processed += chunk;
    }
}

void memory_write(int position, int size, char *buffer)
{
    int processed = 0;

    while (processed < size)
    {
        int abs_position = position + processed;
        int block_offset = abs_position % DSM_BLOCK_SIZE;
        int chunk_size = DSM_BLOCK_SIZE - block_offset;

        if (chunk_size > size - processed)
            chunk_size = size - processed;

        if (chunk_size <= 0) break;

        memory_block_set(abs_position, block_offset, buffer + processed, chunk_size);
        processed += chunk_size;
    }
}
