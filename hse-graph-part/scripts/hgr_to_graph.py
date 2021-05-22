n, m = map(int, input().split())
lines = [input() for i in range(n)]
m = 0
for i in range(len(lines)):
    cur = [str(int(v) + 1) for v in lines[i].split()]
    m += len(cur)
    for v in cur:
        n = max(n, int(v))
    lines[i] = " ".join(cur)

print(n, m // 2)
for i in range(n):
    if (i < len(lines)):
        print(lines[i])
    else:
        print()
