import sys

for line in sys.stdin:
    l1 = line.find(" ||| ")
    l2 = line.find(" ||| ",l1+1)
    if l1 > 0 and l2 > 0:
        sys.stdout.write(line[(l1+5):l2])
        sys.stdout.write(" ||| ")
        sys.stdout.write(line[0:l1])        
        sys.stdout.write(line[l2:])

