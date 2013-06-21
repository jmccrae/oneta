import sys

def main():
    """Build a corpus assuming that we have as input two documents
    of the form 
         title ||| text
    And the documents are sorted, titles aligned etc."""
    if len(sys.argv) != 6:
        exit("Usage: pypy build-corpus.py srcLang trgLang srcOut trgOut minLength")

    minLength = int(sys.argv[5])
    title1 = ""
    title2 = ""
    line1 = "\n"
    line2 = "\n"

    srcCorpus = open(sys.argv[1])
    trgCorpus = open(sys.argv[2])
    srcOut = open(sys.argv[3],"w")
    trgOut = open(sys.argv[4],"w")
    while line1 and line2:
        if title1 == title2:
            ss1 = line1.split(" ||| ")
            ss2 = line2.split(" ||| ")
            if len(ss1) == 2 and len(ss2) == 2 and len(ss1[1].split(" ")) >= minLength and len(ss2[1].split(" ")) >= minLength:
                srcOut.write(ss1[1])
                trgOut.write(ss2[1])
            line1 = srcCorpus.readline()
            line2 = srcCorpus.readline()
            if line1 and line2:
                title1 = line1[0:line1.index(" |||")]
                title2 = line2[0:line2.index(" |||")]
        elif title1 < title2:
            line1 = srcCorpus.readline()
            if line1:
                title1 = line1[0:line1.index(" |||")]
        else:
            line2 = trgCorpus.readline()
            if line2:
                title2 = line2[0:line2.index(" |||")]
    srcOut.flush()
    srcOut.close()
    trgOut.flush()
    trgOut.close()
    srcCorpus.close()
    trgCorpus.close()

if __name__ == "__main__":
    main()
