#include "prelude.h"

static const char *example =
	"two1nine\n"
	"eightwothree\n"
	"abcone2threexyz\n"
	"xtwone3four\n"
	"4nineeightseven2\n"
	"zoneight234\n"
	"7pqrstsixteen\n";

static const char *numbers[] = {
	"zero", "one", "two",   "three", "four",
	"five", "six", "seven", "eight", "nine",
};

static char getDigit(const char *text) {
	if (isdigit(*text)) return *text;
	for (unsigned char i = 0; i < ASZ(numbers); i++)
		if (!strncmp(text, numbers[i], strlen(numbers[i])))
			return '0' + i;
	return -1;
}

static int solve(const char *input) {
	int sum = 0;
	while (*input) {
		char digits[3] = {0};
		for (u32 i = 0; *input && *input != '\n'; input++) {
			const char digit = getDigit(input);
			if (digit < 0) continue;
			if (!i) digits[i++] = digit;
			digits[i] = digit;
		}
		sum += atoi(digits);
		input++;
	}
	return sum;
}

int main(void) {
	const int example_result = solve(example);
	if (example_result != 281) {
		printf("FAIL: expected 281 actual %d\n", example_result);
		return 1;
	}
	printf("Result: %d\n", solve(readToString("1.txt")));
	return 0;
}
