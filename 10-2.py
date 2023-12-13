example = """
...........
.S-------7.
.|F-----7|.
.||.....||.
.||.....||.
.|L-7.F-J|.
.|..|.|..|.
.L--J.L--J.
...........
""".strip()
expected = 4

example2 = """
.F----7F7F7F7F-7....
.|F--7||||||||FJ....
.||.FJ||||||||L7....
FJL7L7LJLJ||LJ.L-7..
L--J.L7...LJS7F-7L7.
....F-J..F7FJ|L7L7L7
....L7.F7||L7|.L7L7|
.....|FJLJ|FJ|F7|.LJ
....FJL-7.||.||||...
....L---J.LJ.LJLJ...
""".strip()
expected2 = 8

example3 = """
FF7FSF7F7F7F7F7F---7
L|LJ||||||||||||F--J
FL-7LJLJ||||||LJL-77
F--JF--7||LJLJ7F7FJ-
L---JF-JLJ.||-FJLJJ7
|F|F-JF---7F7-L7L|7|
|FFJF7L7F-JF7|JL---7
7-L-JL7||F7|L7F-7F7|
L.L7LFJ|||||FJL7||LJ
L7JLJL-JLJLJL--JLJ.L
""".strip()
expected3 = 10

verbose = True

edges = {
    "F": {(1, 0), (0, 1)},
    "7": {(1, 0), (0, -1)},
    "J": {(-1, 0), (0, -1)},
    "L": {(-1, 0), (0, 1)},
    "-": {(0, -1), (0, 1)},
    "|": {(-1, 0), (1, 0)},
}

connections = {
    (-1, 0): {"F", "7", "|", "S"},  # N
    (0, 1): {"J", "7", "-", "S"},  # E
    (1, 0): {"J", "L", "|", "S"},  # S
    (0, -1): {"L", "F", "-", "S"},  # W
}


def steps(grid, pos, prev):
    n = 1
    yield pos
    r, c = pos
    piece = grid[r][c]
    if verbose:
        print(0, (prev[0], prev[1]), grid[prev[0]][prev[1]])
        print(n, (r, c), piece)

    while piece != "S":
        n += 1
        pos = next(
            (
                (r + i, c + j)
                for i, j in edges[piece]
                if (r + i, c + j) != prev and grid[r + i][c + j] in connections[(i, j)]
            )
        )
        prev = r, c
        r, c = pos
        piece = grid[r][c]
        if verbose:
            print(n, (r, c), piece)
        yield pos


def solve(input):
    grid = input.split()
    r, c = next(
        (i, j) for i, row in enumerate(grid) for j, c in enumerate(row) if c == "S"
    )
    directions = [
        (i, j)
        for i, j in connections.keys()
        if r + i in range(len(grid)) and c + j in range(len(grid[0]))
    ]
    prev = (r, c)
    pos = next(
        (r + i, c + j)
        for i, j in directions
        if grid[r + i][c + j] in connections[(i, j)]
    )

    loop = list(steps(grid, pos, prev))
    area = shoelace(loop) // 2
    interior = area - len(loop) // 2 + 1

    return interior

def shoelace(loop):
    pairs = list(zip(loop, loop[1:] + [loop[0]]))
    return abs(sum(x1 * y2 - y1 * x2 for (y1, x1), (y2, x2) in pairs))

if __name__ == "__main__":
    result = solve(example)
    assert result == expected, f"actual {result}, expected {expected}"
    result2 = solve(example2)
    assert result2 == expected2, f"actual {result2}, expected {expected2}"
    result3 = solve(example3)
    assert result3 == expected3, f"actual {result3}, expected {expected3}"

    with open("10.txt", "r") as file:
        print(solve(file.read()))
