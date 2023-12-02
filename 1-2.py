example = """two1nine
eightwothree
abcone2threexyz
xtwone3four
4nineeightseven2
zoneight234
7pqrstsixteen"""

digit_names = [
    [1, "one"],
    [2, "two"],
    [3, "three"],
    [4, "four"],
    [5, "five"],
    [6, "six"],
    [7, "seven"],
    [8, "eight"],
    [9, "nine"],
]


def parse(line):
    for i, c in enumerate(line):
        if c.isdigit():
            yield int(c)

        parsed = next(
            (digit for [digit, name] in digit_names if line[i : i + len(name)] == name),
            None,
        )

        if parsed is not None:
            yield parsed

def numbers(input):
    for line in input.split("\n"):
        if len(line) == 0:
            continue

        digits = list(parse(line))

        yield digits[0] * 10 + digits[-1]


def solve(input):
    return sum(number for number in numbers(input))


if __name__ == "__main__":
    assert solve(example) == 281
    with open("1.txt", "r") as file:
        print(solve(file.read()))
