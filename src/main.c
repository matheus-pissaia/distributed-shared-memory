#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"
#include "memory.h"

int main(int argc, char const *argv[])
{
    // Default values if no arguments
    if (argc == 1)
    {
        printf("\nNo extra command line arguments passed, using default values...\n");
    }

    // Custom values
    else if (argc == 4)
    {
        BLOCK_SIZE = atoi(argv[1]);
        BLOCK_COUNT = atoi(argv[2]);
        PROCESS_COUNT = atoi(argv[3]);
    }

    // Handle invalid number of arguments
    else
    {
        printf("\nInvalid number of arguments passed!\n");
        printf("\nUsage: ./compiled_file <block_size> <block_count> <process_count>\n");
        return 1;
    }

    // Validate BLOCK_SIZE
    if ((BLOCK_COUNT % PROCESS_COUNT) != 0)
    {
        printf("\nBLOCK_COUNT must be divisible by PROCESS_COUNT! (Use powers of 2)\n");
        exit(EXIT_FAILURE);
    }

    // Initialize processes
    init_processes(PROCESS_COUNT);

    //Distribute blocks among processes
    distribute_blocks(get_processes(), PROCESS_COUNT);

    //Test: write something to the first block of process 0
    if (get_processes()[0].num_local_blocks > 0)
    {
        MemoryBlock* block = &get_processes()[0].local_blocks[0];
        strcpy((char*)block->data, "Hello, World!");

        //Test reading 
        char buffer[100];
        int result = read(0, buffer, 13); //Should read "Hello, World!"

        if (result !=0)
        {
            fprintf(stderr, "Error reading data: %s\n", api_error_str(result));
        }
    }
    
    //Free resources
    free_processes();

    return 0;
}
