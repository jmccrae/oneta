import math
import sys
import numpy
from scipy.sparse import *


class SparseArray:
    def __init__(self,i,d):
        args = numpy.argsort(i)
        self.idxs = i
        self.idxs.sort()
        self.data = [ d[i] for i in args ]

def cosSim(a,b):
    """Compute the cosine similarity of two vectors"""
    ab = 0.
    a2 = 0.
    b2 = 0.
    i = 0
    j = 0

    while i < len(a.data) or j < len(b.data):
        if i == len(a.data):
            b2 += b.data[j] * b.data[j]
            j += 1
        elif j == len(b.data):
            a2 += a.data[i] * a.data[i]
            i += 1
        elif a.idxs[i] < b.idxs[j]:
            a2 += a.data[i] * a.data[i]
            i += 1
        elif a.idxs[i] > b.idxs[j]:
            b2 += b.data[j] * b.data[j]
            j += 1
        else:
            a2 += a.data[i] * a.data[i]
            b2 += b.data[j] * b.data[j]
            ab += a.data[i] * b.data[j]
            i += 1
            j += 1

    if a2 == 0 or b2 == 0:
        return 1
    else:
        return ab / math.sqrt(a2*b2)

def main():
    """Apply a mate-finding trial to two sets of topic vectors"""
    if len(sys.argv) != 3:
        exit("Usage: python mate-finding.py topics1 topics2")

    t1 = []
    topic1file = open(sys.argv[1],"r")
    for line in topic1file:
        ll = line.strip().split(" ||| ")
        idxs = [int(x) for x in ll[0].split(" ")] 
        data = [float(x) for x in ll[1].split(" ")]
        if len(idxs) != len(data):
            exit("Bad input")
        t1.append(SparseArray(idxs,data))
        
    t2 = []
    topic2file = open(sys.argv[2],"r")
    for line in topic2file:
        ll = line.strip().split(" ||| ")
        idxs = [int(x) for x in ll[0].split(" ")] 
        data = [float(x) for x in ll[1].split(" ")]
        if len(idxs) != len(data):
            exit("Bad input")
        t2.append(SparseArray(idxs,data))

    assert(len(t1) == len(t2))

    top1prec = 0.0
    top5prec = 0.0
    top10prec = 0.0
    mrr = 0.0
    n = 0

    for i in range(0,len(t1)):
        sim = []
        for j in range(0,len(t1)):
            sim.append(cosSim(t1[i],t2[j]))
        rank = numpy.argsort(sim)
        idx = -1
        for j in range(0,len(t1)):
            if rank[j] == i:
                idx = len(t1) - j
        if idx == 1:
            top1prec += 1
            sys.stdout.write("+")
        elif idx < 10:
            sys.stdout.write(str(idx))
        else:
            sys.stdout.write("-")
        sys.stdout.flush()

        if idx <= 5:
            top5prec += 1
        if idx <= 10:
            top10prec += 1
            mrr += 1.0/idx
        n += 1

    print("")
    print("Top-1  Precision     : " + str(top1prec/n))
    print("Top-5  Precision     : " + str(top5prec/n))
    print("Top-10 Precision     : " + str(top10prec/n))
    print("Mean Reciprocal Rank : " + str(mrr / n))
        

if __name__ == "__main__":
    main()
