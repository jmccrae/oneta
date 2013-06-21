#!/bin/bash

#./oneta data/wiki-en_de.en_train.txt data/wiki-en_de.en_test.txt 1000 working/wiki-en_de.oneta1k-en -sqnorm
#./oneta data/wiki-en_de.de_train.txt data/wiki-en_de.de_test.txt 1000 working/wiki-en_de.oneta1k-de -sqnorm
#./mate-finding working/wiki-en_de.oneta1k-en working/wiki-en_de.oneta1k-de
#./oneta data/wiki-en_de.en_train.txt data/wiki-en_de.en_test.txt 2000 working/wiki-en_de.oneta2k-en -sqnorm
#./oneta data/wiki-en_de.de_train.txt data/wiki-en_de.de_test.txt 2000 working/wiki-en_de.oneta2k-de -sqnorm
#./mate-finding working/wiki-en_de.oneta2k-en working/wiki-en_de.oneta2k-de
#./oneta data/wiki-en_de.en_train.txt data/wiki-en_de.en_test.txt 3000 working/wiki-en_de.oneta3k-en -sqnorm
#./oneta data/wiki-en_de.de_train.txt data/wiki-en_de.de_test.txt 3000 working/wiki-en_de.oneta3k-de -sqnorm
#./mate-finding working/wiki-en_de.oneta3k-en working/wiki-en_de.oneta3k-de
#./oneta data/wiki-en_de.en_train.txt data/wiki-en_de.en_test.txt 4000 working/wiki-en_de.oneta4k-en -sqnorm
#./oneta data/wiki-en_de.de_train.txt data/wiki-en_de.de_test.txt 4000 working/wiki-en_de.oneta4k-de -sqnorm
#./mate-finding working/wiki-en_de.oneta4k-en working/wiki-en_de.oneta4k-de
#./oneta data/wiki-en_de.en_train.txt data/wiki-en_de.en_test.txt 5000 working/wiki-en_de.oneta5k-en -sqnorm
#./oneta data/wiki-en_de.de_train.txt data/wiki-en_de.de_test.txt 5000 working/wiki-en_de.oneta5k-de -sqnorm
#./mate-finding working/wiki-en_de.oneta5k-en working/wiki-en_de.oneta5k-de
#./oneta data/wiki-en_de.en_train.txt data/wiki-en_de.en_test.txt 6000 working/wiki-en_de.oneta6k-en -sqnorm
#./oneta data/wiki-en_de.de_train.txt data/wiki-en_de.de_test.txt 6000 working/wiki-en_de.oneta6k-de -sqnorm
#./mate-finding working/wiki-en_de.oneta6k-en working/wiki-en_de.oneta6k-de
#./oneta data/wiki-en_de.en_train.txt data/wiki-en_de.en_test.txt 7000 working/wiki-en_de.oneta7k-en -sqnorm
#./oneta data/wiki-en_de.de_train.txt data/wiki-en_de.de_test.txt 7000 working/wiki-en_de.oneta7k-de -sqnorm
#/mate-finding working/wiki-en_de.oneta7k-en working/wiki-en_de.oneta7k-de
#./oneta data/wiki-en_de.en_train.txt data/wiki-en_de.en_test.txt 8000 working/wiki-en_de.oneta8k-en -sqnorm
#./oneta data/wiki-en_de.de_train.txt data/wiki-en_de.de_test.txt 8000 working/wiki-en_de.oneta8k-de -sqnorm
#./mate-finding working/wiki-en_de.oneta8k-en working/wiki-en_de.oneta8k-de
#./oneta data/wiki-en_de.en_train.txt data/wiki-en_de.en_test.txt 8900 working/wiki-en_de.oneta9k-en -sqnorm
#./oneta data/wiki-en_de.de_train.txt data/wiki-en_de.de_test.txt 8900 working/wiki-en_de.oneta9k-de -sqnorm
#./mate-finding working/wiki-en_de.oneta9k-en working/wiki-en_de.oneta9k-de
#./oneta data/wiki-en_es.en_train.txt data/wiki-en_es.en_test.txt 1000 working/wiki-en_es.oneta1k-en -sqnorm
#./oneta data/wiki-en_es.es_train.txt data/wiki-en_es.es_test.txt 1000 working/wiki-en_es.oneta1k-es -sqnorm
#./mate-finding working/wiki-en_es.oneta1k-en working/wiki-en_es.oneta1k-es
#./oneta data/wiki-en_es.en_train.txt data/wiki-en_es.en_test.txt 2000 working/wiki-en_es.oneta2k-en -sqnorm
#./oneta data/wiki-en_es.es_train.txt data/wiki-en_es.es_test.txt 2000 working/wiki-en_es.oneta2k-es -sqnorm
#./mate-finding working/wiki-en_es.oneta2k-en working/wiki-en_es.oneta2k-es
#./oneta data/wiki-en_es.en_train.txt data/wiki-en_es.en_test.txt 3000 working/wiki-en_es.oneta3k-en -sqnorm
#./oneta data/wiki-en_es.es_train.txt data/wiki-en_es.es_test.txt 3000 working/wiki-en_es.oneta3k-es -sqnorm
#./mate-finding working/wiki-en_es.oneta3k-en working/wiki-en_es.oneta3k-es
#./oneta data/wiki-en_es.en_train.txt data/wiki-en_es.en_test.txt 4000 working/wiki-en_es.oneta4k-en -sqnorm
#./oneta data/wiki-en_es.es_train.txt data/wiki-en_es.es_test.txt 4000 working/wiki-en_es.oneta4k-es -sqnorm
#./mate-finding working/wiki-en_es.oneta4k-en working/wiki-en_es.oneta4k-es
#./oneta data/wiki-en_es.en_train.txt data/wiki-en_es.en_test.txt 5000 working/wiki-en_es.oneta5k-en -sqnorm
#./oneta data/wiki-en_es.es_train.txt data/wiki-en_es.es_test.txt 5000 working/wiki-en_es.oneta5k-es -sqnorm
#./mate-finding working/wiki-en_es.oneta5k-en working/wiki-en_es.oneta5k-es
#./oneta data/wiki-en_es.en_train.txt data/wiki-en_es.en_test.txt 6000 working/wiki-en_es.oneta6k-en -sqnorm
#./oneta data/wiki-en_es.es_train.txt data/wiki-en_es.es_test.txt 6000 working/wiki-en_es.oneta6k-es -sqnorm
#./mate-finding working/wiki-en_es.oneta6k-en working/wiki-en_es.oneta6k-es
#./oneta data/wiki-en_es.en_train.txt data/wiki-en_es.en_test.txt 7000 working/wiki-en_es.oneta7k-en -sqnorm
#./oneta data/wiki-en_es.es_train.txt data/wiki-en_es.es_test.txt 7000 working/wiki-en_es.oneta7k-es -sqnorm
#/mate-finding working/wiki-en_es.oneta7k-en working/wiki-en_es.oneta7k-es
#./oneta data/wiki-en_es.en_train.txt data/wiki-en_es.en_test.txt 8000 working/wiki-en_es.oneta8k-en -sqnorm
#./oneta data/wiki-en_es.es_train.txt data/wiki-en_es.es_test.txt 8000 working/wiki-en_es.oneta8k-es -sqnorm
#./mate-finding working/wiki-en_es.oneta8k-en working/wiki-en_es.oneta8k-es
./oneta data/wiki-full.en_de.en_train.txt data/wiki-full.en_de.en_test.txt 4000 working/wiki-full.en_de.oneta9k-en
./oneta data/wiki-full.en_de.de_train.txt data/wiki-full.en_de.de_test.txt 4000 working/wiki-full.en_de.oneta9k-de
./mate-finding working/wiki-full.en_de.oneta9k-en working/wiki-full.en_de.oneta9k-de
