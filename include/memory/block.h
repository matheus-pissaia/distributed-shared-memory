#ifndef DSM_BLOCK_H
#define DSM_BLOCK_H

typedef struct
{
    int id;       // ID of the block
    char *data;   // Data stored in the block (depends on BLOCK_SIZE)
    int owner_id; // Process rank ID that owns this block
} MemoryBlock;

/**
 * @brief Initializes a memory block with the given ID and data.
 *
 * @param block_id ID of the block to initialize.
 * @param data Pointer to the data to initialize the block with.
 *
 * @return Pointer to the initialized MemoryBlock.
 */
MemoryBlock *memory_block_init(int block_id, char *data);

/**
 * @brief Gets a memory block by its ID locally or remotely.
 *
 * @param block_id The block ID to find
 *
 * @return Pointer to the block if found, NULL otherwise
 */
MemoryBlock *memory_block_get(int *block_id);

/**
 * @brief Sets the data of a block memory (local or remote).
 *
 * @param position The absolute position in the shared memory.
 * @param offset The offset inside the block.
 * @param data The data to be set.
 * @param size The size of the data to be set.
 */
void memory_block_set(int position, int offset, const char *data, int size);

/**
 * @brief Fetches a memory block by its ID and stores it in the cache.
 *
 * @param block_id The block ID to fetch
 *
 * @return Pointer to the fetched MemoryBlock
 */
MemoryBlock *memory_block_fetch(int *block_id);

/**
 * @brief Frees the memory allocated for a MemoryBlock struct.
 *
 * @return Pointer to the memory block.
 */
void memory_block_free(MemoryBlock *block);

#endif
