#include "prelude.h"

static const char *example =
	"Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green\n"
	"Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue\n"
	"Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red\n"
	"Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red\n"
	"Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green\n";

static const int expected = 2286;

static int mins[] = {
	['r'] = 0,
	['g'] = 0,
	['b'] = 0,
};

static void consumeWord(const char **const input) {
	while (**input && **input != ' ' && **input != '\n') (*input)++;
	while (**input && **input == ' ') (*input)++;
}

static uchar parseColor(const char **const input) {
	uchar c = (uchar) * *input;
	consumeWord(input);
	return c;
}

static int parseNumber(const char **const input) {
	int n = 0;
	for (; **input && **input != ' ' && **input != '\n'; (*input)++) {
		n *= 10;
		n += **input - '0';
	}
	if (n) consumeWord(input);
	return n;
}

static void consumeLine(const char **const input) {
	while (**input && **input != '\n') (*input)++;
	while (**input && **input == '\n') (*input)++;
}

static int solve(const char *input) {
	int power = 0;
	while (*input) {
		memset(mins, 0, sizeof mins);
		consumeWord(&input); // "Game "
		consumeWord(&input); // "1: "
		while (*input && *input != '\n') {
			const int n = parseNumber(&input);
			const uchar color = parseColor(&input);
			if (n > mins[color]) mins[color] = n;
			if (*input == '\n') {
				consumeLine(&input);
				break;
			}
		}
		power += mins['r'] * mins['g'] * mins['b'];
	}
	return power;
}

int main(void) {
	const int actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %d != actual %d\n", expected, actual);
		return 1;
	}
	printf("Result: %d\n", solve(readToString("2.txt")));
	return 0;
}
