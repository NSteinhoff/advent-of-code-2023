#include "prelude.h"

static const char *example =
	"rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7\n";
static const int expected = 1320;

static size_t hash(const char *s, size_t len) {
	size_t h = 0;
	for (; len--; s++) {
		assert(*s > 0);
		h += (uchar)*s;
		h *= 17;
		h %= 1 << 8;
	}
	return h;
}

static int solve(const char *const input) {
	int sum = 0;
	const char *sep = ",\n";
	for (const char *s = input; *s;) {
		size_t len = strcspn(s, sep);
		sum += hash(s, len);
		s += len + 1;
		s += strspn(s, sep);
	}
	return sum;
}

int main(void) {
	const int actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %d actual %d\n", expected, actual);
		return 1;
	} else printf("PASS!\n\n");
	printf("%d\n", solve(readToString("15.txt")));
	return 0;
}
