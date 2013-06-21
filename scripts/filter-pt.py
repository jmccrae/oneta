import sys
import re

for line in sys.stdin:
    if re.match("\w+ \|\|\| \w+ \|\|\|",line):
        sys.stdout.write(line)
