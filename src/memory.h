#ifndef MEMORY_H
#define MEMORY_H

#define CACHE_SIZE 128 // Maximum number of entries in the cache

extern int BLOCK_SIZE;    // Block size in bytes (default to 2KB)
extern int BLOCK_COUNT;   // Number of blocks (default to 1024)
extern int PROCESS_COUNT; // Number of processes (default to 4)

typedef struct
{
    CacheEntry entries[CACHE_SIZE]; // Vector of cache entries
} Cache;

typedef struct
{
    int block_id;        // ID of the block where the data is stored
    int valid;           // 1 if the block is valid, 0 otherwise
    unsigned char *data; // Data stored in the block (depends on BLOCK_SIZE)
} CacheEntry;

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
 * @param data Data to set in the block.
 */
void cache_set(Cache *cache, int block_id, unsigned char data);

/**
 * @brief Invalidates an entry in the cache.
 *
 * @param cache Pointer to the cache.
 * @param block_id ID of the block to invalidate.
 */
void cache_invalidate(Cache *cache, int block_id);

#endif
