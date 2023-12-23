#include "prelude.h"

#define MAX_RACES 8

static const char *const example =
	"Time:      7  15   30\n"
	"Distance:  9  40  200\n";

static const u64 expected = 71503;

static u64 solve(const char *const input) {
	u64 time = 0;
	u64 distance = 0;

	const char *cursor = input;
	while (*cursor != '\n') {
		if (isdigit(*cursor)) {
			time *= 10;
			time += (u64)*cursor - '0';
		}
		cursor++;
	}
	cursor++;
	while (*cursor != '\n') {
		if (isdigit(*cursor)) {
			distance *= 10;
			distance += (u64)*cursor - '0';
		}
		cursor++;
	}

	printf("Time: %lld\nDistance: %lld\n", time, distance);

	u64 n = 0;
	for (u64 s = 0; s < time; s++) {
		u64 t = time - s;
		u64 d = s * t;
		if (d > distance) n++;
	}

	return n;
}

int main(void) {
	const u64 actual = solve(example);

	if (actual != expected) {
		printf("FAIL: expected %lld != actual %lld\n", expected,
		       actual);

		return 1;
	}

	printf("Result: %lld\n", solve(readToString("6.txt")));

	return 0;
}
