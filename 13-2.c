#include "prelude.h"

#define MAX_MAPS 100

static const char *example =
	"#.##..##.\n"
	"..#.##.#.\n"
	"##......#\n"
	"##......#\n"
	"..#.##.#.\n"
	"..##..##.\n"
	"#.#.##.#.\n"
	"\n"
	"#...##..#\n"
	"#....#..#\n"
	"..##..###\n"
	"#####.##.\n"
	"#####.##.\n"
	"..##..###\n"
	"#....#..#\n";
static const int expected = 400;
static bool verbose = true;

typedef struct {
	const char *s;
	size_t rows, cols;
} Map;

static Map maps[MAX_MAPS];
static size_t n_maps = 0;

#define AT(M, Y, X) (M)->s[(Y) * (M)->cols + (X) + (Y)]

static void printMap(const Map *map) {
	if (!verbose) return;
	printf("\nRows: %zu Cols: %zu\n", map->rows, map->cols);
	printf("   ");
	for (u8 i = 0; i < map->cols; i++) printf("%2d|", i + 1);
	printf("\n   ");
	for (u8 i = 0; i < map->cols; i++) printf("---");
	printf("\n");
	for (u8 i = 0; i < map->rows; i++) {
		printf("%2d|", i + 1);
		for (u8 j = 0; j < map->cols; j++)
			printf(" %c%c", AT(map, i, j),
			       j == map->cols - 1 ? '|' : ' ');
		printf("\n");
	}
}

static void parseMaps(const char *s) {
	while (s) {
		assert(n_maps < ASZ(maps));
		Map *map = &maps[n_maps++];
		*map = (Map){.s = s, .cols = strcspn(s, "\n")};
		s = strstr(map->s, "\n\n");
		map->rows = (s ? (size_t)(s + 1 - map->s) : strlen(map->s)) /
		            (map->cols + 1);
		if (s) s += 2;
	}
}

static u8 mirrorCol(const Map *map, u8 col) {
	u8 errors = 0;
	for (u8 d = 0; d < map->cols; d++) {
		u8 a = col - d;
		u8 b = col + d + 1;
		if (a >= map->cols || b >= map->cols) break;
		for (u8 row = 0; row < map->rows; row++) {
			if (AT(map, row, a) != AT(map, row, b)) errors++;
			if (errors > 1) return false;
		}

	}
	return errors == 1;
}

static u8 mirrorRow(const Map *map, u8 row) {
	u8 errors = 0;
	for (u8 d = 0; d < map->rows; d++) {
		u8 a = row - d;
		u8 b = row + d + 1;
		if (a >= map->rows || b >= map->rows) break;
		for (u8 col = 0; col < map->cols; col++) {
			if (AT(map, a, col) != AT(map, b, col)) errors++;
			if (errors > 1) return false;
		}
	}
	return errors == 1;
}

static int solve(const char *const input) {
	n_maps = 0;
	parseMaps(input);
	printf("N: %zu\n", n_maps);
	int total = 0;

	for (size_t i = 0; i < n_maps; i++) {
		Map *map = &maps[i];
		printMap(map);
		for (u8 col = 0; col < map->cols - 1; col++) {
			if (mirrorCol(map, col)) {
				total += col + 1;
				printf("Mirror at col %u\n", col + 1);
				break;
			}
		}
		for (u8 row = 0; row < map->rows - 1; row++) {
			if (mirrorRow(map, row)) {
				total += 100 * (row + 1);
				printf("Mirror at row %u\n", row + 1);
				break;
			}
		}
	}

	return total;
}

int main(void) {
	const int actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %d actual %d\n", expected, actual);
		return 1;
	} else printf("PASS!\n\n");
	printf("%d\n", solve(readToString("13.txt")));
	return 0;
}
