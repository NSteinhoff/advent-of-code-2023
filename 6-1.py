example = """
Time:      7  15   30
Distance:  9  40  200
""".strip()
expected = 288
filename = "6.txt"

from math import prod


def solve(lines):
    ts, ds = [map(int, l.split()[1:]) for l in lines]
    f = lambda t, d: next(
        t - 2 * tt + 1 for tt in range(t) if tt * (t - tt) > d
    )
    return prod(map(f, ts, ds))


actual = solve(example.split("\n"))
assert actual == expected, f"expected {expected}, got {actual}"

with open(filename) as lines:
    print(solve([l.strip() for l in lines]))
