#ifndef API_H
#define API_H

typedef enum {
    API_SUCCESS = 0,          // Operação bem-sucedida
    API_ERR_INVALID_PARAMS = -1,  // Parâmetros inválidos
    API_ERR_BLOCK_BOUNDARY = -2,  // Acesso além do limite do bloco
    API_ERR_REMOTE_READ = -3,     // Falha na leitura remota
    API_ERR_CACHE_MISS = -4,      // Bloco não encontrado na cache
    API_ERR_BLOCK_NOT_LOCAL = -5  // Bloco não é local (owner diferente)
} ApiErrorCode;

/**
 * @brief Reads some data from the shared memory.
 *
 * First, it should try to read the block from the cache. If it fails, request
 * to the owner (process) the desired block and store it in the cache.
 *
 * @param position Initial position in memory from where to read
 * @param buffer Pointer to the variable that has the reading content
 * @param size Amount of bytes to read from the starting position
 *
 * @return
 */
int dsm_read(int position, char *buffer, int size);

/**
 * @brief Writes some data to the shared memory.
 *
 * The write operation is sent to the owner (process) of the block, which will
 * update the data and respond with a confirmation status. Then, a message is
 * sent to all processes that have the block in their cache to invalidate it.
 *
 * @param position Initial position in memory from where to write
 * @param buffer Pointer to the variable that has the writing content
 * @param size Amount of bytes to write from the starting position
 *
 * @return
 */
int dsm_write(int position, char *buffer, int size);


/**
 * @brief Returns a descriptive string for API error codes
 * @param err The error code
 * @return Human-readable error message
 */
const char* api_error_str(ApiErrorCode err);

#endif
