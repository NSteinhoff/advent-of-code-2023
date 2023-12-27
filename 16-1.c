#include "prelude.h"

static const char *example =
	".|...\\....\n"
	"|.-.\\.....\n"
	".....|-...\n"
	"........|.\n"
	"..........\n"
	".........\\\n"
	"..../.\\\\..\n"
	".-.-/..|..\n"
	".|....-|.\\\n"
	"..//.|....\n";
static const int expected = 46;

#define MAX_BEAMS 12000
#define MAX_GRID_SIZE 110 * 111 + 1

typedef enum {
	L = 1,
	R = 2,
	U = 4,
	D = 8,
} Dir;

typedef struct {
	size_t x, y;
	Dir d;
} Beam;

typedef struct {
	Beam beams[MAX_BEAMS];
	size_t n;
} Stack;

static u8 visited[MAX_GRID_SIZE];
#define AT(G, X, Y) (G)[(Y) * (cols + 1) + (X)]

static struct {
	char at;
	Dir from;
	Dir to;
} moves[] = {
	{'.',  L, L},
        {'.',  R, R},
        {'.',  U, U},
        {'.',  D, D},
        {'/',  L, D},
	{'/',  R, U},
        {'/',  U, R},
        {'/',  D, L},
        {'\\', L, U},
        {'\\', R, D},
	{'\\', U, L},
        {'\\', D, R},
        {'-',  L, L},
        {'-',  R, R},
        {'-',  U, L},
	{'-',  U, R},
        {'-',  D, L},
        {'-',  D, R},
        {'|',  L, U},
        {'|',  L, D},
	{'|',  R, U},
        {'|',  R, D},
        {'|',  U, U},
        {'|',  D, D},
};

static Beam move(const Beam *b, Dir d) {
	Beam bb = *b;
	bb.d = d;
	switch (bb.d) {
	case L: bb.x--; break;
	case R: bb.x++; break;
	case U: bb.y--; break;
	case D: bb.y++; break;
	}
	return bb;
}

static void push(Stack *stack, Beam b) {
	for (size_t i = 0; i < stack->n; i++)
		if (!memcmp(&stack->beams[i], &b, sizeof(b))) return;
	assert(stack->n < ASZ(stack->beams));
	stack->beams[stack->n++] = b;
}

static Beam pop(Stack *stack) {
	return stack->beams[--stack->n];
}

static int solve(const char *const input) {
	assert(strlen(input) < ASZ(visited));
	size_t cols = strcspn(input, "\n");
	size_t rows = strlen(input) / (cols + 1);

	static Stack queue;
	queue.n = 0;
	push(&queue, (Beam){0, 0, R});

	memset(visited, 0, sizeof(visited));
	while (queue.n) {
		Beam beam = pop(&queue);
		if (beam.x >= cols || beam.y >= rows) continue;
		if (AT(visited, beam.x, beam.y) & beam.d) continue;

		AT(visited, beam.x, beam.y) |= beam.d;
		for (size_t i = 0; i < ASZ(moves); i++) {
			if (!(AT(input, beam.x, beam.y) == moves[i].at) ||
			    !(beam.d == moves[i].from))
				continue;
			push(&queue, move(&beam, moves[i].to));
		}
	}

	assert(!queue.n);
	int energized = 0;
	for (size_t x = 0; x < cols; x++)
		for (size_t y = 0; y < rows; y++)
			energized += AT(visited, x, y) > 0;
	return energized;
}

int main(void) {
	const int actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %d actual %d\n", expected, actual);
		return 1;
	} else printf("PASS!\n\n");
	printf("%d\n", solve(readToString("16.txt")));
	return 0;
}
