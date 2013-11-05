#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <set>
#include <gsl/gsl_linalg.h>
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
    auto Tinv = inverseMap(T);

    auto C1 = buildTDM(argv[1],N1,words1);
    auto C2 = buildTDM(argv[2],N2,words2);

    double eval[K];

    auto v = arpack_sym_eig(C1,K,eval,true,NULL);

    auto srcWords = transSourceWords(T);

    auto mono = monoKernel(v,K,T,Tinv,words1);

    auto perm = gsl_permutation_alloc(K);

    int signum;
    gsl_linalg_LU_decomp(mono.get(),perm,&signum);
    
    auto v2 = arpack_sym_eig(C2,K,eval,true,NULL);

    auto bi = biKernel(v,v2,K,T,words1,words2);

    return 0;
}
