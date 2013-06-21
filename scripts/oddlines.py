import sys

i = 1
for line in sys.stdin:
    if i % 2 == 1:
        print(line.strip())
    i += 1

