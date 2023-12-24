example = """
???.### 1,1,3
.??..??...?##. 1,1,3
?#?#?#?#?#?#?#? 1,3,1,6
????.#...#... 4,1,1
????.######..#####. 1,6,5
?###???????? 3,2,1
""".strip()

expected = 525152


def cache(func):
    cache = {}

    def cached(*args):
        if args not in cache:
            cache[args] = func(*args)
        return cache[args]

    return cached


@cache
def count(cfg, nums, flag=False):
    if not cfg:
        return 1 if sum(nums) == 0 else 0

    if sum(nums) == 0:
        return 0 if "#" in cfg else 1

    if cfg[0] == "#":
        if flag and nums[0] == 0:
            return 0
        return count(cfg[1:], (nums[0] - 1, *nums[1:]), True)

    if cfg[0] == ".":
        if flag and nums[0] > 0:
            return 0
        return count(cfg[1:], nums[1:] if nums[0] == 0 else nums, False)

    if flag:
        if nums[0] == 0:
            return count(cfg[1:], nums[1:], False)
        return count(cfg[1:], (nums[0] - 1, *nums[1:]), True)

    return count(cfg[1:], nums, False) + count(
        cfg[1:], (nums[0] - 1, *nums[1:]), True
    )

    return 1


def solve(input):
    total = 0
    for line in input.strip().split("\n"):
        cfg, nums = line.split()
        cfg = "?".join([cfg] * 5)
        nums = ",".join([nums] * 5)
        nums = tuple(map(int, nums.split(",")))
        total += count(cfg, nums)
    return total


if __name__ == "__main__":
    actual = solve(example)
    assert actual == expected, f"actual {actual}, expected {expected}"
    with open("12.txt", "r") as file:
        print(solve(file.read()))
