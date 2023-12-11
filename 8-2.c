#include "prelude.h"

#define MAX_NODES 1024
#define MAX_PATHS 8

static const char *const example =
	"LR\n"
	"\n"
	"11A = (11B, XXX)\n"
	"11B = (XXX, 11Z)\n"
	"11Z = (11B, XXX)\n"
	"22A = (22B, XXX)\n"
	"22B = (22C, 22C)\n"
	"22C = (22Z, 22Z)\n"
	"22Z = (22B, 22B)\n"
	"XXX = (XXX, XXX)\n";

static const int expected = 6;

static uint64_t gcd(uint64_t a, uint64_t b) {
	while (b != 0) {
		uint64_t temp = b;
		b = a % b;
		a = temp;
	}
	return a;
}

static uint64_t lcm(uint64_t a, uint64_t b) {
	return (a * b) / gcd(a, b);
}

static uint64_t lcmArray(const uint64_t arr[], uint n) {
	uint64_t result = arr[0];

	for (uint64_t i = 1; i < n; i++) {
		result = lcm(result, arr[i]);
	}

	return result;
}

static uint64_t solve(const char *const input) {
	const char *cursor = input;
	const char *steps = cursor;
	cursor = strchr(cursor, '\n');
	const uint n_steps = (uint)(cursor - steps);
	while (*cursor == '\n') cursor++;

	uint n_nodes = 0;
	static const char *nodes[MAX_NODES];
	for (; *cursor; cursor++) {
		assert(n_nodes < MAX_NODES);
		nodes[n_nodes++] = cursor;
		cursor = strchr(cursor, '\n');
	}

	printf("Nodes:\n");
	static uint connections[MAX_NODES][2];
	for (uint i = 0; i < n_nodes; i++) {
		char *left = strchr(nodes[i], '(') + 1;
		char *right = strchr(nodes[i], ')') - 3;

		bool found_left = false;
		bool found_right = false;
		for (uint j = 0; j < n_nodes; j++) {
			if (!found_left && !strncmp(nodes[j], left, 3)) {
				found_left = true;
				connections[i][0] = j;
			}
			if (!found_right && !strncmp(nodes[j], right, 3)) {
				found_right = true;
				connections[i][1] = j;
			}
			if (found_left && found_right) {
				break;
			}
		}

		assert(found_left && found_right);
		printf("%3u: %.3s (%3u %.3s, %3u %.3s)\n", i, nodes[i],
		       connections[i][0], left, connections[i][1], right);
	}

	uint n_paths = 0;
	uint paths[MAX_PATHS];
	for (uint i = 0; i < n_nodes; i++) {
		assert(n_paths < MAX_PATHS);
		if (nodes[i][2] == 'A') {
			paths[n_paths++] = i;
		}
	}

	printf("Start\n");
	for (uint i = 0; i < n_paths; i++) {
		printf("- %3u %.3s\n", paths[i], nodes[paths[i]]);
	}

	uint64_t path_steps[MAX_PATHS] = {0};
	for (uint i = 0; i < n_paths; i++) {
		uint n = 0;
		while (nodes[paths[i]][2] != 'Z') {
			char step = steps[n++ % n_steps];
			paths[i] = connections[paths[i]][step == 'R'];
		}
		path_steps[i] = n;
	}

	return lcmArray(path_steps, n_paths);
}

int main(void) {
	const int actual = (int)solve(example);

	if (actual != expected) {
		printf("FAIL: expected %d != actual %d\n", expected, actual);

		return 1;
	}

	printf("\n===\n");
	printf("Result: %llu\n", solve(readToString("8.txt")));

	return 0;
}
