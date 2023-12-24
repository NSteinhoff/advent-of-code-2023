#include "prelude.h"

#define MAX_NUMS 16

static const char *example =
	"???.### 1,1,3\n"
	".??..??...?##. 1,1,3\n"
	"?#?#?#?#?#?#?#? 1,3,1,6\n"
	"????.#...#... 4,1,1\n"
	"????.######..#####. 1,6,5\n"
	"?###???????? 3,2,1\n";

static const int expected = 21;

typedef struct {
	int nums[MAX_NUMS];
	const char *s;
	size_t len_s, len_nums;
	bool flag;
} Pattern;

static Pattern parsePattern(const char **s) {
	const char *ss = *s;
	Pattern p = {.s = ss};
	while (*ss++ != ' ') p.len_s++;
	for (; *ss && *ss != '\n'; ss += strcspn(ss, ",\n")) {
		if (*ss == ',') ss++;
		assert(p.len_nums < ASZ(p.nums));
		p.nums[p.len_nums++] = atoi(ss);
	}
	*s = ss + 1;
	return p;
}

static const char *strnchr(const char *s, char c, size_t len) {
	for (size_t i = 0; i < len; i++)
		if (s[i] == c) return s + i;
	return NULL;
}

static Pattern next(Pattern p, bool flag) {
	p.flag = flag;
	assert(p.s && p.len_s);
	p.s++;
	p.len_s--;

	if (flag) {
		assert(p.nums[0]);
		p.nums[0]--;
	}

	if ((!p.flag || !p.len_s) && !p.nums[0]) {
		assert(p.len_nums);
		for (size_t i = 1; i < p.len_nums; i++)
			p.nums[i - 1] = p.nums[i];
		p.len_nums--;
	}

	return p;
}

static int count(Pattern p) {
	return !p.len_s    ? p.len_nums ? 0 : 1
	     : !p.len_nums ? strnchr(p.s, '#', p.len_s) ? 0 : 1
	     : *p.s == '.' ? p.flag && p.nums[0] ? 0 : count(next(p, false))
	     : *p.s == '#' ? p.flag && !p.nums[0] ? 0 : count(next(p, true))
	     : p.flag ? count(next(p, p.nums[0]))
	                   : count(next(p, false)) + count(next(p, true));
}

static int solve(const char *const input) {
	const char *s = input;
	int total = 0;
	while (*s) {
		Pattern p = parsePattern(&s);
		int cnt = count(p);
		total += cnt;
	}
	return total;
}

int main(void) {
	const int actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %d actual %d\n", expected, actual);
		return 1;
	}
	printf("%d\n", solve(readToString("12.txt")));
	return 0;
}
