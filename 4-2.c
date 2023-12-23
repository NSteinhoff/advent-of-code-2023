#include "prelude.h"
#define MAX_NUMBERS 16
#define MAX_WINNERS 32
#define MAX_CARDS 256

static const char *const example =
	"Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53\n"
	"Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19\n"
	"Card 3:  1 21 53 59 44 | 69 82 63 72 16 21 14  1\n"
	"Card 4: 41 92 73 84 69 | 59 84 76 51 58  5 54 83\n"
	"Card 5: 87 83 26 28 32 | 88 30 70 12 93 22 82 36\n"
	"Card 6: 31 18 13 56 72 | 74 77 10 23 35 67 36 11\n";

static const int expected = 30;

typedef enum {
	START_CARD,
	START_NUMBER,
	CONTINUE_NUMBER,
	START_WINNER,
	CONTINUE_WINNER,
} State;

static int solve(char const *const input) {
	static int numbers[MAX_NUMBERS], winners[MAX_WINNERS], cards[MAX_CARDS];
	size_t n_cards = 0;
	size_t n_numbers = 0;
	size_t n_winners = 0;
	memset(cards, 0, sizeof cards);

	State state = START_CARD;
	for (const char *cursor = input; *cursor; cursor++) {
		switch (state) {
		case START_CARD:
			if (*cursor == 'C') {
				// Add the original card
				cards[n_cards++] += 1;
				n_numbers = 0;
				n_winners = 0;
				break;
			}
			if (*cursor == ':') {
				state = START_NUMBER;
				break;
			}
			break;
		case START_NUMBER:
			if (*cursor == '|') {
				state = START_WINNER;
				break;
			}
			if (isdigit(*cursor)) {
				numbers[n_numbers++] = *cursor - '0';
				state = CONTINUE_NUMBER;
				break;
			}
			break;
		case CONTINUE_NUMBER:
			if (*cursor == ' ') {
				state = START_NUMBER;
				break;
			}
			numbers[n_numbers - 1] *= 10;
			numbers[n_numbers - 1] += *cursor - '0';
			break;
		case START_WINNER:
			if (isdigit(*cursor)) {
				winners[n_winners++] = *cursor - '0';
				state = CONTINUE_WINNER;
				break;
			}
			break;
		case CONTINUE_WINNER:
			if (*cursor == ' ') {
				state = START_WINNER;
				break;
			}
			if (*cursor == '\n') {
				int n_matches = 0;
				for (size_t i = 0; i < n_numbers; i++) {
					for (size_t j = 0; j < n_winners; j++) {
						if (numbers[i] == winners[j]) {
							n_matches++;
							break;
						}
					}
				}

				int copies = cards[n_cards - 1];
				for (size_t i = 0; i < (size_t)n_matches; i++) {
					cards[n_cards + i] += copies;
				}

				state = START_CARD;
				break;
			}
			winners[n_winners - 1] *= 10;
			winners[n_winners - 1] += *cursor - '0';
			break;
		}
	}

	int sum = 0;
	for (size_t i = 0; i < n_cards; i++) {
		sum += cards[i];
	}

	return sum;
}

int main(void) {
	const int actual = solve(example);

	if (actual != expected) {
		printf("FAIL: expected %d != actual %d\n", expected, actual);

		return 1;
	}

	printf("Result: %d\n", solve(readToString("4.txt")));

	return 0;
}
