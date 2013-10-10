T=`cat data/OpenSubtitles/docs.heldout`
for K in 100 200 300 400 500
do
    K=$K bash scripts/opensub/all.sh
    gzip -c working/corpus-oneta-en-de.de > ../nimrod/corpus/corpus-de-en.de.gz
    gzip -c working/corpus-oneta-en-de.en > ../nimrod/corpus/corpus-de-en.en.gz
    cd ../nimrod
    ./nimrod scripts/moses-train.scala de en cdecDir=~/external_projects/cdec/
    ./nimrod scripts/moses-simpleeval.scala working/de-en/model/moses.ini ../oneta/OpenSubtitles2012/docs/doc$T.de.txt ../oneta/OpenSubtitles2012/docs/doc$T.en.txt de en result-oneta.$K
    cd ../oneta
    gzip -c working/corpus-lment-en-de.de > ../nimrod/corpus/corpus-de-en.de.gz
    gzip -c working/corpus-lment-en-de.en > ../nimrod/corpus/corpus-de-en.en.gz
    cd ../nimrod
    ./nimrod scripts/moses-train.scala de en cdecDir=~/external_projects/cdec/
    ./nimrod scripts/moses-simpleeval.scala working/de-en/model/moses.ini ../oneta/OpenSubtitles2012/docs/doc$T.de.txt ../oneta/OpenSubtitles2012/docs/doc$T.en.txt de en result-lment.$K
    cd ../oneta
done
