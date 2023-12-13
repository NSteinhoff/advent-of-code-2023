example = """
""".strip()
expected = 0
filename = "7.txt"

from functools import reduce


def solve(lines):
    return 42


actual = solve(example.split("\n"))
assert actual == expected, f"expected {expected}, got {actual}"

with open(filename) as lines:
    print(solve([l.strip() for l in lines]))
