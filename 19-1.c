#include "prelude.h"

static const char *example =
	"px{a<2006:qkq,m>2090:A,rfg}\n"
	"pv{a>1716:R,A}\n"
	"lnx{m>1548:A,A}\n"
	"rfg{s<537:gd,x>2440:R,A}\n"
	"qs{s>3448:A,lnx}\n"
	"qkq{x<1416:A,crn}\n"
	"crn{x>2662:A,R}\n"
	"in{s<1351:px,qqz}\n"
	"qqz{s>2770:qs,m<1801:hdj,R}\n"
	"gd{a>3333:R,R}\n"
	"hdj{m>838:A,pv}\n"
	"\n"
	"{x=787,m=2655,a=1222,s=2876}\n"
	"{x=1679,m=44,a=2067,s=496}\n"
	"{x=2036,m=264,a=79,s=2244}\n"
	"{x=2461,m=1339,a=466,s=291}\n"
	"{x=2127,m=1623,a=2188,s=1013}\n";
static const int expected = 19114;

#define MAX_NAME 4 // 3 + \0
#define MAX_RULES 8
#define MAX_WFLS 600
#define MAX_PARTS 256

/// Rule
typedef struct {
	char attr;
	char op;
	int val;
	char dst[MAX_NAME];
} Rule;

/// Workflow
typedef struct {
	char name[MAX_NAME];
	char dst[MAX_NAME];
	Rule rules[MAX_RULES];
	size_t n;
} Workflow;

/// System
typedef struct {
	Workflow workflows[MAX_WFLS];
	const Workflow *start;
	size_t n;
} System;

static size_t indexof(char c) {
	static const char attrs[] = "xmas";
	char *cc = strchr(attrs, c);
	assert(cc && cc >= attrs);
	ptrdiff_t i = cc - attrs;
	assert(i < 4);
	return (size_t)i;
}

static bool processPart(const System *s, const int p[4]) {
	for (const Workflow *w = s->start; w;) {
		const char *dst = w->dst;
		for (size_t i = 0; i < w->n; i++) {
			const Rule *r = &w->rules[i];
			int v = p[indexof(r->attr)];
			if ((r->op == '>' && v > r->val) ||
			    (r->op == '<' && v < r->val)) {
				dst = r->dst;
				break;
			}
		}
		if (dst[0] == 'A') return true;
		if (dst[0] == 'R') return false;

		for (size_t i = 0; i < s->n; i++) {
			if (!strcmp(s->workflows[i].name, dst)) {
				w = &s->workflows[i];
				break;
			}
		}
	}
	assert(false);
}

static void parseSystem(const char *const input, System *s) {
	memset(s, 0, sizeof *s);
	const char *const end = strstr(input, "\n\n") - 1;
	for (const char *c = input; c < end; c = strchr(c, '\n') + 1) {
		assert(s->n < ASZ(s->workflows));
		Workflow *w = &s->workflows[s->n++];
		const char *rules_start = strchr(c, '{');
		const char *rules_end = strchr(c, '}');
		strncpy(w->name, c, rules_start - c);
		size_t n = 0;
		while (c < rules_end)
			if (*c++ == ',') n++;
		c = rules_start + 1;
		for (size_t i = 0; i < n; i++) {
			assert(i < ASZ(w->rules));
			Rule *r = &w->rules[w->n++];
			r->attr = *c++;
			r->op = *c++;
			r->val = atoi(c);
			c = strchr(c, ':') + 1;
			const char *e = strchr(c, ',');
			strncpy(r->dst, c, e - c);
			c = e + 1;
		}
		strncpy(w->dst, c, rules_end - c);
		if (!strcmp(w->name, "in")) s->start = w;
	}
}

static size_t parseParts(const char *const input, int parts[][4], size_t len) {
	memset(parts, 0, sizeof *parts * len);
	size_t n = 0;
	for (const char *c = strstr(input, "\n\n") + 2; *c;
	     c = strchr(c, '\n') + 1) {
		assert(n < len);
		int *p = parts[n++];
		assert(sscanf(c, "{x=%d,m=%d,a=%d,s=%d}", &p[0], &p[1], &p[2],
		              &p[3]) == 4);
	}
	return n;
}

static int sumPart(const int p[]) {
	int sum = 0;
	for (size_t j = 0; j < 4; j++) sum += p[j];
	return sum;
}

static int solve(const char *const input) {
	static System s;
	parseSystem(input, &s);

	static int ps[MAX_PARTS][4];
	size_t n = parseParts(input, ps, ASZ(ps));

	int total = 0;
	for (size_t i = 0; i < n; i++)
		if (processPart(&s, ps[i])) total += sumPart(ps[i]);

	return total;
}

int main(void) {
	const int actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %d actual %d\n", expected, actual);
		return 1;
	} else printf("PASS!\n\n");
	printf("%d\n", solve(readToString("19.txt")));
	return 0;
}
