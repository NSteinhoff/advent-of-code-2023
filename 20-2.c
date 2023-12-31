#include "prelude.h"

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

static u64 gcd(u64 a, u64 b) {
	while (b != 0) {
		u64 temp = b;
		b = a % b;
		a = temp;
	}
	return a;
}

static u64 lcm(u64 a, u64 b) {
	return (a * b) / gcd(a, b);
}

static Node *find(Node nodes[], size_t len, String *s) {
	for (size_t i = 0; i < len; i++)
		if (stringcmp(&nodes[i].name, s)) return &nodes[i];
	return NULL;
}

static u64 solve(const char *const input) {
	static Node nodes[MAX_NODES];
	memset(&nodes, 0, sizeof nodes);
	size_t num_nodes = 0;
	Node *bc = NULL;
	for (String l = chop(input, "\n"); *l.s;
	     l = chop(l.s + l.len + 1, "\n")) {
		const char *s = l.s;

		assert(num_nodes < ASZ(nodes));
		Node *n = &nodes[num_nodes++];

		if (strchr("%&", *s)) n->t = *s++;
		else n->t = '\0';

		String name = chop(s, " ");
		n->name = name;

		String arrow = chop(s += name.len + 1, " ");
		String targets = chop(s += arrow.len + 1, "\n");
		n->n_out = 0;
		for (; s <= targets.s + targets.len; s += strspn(s, " ,")) {
			String t = chop(s, " ,\n");
			assert(n->n_out < ASZ(n->out));
			n->out[n->n_out++] = t;
			s += t.len + 1;
		}

		if (!n->t) bc = n;
	}
	assert(bc);

	for (size_t i = 0; i < num_nodes; i++) {
		Node *n = &nodes[i];

		n->n_in = 0;
		for (size_t j = 0; n->t == '&' && j < num_nodes; j++) {
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

	static Queue pulses;
	memset(&pulses, 0, sizeof pulses);

	size_t presses = 0;
	size_t cycles[4] = {0};

	while (true) {
		presses++;
		for (size_t i = 0; i < bc->n_out; i++)
			enq(&pulses,
			    (Pulse){.from = bc->name, .to = bc->out[i], false});

		while (pulses.len) {
			Pulse p = deq(&pulses);
			Node *n = find(nodes, num_nodes, &p.to);
			if (!n) continue;

			// Record the cycles of lo pulses to the the inputs of the input to 'rx'
			// clang-format off
			if (!p.hi && !cycles[0] && !strncmp(p.to.s, "st", 2)) cycles[0] = presses;
			if (!p.hi && !cycles[1] && !strncmp(p.to.s, "tn", 2)) cycles[1] = presses;
			if (!p.hi && !cycles[2] && !strncmp(p.to.s, "hh", 2)) cycles[2] = presses;
			if (!p.hi && !cycles[3] && !strncmp(p.to.s, "dt", 2)) cycles[3] = presses;

			// Assume that they run on a perfect cycle and calcualte their least common multiple.
			if (cycles[0] && cycles[1] && cycles[2] && cycles[3]) {
				printf("Cycles: %zu, %zu, %zu, %zu\n", cycles[0], cycles[1], cycles[2], cycles[3]);
				size_t l = cycles[0];
				l = lcm(l, cycles[1]);
				l = lcm(l, cycles[2]);
				l = lcm(l, cycles[3]);
				return l;
			}
			// clang-format on

			if (n->t == '%') {
				if (p.hi) continue;
				n->on = !n->on;

				for (size_t i = 0; i < n->n_out; i++) {
					enq(&pulses, (Pulse){.from = n->name,
					                     .to = n->out[i],
					                     .hi = n->on});
				}
			} else {
				size_t n_hi = 0;
				for (size_t i = 0; i < n->n_in; i++) {
					if (stringcmp(&p.from, &n->in[i].name))
						n->in[i].high = p.hi;
					if (n->in[i].high) n_hi++;
				}
				bool all_hi = n_hi == n->n_in;
				for (size_t i = 0; i < n->n_out; i++) {
					enq(&pulses, (Pulse){.from = n->name,
					                     .to = n->out[i],
					                     .hi = !all_hi});
				}
			}
		}
	}
	assert(false);
}

int main(void) {
	printf("%llu\n", solve(readToString("20.txt")));
	return 0;
}
