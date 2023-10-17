/**
 * @file sirutils.c
 *
 * @brief String utility functions
 *
 * Miscellanous utility functions operating on NULL-terminated strings.
 *
 * @author    Jeffrey H. Johnson \<trnsz@pobox.com\>
 * @date      2018-2023
 * @version   2.2.4
 * @copyright The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "sir/utils.h"

char* sir_strremove(char *str, const char *sub) {

  const char* p;
  char* r;
  char* q;

  if (!sub)
      return str;

  if (*sub && (q = r = strstr(str, sub)) != NULL) {
      size_t len = strnlen(sub, strlen (str));

      while ((r = strstr(p = r + len, sub)) != NULL)
          while (p < r)
              *q++ = *p++;

      while ((*q++ = *p++) != '\0')
          continue;
  }

  return str;
}

char* sir_strsqueeze(char *str) {
  unsigned long j;

  for (unsigned long i = j = 0; str[i]; ++i)
      if ((i > 0 && !isspace((unsigned char)(str[i - 1])))
                 || !isspace((unsigned char)(str[i])))
          str[j++] = str[i];

  str[j] = '\0';

  return str;
}

char* sir_strredact(char *str, const char *sub, const char c) {
  char *p;

  if (!sub || !c || !(p = strstr(str, sub)))
      return str;

  (void)memset(p, c, strnlen(sub, strlen (str)));

  return sir_strredact(str, sub, c);
}

char* sir_strreplace (char *str, const char c, const char n) {
  char *i = str;

  if (!c || !n)
      return str;

  while ((i = strchr(i, c)) != NULL)
      *i++ = n;

  return str;
}

size_t sir_strcreplace(char *str, const char c, const char n, int32_t max) {
  char*  i   = str;
  size_t cnt = 0;

  if (!c || !n || !max)
      return cnt;

  while (cnt < (size_t)max && (i = strchr(i, c)) != NULL) {
      *i++ = n;
      cnt++;
  }

  return cnt;
}
