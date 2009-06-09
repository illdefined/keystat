/* For posix_madvise */
#define _XOPEN_SOURCE 600

#include <arpa/inet.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "keystat.h"

/* Issue standard error message and terminate */
static inline void die(const char *restrict str) {
	perror(str);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	/* Check command-line arguments */
	if (argc != 2) {
		fprintf(stderr,
			"Usage: %s [map]\n\n"
			"  This program dumps the contents of map.\n",
			argv[0]);
		return EXIT_FAILURE;
	}

	/* Open map file */
	int fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		die("open");

	/* Check file size */
	if (lseek(fd, 0, SEEK_END) != SIZE) {
		fputs("Invalid file size!\n", stderr);
		return EXIT_FAILURE;
	}

	/* Memory-map it */
	struct Cell *map = mmap((void *) 0, SIZE, PROT_READ, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED)
		die("mmap");

	/* We do not need this anymore */
	if (close(fd))
		die("close");

	/* We will need the pages immediately */
	if (posix_madvise(map, SIZE, POSIX_MADV_SEQUENTIAL | POSIX_MADV_WILLNEED))
		die("posix_madvise");

	for (unsigned char iter = 0; iter < MAX; ++iter) {
		for (unsigned char jter = 0; jter < MAX; ++jter) {
			for (unsigned char kter = 0; kter < MAX; ++kter) {
				struct Cell *cur = map
					+ iter * MAX * MAX
					+ jter * MAX
					+ kter;
				printf("%.2hhu %.2hhu %.2hhu %.10u %.6u\n",
					iter, jter, kter, ntohl(cur->num), ntohl(cur->avg));
					
			}
		}
	}

	return EXIT_SUCCESS;
}
