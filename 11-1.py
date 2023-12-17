example = """
...#......
.......#..
#.........
..........
......#...
.#........
.........#
..........
.......#..
#...#.....
""".strip()

from itertools import combinations


def solve(input):
    lines = input.split()

    coords = [
        (x, y)
        for y, line in enumerate(lines)
        for x, c in enumerate(line)
        if c == "#"
    ]

    xs, ys = (set(cs) for cs in zip(*coords))
    pairs = list(combinations(coords, 2))

    dists = zip(
        [
            sum(1 if x in xs else 2 for x in range(min(x1, x2), max(x1, x2)))
            for (x1, _), (x2, _) in pairs
        ],
        [
            sum(1 if y in ys else 2 for y in range(min(y1, y2), max(y1, y2)))
            for (_, y1), (_, y2) in pairs
        ],
    )

    return sum(x + y for x, y in dists)


if __name__ == "__main__":
    assert solve(example) == 374
    with open("11.txt", "r") as file:
        print(solve(file.read()))
