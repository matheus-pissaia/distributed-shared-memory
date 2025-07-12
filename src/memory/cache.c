#include <stdlib.h>

#include "../../include/memory/cache.h"
#include "../../include/memory/block.h"

Cache *cache_init()
{
    Cache *cache = (Cache *)malloc(sizeof(Cache));

    for (int i = 0; i < DSM_CACHE_SIZE; i++)
    {
        cache->entries[i].valid = true;
        cache->entries[i].mem_block = NULL; // Init memory block pointer as NULL
    }

    return cache;
}

CacheEntry *cache_get(Cache *cache, int block_id)
{
    for (int i = 0; i < DSM_CACHE_SIZE; i++)
    {
        MemoryBlock *mem_block = cache->entries[i].mem_block;

        if (mem_block != NULL && mem_block->id == block_id)
            return &cache->entries[i]; // Return the cache entry if found
    }

    return NULL;
}

CacheEntry *cache_set(Cache *cache, int block_id, char *data)
{
    // Simple implementation: find first empty or invalid entry
    for (int i = 0; i < DSM_CACHE_SIZE; i++)
    {
        CacheEntry *entry = &cache->entries[i];

        if (entry->mem_block == NULL)
        {
            entry->mem_block = memory_block_init(block_id, data);
            return entry;
        }

        else if (!entry->valid)
        {
            memory_block_free(entry->mem_block); // Free old invalid memory block
            entry->valid = true;
            entry->mem_block = memory_block_init(block_id, data);

            return entry;
        }
    }

    // Cache is full - apply replacement policy
    // Replace the first entry
    // TODO: Implement a more sophisticated replacement policy
    memory_block_free(cache->entries[0].mem_block); // Free the old memory block
    cache->entries[0].valid = true;
    cache->entries[0].mem_block = memory_block_init(block_id, data);
    return &cache->entries[0];
}

void cache_invalidate(Cache *cache, int block_id)
{
    for (int i = 0; i < DSM_CACHE_SIZE; i++)
    {
        MemoryBlock *mem_block = cache->entries[i].mem_block;

        if (mem_block != NULL && mem_block->id == block_id)
        {
            cache->entries[i].valid = false;
            return;
        }
    }
}
