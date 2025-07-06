#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "memory.h"

int BLOCK_SIZE = 2048;
int BLOCK_COUNT = 1024;
int PROCESS_COUNT = 4;

static Process* processes = NULL;
static int current_process_id = 0;

int get_current_process_id() {
    return current_process_id;
}

Process* get_processes() {
    return processes;
}

void init_processes(int num_processes) {
    processes = (Process*)malloc(num_processes * sizeof(Process));
    for (int i = 0; i < num_processes; i++) {
        init_process(&processes[i], i);
    }
}

void free_processes() {
    if (processes != NULL) {
        for (int i = 0; i < PROCESS_COUNT; i++) {
            free_process(&processes[i]);
        }
        free(processes);
        processes = NULL;
    }
}

void distribute_blocks(Process* processes, int num_processes)
{
    int blocks_per_process = BLOCK_COUNT / num_processes;


    for (int i = 0; i < num_processes; i++)
    {
        processes[i].local_blocks = (MemoryBlock*)malloc(blocks_per_process * sizeof(MemoryBlock));
        processes[i].num_local_blocks = blocks_per_process;

        for (int j = 0; j < blocks_per_process; j++)
        {
            int block_id = i * blocks_per_process + j;
            processes[i].local_blocks[j].block_id = block_id;
            processes[i].local_blocks[j].owner_process = i;
            processes[i].local_blocks[j].data = (unsigned char*)malloc(BLOCK_SIZE * sizeof(unsigned char));
            
            memset(processes[i].local_blocks[j].data, 0, BLOCK_SIZE); // Initialize with zeros

        }

        // Initialize cache for the process
        processes[i].cache = cache_init();
    }
}

MemoryBlock* find_local_block(Process* process, int block_id)
{
    for (int i = 0; i < process->num_local_blocks; i++)
    {
        if (process->local_blocks[i].block_id == block_id)
        {
            return &process->local_blocks[i];
        }
    }
    return NULL;
}

void init_process(Process* process, int process_id)
{
    process->process_id = process_id;
    process->local_blocks = NULL;
    process->num_local_blocks = 0;
    process->cache = NULL;
}

void free_process(Process* process)
{
    for (int i = 0; i < process->num_local_blocks; i++)
    {
        free(process->local_blocks[i].data);
    }
    free(process->local_blocks);
    if (process->cache != NULL) {
        free(process->cache);
    }
    
}

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
    for (int i = 0; i < CACHE_SIZE; i++)
    {
        if (cache->entries[i].valid && cache->entries[i].block_id == block_id)
        {
            return &cache->entries[i]; // Return the cache entry if found
        }
    }
    return NULL;
}

void cache_set(Cache *cache, int block_id, unsigned char *data)
{ //Simple implementation: find first invalid entry
    for (int i = 0; i < CACHE_SIZE; i++)
    {
        if (!cache->entries[i].valid)
        {
            cache->entries[i].block_id = block_id;
            cache->entries[i].valid = 1;
            cache->entries[i].data = (unsigned char *)malloc(BLOCK_SIZE * sizeof(unsigned char));
            memcpy(cache->entries[i].data, &data, BLOCK_SIZE); // Copy data into the cache entry
            return;
        }
    }
    // Se chegou aqui, a cache está cheia - aplicar política de substituição
    // Implementação simples: substitui a primeira entrada (poderia ser LRU, FIFO, etc.)
    // TODO: Implementar uma política de substituição mais sofisticada
    
    cache->entries[0].block_id = block_id;
    free(cache->entries[0].data); // Libera os dados antigos
    cache->entries[0].data = (unsigned char *)malloc(BLOCK_SIZE * sizeof(unsigned char));
    memcpy(cache->entries[0].data, data, BLOCK_SIZE);
}


void cache_invalidate(Cache *cache, int block_id)
{
    for (int i = 0; i < CACHE_SIZE; i++)
    {
        if ( cache->entries[i].block_id == block_id)
        {
            cache->entries[i].valid = 0; // Invalidate the cache entry
            free(cache->entries[i].data); // Free the data memory
            cache->entries[i].data = NULL; // Set data pointer to NULL
            return;
        }
    }
}
