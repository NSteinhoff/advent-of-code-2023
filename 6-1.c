#include "prelude.h"

#define MAX_RACES 8

static const char *const example =
	"Time:      7  15   30\n"
	"Distance:  9  40  200\n";

static const int expected = 288;

typedef struct {
	int time, distance;
} Race;

static int solve(const char *input) {
	static Race races[MAX_RACES];
	uint8_t n_races = 0;

	size_t len = strcspn(input, "\n");
	const char *times = input;
	const char *distances = input + len + 1;

	while(*times != ' ' && *times != '\n') times++;
	while(*distances != ' ' && *distances != '\n') distances++;
	while (*times != '\n' && *distances != '\n') {
		while(*times == ' ') times++;
		while(*distances == ' ') distances++;

		races[n_races].time = atoi(times);
		races[n_races].distance = atoi(distances);
		n_races++;

		while(*times != ' ' && *times != '\n') times++;
		while(*distances != ' ' && *distances != '\n') distances++;
	}


	int total = 1;
	for (size_t i = 0; i < n_races; i++) {
		Race *r = races + i;
		int n = 0;
		for (int s = 0; s < r->time; s++) {
			int t = r->time - s;
			int d = s * t;
			if (d > r->distance) {
				n++;
			}
		}
		if (n) {
			total *= n;
		}
	}

	return total;
}

int main(void) {
	const int actual = solve(example);

	if (actual != expected) {
		printf("FAIL: expected %d != actual %d\n", expected, actual);

		return 1;
	}

	printf("Result: %d\n", solve(readToString("6.txt")));

	return 0;
}
