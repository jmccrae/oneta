#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <set>
#include "arpack.h"
#include "translation.h"

#ifdef DEBUG
#define BACKWARD_HAS_BFD 1
#include "backward.hpp"
namespace backward {
    backward::SignalHandling sh;
}
#endif

using namespace std;

/**
 * Taking a sparse matrix, which is index by the words in wordsIds, filter so that it only
 * contains columns 
shared_ptr<SparseMat> filterSparseMatrix(shared_ptr<SparseMat> X, shared_ptr<set<string>> trans, vector<string>& wordIds) {
    vector<unsigned> remapping(wordIds.size());
    fill(remapping.begin(),remapping.end(),0);
    unsigned j = 1;
    for(unsigned i = 0; i < wordIds.size(); i++) {
        if(trans->find(wordIds[i]) != trans->end()) {
            remapping[i] = j++;
        }
    }
    auto X_ = make_shared<SparseMat>(j-1,X->N);
    for(unsigned i = 0; i < X->N; i++) {
        for(unsigned w = 0; w < wordIds.size(); w++) {
            if(remapping[elem.idx] != 0) {
                X_[i * K + w] = X[i * K + remapping[w]];
            }
        }
    }
    return X_;
}

double * filterDenseMatrix(double *X, shared_ptr<set<string>> trans, vector<string>& wordIds, int K) {
    vector<unsigned> remapping(wordIds.size());
    fill(remapping.begin(),remapping.end(),0);
    unsigned j = 1;
    for(unsigned i = 0; i < wordIds.size(); i++) {
        if(trans->find(wordIds[i]) != trans->end()) {
            remapping[i] = j++;
        }
    }
    auto X = new double[K * (j-1)];
    for(unsigned i = 0; i < X->N; i++) {
        map<int,double> colData;
        for(unsigned j = X->cp[i]; j < X->cp[i+1]; j++) {
            auto elem = X->data[j];
            if(remapping[elem.idx] != 0) {
                colData.insert(pair<int,double>(remapping[elem.idx],elem.val));
            }
        }
        X_->add_col(i,colData);
    }
    return X_;
}*/

double *word2vec(shared_ptr<SparseMat> C, double *eval, double *evectors, int K, int W, int N) {
    double *X = new double[K * W];
    memset(X,0,K * W * sizeof(double));
    for(int i = 0; i < K; i++) {
        for(int j = 0; j < N; j++) {
            for(unsigned y = C->cp[j]; y < C->cp[j+1]; y++) {
                auto d = C->data[y];
                X[i * K + d.idx] += eval[i] * evectors[j * N + i] * d.val;
            }
        }
    }
    return X;
}

int main(int argc, char** argv) {
    // Validate arguments
    if(argc != 6) {
        cerr << "Usage: translator-train train-corpus-src train-corpus-trg translations.csv K output-model" << endl;
        cerr << endl;
        cerr << "Learns a topic-based translator" << endl;
        cerr << endl;
        cerr << "  train-corpus-src  The documents in the source language" << endl;
        cerr << "  test-corpus-src   The documents in the target language" << endl;
        cerr << "  translations.csv  The set of known translations for bootstrapping" << endl;
        cerr << "  K                 The number of topics to use" << endl;
        cerr << "  output-model      The output model to write to" << endl;
        return -1;
    }

    unordered_map<string,int> words1,words2;

    int N1 = 0,N2 = 0;

    if(!countCorpus(argv[1],&N1,words1)) {
        return -1;
    }

    if(!countCorpus(argv[2],&N2,words2)) {
        return -1;
    }

    ifstream transCSV(argv[3]);
    if(transCSV.fail()) {
        cerr << "Could not access " << argv[3] << endl;
        return -1;
    }

    int K = atoi(argv[4]);

    if(K < 0) {
        cerr << "K is not a positive integer: " << argv[3] << endl;
        return -1;
    }
    if(K > N1 || K > N2) {
        cerr << "K is too large for the corpora given" << endl;
        return -1;
    }
    
    cerr << N1 << " " << N2 << endl;

    cerr << words1.size() << " " << words2.size() << endl;

    auto T = buildTranslationMap(argv[3]);

    auto C1 = buildTDM(argv[1],N1,words1);
    auto C2 = buildTDM(argv[2],N2,words2);

    double eval[K];

    auto v = arpack_sym_eig(C1,K,eval,true,NULL);

    for(int i = 0; i <= K; i++) {
        for(int j = i*N1; j <= (i+1)*N1; j++) {
            cerr << v[j] << endl;
        }
        cerr << endl;
    }

    auto X = word2vec(C1,eval,v,K,words1.size(),N1);

    auto srcWords = transSourceWords(T);

    vector<string> invWordMap1(words1.size());
    vector<string> invWordMap2(words2.size());

    invertWordMap(words1,invWordMap1);
    invertWordMap(words2,invWordMap2);

    return 0;
}
