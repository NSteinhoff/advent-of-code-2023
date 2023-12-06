#include "prelude.h"

#define MAX_RACES 8

static const char *const example =
	"Time:      7  15   30\n"
	"Distance:  9  40  200\n";

static const uint64_t expected = 71503;

static uint64_t solve(const char *const input) {
	uint64_t time = 0;
	uint64_t distance = 0;

	const char *cursor = input;
	while (*cursor != '\n') {
		if (isdigit(*cursor)) {
			time *= 10;
			time += (uint64_t)*cursor - '0';
		}
		cursor++;
	}
	cursor++;
	while (*cursor != '\n') {
		if (isdigit(*cursor)) {
			distance *= 10;
			distance += (uint64_t)*cursor - '0';
		}
		cursor++;
	}

	printf("Time: %lld\nDistance: %lld\n", time, distance);

	uint64_t n = 0;
	for (uint64_t s = 0; s < time; s++) {
		uint64_t t = time - s;
		uint64_t d = s * t;
		if (d > distance) {
			n++;
		}
	}

	return n;
}

int main(void) {
	const uint64_t actual = solve(example);

	if (actual != expected) {
		printf("FAIL: expected %lld != actual %lld\n", expected,
		       actual);

		return 1;
	}

	printf("Result: %lld\n", solve(readToString("6.txt")));

	return 0;
}
