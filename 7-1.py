example = """
32T3K 765
T55J5 684
KK677 28
KTJJT 220
QQQJA 483
""".strip()
expected = 6440
filename = "7.txt"


def solve(lines):
    return 42


actual = solve(example.split("\n"))
assert actual == expected, f"expected {expected}, got {actual}"

with open(filename) as lines:
    print(solve([l.strip() for l in lines]))
