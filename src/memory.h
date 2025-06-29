#ifndef MEMORY_H
#define MEMORY_H

#define CACHE_SIZE 128 // Maximum number of entries in the cache

extern int BLOCK_SIZE;    // Block size in bytes (default to 2KB)
extern int BLOCK_COUNT;   // Number of blocks (default to 1024)
extern int PROCESS_COUNT; // Number of processes (default to 4)

typedef struct
{
    int block_id;        // ID of the block where the data is stored
    int valid;           // 1 if the block is valid, 0 otherwise
    unsigned char *data; // Data stored in the block (depends on BLOCK_SIZE)
} CacheEntry;

typedef struct
{
    CacheEntry entries[CACHE_SIZE]; // Vector of cache entries
} Cache;

typedef struct 
{
    int block_id;        // ID of the block
    unsigned char *data; // Data stored in the block (depends on BLOCK_SIZE)
    int owner_process;   // Process that owns this block
} MemoryBlock;

typedef struct
{
    int process_id;             // ID of the process
    MemoryBlock* local_blocks;  // Local memory blocks for the process
    int num_local_blocks;       // Number of local memory blocks
    Cache *cache;               // Cache of remote blocks
} Process;

/**
 * @brief Distributes blocks among processes using interval-based distribution
 * 
 * @param processes Array of processes
 * @param num_processes Number of processes
 */
void distribute_blocks(Process* processes, int num_processes);

/**
 * @brief Finds a local block in a process
 * 
 * @param process The process to search in
 * @param block_id The block ID to find
 * @return MemoryBlock* Pointer to the block if found, NULL otherwise
 */
MemoryBlock* find_local_block(Process* process, int block_id);

/**
 * @brief Initializes a process with empty structures
 * 
 * @param process The process to initialize
 * @param process_id The ID to assign
 */
void init_process(Process* process, int process_id);

/**
 * @brief Frees resources allocated for a process
 * 
 * @param process The process to free
 */
void free_process(Process* process);

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
