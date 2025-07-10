#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "memory.h"

int DSM_BLOCK_SIZE = 2048;
int DSM_BLOCK_COUNT = 1024;
int DSM_PROCESS_COUNT = 0;

static Process *process; //  Internal variable to store the local process

int get_owner_from_block_id(int *block_id)
{
    return *block_id % (DSM_BLOCK_COUNT / DSM_PROCESS_COUNT);
}

// -------------- MEMORY BLOCK FUNCTIONS --------------

MemoryBlock *memory_block_init(int block_id, char *data)
{
    MemoryBlock *block = malloc(sizeof(MemoryBlock));

    block->id = block_id;
    block->owner_id = get_owner_from_block_id(&block_id);
    block->data = malloc(DSM_BLOCK_SIZE * sizeof(char));

    memset(block->data, *data, DSM_BLOCK_SIZE);

    return block;
}

void memory_block_free(MemoryBlock *block)
{
    if (block != NULL)
    {
        if (block->data != NULL)
            free(block->data);

        free(block);
    }
}

// Initializes the vector of memory blocks for the local process.
static MemoryBlock *process_blocks_init()
{
    int blocks_per_process = DSM_BLOCK_COUNT / DSM_PROCESS_COUNT;
    int block_id_offset = process->rank_id * blocks_per_process;

    MemoryBlock *blocks = malloc(sizeof(MemoryBlock) * blocks_per_process);

    for (int i = 0; i < blocks_per_process; i++)
    {
        blocks[i] = *memory_block_init(i + block_id_offset, NULL);
    }

    return blocks;
}

// -------------- PROCESS FUNCTIONS --------------

Process *process_get()
{
    return process;
}

Process *process_init()
{
    process->rank_id = -1; // Rank ID will be set during MPI initialization
    process->cache = cache_init();
    process->blocks = process_blocks_init();

    return process;
}

MemoryBlock *process_block_get(int block_id)
{
    int blocks_per_process = DSM_BLOCK_COUNT / DSM_PROCESS_COUNT;

    for (int i = 0; i < blocks_per_process; i++)
    {
        if (process->blocks[i].id == block_id)
            return &process->blocks[i];
    }

    return NULL;
}

// -------------- CACHE FUNCTIONS --------------

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

    // Se chegou aqui, a cache está cheia - aplicar política de substituição
    // Implementação simples: substitui a primeira entrada (poderia ser LRU, FIFO, etc.)
    // TODO: Implementar uma política de substituição mais sofisticada

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
