example = """
Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green
""".strip()

import math, re

# 12 red cubes, 13 green cubes, and 14 blue cubes?
limit = {'r': 12, 'g': 13, 'b': 14}

def f(line):
    bag = {'r': 0, 'g': 0, 'b': 0}
    id = re.match("Game (\d+):", line)[1]
    for n, c in re.findall("(\d+) (\w)", line):
        bag[c] = max(bag[c], int(n))
    if all(v <= limit[k] for k, v in bag.items()):
        return int(id)
    return 0

def solve(lines):
    return sum(map(f, lines))

result = solve(example.split("\n"))
assert result == 8, f"expected 8, got {result}"

with open("2.txt") as lines:
    print(solve(lines))
