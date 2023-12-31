#include "prelude.h"

static const char *example =
	"broadcaster -> a, b, c\n"
	"%a -> b\n"
	"%b -> c\n"
	"%c -> inv\n"
	"&inv -> a\n";
static const u64 expected = 32000000;

#define MAX_NODES 64
#define MAX_OUT 8
#define MAX_IN 12
#define MAX_PULSES 64

typedef struct {
	const char *s;
	size_t len;
} String;

static bool stringcmp(const String *s1, const String *s2) {
	if (s1->len != s2->len) return false;
	return !strncmp(s1->s, s2->s, s1->len);
}

static String chop(const char *const s, const char *const sep) {
	return (String){.s = s, .len = strcspn(s, sep)};
}

typedef struct {
	char t;
	String name;
	size_t n_out;
	String out[MAX_OUT];
	union {
		bool on;
		struct {
			size_t n_in;
			struct {
				String name;
				bool high;
			} in[MAX_IN];
		};
	};
} Node;

typedef struct {
	String from, to;
	bool hi;
} Pulse;

typedef struct {
	Pulse pulses[MAX_PULSES];
	size_t head, tail, len;
} Queue;

static Pulse deq(Queue *q) {
	assert(q->len--);
	Pulse *p = &q->pulses[q->head];
	q->head++;
	q->head %= MAX_PULSES;
	return *p;
}

static void enq(Queue *q, Pulse p) {
	assert(q->len++ < MAX_PULSES);
	q->pulses[q->tail] = p;
	q->tail++;
	q->tail %= MAX_PULSES;
}

static Node *find(Node nodes[], size_t len, String *s) {
	for (size_t i = 0; i < len; i++)
		if (stringcmp(&nodes[i].name, s)) return &nodes[i];
	return NULL;
}

static Node parseNode(const String *line) {
	Node n;

	const char *s = line->s;
	if (strchr("%&", *s)) n.t = *s++;
	else n.t = '\0';

	String name = chop(s, " ");
	n.name = name;

	String arrow = chop(s += name.len + 1, " ");
	String targets = chop(s += arrow.len + 1, "\n");
	n.n_out = 0;
	for (; s <= targets.s + targets.len; s += strspn(s, " ,")) {
		String t = chop(s, " ,\n");
		assert(n.n_out < ASZ(n.out));
		n.out[n.n_out++] = t;
		s += t.len + 1;
	}
	return n;
}

static void addInputs(Node nodes[], size_t len) {
	for (size_t i = 0; i < len; i++) {
		Node *n = &nodes[i];

		n->n_in = 0;
		for (size_t j = 0; n->t == '&' && j < len; j++) {
			Node *nn = &nodes[j];
			for (size_t k = 0; k < nn->n_out; k++) {
				if (!strncmp(n->name.s, nn->out[k].s,
				             n->name.len)) {
					assert(n->n_in < ASZ(n->in));
					n->in[n->n_in].name = nn->name;
					n->in[n->n_in].high = false;
					n->n_in++;
				}
			}
		}
	}
}

static void broadcast(Node *bc, Queue *q) {
	for (size_t i = 0; i < bc->n_out; i++)
		enq(q, (Pulse){.from = bc->name, .to = bc->out[i], false});
}

static void flipFlop(Node *n, const Pulse *p, Queue *q) {
	if (p->hi) return;
	n->on = !n->on;

	for (size_t i = 0; i < n->n_out; i++)
		enq(q, (Pulse){.from = n->name, .to = n->out[i], .hi = n->on});
}

static void conjunctor(Node *n, const Pulse *p, Queue *q) {
	size_t n_hi = 0;
	for (size_t i = 0; i < n->n_in; i++) {
		if (stringcmp(&p->from, &n->in[i].name)) n->in[i].high = p->hi;
		if (n->in[i].high) n_hi++;
	}
	bool hi = n_hi != n->n_in;
	for (size_t i = 0; i < n->n_out; i++)
		enq(q, (Pulse){.from = n->name, .to = n->out[i], .hi = hi});
}

static u64 solve(const char *const input) {
	static Node nodes[MAX_NODES];
	memset(&nodes, 0, sizeof nodes);
	size_t num_nodes = 0;
	Node *bc = NULL;
	for (String line = chop(input, "\n"); *line.s;
	     line = chop(line.s + line.len + 1, "\n")) {
		assert(num_nodes < ASZ(nodes));
		Node *n = &nodes[num_nodes++];
		*n = parseNode(&line);
		if (!n->t) bc = n;
	}
	assert(bc);
	addInputs(nodes, num_nodes);

	static Queue pulses;
	memset(&pulses, 0, sizeof pulses);

	size_t hi = 0;
	size_t lo = 0;
	size_t presses = 1000;
	while (presses--) {
		lo++;
		broadcast(bc, &pulses);

		while (pulses.len) {
			Pulse p = deq(&pulses);
			if (p.hi) hi++;
			else lo++;
			Node *n = find(nodes, num_nodes, &p.to);
			if (!n) continue;
			if (n->t == '%') flipFlop(n, &p, &pulses);
			else conjunctor(n, &p, &pulses);
		}
	}

	return lo * hi;
}

int main(void) {
	const u64 actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %llu actual %llu\n", expected, actual);
		return 1;
	} else printf("PASS!\n\n");
	printf("%llu\n", solve(readToString("20.txt")));
	return 0;
}
