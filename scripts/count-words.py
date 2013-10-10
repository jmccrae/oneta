from nltk import *
import json
import sys

def main():
  """Calculate the word map, i.e., an index of the words in a corpus"""
  if len(sys.argv) != 2:
      exit("Usage python count-corpus.py corpus")
  corpus = open(sys.argv[1])
  words = dict()
  count = 1
  doc = 1

  for line in corpus:
    if doc % 100 == 0:
      sys.stderr.write(".")
    doc += 1
    tokens = word_tokenize(line)
    for token in tokens:
        if token not in words:
           words[token] = count
           count += 1
  sys.stderr.write("\n")
  
  print(len(words))

if __name__ == "__main__":
    main()
