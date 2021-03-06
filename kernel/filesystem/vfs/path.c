/*
 * Path string operation
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

#include <defs.h>

int vfs_path_split(const char* path, char* buf) {
	int count = 0;
	int x = 0, y;
	for (;;) {
		if (path[x] == '\0') {
			break;
		}
		for (y = x; ((path[y] != '/') && (path[y] != '\0')); y++) {
		}
		if (y - x == 0) {
			// empty directory name
			x = y + 1;
			continue;
		}
		strncpy(buf + count * 128, path + x, y - x);
		*(buf + count * 128 + y - x) = '\0';
		count++;
		if (path[y] == '\0') {
			// end of path string
			break;
		}
		x = y + 1;
	}
	return count;
}

int vfs_path_compare(int lhs_parts, const char* lhs_buf, int rhs_parts,
					 const char* rhs_buf) {
	if (lhs_parts != rhs_parts) {
		return 0;
	}
	for (int i = 0; i < lhs_parts; i++) {
		if (strncmp(lhs_buf + i * 128, rhs_buf + i * 128, 128) != 0) {
			return 0;
		}
	}
	return 1;
}
