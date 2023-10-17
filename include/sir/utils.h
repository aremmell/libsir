/**
 * @file utils.h
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
#ifndef _SIR_UTILS_H_INCLUDED
# define _SIR_UTILS_H_INCLUDED

# include "sir/platform.h"
# include "sir/helpers.h"
# include "sir/mutex.h"

# if defined(__cplusplus)
extern "C" {
# endif

/**
 * @brief Remove all occurrences of substring "sub" in string "str".
 */
char* sir_strremove(char *str, const char *sub);

/*
 * @brief Replace all occurrences of repeating whitespace characters
 *        in string "str" with a single space character.
 */
char* sir_strsqueeze(char *str);

/*
 * @brief Redact all occurrences of substring "sub" in
 *        string "str" with character 'c'.
 */
char* sir_strredact(char *str, const char *sub, const char c);

/*
 * @brief Replace all occurrences of character 'c' in
 *        string "str" with character 'n'.
 */
char* sir_strreplace (char *str, const char c, const char n);

/*
 * @brief Replace up to 'max' occurrences of character 'c'
 *        in string "str" with character 'n', returning
 *        the number of replacements performed.
 */
size_t sir_strcreplace(char *str, const char c, const char n, int32_t max);

# if defined(__cplusplus)
}
# endif

#endif /* !_SIR_UTILS_H_INCLUDED */
