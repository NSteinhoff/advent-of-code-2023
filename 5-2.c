#include "prelude.h"

#define MAX_SEEDS 32
#define MAX_MAPPINGS 64
#define MAX_STACK 128

static const char *const example =
	"seeds: 79 14 55 13\n"
	"\n"
	"seed-to-soil map:\n"
	"50 98 2\n"
	"52 50 48\n"
	"\n"
	"soil-to-fertilizer map:\n"
	"0 15 37\n"
	"37 52 2\n"
	"39 0 15\n"
	"\n"
	"fertilizer-to-water map:\n"
	"49 53 8\n"
	"0 11 42\n"
	"42 0 7\n"
	"57 7 4\n"
	"\n"
	"water-to-light map:\n"
	"88 18 7\n"
	"18 25 70\n"
	"\n"
	"light-to-temperature map:\n"
	"45 77 23\n"
	"81 45 19\n"
	"68 64 13\n"
	"\n"
	"temperature-to-humidity map:\n"
	"0 69 1\n"
	"1 0 69\n"
	"\n"
	"humidity-to-location map:\n"
	"60 56 37\n"
	"56 93 4\n";

static const i64 expected = 46;

typedef struct {
	i64 dst, src, len;
} Mapping;

typedef struct {
	i64 src, len;
} Range;

typedef struct {
	const char *name;
	size_t len;
	Mapping mappings[MAX_MAPPINGS];
} Map;

typedef struct {
	Range elements[MAX_STACK];
	size_t len;
} Stack;

static const Range *pop(Stack *stack) {
	assert(stack->len > 0);

	return &stack->elements[--stack->len];
}

static void push(Stack *stack, const Range *range) {
	assert(stack->len < ASZ(stack->elements));

	stack->elements[stack->len++] = *range;
}

typedef struct {
	size_t n_seeds;
	Range seeds[MAX_SEEDS];
	Map maps[7];
} Almanac;

typedef enum {
	SEED_TITLE,
	SEED_SRC,
	SEED_LEN,
	FIND_BLOCK,
	BLOCK_TITLE,
	MAPPING_NEW,
	MAPPING_DST,
	MAPPING_SRC,
	MAPPING_LEN,
} State;

#define MIN(A, B) (A) <= (B) ? (A) : (B)
#define MAX(A, B) (A) >= (B) ? (A) : (B)

static void parseAlmanac(char const *const input, Almanac *almanac) {
	size_t n_blocks = 0;
	almanac->n_seeds = 0;

	State state = SEED_TITLE;
	const char *cursor = input;

	Map *map = NULL;
	Mapping *mapping = NULL;
	for (char c; (c = *cursor); cursor++) {
		switch (state) {
		case SEED_TITLE:
			if (c == ' ') {
				almanac->seeds[almanac->n_seeds++] = (Range){0};
				state = SEED_SRC;
				break;
			}
			break;
		case SEED_SRC: {
			if (c == ' ') {
				state = SEED_LEN;
				break;
			}
			Range *seed = &almanac->seeds[almanac->n_seeds - 1];
			seed->src *= 10;
			seed->src += (size_t)c - '0';
			break;
		}
		case SEED_LEN: {
			if (c == ' ') {
				almanac->seeds[almanac->n_seeds++] = (Range){0};
				state = SEED_SRC;
				break;
			}
			if (c == '\n') {
				state = FIND_BLOCK;
				break;
			}
			Range *seed = &almanac->seeds[almanac->n_seeds - 1];
			seed->len *= 10;
			seed->len += (size_t)c - '0';
			break;
		}
		case FIND_BLOCK:
			if (c != '\n') {
				map = &almanac->maps[n_blocks++];
				map->len = 0;
				state = BLOCK_TITLE;
				break;
			}
			break;
		case BLOCK_TITLE:
			if (c == '\n') {
				state = MAPPING_NEW;
			}
			break;
		case MAPPING_NEW:
			if (c == '\n') {
				state = FIND_BLOCK;
				break;
			}

			assert(map && map->len < MAX_MAPPINGS);
			mapping = &map->mappings[map->len++];
			*mapping = (Mapping){.dst = c - '0'};

			state = MAPPING_DST;
			break;
		case MAPPING_DST:
			if (c == ' ') {
				state = MAPPING_SRC;
				break;
			}
			assert(mapping);
			mapping->dst *= 10;
			mapping->dst += (size_t)c - '0';
			break;
		case MAPPING_SRC:
			if (c == ' ') {
				state = MAPPING_LEN;
				break;
			}
			assert(mapping);
			mapping->src *= 10;
			mapping->src += (size_t)c - '0';
			break;
		case MAPPING_LEN:
			if (c == '\n') {
				state = MAPPING_NEW;
				break;
			}
			assert(mapping);
			mapping->len *= 10;
			mapping->len += (size_t)c - '0';
			break;
		}
	}
}

