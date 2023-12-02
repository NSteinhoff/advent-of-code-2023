#include "prelude.h"

static const bool verbose = false;

static const char *example =
	"Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green\n"
	"Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue\n"
	"Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red\n"
	"Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red\n"
	"Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green\n";

static const int expected = 8;

static const char *r = "red";
static const char *g = "green";
static const char *b = "blue";

static void consume_word(const char **const input) {
	while (**input && **input != ' ' && **input != '\n') {
		(*input)++;
	}
	while (**input && **input == ' ') {
		(*input)++;
	}
}

static const char *parse_color(const char **const input) {
	const char *color = **input == 'r' ? r
	                  : **input == 'g' ? g
	                  : **input == 'b' ? b
	                                   : NULL;

	if (color) {
		consume_word(input);
	}

	return color;
}

static int parse_number(const char **const input) {
	int n = 0;

	for (; **input && **input != ' ' && **input != '\n'; (*input)++) {
		n *= 10;
		n += **input - '0';
	}

	if (n) {
		consume_word(input);
	}

	return n;
}

static void consume_line(const char **const input) {
	while (**input && **input != '\n') {
		(*input)++;
	}

	while (**input && **input == '\n') {
		(*input)++;
	}
}

static int line_length(const char *input) {
	int len = 0;
	for(; *input != '\n'; input++) {
		len++;
	}
	return len;
}

static int solve(const char *input) {
	if (verbose) {
		printf("Input:\n%s\n", input);
	}

	static const int r_max = 12;
	static const int g_max = 13;
	static const int b_max = 14;

	int sum = 0;
	for (int game = 1; *input; game++) {
		if (verbose) {
			printf("Game: %d: '%.*s'\n", game,
			       line_length(input), input);
		}
		bool impossible = false;

		consume_word(&input); // "Game "
		consume_word(&input); // "1: "

		while (*input && !impossible && *input != '\n') {
			if (verbose) {
				printf("Draw: %.*s\n",
				       line_length(input),
				       input);
			}

			const int n = parse_number(&input);
			assert(n && "Unable to parse number!");
			const char *const color = parse_color(&input);
			assert(color && "Unable to parse color!");

			if (verbose) {
				printf("Drew: %d %s\n", n, color);
			}

			const int max = color == r ? r_max
			              : color == g ? g_max
			              : color == b ? b_max
			                           : 0;
			assert(max && "Invalid color!");

			if (n > max) {
				if (verbose) {
					printf("IMPOSSIBLE: %d %s > %d max\n",
					       n, color, max);
				}

				impossible = true;
			}

			if (impossible || *input == '\n') {
				consume_line(&input);
				break;
			}
		}

		if (!impossible) {
			sum += game;
		}
	}

	return sum;
}

int main(void) {
	const int actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %d != actual %d\n", expected, actual);

		return 1;
	}

	printf("Result: %d\n", solve(read_to_string("2.txt")));
	return 0;
}
