#include "prelude.h"

static const char *example =
	"1,0,1~1,2,1\n"
	"0,0,2~2,0,2\n"
	"0,2,3~2,2,3\n"
	"0,0,4~0,2,4\n"
	"2,0,5~2,2,5\n"
	"0,1,6~2,1,6\n"
	"1,1,8~1,1,9\n";

static const int expected = 5;
#define MAX_BRICKS (1400)

typedef struct {
	size_t x, y, z;
} Pos;

typedef struct {
	Pos start, end;
} Brick;

static int solve(const char *const input) {
	static Brick bricks[MAX_BRICKS];

	size_t N = 0;
	for (const char *line = input; *line; line = strchr(line, '\n') + 1) {
		assert(N < ASZ(bricks));
		Brick *b = &bricks[N++];
		sscanf(line, "%zu,%zu,%zu~%zu,%zu,%zu\n", &b->start.x,
		       &b->start.y, &b->start.z, &b->end.x, &b->end.y,
		       &b->end.z);
	}
	printf("N: %zu\n", N);

	return 42;
}

int main(void) {
	const int actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %d actual %d\n", expected, actual);
		return 1;
	} else printf("PASS!\n\n");
	printf("%d\n", solve(readToString("21.txt")));
	return 0;
}
