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
