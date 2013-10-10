import sys
import nltk
import nltk.corpus
import re
import string

def main():
    if len(sys.argv) != 2:
        sys.stderr.write("Usage:\n\t python normalize-corpus.py language < in > out\n")
        sys.exit(-1)
    sw = [w.decode(sys.getfilesystemencoding()) for w in nltk.corpus.stopwords.words(sys.argv[1])]
    for line in sys.stdin:
        tokens = nltk.word_tokenize(line.decode(sys.getfilesystemencoding()))
        for token in tokens:
            token = string.lower(token)
            while token.endswith("."):
                token = token[0:len(token)-1]
            if re.match("\w",token,flags=re.UNICODE) and token not in sw:
                sys.stdout.write(token.encode(sys.getfilesystemencoding()) + " ")
        print

if __name__ == "__main__":
    main()
