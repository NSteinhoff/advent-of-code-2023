#include "prelude.h"
#define MAX_SEEDS 32
#define MAX_RANGES 64

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

static const size_t expected = 35;

typedef struct {
	size_t dst, src, len;
} Range;

typedef struct {
	const char *name;
	size_t len;
	Range mappings[MAX_RANGES];
} Map;

typedef struct {
	size_t n_seeds;
	size_t seeds[MAX_SEEDS];
	Map maps[7];
} Almanac;

typedef enum {
	SEED_TITLE,
	SEEDS,
	FIND_BLOCK,
	BLOCK_TITLE,
	RANGE_NEW,
	RANGE_DST,
	RANGE_SRC,
	RANGE_LEN,
} State;

static void parseAlmanac(char const *const input, Almanac *almanac) {
	size_t n_maps = 0;

	State state = SEED_TITLE;
	const char *cursor = input;

	Map *map = NULL;
	Range *range = NULL;
	for (char c; (c = *cursor); cursor++) {
		switch (state) {
		case SEED_TITLE:
			if (c == ':') {
				state = SEEDS;
			}
			break;
		case SEEDS:
			if (c == '\n') {
				state = FIND_BLOCK;
				break;
			}
			if (c == ' ') {
				almanac->seeds[almanac->n_seeds++] = 0;
				break;
			}
			almanac->seeds[almanac->n_seeds - 1] *= 10;
			almanac->seeds[almanac->n_seeds - 1] += (size_t)c - '0';
			break;
		case FIND_BLOCK:
			if (c != '\n') {
				map = &almanac->maps[n_maps++];
				map->len = 0;
				state = BLOCK_TITLE;
				break;
			}
			break;
		case BLOCK_TITLE:
			if (c == '\n') {
				state = RANGE_NEW;
			}
			break;
		case RANGE_NEW:
			if (c == '\n') {
				state = FIND_BLOCK;
				break;
			}

			assert(map);
			range = &map->mappings[map->len++];

			range->dst = (size_t)c - '0';
			range->src = 0;
			range->len = 0;

			state = RANGE_DST;
			break;
		case RANGE_DST:
			if (c == ' ') {
				state = RANGE_SRC;
				break;
			}
			assert(range);
			range->dst *= 10;
			range->dst += (size_t)c - '0';
			break;
		case RANGE_SRC:
			if (c == ' ') {
				state = RANGE_LEN;
				break;
			}
			assert(range);
			range->src *= 10;
			range->src += (size_t)c - '0';
			break;
		case RANGE_LEN:
			if (c == '\n') {
				state = RANGE_NEW;
				break;
			}
			assert(range);
			range->len *= 10;
			range->len += (size_t)c - '0';
			break;
		}
	}
}

static void printAlmanac(Almanac *almanac) {
	printf("Seeds: ");
	for (size_t i = 0; i < almanac->n_seeds; i++) {
		printf("%zu%s", almanac->seeds[i],
		       i < almanac->n_seeds - 1 ? ", " : "\n");
	}
	printf("Maps:\n");
	for (size_t i = 0; i < ASZ(almanac->maps); i++) {
		Map *map = &almanac->maps[i];
		printf("  %s (%zu)\n", map->name, map->len);
		for (size_t j = 0; j < map->len; j++) {
			Range *range = &map->mappings[j];
			printf("    [%zu:%zu:%zu]\n", range->dst, range->src,
			       range->len);
		}
	}
}

static size_t findLocation(Almanac *almanac) {
	size_t location = 0;

	for (size_t i = 0; i < almanac->n_seeds; i++) {
		size_t next = almanac->seeds[i];
		for (size_t j = 0; j < ASZ(almanac->maps); j++) {
			Map *map = &almanac->maps[j];
			for (size_t j = 0; j < map->len; j++) {
				Range *range = &map->mappings[j];
				printf("%zu in [%zu:%zu:%zu]", next, range->dst,
				       range->src, range->len);

				if (next < range->src ||
				    next >= range->src + range->len) {
					printf(" X\n");
					continue;
				}

				next = range->dst + (next - range->src);

				printf(" -> %zu\n", next);
				break;
			}
		}

		if (!location || location > next) {
			location = next;
		}
	}

	return location;
}

static size_t solve(char const *const input) {
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
	const size_t actual = solve(example);

	if (actual != expected) {
		printf("FAIL: expected %zu != actual %zu\n", expected, actual);

		return 1;
	}

	printf("Result: %zu\n", solve(readToString("5.txt")));

	return 0;
}
