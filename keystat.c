/* For posix_(fallocate|madvise) */
#define _XOPEN_SOURCE 600

#include <arpa/inet.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <limits.h>
#include <sched.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/input.h>

#define NUM (256 * 256)
#define SIZE (NUM * sizeof (struct Cell))

#define inv (idx ? 0 : 1)

#define readkey() \
	do { \
		if (read(ifd, ev + idx, sizeof (struct input_event)) < sizeof (struct input_event)) \
			die("read"); \
	} while (ev[idx].type != EV_KEY || ev[idx].value != 1 || ev[idx].code > 255);

#pragma pack(push)
#pragma pack(4)
struct Cell {
	uint32_t num;
	uint32_t avg;
};
#pragma pack(pop)

/* Issue standard error message and terminate */
static inline void die(const char *restrict str) {
	perror(str);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	struct input_event ev[2];
	unsigned char idx = 0;

	/* Check command-line arguments */
	if (argc != 3) {
		fprintf(stderr,
			"Usage: %s [map] [device]\n\n"
			"  This program measures the time difference between consecutive key events read\n"
			"  from device and stores their average per key tuple in map.\n",
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

	/* Wait for first key press */
	readkey();
	idx = inv;

	for (;;) {
		readkey();

		uint32_t diff =
			(ev[idx].time.tv_sec * 1000 + ev[idx].time.tv_usec / 1000) -
			(ev[inv].time.tv_sec * 1000 + ev[inv].time.tv_usec / 1000);

		/* Ignore differences larger than 300ms */
		if (diff <= 300) {
			/* Calculate offset and update average */
			struct Cell *cur = map + ev[idx].code + ev[inv].code * 256;

			if (ntohl(cur->num) == UINT32_MAX) {
				puts("Enough data :-)");
				return EXIT_SUCCESS;
			}

			cur->num = htonl(ntohl(cur->avg) + 1);

			/* As we are using unsigned types, we have to differentiate */
			if (diff > ntohl(cur->avg))
				cur->avg = htonl(ntohl(cur->avg) + (diff - ntohl(cur->avg)) / ntohl(cur->num));
			else if (diff < ntohl(cur->avg))
				cur->avg = htonl(ntohl(cur->avg) - (ntohl(cur->avg) - diff) / ntohl(cur->num));

#ifndef NDEBUG
			printf("%hhu -> %hhu: %lums\n", ev[inv].code, ev[idx].code, diff);
#endif
		}
		
		idx = inv;
	}

	return EXIT_SUCCESS;
}
