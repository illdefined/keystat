#pragma once
#ifndef CELL_H
#define CELL_H

#include <stdint.h>

#define MAX 58
#define NUM (MAX * MAX * MAX)
#define SIZE (NUM * sizeof (struct Cell))

#pragma pack(push)
#pragma pack(4)
struct __attribute__((__packed__)) Cell {
	uint32_t num;
	uint32_t avg;
};
#pragma pack(pop)

#endif
