#!/bin/bash

python merge-projections.py working/wiki-en_es.oneta9k-en working/wiki-en_es.trans-to-es working/wiki-en_es.merge9k-en 0.01
python merge-projections.py working/wiki-en_es.oneta9k-es working/wiki-en_es.dummy-es working/wiki-en_es.merge9k-es 0.01
./mate-finding working/wiki-en_es.merge9k-en  working/wiki-en_es.merge9k-es
python merge-projections.py working/wiki-en_es.oneta9k-en working/wiki-en_es.trans-to-es working/wiki-en_es.merge9k-en 0.009
python merge-projections.py working/wiki-en_es.oneta9k-es working/wiki-en_es.dummy-es working/wiki-en_es.merge9k-es 0.009
./mate-finding working/wiki-en_es.merge9k-en  working/wiki-en_es.merge9k-es
python merge-projections.py working/wiki-en_es.oneta9k-en working/wiki-en_es.trans-to-es working/wiki-en_es.merge9k-en 0.008
python merge-projections.py working/wiki-en_es.oneta9k-es working/wiki-en_es.dummy-es working/wiki-en_es.merge9k-es 0.008
./mate-finding working/wiki-en_es.merge9k-en  working/wiki-en_es.merge9k-es
python merge-projections.py working/wiki-en_es.oneta9k-en working/wiki-en_es.trans-to-es working/wiki-en_es.merge9k-en 0.007
python merge-projections.py working/wiki-en_es.oneta9k-es working/wiki-en_es.dummy-es working/wiki-en_es.merge9k-es 0.007
./mate-finding working/wiki-en_es.merge9k-en  working/wiki-en_es.merge9k-es
python merge-projections.py working/wiki-en_es.oneta9k-en working/wiki-en_es.trans-to-es working/wiki-en_es.merge9k-en 0.006
python merge-projections.py working/wiki-en_es.oneta9k-es working/wiki-en_es.dummy-es working/wiki-en_es.merge9k-es 0.006
./mate-finding working/wiki-en_es.merge9k-en  working/wiki-en_es.merge9k-es
python merge-projections.py working/wiki-en_es.oneta9k-en working/wiki-en_es.trans-to-es working/wiki-en_es.merge9k-en 0.005
python merge-projections.py working/wiki-en_es.oneta9k-es working/wiki-en_es.dummy-es working/wiki-en_es.merge9k-es 0.005
./mate-finding working/wiki-en_es.merge9k-en  working/wiki-en_es.merge9k-es
python merge-projections.py working/wiki-en_es.oneta9k-en working/wiki-en_es.trans-to-es working/wiki-en_es.merge9k-en 0.004
python merge-projections.py working/wiki-en_es.oneta9k-es working/wiki-en_es.dummy-es working/wiki-en_es.merge9k-es 0.004
./mate-finding working/wiki-en_es.merge9k-en  working/wiki-en_es.merge9k-es
python merge-projections.py working/wiki-en_es.oneta9k-en working/wiki-en_es.trans-to-es working/wiki-en_es.merge9k-en 0.003
python merge-projections.py working/wiki-en_es.oneta9k-es working/wiki-en_es.dummy-es working/wiki-en_es.merge9k-es 0.003
./mate-finding working/wiki-en_es.merge9k-en  working/wiki-en_es.merge9k-es
python merge-projections.py working/wiki-en_es.oneta9k-en working/wiki-en_es.trans-to-es working/wiki-en_es.merge9k-en 0.002
python merge-projections.py working/wiki-en_es.oneta9k-es working/wiki-en_es.dummy-es working/wiki-en_es.merge9k-es 0.002
./mate-finding working/wiki-en_es.merge9k-en  working/wiki-en_es.merge9k-es
python merge-projections.py working/wiki-en_es.oneta9k-en working/wiki-en_es.trans-to-es working/wiki-en_es.merge9k-en 0.001
python merge-projections.py working/wiki-en_es.oneta9k-es working/wiki-en_es.dummy-es working/wiki-en_es.merge9k-es 0.001
./mate-finding working/wiki-en_es.merge9k-en  working/wiki-en_es.merge9k-es

