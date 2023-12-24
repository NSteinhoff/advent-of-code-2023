#include "prelude.h"

#define MAX_NUMS 16 * 5
#define CACHE_SIZE 5000
#define MAX_PATTERN_STORAGE 1 << 20

static const char *example =
	"???.### 1,1,3\n"
	".??..??...?##. 1,1,3\n"
	"?#?#?#?#?#?#?#? 1,3,1,6\n"
	"????.#...#... 4,1,1\n"
	"????.######..#####. 1,6,5\n"
	"?###???????? 3,2,1\n";

static const u64 expected = 525152;

typedef struct {
	const char *s;
	size_t len_nums;
	int nums[MAX_NUMS];
	bool flag;
} Pattern;

static bool equal(const Pattern *left, const Pattern *right) {
	if (left->flag != right->flag) return false;
	if (left->len_nums != right->len_nums) return false;
	if (left->s != right->s) return false;
	for (size_t i = 0; i < left->len_nums; i++)
		if (left->nums[i] != right->nums[i]) return false;

	return true;
}

typedef struct {
	Pattern pattern;
	u64 count;
} Result;

static struct {
	Result entries[CACHE_SIZE];
	size_t len;
} cache;

static Result *getResult(const Pattern *pattern) {
	for (size_t i = 0; i < cache.len; i++)
		if (equal(pattern, &cache.entries[i].pattern))
			return &cache.entries[i];
	return NULL;
}

static Result *createResult(const Pattern *pattern, u64 count) {
	assert(cache.len < ASZ(cache.entries));
	Result *result = &cache.entries[cache.len++];
	result->pattern = *pattern;
	result->count = count;
	return result;
}

static Pattern parsePattern(const char **s) {
	static char patterns[MAX_PATTERN_STORAGE];
	static size_t patterns_len = 0;

	const char *ss = *s;
	Pattern p = {.s = &patterns[patterns_len]};
	size_t len = 0;
	for (char c; (c = *ss++) != ' '; len++) {
		assert(patterns_len < ASZ(patterns));
		patterns[patterns_len++] = c;
	}
	for (; *ss && *ss != '\n'; ss += strcspn(ss, ",\n")) {
		if (*ss == ',') ss++;
		assert(p.len_nums < ASZ(p.nums));
		p.nums[p.len_nums++] = atoi(ss);
	}
	*s = ss + 1;

	// Expand input
	size_t len_nums = p.len_nums;
	for (size_t i = 0; i < 4; i++) {
		assert(patterns_len < ASZ(patterns));
		patterns[patterns_len++] = '?';
		for (size_t j = 0; j < len; j++) {
			assert(patterns_len < ASZ(patterns));
			patterns[patterns_len++] = p.s[j];
		}
		for (size_t j = 0; j < len_nums; j++) {
			assert(p.len_nums < ASZ(p.nums));
			p.nums[p.len_nums++] = p.nums[j];
		}
	}
	patterns[patterns_len++] = '\0';

	return p;
}

static Pattern next(Pattern p, bool flag) {
	assert(p.s && *p.s);
	p.s++;

	p.flag = flag;
	if (flag) {
		assert(p.nums[0]);
		p.nums[0]--;
	}

	if ((!p.flag || !*p.s) && !p.nums[0]) {
		assert(p.len_nums);
		for (size_t i = 1; i < p.len_nums; i++)
			p.nums[i - 1] = p.nums[i];
		p.len_nums--;
	}

	return p;
}

static u64 count(Pattern p) {
	if (!*p.s) return p.len_nums ? 0 : 1;
	if (!p.len_nums) return strchr(p.s, '#') ? 0 : 1;
	if (*p.s == '.' && p.flag && p.nums[0]) return 0;
	if (*p.s == '#' && p.flag && !p.nums[0]) return 0;

	Result *result = getResult(&p);
	if (result) return result->count;
	u64 c = *p.s == '.' ? count(next(p, false))
	      : *p.s == '#' ? count(next(p, true))
	      : p.flag      ? count(next(p, p.nums[0]))
	                    : count(next(p, false)) + count(next(p, true));
	return createResult(&p, c)->count;
}

static u64 solve(const char *const input) {
	const char *s = input;
	u64 total = 0;
	while (*s) {
		cache.len = 0;
		total += count(parsePattern(&s));
	}
	return total;
}

int main(void) {
	const u64 actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %llu actual %llu\n", expected, actual);
		return 1;
	}
	printf("%llu\n", solve(readToString("12.txt")));
	return 0;
}
