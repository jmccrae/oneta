import sys
import nltk
import nltk.corpus
import re
import string

def main():
    stops = nltk.corpus.stopwords.words(sys.argv[1])
    for line in sys.stdin:
        tokens = nltk.word_tokenize(line.decode(sys.getfilesystemencoding()))
        for token in tokens:
            if re.match("\w",token,flags=re.UNICODE) and token not in stops:
                sys.stdout.write(string.lower(token) + " ")
        print ""



if __name__ == "__main__":
    main()
