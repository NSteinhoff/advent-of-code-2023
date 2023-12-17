example = """
7-F7-
.FJ|7
SJLL7
|F--J
LJ.LJ
""".strip()

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
                if (r + i, c + j) != prev
                and grid[r + i][c + j] in connections[(i, j)]
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
        (i, j)
        for i, row in enumerate(grid)
        for j, c in enumerate(row)
        if c == "S"
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

    return len(list(steps(grid, pos, prev))) // 2


if __name__ == "__main__":
    assert solve(example) == 8
    with open("10.txt", "r") as file:
        print(solve(file.read()))
