from nltk import *
import json
import sys

def main():
    """Simply project the word frequency vector as the topic vector"""
    if len(sys.argv) != 4:
        exit("Usage: python dummy.py corpus word-map out")

    wordMap = json.loads(open(sys.argv[2]).read())

    corpus = open(sys.argv[1])
    out = open(sys.argv[3],"w")
    for line in corpus:
        tokens = word_tokenize(line.decode("utf-8"))
        freqs = dict()
        for token in tokens:
            if token in wordMap.keys():
                if token in freqs.keys():
                    freqs[token] += 1
                else:
                    freqs[token] = 1

        for t in freqs.keys():
            out.write(str(wordMap[t]) + " ")
                
        out.write("||| ")
        for f in freqs.values():
            out.write(str(f) + " ")
        out.write("\n")

    out.flush()
    out.close()

if __name__ == "__main__":
    main()
