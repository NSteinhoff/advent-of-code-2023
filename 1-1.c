#include "prelude.h"

static const char *example =
	"1abc2\n"
	"pqr3stu8vwx\n"
	"a1b2c3d4e5f\n"
	"treb7uchet\n";

static char getDigit(const char *text) {
	if (*text >= '0' && *text <= '9') {
		return *text;
	}

	return -1;
}

static int solve(const char *input) {
	int sum = 0;

	while (*input) {
		char digits[3] = {0};
		bool found_first = false;

		while (*input && *input != '\n') {
			const char digit = getDigit(input);

			if (digit >= 0) {
				if (!found_first) {
					digits[0] = digit;
					digits[1] = digit;
					found_first = true;
				} else {
					digits[1] = digit;
				}
			}
			input++;
		}

		sum += atoi(digits);
		found_first = false;
		input++;
	}

	return sum;
}

int main(void) {
	const int example_result = solve(example);
	if (example_result != 142) {
		printf("FAIL: expected 142 actual %d\n", example_result);

		return 1;
	}

	printf("Result: %d\n", solve(readToString("1.txt")));

	return 0;
}
