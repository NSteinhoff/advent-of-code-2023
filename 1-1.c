#include "prelude.h"

static const char *example =
	"1abc2\n"
	"pqr3stu8vwx\n"
	"a1b2c3d4e5f\n"
	"treb7uchet\n";

static int solve(const char *input) {
	int sum = 0;
	while (*input) {
		char digits[3] = {0};
		for (uint i = 0; *input && *input != '\n'; input++) {
			if (!isdigit(*input)) continue;
			if (!i) digits[i++] = *input;
			digits[i] = *input;
		}
		sum += atoi(digits);
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
