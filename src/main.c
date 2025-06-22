#include <stdio.h>
#include <stdlib.h>
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

    return 0;
}
