#include "util.h"

/* ========== Standard Library Replacement Functions ========== */
/* These are the actual function names expected by the compiler/linker */

/**
 * @brief Copy memory from source to destination
 */
void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    while (n--) {
        *d++ = *s++;
    }

    return dest;
}

/**
 * @brief Copy memory from source to destination (alias)
 */
void *xmemcpy(void *dest, const void *src, size_t n) {
    return memcpy(dest, src, n);
}

/**
 * @brief Set memory to a value
 */
void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;
    uint8_t value = (uint8_t)c;

    while (n--) {
        *p++ = value;
    }

    return s;
}

/**
 * @brief Set memory to a value (alias)
 */
void *xmemset(void *s, int c, size_t n) {
    return memset(s, c, n);
}

/**
 * @brief Compare two memory regions
 */
int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }

    return 0;
}

/**
 * @brief Compare two memory regions (alias)
 */
int xmemcmp(const void *s1, const void *s2, size_t n) {
    return memcmp(s1, s2, n);
}

/**
 * @brief Move memory (handles overlapping regions)
 */
void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    if (d < s) {
        /* Copy forward */
        while (n--) {
            *d++ = *s++;
        }
    } else if (d > s) {
        /* Copy backward */
        d += n;
        s += n;
        while (n--) {
            *--d = *--s;
        }
    }

    return dest;
}

/**
 * @brief Move memory (alias)
 */
void *xmemmove(void *dest, const void *src, size_t n) {
    return memmove(dest, src, n);
}

/**
 * @brief Get string length
 */
size_t strlen(const char *s) {
    size_t len = 0;

    while (*s++) {
        len++;
    }

    return len;
}

/**
 * @brief Get string length (alias)
 */
size_t xstrlen(const char *s) {
    return strlen(s);
}

/**
 * @brief Compare two strings
 */
int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    return *(const uint8_t *)s1 - *(const uint8_t *)s2;
}

/**
 * @brief Compare two strings (alias)
 */
int xstrcmp(const char *s1, const char *s2) {
    return strcmp(s1, s2);
}

/**
 * @brief Copy string from source to destination
 */
char *strcpy(char *dest, const char *src) {
    char *d = dest;

    while ((*d++ = *src++));

    return dest;
}

/**
 * @brief Copy string from source to destination (alias)
 */
char *xstrcpy(char *dest, const char *src) {
    return strcpy(dest, src);
}

/**
 * @brief Copy at most n characters from string
 */
char *strncpy(char *dest, const char *src, size_t n) {
    char *d = dest;

    while (n && (*d++ = *src++)) {
        n--;
    }

    /* Pad with null bytes if necessary */
    while (n--) {
        *d++ = '\0';
    }

    return dest;
}

/**
 * @brief Copy at most n characters from string (alias)
 */
char *xstrncpy(char *dest, const char *src, size_t n) {
    return strncpy(dest, src, n);
}

/* ========== Printf/Assert Stubs ========== */

/**
 * @brief Dummy printf (disabled to save memory)
 */
int printf(const char *format, ...) {
    (void)format;
    return 0;
}

/**
 * @brief Dummy assert function
 */
void __assert_func(const char *file, int line, const char *func, const char *expr) {
    (void)file;
    (void)line;
    (void)func;
    (void)expr;
    /* Could add logging here if needed */
    while(1); /* Hang on assertion failure */
}

/* ========== Malloc/Free Stubs ========== */

/**
 * @brief Dummy malloc (should never be called - LFS_NO_MALLOC is set)
 */
void *malloc(size_t size) {
    (void)size;
    return (void*)0;
}

/**
 * @brief Dummy free (should never be called)
 */
void free(void *ptr) {
    (void)ptr;
}

/* ========== Compiler Builtins ========== */

/**
 * @brief Byte swap 32-bit
 */
unsigned int __bswapsi2(unsigned int x) {
    return ((x & 0xFF000000) >> 24) |
           ((x & 0x00FF0000) >> 8)  |
           ((x & 0x0000FF00) << 8)  |
           ((x & 0x000000FF) << 24);
}

/**
 * @brief Count leading zeros
 */
int __clzsi2(unsigned int x) {
    int count = 0;
    if (x == 0) return 32;

    while ((x & 0x80000000) == 0) {
        count++;
        x <<= 1;
    }
    return count;
}

/**
 * @brief Count trailing zeros
 */
int __ctzsi2(unsigned int x) {
    int count = 0;
    if (x == 0) return 32;

    while ((x & 1) == 0) {
        count++;
        x >>= 1;
    }
    return count;
}

/**
 * @brief Population count (count set bits)
 */
int __popcountsi2(unsigned int x) {
    int count = 0;
    while (x) {
        count += x & 1;
        x >>= 1;
    }
    return count;
}
