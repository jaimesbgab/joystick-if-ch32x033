/**
 * @file syscalls.c
 * @brief Newlib syscalls stubs for embedded systems
 *
 * This file provides minimal implementations of system calls required by
 * the newlib C library for bare-metal RISC-V embedded applications.
 */
#if USE_THIRD_PARTY_HAL_DRIVER == 0
#include <sys/stat.h>
#include <errno.h>

#undef errno
extern int errno;

/**
 * @brief Exit a program without cleaning up files
 * @param status Exit status code
 */
void _exit(int status)
{
    while (1) {
        /* Infinite loop - no OS to return to */
    }
}

/**
 * @brief Close a file
 * @param file File descriptor
 * @return -1 (not implemented)
 */
int _close(int file)
{
    return -1;
}

/**
 * @brief Status of an open file
 * @param file File descriptor
 * @param st Status structure
 * @return 0 on success
 */
int _fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

/**
 * @brief Query whether output stream is a terminal
 * @param file File descriptor
 * @return 1 (yes, assume terminal)
 */
int _isatty(int file)
{
    return 1;
}

/**
 * @brief Set position in a file
 * @param file File descriptor
 * @param ptr Offset
 * @param dir Direction
 * @return 0 (success)
 */
int _lseek(int file, int ptr, int dir)
{
    return 0;
}

/**
 * @brief Read from a file
 * @param file File descriptor
 * @param ptr Buffer to read into
 * @param len Number of bytes to read
 * @return 0 (no data read)
 */
int _read(int file, char *ptr, int len)
{
    return 0;
}

/**
 * @brief Get process ID
 * @return 1 (single process)
 */
int _getpid(void)
{
    return 1;
}

/**
 * @brief Send a signal
 * @param pid Process ID
 * @param sig Signal number
 * @return -1 (not implemented)
 */
int _kill(int pid, int sig)
{
    errno = EINVAL;
    return -1;
}
#endif