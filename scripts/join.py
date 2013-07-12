import sys 

# This script takes two files as input and appends them line-by-line

if len(sys.argv) != 3:
    exit("Usage: python join.py file1 file2 > joint")

f1 = open(sys.argv[1])
f2 = open(sys.argv[2])
l1 = "\n"
l2 = "\n"

while l1 and l2:
    l1 = f1.readline()
    l2 = f2.readline()
    print(l1.strip() + " " + l2.strip())

