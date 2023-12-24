#include "prelude.h"

#define MAX_NUMS 16
#define MAX_PATTERNS 32

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

typedef struct {
	Pattern data[MAX_PATTERNS];
	size_t len;
} Stack;

static void push(Stack *s, const Pattern p) {
	assert(s->len < MAX_PATTERNS && "OOM");
	s->data[s->len++] = p;
}

static Pattern *pop(Stack *s) {
	if (!s->len) return NULL;
	return s->data + --s->len;
}

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

static void printPattern(const Pattern *p) {
	printf("'%.*s' [", (int)p->len_s, p->s);
	for (u8 i = 0; i < p->len_nums; i++)
		printf("%d%s", p->nums[i], i == p->len_nums - 1 ? "" : ",");
	printf("]%s\n", p->flag ? " X" : "");
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

static int count(const Pattern *p) {
	int cnt = 0;

	Stack s = {0};
	push(&s, *p);

	while ((p = pop(&s))) {
		Pattern pp = *p;
		if (!pp.len_s) {
			if (!pp.len_nums) cnt++;
			continue;
		}

		if (!pp.len_nums) {
			if (!strnchr(pp.s, '#', pp.len_s)) {
				cnt++;
			}
			continue;
		}

		if (*pp.s == '.') {
			if (!(pp.flag && pp.nums[0])) push(&s, next(pp, false));
			continue;
		}

		if (*pp.s == '#') {
			if (!(pp.flag && !pp.nums[0])) push(&s, next(pp, true));
			continue;
		}

		if (pp.flag) {
			push(&s, next(pp, pp.nums[0]));
			continue;
		}

		push(&s, next(pp, false));
		push(&s, next(pp, true));
	}

	return cnt;
}

static int solve(const char *const input) {
	const char *s = input;
	int total = 0;
	while (*s) {
		Pattern p = parsePattern(&s);
		printPattern(&p);
		int cnt = count(&p);
		printf("Count: %d\n", cnt);
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
