all: mate-finding lda lsi oneta

bin:
	mkdir bin

mate-finding: bin
	g++ -O2 -Wall src/mate-finding.cpp --std=c++0x -o bin/mate-finding

lda: bin
	g++ -O2 -Wall src/lda.cpp --std=c++0x -o bin/lda

lsi: bin -larpack
	g++ -O2 -Wall src/lsi.cpp --std=c++0x -larpack -o bin/lsi

oneta: bin -lgsl -lblas
	g++ -O2 -Wall src/oneta-gsl.cpp --std=c++0x -lblas -lgsl -o bin/oneta
