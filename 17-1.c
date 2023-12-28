#include "prelude.h"

static const char *example =
	"2413432311323\n"
	"3215453535623\n"
	"3255245654254\n"
	"3446585845452\n"
	"4546657867536\n"
	"1438598798454\n"
	"4457876987766\n"
	"3637877979653\n"
	"4654967986887\n"
	"4564679986453\n"
	"1224686865563\n"
	"2546548887735\n"
	"4322674655533\n";
static const int expected = 102;

#define MAX_STRIDE 3
#define MAX_GRID 150 * 150
#define MAX_QUEUE 10000
#define AT(G, X, Y) (G)[(Y) * (cols) + (X)]

typedef enum {
	U,
	D,
	L,
	R,
	NOP,
} Dir;

typedef struct {
	size_t x, y;
	u32 c;
	Dir d;
	u8 n;
} Node;

typedef struct {
	Node ns[MAX_QUEUE];
	size_t len;
} Queue;

static u8 grid[MAX_GRID];
static Queue tovisit;
static bool visited[MAX_GRID][MAX_STRIDE * NOP];

static void push(Queue *q, Node n) {
	assert(q->len < ASZ(q->ns));
	// append at the end
	q->ns[q->len++] = n;

	// heapify-up
	size_t i = q->len - 1;
	size_t pi = (i - 1) / 2;
	while (i && q->ns[i].c < q->ns[pi].c) {
		Node tmp = q->ns[pi];
		q->ns[pi] = q->ns[i];
		q->ns[i] = tmp;
		i = pi;
		pi = (i - 1) / 2;
	}
}

static Node pop(Queue *q) {
	assert(q->len);
	// remove root element
	Node top = q->ns[0];

	// make the last element the new root
	q->ns[0] = q->ns[--q->len];

	// heapify-down
	size_t i = 0;
	size_t li = 1;
	size_t ri = 2;
	while ((li < q->len && q->ns[li].c < q->ns[i].c) ||
	       (ri < q->len && q->ns[ri].c < q->ns[i].c)) {
		size_t ti = ri < q->len && q->ns[ri].c < q->ns[li].c ? ri : li;
		Node tmp = q->ns[ti];
		q->ns[ti] = q->ns[i];
		q->ns[i] = tmp;
		i = ti;
		li = 2 * i + 1;
		ri = 2 * i + 2;
	}

	return top;
}

static int solve(const char *const input) {
	assert(strlen(input) < MAX_GRID);
	const size_t cols = strcspn(input, "\n");
	const size_t rows = strlen(input) / (cols + 1);
	printf("Grid: %zu x %zu\n", rows, cols);
	for (size_t y = 0; y < rows; y++) {
		for (size_t x = 0; x < cols; x++) {
			int n = input[y * (cols + 1) + x] - '0';
			assert(n < UINT8_MAX);
			AT(grid, x, y) = (u8)n;
		}
	}

	memset(visited, 0, sizeof(visited));
	push(&tovisit, (Node){.c = 0, .x = 0, .y = 0, .d = NOP, .n = 0});
	while (tovisit.len) {
		Node n = pop(&tovisit);
		if (n.x == cols - 1 && n.y == rows - 1) return (int)n.c;
		if (n.d != NOP && n.n) {
			if (AT(visited, n.x, n.y)[(n.n - 1) * NOP + n.d])
				continue;
			AT(visited, n.x, n.y)[(n.n - 1) * NOP + n.d] = true;
		}

		// Straight
		if (n.n < MAX_STRIDE && n.d != NOP) {
			Node nn = {
				.x = n.d == L ? n.x - 1
			           : n.d == R ? n.x + 1
			                      : n.x,
				.y = n.d == U ? n.y - 1
			           : n.d == D ? n.y + 1
			                      : n.y,
				.n = n.n + 1,
				.c = n.c + AT(grid, nn.x, nn.y),
				.d = n.d,
			};

			if (nn.x < cols && nn.y < rows) push(&tovisit, nn);
		}
		// Turn
		for (u8 d = 0; d < NOP; d++) {
			if (n.d == d) continue;
			if (n.d + d == L + R || n.d + d == U + D) continue;
			Node nn = {
				.x = d == L ? n.x - 1
			           : d == R ? n.x + 1
			                    : n.x,
				.y = d == U ? n.y - 1
			           : d == D ? n.y + 1
			                    : n.y,
				.n = 1,
				.c = n.c + AT(grid, nn.x, nn.y),
				.d = d,
			};
			if (nn.x < cols && nn.y < rows) push(&tovisit, nn);
		}
	}
	assert(false);
}

int main(void) {
	const int actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %d actual %d\n", expected, actual);
		return 1;
	} else printf("PASS!\n\n");
	printf("%d\n", solve(readToString("17.txt")));
	return 0;
}
