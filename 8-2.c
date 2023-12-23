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

static u64 gcd(u64 a, u64 b) {
	while (b != 0) {
		u64 temp = b;
		b = a % b;
		a = temp;
	}
	return a;
}

static u64 lcm(u64 a, u64 b) {
	return (a * b) / gcd(a, b);
}

static u64 lcmArray(const u64 arr[], u32 n) {
	u64 result = arr[0];

	for (u64 i = 1; i < n; i++) {
		result = lcm(result, arr[i]);
	}

	return result;
}

static u64 solve(const char *const input) {
	const char *cursor = input;
	const char *steps = cursor;
	cursor = strchr(cursor, '\n');
	const u32 n_steps = (u32)(cursor - steps);
	while (*cursor == '\n') cursor++;

	u32 n_nodes = 0;
	static const char *nodes[MAX_NODES];
	for (; *cursor; cursor++) {
		assert(n_nodes < MAX_NODES);
		nodes[n_nodes++] = cursor;
		cursor = strchr(cursor, '\n');
	}

	printf("Nodes:\n");
	static u32 connections[MAX_NODES][2];
	for (u32 i = 0; i < n_nodes; i++) {
		char *left = strchr(nodes[i], '(') + 1;
		char *right = strchr(nodes[i], ')') - 3;

		bool found_left = false;
		bool found_right = false;
		for (u32 j = 0; j < n_nodes; j++) {
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

	u32 n_paths = 0;
	u32 paths[MAX_PATHS];
	for (u32 i = 0; i < n_nodes; i++) {
		assert(n_paths < MAX_PATHS);
		if (nodes[i][2] == 'A') {
			paths[n_paths++] = i;
		}
	}

	printf("Start\n");
	for (u32 i = 0; i < n_paths; i++) {
		printf("- %3u %.3s\n", paths[i], nodes[paths[i]]);
	}

	u64 path_steps[MAX_PATHS] = {0};
	for (u32 i = 0; i < n_paths; i++) {
		u32 n = 0;
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
