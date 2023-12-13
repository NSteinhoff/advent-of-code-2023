example = """
0 3 6 9 12 15
1 3 6 10 15 21
10 13 16 21 30 45
""".strip()
expected = 2
filename = "9.txt"


def solve(lines):
    return sum(extrapolate([int(x) for x in line.split()]) for line in lines)


def extrapolate(seq):
    ds = [seq]
    while any(x != 0 for x in ds[-1]):
        ds.append([a - b for a, b in zip(ds[-1][1:], ds[-1])])
    for dx, x in zip(ds[-1:0:-1], ds[-2::-1]):
        xs.insert(0, x[0] - dx[0])
    return seq[0]


actual = solve(example.split("\n"))
assert actual == expected, f"expected {expected}, got {actual}"

with open(filename) as lines:
    print(solve([l.strip() for l in lines]))
