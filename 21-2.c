#include "prelude.h"
/// Assumptions:
///
/// 1. Grid has uneven number of rows and columns 131
/// 2. We start in the center of the initial grid at 65, 65
/// 3. The outer perimeter of the grid is empty
/// 4. The center row and the center column are empty
///
/// From this it follows that:
/// 1. It takes an even number of steps to get to any edge from the starting
///    position.
/// 2. It takes an odd number of steps to get from the center of one grid to
///    the center of any of its cardinal neighbors.
/// 3. The reachable tiles flip every grid
/// 4. There is a maximum number of reachable tiles in each grid depending on
///    the parity of the remaining steps when entering the grid and the
///    position from where it is entered.

#define MAX_TO_VISIT 1024
#define MAX_GRID (131 + 1) * 131

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

// Assumptions
static const size_t size = 131;
static const size_t rows = size;
static const size_t cols = size;
static const Pos start = {.x = size / 2, .y = size / 2};

static const size_t steps = 26501365;

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

static u32 count(const char *grid, const Pos *start, size_t steps) {
	init();
	u32 n_reachable = 0;
	enq(&tovisit, start, steps);
	AT(visited, start->x, start->y) = true;
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
			    AT(grid, p->x, p->y) == '#')
				continue;
			enq(&tovisit, p, step.remaining - 1);
			AT(visited, p->x, p->y) = true;
		}
	}

	return n_reachable;
}

static u64 solve(const char *const input) {
	printf("Grid:  %3zu, %3zu\n", rows, cols);
	printf("Start: %3zu, %3zu\n", start.x, start.y);

	assert(steps % size == size / 2);

	u32 even = count(input, &start, size * 2);
	u32 odd = count(input, &start, size * 2 + 1);
	printf("Maximum:\n");
	printf("even = %u\n", even);
	printf("odd  = %u\n", odd);

	// The fully covered grids
	size_t width = steps / size - 1;
	size_t n_even = (width + 1) / 2 * 2;
	n_even *= n_even;
	size_t n_odd = width / 2 * 2 + 1;
	n_odd *= n_odd;

	u64 reachable = n_even * even + n_odd * odd;

	// The corners
	Pos corner_starts[] = {
		{.x = 65,       .y = size - 1},
		{.x = 0,        .y = 65      },
		{.x = 65,       .y = 0       },
		{.x = size - 1, .y = 65      },
	};
	for (size_t i = 0; i < ASZ(corner_starts); i++)
		reachable += count(input, &corner_starts[i], size - 1);

	// Partials
	Pos partial_starts[] = {
		{.x = 0,        .y = size - 1},
		{.x = 0,        .y = 0       },
		{.x = size - 1, .y = size - 1},
		{.x = size - 1, .y = 0       },
	};
	for (size_t i = 0; i < ASZ(partial_starts); i++) {
		reachable += count(input, &partial_starts[i], size / 2 - 1) *
		         (width + 1);
		reachable += count(input, &partial_starts[i], size / 2 - 1 + size) *
		         width;
	}

	return reachable;
}

int main(void) {
	printf("%llu\n", solve(readToString("21.txt")));
	return 0;
}
