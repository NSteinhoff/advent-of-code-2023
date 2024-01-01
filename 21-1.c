#include "prelude.h"

static const char *example =
	"...........\n"
	".....###.#.\n"
	".###.##..#.\n"
	"..#.#...#..\n"
	"....#.#....\n"
	".##..S####.\n"
	".##..#...#.\n"
	".......##..\n"
	".##.#.####.\n"
	".##..##.##.\n"
	"...........\n";

static const int expected = 16;
static u8 steps = 6;

#define MAX_TO_VISIT 1024
#define MAX_GRID 140 * 140

#define AT(G, X, Y) G[Y * (cols + 1) + X]

typedef struct {
	size_t x, y;
} Pos;

typedef struct {
	Pos pos;
	size_t remaining;
} Step;

typedef struct {
	Step items[MAX_TO_VISIT];
	size_t head, tail, len;
} Queue;

static bool visited[MAX_GRID];
static Queue tovisit;

static void init(void) {
	memset(&tovisit, 0, sizeof tovisit);
	memset(&visited, 0, sizeof visited);
}

static void enq(Queue *q, const Pos *p, size_t remaining) {
	assert(q->len++ < ASZ(q->items));
	q->items[q->tail++] = (Step){.pos = *p, .remaining = remaining};
	q->tail %= ASZ(q->items);
}

static Step deq(Queue *q) {
	assert(q->len--);
	Step *s = &q->items[q->head];
	q->head++;
	q->head %= ASZ(q->items);
	return *s;
}

static int solve(const char *const input) {
	init();
	size_t const cols = strcspn(input, "\n");
	size_t const rows = strlen(input) / (cols + 1);
	const char *s = strchr(input, 'S');
	Pos start = {.x = (size_t)(s - input) % (cols + 1),
	             .y = (size_t)(s - input) / (cols + 1)};
	printf("Grid:  %3zu, %3zu\n", rows, cols);
	printf("Start: %3zu, %3zu\n", start.x, start.y);

	size_t n_reachable = 0;
	enq(&tovisit, &start, steps);
	AT(visited, start.x, start.y) = true;
	while (tovisit.len) {
		Step step = deq(&tovisit);
		if (step.remaining % 2 == 0) n_reachable++;
		if (!step.remaining) continue;
		Pos next[] = {
			{step.pos.x + 1, step.pos.y    },
			{step.pos.x - 1, step.pos.y    },
			{step.pos.x,     step.pos.y - 1},
			{step.pos.x,     step.pos.y + 1},
		};
		for (size_t i = 0; i < ASZ(next); i++) {
			const Pos *p = &next[i];
			if (p->x >= cols || p->y >= rows ||
			    AT(visited, p->x, p->y) ||
			    AT(input, p->x, p->y) == '#')
				continue;
			enq(&tovisit, p, step.remaining - 1);
			AT(visited, p->x, p->y) = true;
		}
	}

	return (int)n_reachable;
}

int main(void) {
	const int actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %d actual %d\n", expected, actual);
		return 1;
	} else printf("PASS!\n\n");
	steps = 64;
	printf("%d\n", solve(readToString("21.txt")));
	return 0;
}
