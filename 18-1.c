#include "prelude.h"

static const char *example =
	"R 6 (#70c710)\n"
	"D 5 (#0dc571)\n"
	"L 2 (#5713f0)\n"
	"D 2 (#d2c081)\n"
	"R 2 (#59c680)\n"
	"D 2 (#411b91)\n"
	"L 5 (#8ceee2)\n"
	"U 2 (#caa173)\n"
	"L 1 (#1b58a2)\n"
	"U 2 (#caa171)\n"
	"R 2 (#7807d2)\n"
	"U 3 (#a77fa3)\n"
	"L 2 (#015232)\n"
	"U 2 (#7a21e3)\n";
static const int expected = 62;

#define MAX_PATH 1000

typedef struct {
	i32 x, y;
} Pos;

typedef struct {
	Pos steps[MAX_PATH];
	u32 len;
	size_t n;
} Path;

static Path path;

static void push(Path *path, const Pos *pos) {
	assert(path->n < ASZ(path->steps));
	path->steps[path->n++] = *pos;
}

/// Calculate the area of a polygon
static u64 shoelace(const Path *path) {
	size_t n = path->n;
	const Pos *ps = path->steps;
	i64 sum = 0;
	for (size_t i = 0; i < n; i++) {
		sum += ps[i].x * (ps[i + 1 % n].y - ps[i - 1 % n].y);
	}
	return (u64)(sum > 0 ? sum : -sum) / 2;
}

/// Calculate the number of integer coordinates inside of a polygon
static u64 interior(const Path *path) {
	u64 area = shoelace(path);
	return area - path->len / 2 + 1;
}

/// Calculate number of integer points covered by a polygon
static u64 area(const Path *path) {
	return interior(path) + path->len;
}

static int solve(const char *const input) {
	path.len = 0;
	path.n = 0;
	const char *s = input;
	Pos pos = {0};
	push(&path, &pos);
	while (*s) {
		char d = *s;
		s += 2;
		int nn = atoi(s);
		s = strchr(s, '\n') + 1;
		assert(nn > 0);
		size_t n = (size_t)nn;
		path.len += n;
		switch (d) {
		case 'U': pos.y -= n; break;
		case 'D': pos.y += n; break;
		case 'L': pos.x -= n; break;
		case 'R': pos.x += n; break;
		}
		push(&path, &pos);
	}

	return (int)area(&path);
}

int main(void) {
	const int actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %d actual %d\n", expected, actual);
		return 1;
	} else printf("PASS!\n\n");
	printf("%d\n", solve(readToString("18.txt")));
	return 0;
}
