import re
import sys

def main():
    """Simply append two project vectors for mate finding trial"""
    if len(sys.argv) != 4 and len(sys.argv) != 5:
        exit("Usage: python merge-projections.py proj1 proj2 proj-out")

    proj1 = open(sys.argv[1])
    proj2 = open(sys.argv[2])
    projout = open(sys.argv[3],"w")
    if len(sys.argv) == 5:
        l = float(sys.argv[4])
    else:
        l = 1.0
    while True:
        l1 = proj1.readline()
        l2 = proj2.readline()
        if l1 == '' or l2 == '':
            break

        ss1 = l1.split(" ||| ")
        ss2 = l2.split(" ||| ")

        idxs1 = [int(x) for x in ss1[0].split(" ")]
        # This isn't correct to proj1 must be a fixed size system
        m = max(idxs1) + 1
        idxs2 = [str(int(x) + m) for x in ss2[0].split(" ")]

        vals2 = [str(float(x)*l) for x in ss2[1].strip().split(" ")]

        projout.write(ss1[0] + " " + (" ".join(idxs2)) + " ||| " + ss1[1].strip() + " " + " ".join(vals2) + "\n")


if __name__ == "__main__":
    main()
