#!/bin/bash

TIME='/usr/bin/time'
do_time() {
    $TIME -f "Real: %Us\nMemory: %MkB" $@
}

EN_TEST=data/wiki-filtered-en-es/en_test.txt
ES_TEST=data/wiki-filtered-en-es/es_test.txt
EN_TRAIN=data/wiki-filtered-en-es/en_train.txt
ES_TRAIN=data/wiki-filtered-en-es/es_train.txt
# ENES was joined by scripts/join.py
ENES_TRAIN=data/wiki-filtered-en-es/enes_train.txt
ENES_TEST=data/wiki-filtered-en-es/enes_test.txt
PHRASE_TABLE=data/europarl-en_es.1wpt.gz
OUT=working/wiki-filtered-en-es.1

# ESA
do_time bin/oneta $EN_TRAIN $EN_TEST 0 $OUT.esa-en -sqnorm
do_time bin/oneta $ES_TRAIN $ES_TEST 0 $OUT.esa-es -sqnorm
echo "ESA"
bin/mate-finding $OUT.esa-en $OUT.esa-es

# ONETA
for i in 1 2 3 4 5 6 7 8 9
do
    do_time bin/oneta $EN_TRAIN $EN_TEST $[$i*1000] $OUT.oneta-${i}k-en
    do_time bin/oneta $ES_TRAIN $ES_TEST $[$i*1000] $OUT.oneta-${i}k-es
    echo "ONETA ${i}k"
    bin/mate-finding $OUT.oneta-${i}k-en $OUT.oneta-${i}k-es
done
do_time bin/oneta $EN_TRAIN $EN_TEST 9332 $OUT.oneta-full-en
do_time bin/oneta $ES_TRAIN $ES_TEST 9332 $OUT.oneta-full-es
echo "ONETA Full"
bin/mate-finding $OUT.oneta-full-en $OUT.oneta-full-es


# Translation
python scripts/words-in-corpus.py $ENES_TEST > $OUT.wordMap
do_time python scripts/word-translation.py $PHRASE_TABLE $OUT.wordMap $EN_TEST $OUT.trans-to-es
python scripts/dummy.py $ES_TEST $OUT.wordMap $OUT.dummy-es
echo "TRANSLATION"
bin/mate-finding $OUT.trans-to-es $OUT.dummy-es

# With translation
python scripts/merge-projections.py $OUT.oneta-full-en $OUT.trans-to-es $OUT.merge-en 0.005
python scripts/merge-projections.py $OUT.oneta-full-es $OUT.dummy-es $OUT.merge-es 0.005
echo "TRANSLATION+ONETA"
bin/mate-finding $OUT.merge-en $OUT.merge-es

# LSI
do_time bin/lsi $ENES_TRAIN $EN_TEST $ES_TEST 4000 $OUT.lsi-en $OUT.lsi-es 
echo "LSI"
bin/mate-finding $OUT.lsi-en $OUT.lsi-es

# LDA
do_time bin/lda $EN_TRAIN $ES_TRAIN $EN_TEST $ES_TEST 4000 $OUT.lda-en $OUT.lda-en
echo "LDA"
bin/mate-finding $OUT.lda-en $OUT.lda-es
