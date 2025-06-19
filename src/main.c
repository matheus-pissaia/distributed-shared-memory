int main(int argc, char const *argv[])
{
    return 0;
}

/**
 * @brief Reads some data from the shared memory
 *
 * @param position Initial position in memory from where you want to read some content
 * @param buffer   Pointer to the char array where the data will be stored
 * @param size     Amount of bytes to read from the starting position
 *
 * @return 0 on success
 */
int read(int position, char *buffer, int size) {}

/**
 * @brief Writes some data to the shared memory
 *
 * @param position Initial position in memory from where you want to write some content
 * @param buffer   Pointer to the char array where the data will be stored
 * @param size     Amount of bytes to write from the starting position
 *
 * @return 0 on success
 */
int write(int position, char *buffer, int size) {}
