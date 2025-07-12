#include <stdio.h>
#include <string.h>

#include "../api/api.h"

int main(int argc, char **argv)
{
    char *write_data = "Hello World!";
    int size = strlen(write_data);

    dsm_write(28, write_data, size);

    char resp_data[size];
    dsm_read(28, resp_data, size);

    printf("Response data: %s\n", resp_data);

    return 0;
}
