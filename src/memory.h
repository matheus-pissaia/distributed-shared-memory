#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>

#define DSM_CACHE_SIZE 128 // Maximum number of entries in the cache

extern int DSM_BLOCK_SIZE;    // Block size in bytes (default to 2KB)
extern int DSM_BLOCK_COUNT;   // Number of blocks (default to 1024)
extern int DSM_PROCESS_COUNT; // Number of processes
extern int DSM_PROCESS_RANK;  // Rank of the current process

typedef struct
{
    int id;              // ID of the block
    unsigned char *data; // Data stored in the block (depends on BLOCK_SIZE)
    int owner_id;        // Process rank ID that owns this block
} MemoryBlock;

typedef struct
{
    bool valid;             // Boolean indicating if the entry is valid
    MemoryBlock *mem_block; // Pointer to the cached memory block
} CacheEntry;

typedef struct
{
    CacheEntry entries[DSM_CACHE_SIZE]; // Vector of cache entries
} Cache;

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

/**
 * @brief Initializes a memory block with the given ID and data.
 *
 * @param block_id ID of the block to initialize.
 * @param data Pointer to the data to initialize the block with.
 *
 * @return Pointer to the initialized MemoryBlock.
 */
MemoryBlock *memory_block_init(int block_id, unsigned char *data);

/**
 * @brief Frees the memory allocated for a MemoryBlock struct.
 *
 * @return Pointer to the memory block.
 */
void memory_block_free(MemoryBlock *block);

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
 */
void cache_set(Cache *cache, int block_id, unsigned char *data);

/**
 * @brief Invalidates an entry in the cache.
 *
 * @param cache Pointer to the cache.
 * @param block_id ID of the block to invalidate.
 */
void cache_invalidate(Cache *cache, int block_id);

#endif
