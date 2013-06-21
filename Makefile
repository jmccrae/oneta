all: mate-finding lda lsi oneta

bin:
	mkdir bin

mate-finding: bin
	g++ -O2 -Wall src/mate-finding.cpp --std=c++11 -o bin/mate-finding

lda: bin
	g++ -O2 -Wall src/lda.cpp --std=c++11 -o bin/lda

lsi: bin -larpack
	g++ -O2 -Wall src/lsi.cpp --std=c++11 -larpack -o bin/lsi

oneta: bin -larmadillo
	g++ -O2 -Wall src/oneta.cpp --std=c++11 -larmadillo -o bin/oneta
