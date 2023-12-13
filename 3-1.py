example = """
467..114..
...*......
..35..633.
......#...
617*......
.....+.58.
..592.....
......755.
...$.*....
.664.598..
""".strip()

import re

def solve(lines):
    board = list(l.strip() for l in lines)
    chars = {
        (r, c): []
        for r in range(len(board))
        for c in range(len(board[0]))
        if board[r][c] not in "01234566789."
    }

    for r, row in enumerate(board):
        for n in re.finditer(r"\d+", row):
            edge = {
                (r, c)
                for r in (r - 1, r, r + 1)
                for c in range(n.start() - 1, n.end() + 1)
            }
            for pos in edge & chars.keys():
                chars[pos].append(int(n[0]))

    return sum(sum(n) for n in chars.values())


result = solve(example.split("\n"))
assert result == 4361, f"expected 4361, got {result}"

with open("3.txt") as lines:
    print(solve(lines))
