example = """1abc2
pqr3stu8vwx
a1b2c3d4e5f
treb7uchet"""


def solve(input):
    digits = (
        [c for c in line if c.isdigit()] for line in input.split("\n") if len(line) > 0
    )
    numbers = (int(ds[0] + ds[-1]) for ds in digits)
    return sum(numbers)


if __name__ == "__main__":
    assert solve(example) == 142
    with open("1.txt", "r") as file:
        print(solve(file.read()))
