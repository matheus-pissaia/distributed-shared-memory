#include <stdio.h>
#include <string.h>

#include "../api/api.h"

int main(int argc, char **argv)
{
    char *write_data = "Hello World!";
    int str_len = strlen(write_data);
    int total_size = str_len + 1; 

    dsm_write(28, write_data, total_size);

    char resp_data[total_size];
    memset(resp_data, 0, total_size);
    dsm_read(28, resp_data, total_size);

    printf("Response data: %s\n", resp_data);

    return 0;
}
