#include "prelude.h"

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
static const int expected = 136;

#define AT(R, C) input[(R) * (cols + 1) + (C)]

static int solve(const char *const input) {
	(void)input;
	size_t cols = (size_t)(strchr(input, '\n') - input);
	size_t rows = strlen(input) / (cols + 1);
	printf("%zu, %zu\n", rows, cols);
	size_t load = 0;
	for (size_t c = 0; c < cols; c++) {
		size_t stack = 0;
		size_t stop = 0;
		for (size_t r = 0; r < rows; r++) {
			char s = AT(r, c);
			if (s == '#') {
				stack = 0;
				stop = r + 1;
			}
			if (s == 'O') {
				load += rows - (stop + stack);
				stack++;
			}
		}
	}

	return (int)load;
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
