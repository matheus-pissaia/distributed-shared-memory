#include <stdlib.h>
#include "memory.h"

int BLOCK_SIZE = 2048;
int BLOCK_COUNT = 1024;
int PROCESS_COUNT = 4;

Cache *cache_init()
{
    Cache *cache = (Cache *)malloc(sizeof(Cache));

    for (int i = 0; i < CACHE_SIZE; i++)
    {
        cache->entries[i].valid = 0;     // Initialize all entries as invalid
        cache->entries[i].block_id = -1; // Initialize block_id to -1 (non-existent)
    }

    return cache;
}

CacheEntry *cache_get(Cache *cache, int block_id)
{
    // TODO implement
    return NULL;
}

void cache_set(Cache *cache, int block_id, unsigned char data)
{
    // TODO implement (if cache is full apply some replacement policy?)
}

void cache_invalidate(Cache *cache, int block_id)
{
    // TODO implement
}
