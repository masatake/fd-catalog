/*
 * MIT License
 * 
 * Copyright (c) 2021 Masatake YAMATO
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "config.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define CLOSED -3
#define SKIP   -2

struct factory {
	const char *name;
	const char *desc;
	bool need_privilege;
	int fd;
	int fd2;
	int e;
	int (* open)(struct factory *f);
};

static int
open_ro_regular_file(struct factory *f)
{
	return open("/etc/passwd", O_RDONLY);
}

static int
open_ro_directory(struct factory *f)
{
	return open("/", O_RDONLY|O_DIRECTORY);
}

static int
open_pipe(struct factory *f)
{
	int pd[2];
	int r = pipe(pd);
	if (r < 0)
		return r;
	f->fd2 = pd[1];
	return pd[0];
}

static int
open_socketpair(struct factory *f,
		int domain, int type, int protocol)
{
	int sv[2];
	int r = socketpair(domain, type, protocol, sv);
	if (r < 0)
		return r;
	f->fd2 = sv[1];
	return sv[0];
}

static int
open_socketpair_unix_stream(struct factory *f)
{
	return open_socketpair(f, AF_UNIX, SOCK_STREAM, 0);
}

static int
open_socketpair_unix_dgram(struct factory *f)
{
	return open_socketpair(f, AF_UNIX, SOCK_DGRAM, 0);
}

static int
open_socketpair_unix_seqpacket(struct factory *f)
{
	return open_socketpair(f, AF_UNIX, SOCK_SEQPACKET, 0);
}

static struct factory factories[] = {
	{
		.name = "ro-regular-file",
		.desc = "read-only regular file (/etc/passwd)",
		.open = open_ro_regular_file,
	},
	{
		.name = "ro-directory",
		.desc = "read-only directory (/)",
		.open = open_ro_directory,
	},
	{
		.name = "pipe",
		.desc = "pipe",
		.open = open_pipe,
	},
	{
		.name = "socketpair-unix-stream",
		.desc = "unix-stream sockets created with socketpair",
		.open = open_socketpair_unix_stream,
	},
	{
		.name = "socketpair-unix-dgram",
		.desc = "unix-dgram sockets created with socketpair",
		.open = open_socketpair_unix_dgram,
	},
	{
		.name = "socketpair-unix-seqpacket",
		.desc = "unix-seqpacket sockets created with socketpair",
		.open = open_socketpair_unix_seqpacket,
	},	
};

static void
do_open(struct factory *f)
{
	f->fd = f->open(f);
	if (f->fd < 0)
		f->e = errno;
}

static void
do_report(struct factory *f)
{
	switch (f->fd) {
	case SKIP:
		printf("%32s...skipped\n", f->name);
		break;
	case -1:
		printf("%32s...error (%s)\n", strerror(f->e));
		break;
	default:
		if (f->fd2 >= 0)
			printf("%32s...%d, %d\n", f->name, f->fd, f->fd2);
		else
			printf("%32s...%d\n", f->name, f->fd);
		break;
	}
}

int
main(int argc, char **argv)
{
	bool root = (getuid()  == 0);
	size_t count = sizeof(factories) / sizeof(factories[0]);

	for (size_t i = 0; i < count; i++) {
		factories[i].fd  = CLOSED;
		factories[i].fd2 = CLOSED;		
	}

	for (size_t i = 0; i < count; i++) {
		struct factory *f = factories + i;
		if (f->fd != CLOSED)
			continue;
		if ((f->need_privilege && !root)
		    || !f->open) {
			f->fd = SKIP;
			continue;
		}
		do_open(f);
		do_report(f);
	}

	pause();
	return 0;
}

/* Local Variables:  */
/* mode: c           */
/* c-file-style: "linux" */
/* End:              */
