/*
 * mount.c - mount devices
 *
 * Copyright (c) 2007 Sascha Hauer <s.hauer@pengutronix.de>, Pengutronix
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <common.h>
#include <command.h>
#include <fs.h>
#include <errno.h>

static int do_mount (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret = 0;
	struct mtab_entry *entry = NULL;

	if (argc == 1) {
		do {
			entry = mtab_next_entry(entry);
			if (entry) {
				printf("%s on %s type %s\n",
					entry->parent_device ? entry->parent_device->id : "none",
					entry->path,
					entry->dev->name);
			}
		} while (entry);
		return 0;
	}

	if (argc != 4) {
		u_boot_cmd_usage(cmdtp);
		return 1;
	}

	if ((ret = mount(argv[1], argv[2], argv[3]))) {
		perror("mount");
		return 1;
	}
	return 0;
}

static __maybe_unused char cmd_mount_help[] =
"Usage: mount:         list mounted filesystems\n"
"or:    mount <device> <fstype> <mountpoint>\n"
"\n"
"Mount a filesystem of a given type to a mountpoint.\n"
"<device> can be one of /dev/* or some arbitrary string if no\n"
"device is needed for this driver (for example ramfs).\n"
"<fstype> is the filesystem driver to use. Try the 'devinfo' command\n"
"for a list of available drivers.\n"
"<mountpoint> must be an empty directory descending directly from the\n"
"root directory.\n";

U_BOOT_CMD_START(mount)
	.maxargs	= 4,
	.cmd		= do_mount,
	.usage		= "mount a filesystem to a device",
	U_BOOT_CMD_HELP(cmd_mount_help)
U_BOOT_CMD_END