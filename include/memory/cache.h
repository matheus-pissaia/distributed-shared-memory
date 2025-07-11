#ifndef DSM_CACHE_H
#define DSM_CACHE_H

#include <stdbool.h>
#include "block.h"

#define DSM_CACHE_SIZE 128 // Maximum number of entries in the cache

typedef struct
{
    bool valid;             // Boolean indicating if the entry is valid
    MemoryBlock *mem_block; // Pointer to the cached memory block
} CacheEntry;

typedef struct
{
    CacheEntry entries[DSM_CACHE_SIZE]; // Vector of cache entries
} Cache;

/**
 * @brief Initializes a new cache.
 *
 * @return Pointer to the new created cache.
 */
Cache *cache_init();

/**
 * @brief Retrieves an entry from the cache.
 *
 * @param cache Pointer to the cache.
 * @param block_id ID of the block to retrieve.
 *
 * @return Pointer to the cache entry.
 */
CacheEntry *cache_get(Cache *cache, int block_id);

/**
 * @brief Sets an entry in the cache.
 *
 * @param cache Pointer to the cache.
 * @param block_id ID of the block to set.
 * @param data Pointer to the data to set in the block.
 *
 * @return Pointer to the new cache entry.
 */
CacheEntry *cache_set(Cache *cache, int block_id, char *data);

/**
 * @brief Invalidates an entry in the cache.
 *
 * @param cache Pointer to the cache.
 * @param block_id ID of the block to invalidate.
 */
void cache_invalidate(Cache *cache, int block_id);

#endif
