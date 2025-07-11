#ifndef DSM_UTILS_H
#define DSM_UTILS_H

/**
 * @brief Reads data from the shared memory.
 *
 * @param position The absolute position in the shared memory.
 * @param size The size of the data to be read.
 * @param buffer The buffer to store the read data.
 */
void memory_read(int position, int size, char *buffer);

/**
 * @brief Writes data to the shared memory.
 *
 * @param position The absolute position in the shared memory.
 * @param size The size of the data to be written.
 * @param buffer The buffer containing the data to be written.
 */
void memory_write(int position, int size, char *buffer);

#endif
