/**
 * @file xprintf.c
 * @brief Lightweight printf implementation for embedded systems
 *
 * This file provides a minimal printf-style formatting library optimized
 * for resource-constrained embedded systems. It supports basic format
 * specifiers (%d, %u, %x, %b, %c, %s) and width specifications.
 */

#include "xprintf.h"

#include <stdint.h>
#include <stdarg.h>

static xprintf_putc_t xprintf_putc = NULL;     /**< Function pointer for character output */
static xprintf_flush_t xprintf_flush = NULL;   /**< Function pointer for output flush */

/**
 * @brief Convert integer to ASCII string and output
 *
 * Converts an integer value to its ASCII representation in the specified
 * radix (base) and outputs it character by character. Supports padding
 * and sign handling.
 *
 * @param val Integer value to convert
 * @param rad Radix (base) for conversion. Negative values enable signed conversion
 * @param len Minimum field width. Negative values use '0' padding instead of space
 * @return None
 */
static void xprintf_itoa(int32_t val, int8_t rad, int8_t len)
{
  char c, sgn = 0, pad = ' ';
  char s[20];
  uint8_t i = 0;

  if(xprintf_putc == NULL || xprintf_flush == NULL)
    return;

  if (rad < 0)
  {
    rad = -rad;
    if (val < 0)
    {
      val = -val;
      sgn = '-';
    }
  }
  if (len < 0)
  {
    len = -len;
    pad = '0';
  }
  if (len > 20)
    return;
  do
  {
    c = (char)((uint32_t)val % rad);
    if (c >= 10)
      c += ('A' - 10);
    else
      c += '0';
    s[i++] = c;
    val = (uint32_t)val / rad;
  } while (val);
  if ((sgn != 0) && (pad != '0'))
    s[i++] = sgn;
  while (i < len)
    s[i++] = pad;
  if ((sgn != 0) && (pad == '0'))
    s[i++] = sgn;
  do
    xprintf_putc(s[--i]);
  while (i);
}

/**
 * @brief Process format string and output formatted text
 *
 * Internal function that processes a format string and variable argument list,
 * generating formatted output. Supports the following format specifiers:
 * - %d: signed decimal integer
 * - %u: unsigned decimal integer
 * - %x: hexadecimal integer
 * - %b: binary integer
 * - %c: character
 * - %s: string
 * - %%: literal percent sign
 *
 * Width specifiers can be used (e.g., %08x for zero-padded hex).
 *
 * @param str Format string
 * @param arp Variable argument list
 * @return None
 */
static void xprintf_vfprintf(const char *str, va_list arp)
{
  int32_t d, r, w, s;
  char *c;

  if(xprintf_putc == NULL || xprintf_flush == NULL)
    return;

  while ((d = *str++) != 0)
  {
    if (d != '%')
    {
      xprintf_putc(d);
      continue;
    }
    d = *str++;
    w = r = s = 0;
    if (d == '%')
    {
      xprintf_putc(d);
      d = *str++;
    }
    if (d == '0')
    {
      d = *str++;
      s = 1;
    }
    while ((d >= '0') && (d <= '9'))
    {
      w += w * 10 + (d - '0');
      d = *str++;
    }
    if (s)
      w = -w;
    if (d == 's')
    {
      c = va_arg(arp, char *);
      while (*c)
      xprintf_putc(*(c++));
      continue;
    }
    if (d == 'c')
    {
      xprintf_putc((char)va_arg(arp, int));
      continue;
    }
    if (d == '\0')
      break;
    else if (d == 'u')
      r = 10;
    else if (d == 'd')
      r = -10;
    else if (d == 'x')
      r = 16;
    else if (d == 'b')
      r = 2;
    else
      str--;
    if (r == 0)
      continue;
    if (r > 0)
      xprintf_itoa((uint32_t)va_arg(arp, int32_t), r, w);
    else
      xprintf_itoa((int32_t)va_arg(arp, int32_t), r, w);
  }
}

void xprintf_init(xprintf_putc_t putc, xprintf_flush_t flush)
{
  xprintf_putc = putc;
  xprintf_flush = flush;
}

void xprintf(const char *format, ...)
{
  if(xprintf_putc == NULL || xprintf_flush == NULL)
    return;

  va_list arg;
  va_start(arg, format);
  xprintf_vfprintf(format, arg);
  va_end(arg);

  xprintf_flush();
}
