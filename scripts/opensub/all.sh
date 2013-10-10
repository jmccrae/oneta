if [ -z $K ]
then
    scala scripts/opensub/build-for-oneta.scala en
    scala scripts/opensub/build-for-oneta.scala de

    mv docs.en data/OpenSubtitles/
    mv docs.de data/OpenSubtitles/

    #scala scripts/deduplicate.scala data/OpenSubtitles/docs.en data/OpenSubtitles/docs.de 0.99 data/OpenSubtitles/docs.dedup.en data/OpenSubtitles/docs.dedup.de data/OpenSubtitles/dedups

    scala scripts/opensub/hold-out-one.scala data/OpenSubtitles/docs en de

    python scripts/normalize-corpus.py english < data/OpenSubtitles/docs.train.en > data/OpenSubtitles/docs.train.norm.en
    python scripts/normalize-corpus.py german < data/OpenSubtitles/docs.train.de > data/OpenSubtitles/docs.train.norm.de
    python scripts/normalize-corpus.py english < data/OpenSubtitles/docs.test.en > data/OpenSubtitles/docs.test.norm.en
    python scripts/normalize-corpus.py german < data/OpenSubtitles/docs.test.de > data/OpenSubtitles/docs.test.norm.de
    K=100
fi

echo "K=$K"

# ONETA
bin/oneta data/wiki-en-de/de_train.txt data/OpenSubtitles/docs.train.norm.de 3000 working/opensub-de.vecs
bin/oneta data/wiki-en-de/de_train.txt data/OpenSubtitles/docs.test.norm.de 3000 working/opensub-de.test.vecs
bin/sim-csv working/opensub-de.vecs  working/opensub-de.test.vecs > working/opensub-de.csv
scala scripts/opensub/take-topN.scala working/opensub-de.csv $K > working/opensub-de.oneta.top$K

# Entropy
rm working/opensub-de.lm
IRSTLM=~/moses/irstlm/ ~/moses/irstlm/bin/build-lm.sh -i data/OpenSubtitles/docs.train.de -t tmp -p -s improved-kneser-ney -o working/opensub-de.lm
gunzip working/opensub-de.lm.gz
~/moses/irstlm/bin/score-lm -lm working/opensub-de.lm < data/OpenSubtitles/docs.train.de > working/opensub-de.out.lmscores
rm working/opensub-de.test.lm
IRSTLM=~/moses/irstlm/ ~/moses/irstlm/bin/build-lm.sh -i data/OpenSubtitles/docs.test.de -t tmp -p -s improved-kneser-ney -o working/opensub-de.test.lm
gunzip working/opensub-de.test.lm.gz
~/moses/irstlm/bin/score-lm -lm working/opensub-de.test.lm < data/OpenSubtitles/docs.train.de > working/opensub-de.in.lmscores
scala scripts/opensub/topNentropy.scala working/opensub-de.in.lmscores working/opensub-de.out.lmscores $K data/OpenSubtitles/docs.train.de > working/opensub-de.lment.top$K

scala scripts/opensub/topN-corpus.scala OpenSubtitles2012/docs/doc .en.txt working/opensub-de.oneta.top$K > working/corpus-oneta-en-de.en
scala scripts/opensub/topN-corpus.scala OpenSubtitles2012/docs/doc .de.txt working/opensub-de.oneta.top$K > working/corpus-oneta-en-de.de
scala scripts/opensub/topN-corpus.scala OpenSubtitles2012/docs/doc .en.txt working/opensub-de.lment.top$K > working/corpus-lment-en-de.en
scala scripts/opensub/topN-corpus.scala OpenSubtitles2012/docs/doc .de.txt working/opensub-de.lment.top$K > working/corpus-lment-en-de.de
