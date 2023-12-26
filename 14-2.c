#include "prelude.h"

// 100 rows * (100 cols + \n) + \0
#define GRID_SIZE 100 * 101 + 1
#define MAX_GRIDS 256

static const char *example =
	"O....#....\n"
	"O.OO#....#\n"
	".....##...\n"
	"OO.#O....O\n"
	".O.....O#.\n"
	"O.#..O.#.#\n"
	"..O..#O..O\n"
	".......O..\n"
	"#....###..\n"
	"#OO..#....\n";
static const int expected = 64;

static const size_t cycles = 1000000000;

static char grids[MAX_GRIDS][GRID_SIZE];
static size_t n_grids = 0;

#define AT(R, C) grid[(R) * (cols + 1) + (C)]
#define PUSH(G)                                                               \
	do {                                                                   \
		assert(n_grids < MAX_GRIDS);                                   \
		strcpy(grids[n_grids++], (G));                                 \
	} while (0)
#define GET() grids[n_grids - 1]

static size_t findPreviousIndex(const char *grid) {
	assert(n_grids > 1);
	for (size_t i = 0; i < (size_t)n_grids - 1; i++)
		if (!strcmp(grid, grids[i])) return i;
	return 0;
}

static void roll(char *grid, size_t rows, size_t cols, char d) {
	switch (d) {
	case 'N':
		for (size_t c = 0; c < cols; c++) {
			size_t os = 0;
			size_t bs = 0;
			for (size_t r = 0; r < rows; r++) switch (AT(r, c)) {
				case '#':
					os = 0;
					bs = r + 1;
					break;
				case 'O':
					AT(r, c) = '.';
					AT(bs + os, c) = 'O';
					os++;
					break;
				}
		}
		break;
	case 'W':
		for (size_t r = 0; r < rows; r++) {
			size_t os = 0;
			size_t bs = 0;
			for (size_t c = 0; c < cols; c++) {
				switch (AT(r, c)) {
				case '#':
					os = 0;
					bs = c + 1;
					break;
				case 'O':
					AT(r, c) = '.';
					AT(r, bs + os) = 'O';
					os++;
					break;
				}
			}
		}
		break;
	case 'S':
		for (size_t c = 0; c < cols; c++) {
			size_t os = 0;
			size_t bs = rows - 1;
			for (size_t r = rows - 1; r < rows; r--) {
				switch (AT(r, c)) {
				case '#':
					os = 0;
					bs = r - 1;
					break;
				case 'O':
					AT(r, c) = '.';
					AT(bs - os, c) = 'O';
					os++;
					break;
				}
			}
		}
		break;
	case 'E':
		for (size_t r = 0; r < rows; r++) {
			size_t os = 0;
			size_t bs = cols - 1;
			for (size_t c = cols - 1; c < cols; c--) {
				switch (AT(r, c)) {
				case '#':
					os = 0;
					bs = c - 1;
					break;
				case 'O':
					AT(r, c) = '.';
					AT(r, bs - os) = 'O';
					os++;
					break;
				}
			}
		}
		break;
	}
}

static int solve(const char *const input) {
	assert(strlen(input) < sizeof(grids[0]));
	printf("Storage: %zu KB\n", sizeof(grids) / (1 << 10));
	n_grids = 0;
	PUSH(input);
	char *grid = GET();

	size_t cols = (size_t)(strchr(grid, '\n') - grid);
	size_t rows = strlen(grid) / (cols + 1);

	size_t cycle_start = 0;
	size_t cycle_end = 0;
	const char ds[] = {'N', 'W', 'S', 'E'};
	for (size_t cycle = 0; cycle < cycles; cycle++) {
		PUSH(grid);
		grid = GET();
		for (size_t d = 0; d < 4; d++) roll(grid, rows, cols, ds[d]);
		cycle_start = findPreviousIndex(grid);
		cycle_end = cycle + 1;
		if (cycle_start > 0) break;
	}

	assert(cycle_start > 0);
	printf("Cycle from %zu to %zu\n", cycle_start, cycle_end);
	size_t final_index =
		(cycles - cycle_start) % (cycle_end - cycle_start) +
		cycle_start;
	assert(0 <= final_index && (size_t)final_index < n_grids);
	grid = grids[final_index];

	int load = 0;
	for (size_t r = 0; r < rows; r++)
		for (size_t c = 0; c < cols; c++)
			if (AT(r, c) == 'O') load += rows - r;
	return load;
}

int main(void) {
	const int actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %d actual %d\n", expected, actual);
		return 1;
	} else printf("PASS!\n\n");
	printf("%d\n", solve(readToString("14.txt")));
	return 0;
}
