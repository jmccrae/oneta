l1=en
l1Name=english
l2=de
l2Name=german

scala scripts/opensub/build-for-oneta.scala $l1
scala scripts/opensub/build-for-oneta.scala $l2

mv docs.$l1 data/OpenSubtitles/
mv docs.$l2 data/OpenSubtitles/

#scala scripts/deduplicate.scala data/OpenSubtitles/docs.en data/OpenSubtitles/docs.de 0.99 data/OpenSubtitles/docs.dedup.en data/OpenSubtitles/docs.dedup.de data/OpenSubtitles/dedups
if [ $1 ]
then
    cp data/OpenSubtitles/docs.$l1 data/OpenSubtitles/docs.train.$l1
    cp data/OpenSubtitles/docs.$l2 data/OpenSubtitles/docs.train.$l2
    scala scripts/opensub/build-single.scala OpenSubtitles2012/dedup-all/doc${1}.${l1}.txt > data/OpenSubtitles/docs.test.$l1
    scala scripts/opensub/build-single.scala OpenSubtitles2012/dedup-all/doc${1}.${l2}.txt > data/OpenSubtitles/docs.test.$l2
    echo $1 > data/OpenSubtitles/docs.heldout 
else
    scala scripts/opensub/hold-out-one.scala data/OpenSubtitles/docs $l1 $l2
fi

python scripts/normalize-corpus.py $l1Name < data/OpenSubtitles/docs.train.$l1 > data/OpenSubtitles/docs.train.norm.$l1
python scripts/normalize-corpus.py $l2Name < data/OpenSubtitles/docs.train.$l2 > data/OpenSubtitles/docs.train.norm.$l2
python scripts/normalize-corpus.py $l1Name < data/OpenSubtitles/docs.test.$l1 > data/OpenSubtitles/docs.test.norm.$l1
python scripts/normalize-corpus.py $l2name < data/OpenSubtitles/docs.test.$l2 > data/OpenSubtitles/docs.test.norm.$l2
