from scipy.sparse import *
from numpy import *
from nltk import *
import sys
import json

def main():
    """Compute the document-by-document cooccurrence matrix
       The parameters required are the corpus, which should be cleaned and on a document per line basis
       The word map (a JSON object), the number of documents and the file to write the co-occurrence matrix to"""
    if len(sys.argv) != 5:
        exit("Usage: python make-cooc.py corpus wordMap D cooc")

    wordMap = json.loads(open(sys.argv[2]).read())
    W = len(wordMap)
    D = int(sys.argv[3])

    X = lil_matrix((W,D))

    corpus = open(sys.argv[1],"r")

    j = 0

    for line in corpus:
        sys.stderr.write(".")
        tokens = word_tokenize(line)
        for token in tokens:
            tk_decoded = token.decode("utf-8")
            if tk_decoded in wordMap.keys():
                X[wordMap[tk_decoded],j] = X[wordMap[tk_decoded],j] + 1
            else:
                sys.stderr.write("Token not in word map: " + token)
        j += 1

    sys.stderr.write("Calculating Kernel")
    XTX = (X.transpose() * X).todense()

    save(sys.argv[4],XTX)


if __name__ == "__main__":
    main()
