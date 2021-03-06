/*
 * atexit function
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

extern void (*__libc_atexit_funcs[])(void);
extern int __libc_atexit_count;

#define ATEXIT_MAX 32

int atexit(void (*func)(void)) {
	if (__libc_atexit_count >= ATEXIT_MAX - 1) {
		return -1;
	}
	__libc_atexit_count++;
	__libc_atexit_funcs[__libc_atexit_count] = func;
	return 0;
}
