example = """
0 3 6 9 12 15
1 3 6 10 15 21
10 13 16 21 30 45
""".strip()
expected = 114
filename = "9.txt"


def solve(lines):
    return sum(extrapolate([int(x) for x in line.split()]) for line in lines)


def extrapolate(seq):
    derivations = [seq]
    while any(x != 0 for x in derivations[-1]):
        derivations.append(
            [a - b for a, b in zip(derivations[-1][1:], derivations[-1])]
        )

    for dx, xs in zip(derivations[-1:0:-1], derivations[-2::-1]):
        xs.append(xs[-1] + dx[-1])

    print(derivations)
    print(seq)
    print("===========================")

    return seq[-1]


actual = solve(example.split("\n"))
assert actual == expected, f"expected {expected}, got {actual}"

with open(filename) as lines:
    print(solve([l.strip() for l in lines]))