static void printAlmanac(Almanac *almanac) {
	printf("Seeds:\n");
	for (size_t i = 0; i < almanac->n_seeds; i++) {
		Range *seed = almanac->seeds + i;
		printf("[%lld:%lld]\n", seed->src, seed->len);
	}
	printf("Maps:\n");
	for (size_t i = 0; i < ASZ(almanac->maps); i++) {
		Map *map = &almanac->maps[i];
		printf("  %s (%zu)\n", map->name, map->len);
		for (size_t j = 0; j < map->len; j++) {
			Mapping *range = &map->mappings[j];
			printf("    [%lld:%lld:%lld]\n", range->dst, range->src,
			       range->len);
		}
	}
}

static i64 findLocation(Almanac *almanac) {
	i64 loc = INT64_MAX;

	static Stack outputs;
	for (size_t i = 0; i < almanac->n_seeds; i++)
		push(&outputs, &almanac->seeds[i]);

	for (size_t i = 0; i < ASZ(almanac->maps); i++) {
		Map *map = &almanac->maps[i];

		static Stack inputs;
		while (outputs.len) push(&inputs, pop(&outputs));

		while (inputs.len) {
			Range input = *pop(&inputs);
			i64 s1 = input.src;
			i64 e1 = input.src + input.len;

			bool overlap = false;
			for (size_t i = 0; i < map->len; i++) {
				Mapping *r = &map->mappings[i];
				i64 s2 = r->src;
				i64 e2 = r->src + r->len;
				i64 delta = r->dst - r->src;

				i64 os = MAX(s1, s2);
				i64 oe = MIN(e1, e2);
				if (os >= oe) continue; // no overlap

				push(&outputs, &(Range){os + delta, oe - os});
				if (s1 < os)
					push(&inputs, &(Range){s1, os - s1});
				if (e1 > oe)
					push(&inputs, &(Range){oe, e1 - oe});

				overlap = true;
				break;
			}
			if (!overlap) push(&outputs, &input);
		}
	}

	while (outputs.len) {
		i64 src = pop(&outputs)->src;
		if (src < loc) loc = src;
	}

	return loc;
}

static i64 solve(char const *const input) {
	static Almanac almanac = {
		.n_seeds = 0,
		.maps = {
			 {.name = "seed-to-soil"},
			 {.name = "soil-to-fertilizer"},
			 {.name = "fertilizer-to-water"},
			 {.name = "water-to-light"},
			 {.name = "light-to-temperature"},
			 {.name = "temperature-to-humidity"},
			 {.name = "humidity-to-location"},
			 }
        };

	parseAlmanac(input, &almanac);
	printAlmanac(&almanac);

	return findLocation(&almanac);
}

int main(void) {
	const i64 actual = solve(example);

	if (actual != expected) {
		printf("FAIL: expected %lld != actual %lld\n", expected,
		       actual);

		return 1;
	}

	printf("Result: %lld\n", solve(readToString("5.txt")));

	return 0;
}
