/*-
 * Copyright (c) 2015 Y. Burmeister <yamagi@yamagi.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * ==========================================================================
 *
 * Simple tool to TRIM whole disks or partitions on FreeBSD. Make sure you've
 * got a backup or the data isn't needed anymore. Also don't apply this on
 * mounted partitions and / or disks, it may panic your kernel. :)
 *
 * Usage:
 *  ./trimdisk <device>
 *
 * ==========================================================================
 */

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/disk.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

void
usage(void)
{
	fprintf(stderr, "trimdisk <device>\n");
	exit(1);
}

int
main(int argc, char **argv)
{
	char *dev;
	double elapsed;
	int32_t fd;
	off_t trim[2];
	struct stat sb;
	struct timeval before, after;

	if (argc != 2)
	{
		usage();
	}

	dev = argv[1];
	trim[0] = 0;

	if ((stat(dev, &sb)) != 0)
	{
		fprintf(stderr, "Couldn't stat %s: %s\n", dev, strerror(errno));
		exit(1);
	}

	if (!S_ISCHR(sb.st_mode))
	{
		fprintf(stderr, "%s: Not a character device\n", dev);
		exit(1);
	}

	if ((fd = open(dev, O_RDWR)) < 0)
	{
		fprintf(stderr, "Couldn't open %s: %s\n", dev, strerror(errno));
		exit(1);
	}

	if (ioctl(fd, DIOCGMEDIASIZE, &trim[1]) == -1)
	{
		fprintf(stderr, "Couldn't get mediasize: %s\n", strerror(errno));
		exit(1);
	}

	gettimeofday(&before, NULL);

	if (ioctl(fd, DIOCGDELETE, trim) == -1)
	{
		fprintf(stderr, "Couldn't TRIM %li bytes: %s\n", trim[1], strerror(errno));
		exit(1);
	}

	gettimeofday(&after, NULL);

	elapsed = after.tv_sec - before.tv_sec + ((after.tv_usec - before.tv_usec) * 0.000001);
	printf("TRIMed %li bytes in %f seconds\n", trim[1], elapsed);

	close(fd);

	return 0;
}

