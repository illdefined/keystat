/* For posix_madvise */
#define _XOPEN_SOURCE 600

#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "cell.h"
#include "map.h"

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
	struct Cell *map = map_open(argv[1], false, true);

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
