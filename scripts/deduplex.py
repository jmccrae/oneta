import sys

def main():
    """Seperates a file into two parallel files"""
    if len(sys.argv) != 3:
        exit("Usage: cat file1 | pypy deduplex.py odd-lines even-lines")

    odd = open(sys.argv[1],"w")
    even = open(sys.argv[2],"w")
    count = 1

    for line in sys.stdin:
        if count % 2 == 1:
            odd.write(line)
        else:
            even.write(line)
        count += 1

if __name__ == "__main__":
    main()
