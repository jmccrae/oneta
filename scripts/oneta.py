from nltk import *
from numpy import *
from scipy.sparse import *
import json
import sys

def main():
    """Apply the orthonormalization explicit topic analysis method"""
    if len(sys.argv) != 5:
        exit("Usage: python oneta.py train-corpus test-corpus kernel-size output")

    w1Words = dict()
    w2Words = dict()
    W1 = 0
    W2 = 0

    D1 = int(sys.argv[3])

    sys.stderr.write("First scan of training data\n")
    
    J = 0
    # Read through the corpus to decided which words are in the dense set and which in the sparse set
    corpus = open(sys.argv[1],"r")
    for line in corpus:
        tokens = word_tokenize(line)
        for token in tokens:
            tk_decoded = token.decode("utf-8")
            if J < D1 and tk_decoded not in w1Words:
                w1Words[tk_decoded] = W1
                W1 += 1
            elif J >= D1 and tk_decoded not in w2Words:
                w2Words[tk_decoded] = W2
                W2 += 1
        J += 1
    corpus.close()

    D2 = J - D1

    # Partition the corpus into a L-shaped matrix
    sys.stderr.write("Building matrices")
    At = lil_matrix((D1,W1))
    B = lil_matrix((W1,D2))
    Ct = lil_matrix((D2,W2))

    corpus = open(sys.argv[1],"r")

    j = 0
    for line in corpus:
        sys.stderr.write(".")
        tokens = word_tokenize(line)
        docsq = 0.
        for token in tokens:
            tk_decoded = token.decode("utf-8")
            if j < D1: # tk_decoded in w1words
                tkId = w1Words[tk_decoded]
                docsq += (At[j,tkId]+1)**2 - (At[j,tkId])**2
                At[j,tkId] += 1.
            elif tk_decoded in w1Words:
                tkId = w1Words[tk_decoded]
                docsq += (B[tkId,j-D1]+1)**2 - (B[tkId,j-D1])**2
                B[tkId,j-D1] += 1.
            else:
                tkId = w2Words[tk_decoded]
                docsq += (Ct[j-D1,tkId]+1)**2 - (Ct[j-D1,tkId])**2
                Ct[j-D1,tkId] += 1.
        if j < D1:
            At[j,:] /= math.sqrt(docsq)
        else:
            for w in range(0,W1):
              B[w,j-D1] /= math.sqrt(docsq)
            Ct[j-D1,:] /= math.sqrt(docsq)
        j += 1

    sys.stderr.write("\nBuild Cn\n")
    Cn = zeros((D2,1))
    Ct = Ct.tocsr()
    for i in range(0,D2):
        v = ((Ct[i,:] * Ct[i,:].transpose())[0,0])
        if v == 0:
            Cn[i,0] = 1.
        else:
            Cn[i,0] = v

    # Building real matrices
    sys.stderr.write("Calculating ATA\n")
    ATA = (At * At.transpose()).todense() # D1 x D1
    At = At.tocsr()
    B = B.tocsc()

    sys.stderr.write("Solve inverse\n")
    ATAi = linalg.inv(ATA)

    # The real calculation is that if we have input vector [ d_1 d_2 ] ^ T 
    # We yield [ (A^T * A)^-1 * A^T ( d1^T - B * (C^T * d2 / Cn) ) (C^T * d2 / Cn)
    sys.stderr.write("Calculating projected vectors\n")

    out = open(sys.argv[4],"w")
    testDocs = open(sys.argv[2],"r")
    for testDoc in testDocs:
        sys.stderr.write(".")
        corpus = open(sys.argv[1],"r")
        d1 = zeros((W1,1))
        d2 = zeros((W2,1))
        tokens = word_tokenize(testDoc)
        for token in tokens:
            tk_decoded = token.decode("utf-8")
            if tk_decoded in w1Words:
                d1[w1Words[tk_decoded],0] += 1
            elif tk_decoded in w2Words:
                d2[w2Words[tk_decoded],0] += 1
        norm = sqrt(sum(d1**2) + sum(d2**2))
        d1 /= norm 
        d2 /= norm
        v2 = (Ct * d2) / Cn
        v1 = ATAi * (At * (d1 - B * v2))
        for j in range(0,D1+D2):
            out.write(str(j) + " ")
        out.write("||| ")
        for j in range(0,D1):
            out.write(str(v1[j,0]) + " ")
        for j in range(0,D2):
            out.write(str(v2[j,0]) + " ")
        out.write("\n")

    out.flush()
    out.close()
    sys.stderr.write("\n")

if __name__ == "__main__":
    main()
