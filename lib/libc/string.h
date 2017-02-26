/* Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
 *
 * This file is part of AIM.
 *
 * AIM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _STRING_H
#define _STRING_H

#include <sys/types.h>

#ifndef __ASSEMBLER__

void *memset(void *dst, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst0, const void *src0, size_t length);
int strcmp(const char *s1, const char *s2);
size_t strlcat(char *dst, const char *src, size_t dsize);
size_t strlcpy(char *dst, const char *src, size_t dsize);
size_t strlen(const char *str);
size_t strnlen(const char *str, size_t maxlen);

#endif /* !__ASSEMBLER__ */

#endif

