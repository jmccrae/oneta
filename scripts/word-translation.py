from nltk import *
import math
import json
import sys
import gzip
import random

def main():
    """Calculate the word-by-word translation of a set of document"""
    if len(sys.argv) != 5:
        exit("Usage: python word-translation.py phrase-table_ft[.gz] word-map corpus_f out_t")

    wordMap = json.loads(open(sys.argv[2]).read())

    if sys.argv[1].endswith(".gz"):
        pt = gzip.open(sys.argv[1])
    else:
        pt = open(sys.argv[1])

    s = ""

    f2t = dict()
    f = ""
    t = ""
    best = -1e100
    linesRead = 1

    sys.stderr.write("Extracting translation index")
    wk = wordMap.keys()
    for line in pt:
      if linesRead % 10000 == 0:
          sys.stderr.write(".")
      linesRead += 1
      ss = line.decode("utf-8").strip().split(" ||| ")
      if len(ss) > 2 and ss[0] in wk and ss[1] in wk:
        if f != ss[0]:
          if f != "":        
              f2t[f] = t
          best = -1e100
          f = ss[0]
        score = sum([math.log(float(x)) for x in ss[2].split(" ")])
        if score > best:
          best = score
          t = ss[1]
    sys.stderr.write("\n")

    if f != "":
        f2t[f] = t

    corpus = open(sys.argv[3])
    out = open(sys.argv[4],"w")
    sys.stderr.write("Applying translation")
    linesRead = 1
    for line in corpus:
        if linesRead % 10 == 0:
            sys.stderr.write(".")
        linesRead += 1
        tokens = word_tokenize(line)
        freqs = dict()
        for token in tokens:
            tk_decoded = token.decode("utf-8")
            if tk_decoded not in freqs:
                freqs[tk_decoded] = 1
            else:
                freqs[tk_decoded] += 1
        freqs2 = dict()
        for t,f in freqs.items():
            if t in f2t.keys():
                if random.random() < 0.001:
                    print("%s => %s" % (t,f2t[t]))
                k = wordMap[f2t[t]]
                if k in freqs2:
                    freqs2[k] += f
                else:
                    freqs2[k] = f
#            elif t in wordMap.keys():
#                k = wordMap[t]
#                if k in freqs2:
#                    freqs2[k] += f
#                else:
#                    freqs2[k] = f
        
        for t in freqs2.keys():
            out.write(str(t) + " ")
        out.write("||| ")
        for f in freqs2.values():
            out.write(str(f) + " ")
        out.write("\n")

    sys.stderr.write("\n")
    out.flush()
    out.close()

if __name__ == "__main__":
    main()
