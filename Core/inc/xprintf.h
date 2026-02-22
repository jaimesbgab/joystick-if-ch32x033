/**
 * @file xprintf.h
 * @brief Lightweight printf interface for embedded systems
 *
 * This file provides a minimal printf-style formatting library optimized
 * for resource-constrained embedded systems. It supports basic format
 * specifiers (%d, %u, %x, %b, %c, %s) and width specifications.
 */

#ifndef __XPRINTF_H
#define __XPRINTF_H

#ifndef NULL
  #define NULL ((void *)0)
#endif

/**
 * @brief Function pointer type for character output
 *
 * @param c Character to output
 */
typedef void (*xprintf_putc_t)(char c);

/**
 * @brief Function pointer type for output flush
 */
typedef void (*xprintf_flush_t)(void);

/**
 * @brief Initialize xprintf with output functions
 *
 * Registers the character output and flush callback functions that
 * xprintf will use for generating output. Must be called before
 * using xprintf().
 *
 * @param putc Function pointer to character output function
 * @param flush Function pointer to output flush function
 * @return None
 */
void xprintf_init(xprintf_putc_t putc, xprintf_flush_t flush);

/**
 * @brief Formatted output function
 *
 * Printf-style function that formats and outputs text according to
 * the format string. Automatically flushes output after formatting.
 *
 * Supported format specifiers:
 * - %d: signed decimal
 * - %u: unsigned decimal
 * - %x: hexadecimal
 * - %b: binary
 * - %c: character
 * - %s: string
 *
 * @param format Format string with optional format specifiers
 * @param ... Variable arguments matching format specifiers
 * @return None
 */
void xprintf(const char *format, ...);

#endif /* __XPRINTF_H */
