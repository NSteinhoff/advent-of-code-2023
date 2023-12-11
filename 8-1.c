#include "prelude.h"

#define MAX_NODES 1024

static const char *const example =
	"LLR\n"
	"\n"
	"AAA = (BBB, BBB)\n"
	"BBB = (AAA, ZZZ)\n"
	"ZZZ = (ZZZ, ZZZ)\n";

static const int expected = 6;


static uint solve(const char *const input) {
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

	uint dst = 0;
	uint start = 0;
	for (uint i = 0; i < n_nodes; i++) {
		if (!strncmp(nodes[i], "AAA", 3)) {
			start = i;
		}
		if (!strncmp(nodes[i], "ZZZ", 3)) {
			dst = i;
		}
	}

	printf("Start: %3u %.3s\n", start, nodes[start]);
	printf("Dst:   %3u %.3s\n", dst, nodes[dst]);

	uint n = 0;
	for (uint current = start; current != dst; n++) {
		char step = steps[n % n_steps];
		uint next = connections[current][step == 'R'];
		current = next;
	}

	return n;
}

int main(void) {
	const int actual = (int)solve(example);

	if (actual != expected) {
		printf("FAIL: expected %d != actual %d\n", expected, actual);

		return 1;
	}

	printf("\n===\n");
	printf("Result: %u\n", solve(readToString("8.txt")));

	return 0;
}
