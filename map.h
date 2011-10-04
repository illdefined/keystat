#pragma once
#ifndef MAP_H
#define MAP_H

/* For posix_madvise */
#define _XOPEN_SOURCE 600

#include <sys/mman.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cell.h"

/* Issue standard error message and terminate */
static inline void die(const char *restrict str) {
	perror(str);
	exit(EXIT_FAILURE);
}

static struct Cell *map_open(const char *restrict path, bool rw, bool seq) {
	int fd;

	/* Open map file */
	if (rw)
		fd = open(path, O_RDWR | O_CREAT, 0600);
	else
		fd = open(path, O_RDONLY);

	if (fd < 0)
		die("open");

	if (rw) {
		/* Lock the file */
		if (lockf(fd, F_TLOCK, SIZE))
			die("lockf");

		/* Truncate map file to the correct size */
		if (ftruncate(fd, SIZE))
			die("ftruncate");

		/* Allocate file space */
		if (posix_fallocate(fd, 0, SIZE))
			die("posix_fallocate");
	}
	else {
		if (lseek(fd, 0, SEEK_END) != SIZE) {
			fputs("Invalid file size!\n", stderr);
			exit(EXIT_FAILURE);
		}
	}

	/* Memory-map it */
	struct Cell *map = mmap((void *) 0, SIZE, rw ? PROT_READ | PROT_WRITE : PROT_READ, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED)
		die("mmap");

	/* We will need the pages immediately */
	if (posix_madvise(map, SIZE, seq ? POSIX_MADV_SEQUENTIAL | POSIX_MADV_WILLNEED : POSIX_MADV_RANDOM | POSIX_MADV_WILLNEED))
		die("posix_madvise");

	return map;
}

#endif
