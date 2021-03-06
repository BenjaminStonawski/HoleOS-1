/*
 * stdio.h header
 *
 * This file is part of HoleOS.
 *
 * HoleOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HoleOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HoleOS.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _LIBC_STDIO_H
#define _LIBC_STDIO_H

#include <stddef.h>

typedef struct {
	int fd;
} FILE;

typedef int fpos_t;

#define EOF -1

#define SEEK_CUR 0
#define SEEK_END 1
#define SEEK_SET 2

// standard input,output,error
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

// file access functions
int fclose(FILE* stream);
FILE* fopen(const char* restrict filename, const char* restrict mode);

// formatted input/output functions
int printf(const char* restrict format, ...);

// character input/output functions
int fgetc(FILE* stream);
char* fgets(char* restrict s, int n, FILE* restrict stream);
int fputc(int c, FILE* stream);
int fputs(const char* restrict s, FILE* restrict stream);
int getc(FILE* stream);
int getchar(void);
int putc(int c, FILE* stream);
int putchar(int c);
int puts(const char* s);

// file positioning functions
int fgetpos(FILE* restrict stream, fpos_t* restrict pos);
int fseek(FILE* stream, long offset, int whence);
int fsetpos(FILE* stream, const fpos_t* pos);
long ftell(FILE* stream);
void rewind(FILE* stream);

#endif
