example = """
seeds: 79 14 55 13

seed-to-soil map:
50 98 2
52 50 48

soil-to-fertilizer map:
0 15 37
37 52 2
39 0 15

fertilizer-to-water map:
49 53 8
0 11 42
42 0 7
57 7 4

water-to-light map:
88 18 7
18 25 70

light-to-temperature map:
45 77 23
81 45 19
68 64 13

temperature-to-humidity map:
0 69 1
1 0 69

humidity-to-location map:
60 56 37
56 93 4
""".strip()
expected = 46
filename = "5.txt"

from functools import reduce


def solve(input):
    seeds, *mappings = input.strip().split("\n\n")
    seeds = list(map(int, seeds.split()[1:]))

    return min(reduce(lookup, mappings, zip(seeds[0::2], seeds[1::2])))[0]


def lookup(inputs, mapping):
    for start, length in inputs:
        while length > 0:
            for m in mapping.split("\n")[1:]:
                dst, src, len = map(int, m.split())
                delta = start - src
                if delta in range(len):
                    len = min(len - delta, length)
                    yield (dst + delta, len)
                    start += len
                    length -= len
                    break
            else:
                yield (start, length)
                break


actual = solve(example)
assert actual == expected, f"expected {expected}, got {actual}"

with open(filename) as input:
    print(solve(input.read()))
