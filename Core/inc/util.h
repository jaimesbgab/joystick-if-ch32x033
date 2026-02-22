/**
 * @file util.h
 * @brief Utility functions - Standard library replacements
 *
 * This file provides lightweight implementations of standard library functions
 * to avoid linking the full C library and reduce memory footprint.
 *
 * All functions are provided in both standard names (memcpy, strlen, etc.)
 * and x-prefixed aliases (xmemcpy, xstrlen, etc.) for convenience.
 */

#ifndef __UTIL_H
#define __UTIL_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/* ========== Memory Functions ========== */

/**
 * @brief Copy memory from source to destination
 * @param dest Destination pointer
 * @param src Source pointer
 * @param n Number of bytes to copy
 * @return Destination pointer
 */
void *memcpy(void *dest, const void *src, size_t n);
void *xmemcpy(void *dest, const void *src, size_t n);

/**
 * @brief Set memory to a value
 * @param s Pointer to memory
 * @param c Value to set (converted to unsigned char)
 * @param n Number of bytes to set
 * @return Pointer to memory
 */
void *memset(void *s, int c, size_t n);
void *xmemset(void *s, int c, size_t n);

/**
 * @brief Compare two memory regions
 * @param s1 First memory region
 * @param s2 Second memory region
 * @param n Number of bytes to compare
 * @return 0 if equal, <0 if s1 < s2, >0 if s1 > s2
 */
int memcmp(const void *s1, const void *s2, size_t n);
int xmemcmp(const void *s1, const void *s2, size_t n);

/**
 * @brief Move memory (handles overlapping regions)
 * @param dest Destination pointer
 * @param src Source pointer
 * @param n Number of bytes to move
 * @return Destination pointer
 */
void *memmove(void *dest, const void *src, size_t n);
void *xmemmove(void *dest, const void *src, size_t n);

/* ========== String Functions ========== */

/**
 * @brief Get string length
 * @param s String pointer
 * @return Length of string (excluding null terminator)
 */
size_t strlen(const char *s);
size_t xstrlen(const char *s);

/**
 * @brief Compare two strings
 * @param s1 First string
 * @param s2 Second string
 * @return 0 if equal, <0 if s1 < s2, >0 if s1 > s2
 */
int strcmp(const char *s1, const char *s2);
int xstrcmp(const char *s1, const char *s2);

/**
 * @brief Copy string from source to destination
 * @param dest Destination pointer
 * @param src Source string
 * @return Destination pointer
 */
char *strcpy(char *dest, const char *src);
char *xstrcpy(char *dest, const char *src);

/**
 * @brief Copy at most n characters from string
 * @param dest Destination pointer
 * @param src Source string
 * @param n Maximum number of characters to copy
 * @return Destination pointer
 */
char *strncpy(char *dest, const char *src, size_t n);
char *xstrncpy(char *dest, const char *src, size_t n);

/* ========== Printf/Assert Stubs ========== */

/**
 * @brief Dummy printf (disabled to save memory)
 * @param format Format string
 * @return Always returns 0
 */
int printf(const char *format, ...);

/**
 * @brief Assert function stub
 * @param file Source file name
 * @param line Line number
 * @param func Function name
 * @param expr Expression that failed
 * @note This function hangs indefinitely on assertion failure
 */
void __assert_func(const char *file, int line, const char *func, const char *expr);

/* ========== Malloc/Free Stubs ========== */

/**
 * @brief Dummy malloc (should never be called)
 * @param size Size to allocate
 * @return Always returns NULL
 */
void *malloc(size_t size);

/**
 * @brief Dummy free (should never be called)
 * @param ptr Pointer to free
 */
void free(void *ptr);

/* ========== Compiler Builtins ========== */

/**
 * @brief Byte swap 32-bit value
 * @param x Value to swap
 * @return Byte-swapped value
 */
unsigned int __bswapsi2(unsigned int x);

/**
 * @brief Count leading zeros
 * @param x Input value
 * @return Number of leading zero bits (0-32)
 */
int __clzsi2(unsigned int x);

/**
 * @brief Count trailing zeros
 * @param x Input value
 * @return Number of trailing zero bits (0-32)
 */
int __ctzsi2(unsigned int x);

/**
 * @brief Population count (count set bits)
 * @param x Input value
 * @return Number of set bits
 */
int __popcountsi2(unsigned int x);

#endif /* __UTIL_H */
