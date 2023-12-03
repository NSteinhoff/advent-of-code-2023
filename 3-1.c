#include "prelude.h"

static const bool verbose = false;

#define MAX_SYMBOLS 1 << 10

static const char *const example =
	"467..114..\n"
	"...*......\n"
	"..35..633.\n"
	"......#...\n"
	"617*......\n"
	".....+.58.\n"
	"..592.....\n"
	"......755.\n"
	"...$.*....\n"
	".664.598..\n";
static const int expected = 4361;

static void printSymbol(const char *cursor, int ncols) {
	cursor -= ncols + 1; // up a row
	cursor--;            // left one

	for (int i = 0; i < 9; i++) {
		printf("%c", *cursor);

		if (0 == (i + 1) % 3) {
			printf("\n");
			cursor += ncols + 1;
			cursor -= 2;
		} else {
			cursor++;
		}
	}
}

static bool isDigit(const char c) {
	return c >= '0' && c <= '9';
}

static bool isSymbol(const char c) {
	return !isDigit(c) && c != '\n' && c != '.';
}

static int parseNumber(const char *const input, const char *cursor) {
	while (cursor > input && *(cursor - 1) != '\n' &&
	       isDigit(*(cursor - 1))) {
		cursor--;
	}

	int number = 0;
	for (; isDigit(*cursor); cursor++) {
		number *= 10;
		number += *cursor - '0';
	}

	return number;
}

static int sumNumbers(const char *const input, const char *cursor, int ncols) {
	cursor -= ncols + 1; // up a row
	cursor--;            // left one

	const char *numbers[9];
	int n_numbers = 0;

	bool inside = false;
	for (int i = 0; i < 9; i++) {
		if (!inside && isDigit(*cursor)) {
			inside = true;
			numbers[n_numbers++] = cursor;
		}

		if (inside && !isDigit(*cursor)) {
			inside = false;
		}

		if (0 == (i + 1) % 3) {
			inside = false;
			cursor += ncols + 1;
			cursor -= 2;
		} else {
			cursor++;
		}
	}

	int sum = 0;
	for (int i = 0; i < n_numbers; i++) {
		sum += parseNumber(input, numbers[i]);
	}

	return sum;
}

static int solve(const char *const input) {
	static const char *symbols[MAX_SYMBOLS];
	size_t n_symbols = 0;
	int ncols = 0;

	int col = 0;
	for (const char *cursor = input; *cursor; cursor++) {
		if (*cursor == '\n') {
			ncols = col;
			col = 0;

			continue;
		}

		if (isSymbol(*cursor)) {
			assert(n_symbols < MAX_SYMBOLS);
			symbols[n_symbols++] = cursor;
		}

		col++;
	}

	int sum = 0;
	for (size_t i = 0; i < n_symbols; i++) {
		const char *const symbol = symbols[i];
		const int number = sumNumbers(input, symbol, ncols);
		sum += number;

		if (verbose) {
			const int row = (int)(symbol - input) / (ncols + 1);
			const int col = (int)(symbol - input) % (ncols + 1);
			printf("\n[%d,%d] %c\n", row, col, *symbol);
			printSymbol(symbol, ncols);
			printf("=> %d\n", number);
		}
	}

	return sum;
}

int main(void) {
	const int actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %d actual %d\n", expected, actual);
		return 1;
	}

	printf("%d\n", solve(read_to_string("3.txt")));

	return 0;
}
