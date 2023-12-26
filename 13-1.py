example = """
#.##..##.
..#.##.#.
##......#
##......#
..#.##.#.
..##..##.
#.#.##.#.

#...##..#
#....#..#
..##..###
#####.##.
#####.##.
..##..###
#....#..#
""".strip()

expected = 405

def find_mirror(grid):
    for r in range(1, len(grid)):
        above = grid[:r][::-1]
        below = grid[r:]

        if all(x == y for x, y in zip(above, below)):
            return r


def solve(input):
    total = 0;

    for block in input.strip().split("\n\n"):
        grid = block.splitlines();
        print()
        print(block)

        row = find_mirror(grid)
        if row:
            print("Row", row)
            total += row * 100

        col = find_mirror(list(zip(*grid)))
        if col:
            print("Col", col)
            total += col

    return total


if __name__ == "__main__":
    actual = solve(example)
    assert actual == expected, f"actual {actual}, expected {expected}"
    with open("13.txt", "r") as file:
        print(solve(file.read()))
