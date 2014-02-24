scala scripts/opensub/random-baseline.scala en > working/random-baseline

for J in 1 2 3 4 5
do
    T=`cat data/OpenSubtitles$J/docs.heldout`
    echo "T=$T"
    for K in 20 40 60 80 100
    do
        echo "K=$K"

        # ONETA
        bin/oneta data/wiki-en-de/de_train.txt data/OpenSubtitles$J/docs.train.norm.de 3000 working/opensub-de.vecs
        bin/oneta data/wiki-en-de/de_train.txt data/OpenSubtitles$J/docs.test.norm.de 3000 working/opensub-de.test.vecs
        bin/sim-csv working/opensub-de.vecs  working/opensub-de.test.vecs > working/opensub-de.csv
        scala scripts/opensub/take-topN.scala working/opensub-de.csv $K > working/opensub-de.oneta.top$K

        # Entropy
        rm working/opensub-de.lm
        IRSTLM=~/moses/irstlm/ ~/moses/irstlm/bin/build-lm.sh -i data/OpenSubtitles$J/docs.train.de -t tmp -p -s improved-kneser-ney -o working/opensub-de.lm
        gunzip working/opensub-de.lm.gz
        ~/moses/irstlm/bin/score-lm -lm working/opensub-de.lm < data/OpenSubtitles$J/docs.train.de > working/opensub-de.out.lmscores
        rm working/opensub-de.test.lm
        IRSTLM=~/moses/irstlm/ ~/moses/irstlm/bin/build-lm.sh -i data/OpenSubtitles$J/docs.test.de -t tmp -p -s improved-kneser-ney -o working/opensub-de.test.lm
        gunzip working/opensub-de.test.lm.gz
        ~/moses/irstlm/bin/score-lm -lm working/opensub-de.test.lm < data/OpenSubtitles$J/docs.train.de > working/opensub-de.in.lmscores
        scala scripts/opensub/topNentropy.scala working/opensub-de.in.lmscores working/opensub-de.out.lmscores $K data/OpenSubtitles$J/docs.train.de > working/opensub-de.lment.top$K

        # Random
        tail -n $K < working/random-baseline > working/opensub-de.random.top$K

        scala scripts/opensub/topN-corpus.scala OpenSubtitles2012/dedup/doc .en.txt working/opensub-de.oneta.top$K > working/corpus-oneta-en-de.en
        scala scripts/opensub/topN-corpus.scala OpenSubtitles2012/dedup/doc .de.txt working/opensub-de.oneta.top$K > working/corpus-oneta-en-de.de
        scala scripts/opensub/topN-corpus.scala OpenSubtitles2012/dedup/doc .en.txt working/opensub-de.lment.top$K > working/corpus-lment-en-de.en
        scala scripts/opensub/topN-corpus.scala OpenSubtitles2012/dedup/doc .de.txt working/opensub-de.lment.top$K > working/corpus-lment-en-de.de
        scala scripts/opensub/topN-corpus.scala OpenSubtitles2012/dedup/doc .en.txt working/opensub-de.random.top$K > working/corpus-random-en-de.en
        scala scripts/opensub/topN-corpus.scala OpenSubtitles2012/dedup/doc .de.txt working/opensub-de.random.top$K > working/corpus-random-en-de.de


        gzip -c working/corpus-oneta-en-de.de > ../nimrod/corpus/corpus-de-en.de.gz
        gzip -c working/corpus-oneta-en-de.en > ../nimrod/corpus/corpus-de-en.en.gz
        cd ../nimrod
        if [ -d ~/external_projects/cdec ]
        then
            ./nimrod scripts/moses-train.scala de en cdecDir=~/external_projects/cdec/
        else
            ./nimrod scripts/moses-train.scala de en
        fi
        ./nimrod scripts/moses-simpleeval.scala working/de-en/model/moses.ini ../oneta/OpenSubtitles2012/dedup/doc$T.de.txt ../oneta/OpenSubtitles2012/dedup/doc$T.en.txt de en result$J-oneta.$K
        cd ../oneta
        gzip -c working/corpus-lment-en-de.de > ../nimrod/corpus/corpus-de-en.de.gz
        gzip -c working/corpus-lment-en-de.en > ../nimrod/corpus/corpus-de-en.en.gz
        cd ../nimrod
        if [ -d ~/external_projects/cdec ]
        then
            ./nimrod scripts/moses-train.scala de en cdecDir=~/external_projects/cdec/
        else
            ./nimrod scripts/moses-train.scala de en
        fi
        ./nimrod scripts/moses-simpleeval.scala working/de-en/model/moses.ini ../oneta/OpenSubtitles2012/dedup/doc$T.de.txt ../oneta/OpenSubtitles2012/dedup/doc$T.en.txt de en result$J-lment.$K
        cd ../oneta
        gzip -c working/corpus-random-en-de.de > ../nimrod/corpus/corpus-de-en.de.gz
        gzip -c working/corpus-random-en-de.en > ../nimrod/corpus/corpus-de-en.en.gz
        cd ../nimrod
        if [ -d ~/external_projects/cdec ]
        then
            ./nimrod scripts/moses-train.scala de en cdecDir=~/external_projects/cdec/
        else
            ./nimrod scripts/moses-train.scala de en
        fi
        ./nimrod scripts/moses-simpleeval.scala working/de-en/model/moses.ini ../oneta/OpenSubtitles2012/dedup/doc$T.de.txt ../oneta/OpenSubtitles2012/dedup/doc$T.en.txt de en result$J-random.$K
        cd ../oneta
    done
done
