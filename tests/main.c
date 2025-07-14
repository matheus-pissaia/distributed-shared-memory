#include <stdio.h>
#include <string.h>

#include "../api/api.h"

void compare_response(char *resp_data, char *expected_data)
{
    if (strcmp(resp_data, expected_data) != 0)
    {
        printf("RESPONSE DATA DOES NOT MATCH EXPECTED\n");
        printf("Response data: %s\n", resp_data);
        printf("Expected data: %s\n", expected_data);
    }
    else
        printf("RECEIVED DATA: %s\n", resp_data);
}

// Write data that fits in a single block
void write_data_single_block()
{
    printf("\nTESTING WRITE DATA SINGLE BLOCK\n");

    int position = 0;
    char *write_data = "Hello World!";
    int size = strlen(write_data) + 1;

    printf("SENDING DATA: %s\n", write_data);
    dsm_write(position, write_data, size);

    char resp_data[size];
    memset(resp_data, 0, size);
    dsm_read(position, resp_data, size);

    compare_response(resp_data, write_data);
}

// Write data that occupies more than one block
void write_data_multiple_blocks()
{
    printf("\nTESTING WRITE DATA MULTIPLE BLOCKS\n");

    int position = 0;
    char *write_data = "This message will be stored in multiple blocks!";
    int size = strlen(write_data) + 1;

    printf("SENDING DATA: %s\n", write_data);
    dsm_write(position, write_data, size);

    char resp_data[size];
    memset(resp_data, 0, size);
    dsm_read(position, resp_data, size);

    compare_response(resp_data, write_data);
}

// Force firts block cache invalidation by sending a write operation on the
// second block
void force_cache_invalidate()
{
    printf("\nTESTING CACHE INVALIDATION\n");

    // Starting at 27th position, so we force the data to be stored in the first
    // and second block
    int position = 27;
    char *strawbery = "Strawbery";
    int size = strlen(strawbery) + 1;

    printf("SENDING DATA: %s\n", strawbery);
    dsm_write(position, strawbery, size);

    char resp_data[size];
    memset(resp_data, 0, size);
    // Read to force data caching
    dsm_read(position, resp_data, size);

    compare_response(resp_data, strawbery);

    char *berry = "berry";
    size = strlen(berry) + 1;

    printf("SENDING DATA: %s\n", berry);
    // Force block cache invalidation in the first process by writing only in
    // the second block (32nd position)
    dsm_write(32, berry, size);

    char *strawberry = "Strawberry";
    size = strlen("Strawberry") + 1;

    char resp_data_updated[size];
    memset(resp_data_updated, 0, size);
    // Read the whole data again to force update and data caching
    dsm_read(position, resp_data_updated, size);

    compare_response(resp_data_updated, strawberry);
}

// Run test program with 2 processes and:
// DSM_BLOCK_SIZE = 32;
// DSM_BLOCK_COUNT = 2;
int main(int argc, char **argv)
{
    write_data_single_block();
    write_data_multiple_blocks();
    force_cache_invalidate();

    return 0;
}
