/* For posix_(fallocate|madvise) */
#define _XOPEN_SOURCE 600

#include <arpa/inet.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <limits.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/input.h>

#include "keystat.h"

/* Issue standard error message and terminate */
static inline void die(const char *restrict str) {
	perror(str);
	exit(EXIT_FAILURE);
}

/* Time difference in microseconds */
static inline uint32_t diff(const struct timeval *restrict min, const struct timeval *restrict sub) {
	return (uint32_t)
		(((intmax_t) min->tv_sec  - (intmax_t) sub->tv_sec) * INTMAX_C(1000000) +
		 ((intmax_t) min->tv_usec - (intmax_t) sub->tv_usec));
}

int main(int argc, char *argv[]) {
	struct input_event ev[3];
	unsigned char idx = 0;
	unsigned char fil = 0;

	/* Check command-line arguments */
	if (argc != 3) {
		fprintf(stderr,
			"Usage: %s [map] [device]\n\n"
			"  This program measures the time difference between consecutive key events read\n"
			"  from device and stores their average per key triple in map.\n",
			argv[0]);
		return EXIT_FAILURE;
	}

	/* Open event device */
	int ifd = open(argv[2], O_RDONLY);
	if (ifd < 0)
		die("open");

	/* Open map file */
	int mfd = open(argv[1], O_RDWR | O_CREAT, 0660);
	if (mfd < 0)
		die("open");

	/* Truncate map file to the correct size */
	if (ftruncate(mfd, SIZE))
		die("ftruncate");

	/* Allocate file space */
	if (posix_fallocate(mfd, 0, SIZE))
		die("posix_fallocate");

	/* Memory-map it */
	struct Cell *map = mmap((void *) 0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mfd, 0);
	if (map == MAP_FAILED)
		die("mmap");

	/* We do not need this anymore */
	if (close(mfd))
		die("close");

	/* We will need the pages immediately */
	if (posix_madvise(map, SIZE, POSIX_MADV_RANDOM | POSIX_MADV_WILLNEED))
		die("posix_madvise");

	/* Set real-time scheduling policy */
	struct sched_param param;
	param.sched_priority = sched_get_priority_max(SCHED_FIFO);
	if (sched_setscheduler(0, SCHED_FIFO, &param) == -1)
		die("sched_setscheduler");

	for (;;) {
		do {
			if (read(ifd, ev + idx, sizeof (struct input_event)) < sizeof (struct input_event))
				die("read");
		} while (ev[idx].type != EV_KEY || ev[idx].value != 1);

		/* Ignore differences larger than 300ms and invalid key codes */
		if (ev[idx].code >= MAX || (fil > 0 && diff(&ev[idx].time, &ev[(idx + 2) % 3].time) > 300000))
			fil = 0;
		else if (fil < 3)
			++fil;

		if (fil == 3) {
			/* Calculate offset and update average */
			struct Cell *cur = map
				+ ev[idx].code
				+ ev[(idx + 2) % 3].code * MAX
				+ ev[(idx + 1) % 3].code * MAX * MAX;

			if (ntohl(cur->num) == UINT32_MAX) {
				puts("Enough data :-)");
				return EXIT_SUCCESS;
			}

			cur->num = htonl(ntohl(cur->num) + 1);

			uint32_t df =
				diff(          &ev[idx].time, &ev[(idx + 2) % 3].time) +
				diff(&ev[(idx + 2) % 3].time, &ev[(idx + 1) % 3].time);

			/* As we are using unsigned types, we have to differentiate */
			if (df > ntohl(cur->avg))
				cur->avg = htonl(ntohl(cur->avg) + (df - ntohl(cur->avg)) / ntohl(cur->num));
			else if (df < ntohl(cur->avg))
				cur->avg = htonl(ntohl(cur->avg) - (ntohl(cur->avg) - df) / ntohl(cur->num));

#ifdef DEBUG
			printf("%.2hu %.2hu %.2hu: %.6uus\n",
				ev[(idx + 1) % 3].code, ev[(idx + 2) % 3].code, ev[idx].code, df);
#endif
		}

		idx = (idx + 1) % 3;
	}

	return EXIT_SUCCESS;
}
