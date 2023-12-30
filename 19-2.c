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
static const u64 expected = 167409079868000;

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

typedef int Part[4][2];
typedef struct {
	Part p;
	const char *dst;
} Range;

static const char attrs[4] = "xmas";
static size_t indexof(char c) {
	char *cc = strchr(attrs, c);
	assert(cc && cc >= attrs);
	ptrdiff_t i = cc - attrs;
	assert(i < 4);
	return (size_t)i;
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

static const Workflow *findWorkflow(const System *s, const char *const name) {
	for (size_t i = 0; i < s->n; i++)
		if (!strcmp(name, s->workflows[i].name))
			return &s->workflows[i];
	return NULL;
}

static u64 product(Part p) {
	u64 prod = 1;
	for (size_t i = 0; i < 4; i++) prod *= (u64)(p[i][1] - p[i][0] + 1);
	return prod;
}

static u64 solve(const char *const input) {
	static System s;
	parseSystem(input, &s);
	static Range ranges[MAX_PARTS];
	for (size_t i = 0; i < sizeof attrs; i++) {
		ranges[0].p[i][0] = 1;
		ranges[0].p[i][1] = 4000;
		ranges[0].dst = s.start->name;
	}
	size_t n = 1;

	u64 total = 0;
	while (n) {
		Range *head = &ranges[--n];
		if (*head->dst == 'A') total += product(head->p);
		if (strchr("RA", *head->dst)) continue;

		Range range;
		memcpy(&range, head, sizeof range);

		const Workflow *w = findWorkflow(&s, range.dst);
		assert(w);
		bool matchall = false;

		for (size_t i = 0; i < w->n; i++) {
			const Rule *r = &w->rules[i];
			const size_t at = indexof(r->attr);

			// Create matching and non-matching ranges for the attribute
			int match[2], nomatch[2];
			memcpy(&match, range.p[at], sizeof match);
			memcpy(&nomatch, range.p[at], sizeof nomatch);
			if (r->op == '<') {
				match[1] = r->val - 1;
				nomatch[0] = r->val;
			} else {
				match[0] = r->val + 1;
				nomatch[1] = r->val;
			}

			// Add the matching range to the undecided set with the new workflow
			if (match[0] <= match[1]) {
				Range new_range;
				memcpy(&new_range, &range, sizeof new_range);
				new_range.dst = r->dst;
				memcpy(&new_range.p[at], &match,
				       sizeof new_range.p[at]);
				memcpy(&ranges[n++], &new_range,
				       sizeof *ranges);
			}

			// No non-matching range left
			if (nomatch[0] > nomatch[1]) {
				matchall = true;
				break;
			}

			// Continue with the non-matching part of the range
			memcpy(&range.p[at], &nomatch, sizeof range.p[at]);
		}

		if (!matchall) {
			Range new_range;
			new_range.dst = w->dst;
			memcpy(new_range.p, range.p, sizeof new_range.p);
			memcpy(&ranges[n++], &new_range, sizeof *ranges);
		}
	}
	return total;
}

int main(void) {
	const u64 actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %llu actual %llu\n", expected, actual);
		return 1;
	} else printf("PASS!\n\n");
	printf("%llu\n", solve(readToString("19.txt")));
	return 0;
}
