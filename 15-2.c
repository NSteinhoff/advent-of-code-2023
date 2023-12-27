#include "prelude.h"

#define N 256
#define MAX_NODES 16

static const char *example =
	"rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7\n";
static const int expected = 145;
static bool verbose = false;

typedef struct {
	const char *s;
	u8 len;
} Key;

typedef struct {
	Key key;
	u8 focal;
} Node;

typedef struct {
	Node nodes[MAX_NODES];
	size_t len;
} Bucket;

static Bucket table[N];

static void reset() {
	memset(table, 0, sizeof(table));
}

static size_t hash(const char *s, size_t len) {
	size_t h = 0;
	for (; len--; s++) {
		assert(*s > 0);
		h += (uchar)*s;
		h *= 17;
		h %= 1 << 8;
	}
	return h;
}

static bool cmp(const Key *a, const Key *b) {
	return a->len == b->len && !strncmp(a->s, b->s, a->len);
}

static void insert(Node *node) {
	size_t i = hash(node->key.s, node->key.len);
	assert(i < ASZ(table));
	Bucket *bucket = &table[i];
	for (size_t j = 0; j < bucket->len; j++) {
		if (cmp(&node->key, &bucket->nodes[j].key)) {
			bucket->nodes[j] = *node;
			return;
		}
	}
	assert(bucket->len < ASZ(bucket->nodes));
	bucket->nodes[bucket->len++] = *node;
}

static void delete(const Key *key) {
	size_t i = hash(key->s, key->len);
	assert(i < ASZ(table));
	Bucket *bucket = &table[i];
	bool found = false;
	for (size_t j = 0; j < bucket->len; j++) {
		if (!found && cmp(key, &bucket->nodes[j].key)) {
			found = true;
			continue;
		}
		if (found) bucket->nodes[j - 1] = bucket->nodes[j];
	}
	if (found) bucket->len--;
}

static void printTable() {
	for (size_t i = 0; i < ASZ(table); i++) {
		Bucket *b = &table[i];
		if (!b->len) continue;
		printf("Box %zu:", i);
		for (size_t j = 0; j < b->len; j++) {
			Node *n = &b->nodes[j];
			printf("[%.*s %u]", n->key.len, n->key.s, n->focal);
		}
		printf("\n");
	}
}

static int power() {
	int p = 0;
	for (size_t i = 0; i < ASZ(table); i++) {
		Bucket *b = &table[i];
		if (!b->len) continue;
		for (size_t j = 0; j < b->len; j++) {
			Node *n = &b->nodes[j];
			p += (i + 1) * (j + 1) * n->focal;
		}
	}
	return p;
}

static int solve(const char *const input) {
	reset();
	for (const char *s = input; *s;) {
		Key k = (Key){
			.s = s,
			.len = (u8)strcspn(s, "=-"),
		};
		s += k.len;
		char op = *s++;
		switch (op) {
		case '-':
			if (verbose) printf("Delete %.*s\n", k.len, k.s);
			delete (&k);
			break;
		case '=': {
			int focal = atoi(s);
			assert(focal > 0 || focal < 256);
			Node n = (Node){
				.key = k,
				.focal = (u8)focal,
			};
			if (verbose)
				printf("Insert %.*s %d\n", k.len, k.s, n.focal);
			insert(&n);
			break;
		default:
			assert(false);
			break;
		}
		}
		s += strcspn(s, ",\n") + 1;
		if (verbose) printTable();
	}
	return power();
}

int main(void) {
	const int actual = solve(example);
	if (actual != expected) {
		printf("FAIL: expected %d actual %d\n", expected, actual);
		return 1;
	} else printf("PASS!\n\n");
	printf("%d\n", solve(readToString("15.txt")));
	return 0;
}
