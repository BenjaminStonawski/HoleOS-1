/*
 * dir program
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

#include <holeos.h>

int main() {
	int handle = dir_open("/");
	if (handle < 0) {
		printf(1, "dir_open failed with %d\n", handle);
		proc_exit();
	}

	char filename[256];
	while (dir_read(handle, filename)) {
		printf(1, "%s %d\n", filename, file_get_size(filename));
	}
	dir_close(handle);
	proc_exit();
}
