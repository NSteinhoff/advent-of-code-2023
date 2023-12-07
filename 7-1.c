#include "prelude.h"

static const char *const example =
	"32T3K 765\n"
	"T55J5 684\n"
	"KK677 28\n"
	"KTJJT 220\n"
	"QQQJA 483\n";

static const int expected = 6440;

#define CARDS_IN_HAND 5
#define MAX_HANDS 1000

static const uchar faces[] = {
	'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A',
};

static const uchar strengths[] = {
	['2'] = 1,  ['3'] = 2,  ['4'] = 3,  ['5'] = 4, ['6'] = 5,
	['7'] = 6,  ['8'] = 7,  ['9'] = 8,  ['T'] = 9, ['J'] = 10,
	['Q'] = 11, ['K'] = 12, ['A'] = 13,
};

typedef enum {
	HIGH,
	ONE,
	TWO,
	THREE,
	FULL,
	FOUR,
	FIVE,
} Type;

static const char *names[] = {
	[HIGH] = "HIGH", [ONE] = "ONE",   [TWO] = "TWO",   [THREE] = "THREE",
	[FULL] = "FULL", [FOUR] = "FOUR", [FIVE] = "FIVE",
};

typedef struct {
	Type type;
	uint bid;
	uchar cards[CARDS_IN_HAND];
} Hand;

static Type type(const uchar *cards) {
	int face_counts[ASZ(strengths)] = {0};
	for (size_t i = 0; i < CARDS_IN_HAND; i++) {
		face_counts[cards[i]]++;
	}

	int multiples[CARDS_IN_HAND + 1] = {0};
	for (size_t i = 0; i < ASZ(faces); i++) {
		multiples[face_counts[faces[i]]]++;
	}

	return multiples[5]                 ? FIVE
	     : multiples[4]                 ? FOUR
	     : multiples[3] && multiples[2] ? FULL
	     : multiples[3]                 ? THREE
	     : multiples[2] == 2            ? TWO
	     : multiples[2]                 ? ONE
	                                    : HIGH;
}

static bool beats(const Hand *left, const Hand *right) {
	if (left->type != right->type) {
		return left->type > right->type;
	}

	for (size_t j = 0; j < CARDS_IN_HAND; j++) {
		uchar l = strengths[left->cards[j]];
		uchar r = strengths[right->cards[j]];

		if (l != r) {
			return l > r;
		}
	}

	return false;
}

static void bubbleSort(Hand *hands, size_t len) {
	while (len > 1) {
		for (size_t i = 0; i < len - 1; i++) {
			if (beats(hands + i, hands + i + 1)) {
				Hand tmp = hands[i];
				hands[i] = hands[i + 1];
				hands[i + 1] = tmp;
			}
		}
		len--; // last element is in its final position
	}
}

static void printHands(const Hand *hands, size_t len) {
	for (size_t i = 0; i < len; i++) {
		printf("%4zu: %s %3d => %s\n", i + 1, hands[i].cards,
		       hands[i].bid, names[hands[i].type]);
	}
}

static int solve(const char *const input) {
	static Hand hands[MAX_HANDS];
	size_t n = 0;

	for (const char *cursor = input; *cursor && n < MAX_HANDS; n++) {
		sscanf(cursor, "%5s %d\n", hands[n].cards, &hands[n].bid);
		hands[n].type = type(hands[n].cards);
		while (*cursor++ != '\n')
			;
	}

	bubbleSort(hands, n);
	printHands(hands, n);

	int total = 0;
	for (size_t i = 0; i < n; i++) {
		total += (i + 1) * hands[i].bid;
	}

	return total;
}

int main(void) {
	const int actual = solve(example);

	if (actual != expected) {
		printf("FAIL: expected %d != actual %d\n", expected, actual);

		return 1;
	}

	printf("\n===\n");
	printf("Result: %d\n", solve(readToString("7.txt")));

	return 0;
}
