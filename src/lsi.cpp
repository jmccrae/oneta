#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <map>
#include <sstream>
#include <cstring>
#include <fstream>
#include <memory>
#include "arpack.h"
#include <unistd.h>

#ifdef DEBUG
#define BACKWARD_HAS_BFD 1
#include "backward.hpp"
namespace backward {
    backward::SignalHandling sh;
}
#endif

using namespace std;

void divide(shared_ptr<vector<double>> v1, const double *v2) {
    for(unsigned i = 0; i < v1->size(); i++) {
        (*v1)[i] /= v2[i];
    }
}

/**
 * Project the vectors
 * @param fName The file to write to
 * @param testCorpus The test documents
 * @param W Number of unique tokens
 * @param U Eigenvector matrix
 * @param words The word index map
 * @param eval The eigenvectors
 * @param K The other dimension of the eigenvector matrix
 */
void projectVectors(const char * fName, ifstream& testCorpus, const int W, DenseMat& U, unordered_map<string,int>& words, const double *eval, const int K) {
    string line;
    cout << "Projecting vectors" << endl;

    ofstream out(fName);
    while(getline(testCorpus,line)) {
        cout << ".";
        cout.flush();
        auto d = make_shared<vector<double>>(W,0); 
        stringstream tokens(line);
        string token;
        while(getline(tokens,token,' ')) {
            if(words.find(token) != words.end()) {
                (*d)[words[token]] += 1;
            }
        }
        auto r = U * d;
        divide(r,eval);

        for(int i = 0; i < K; i++) {
            out << i << " ";
        }
        out << "||| ";
        for(int i = 0; i < K; i++) {
            out << (*r)[i] << " ";
        }
        out << endl;
    }
    cout << endl;
    out.flush();
    out.close();
    testCorpus.close();
} 

int main(int argc, char **argv) {
    bool use_mmap = false;
    int c;
    while((c = getopt(argc,argv,"m")) != -1) {
        switch(c) {
            case 'm':
                use_mmap=true;
                break;
            default:
                cerr << c << " not recognized" << endl;
        }
    }
    if(argc - optind != 6) {
        cerr << "Usage: ./lsi train-corpus test-corpus-src test-corpus-trg K vectors-src vectors-trg" << endl;
        cerr << endl;
        cerr << "Applies the Latent Semantic Indexing method" << endl;
        cerr << endl;
        cerr << "  train-corpus     The training corpus where documents consist" << endl;
        cerr << "                     of both languages appended (i.e., by using" << endl;
        cerr << "                     `scripts/join.py`)" << endl;
        cerr << "  test-corpus-src  The source language evaluation corpus" << endl;
        cerr << "  test-corpus-trg  The target language evaluation corpus" << endl;
        cerr << "  K                The number of topics to calculate in the" << endl;
        cerr << "                     representation" << endl;
        cerr << "  vectors-src      The output file to write the source language" << endl;
        cerr << "                     representations to" << endl;
        cerr << "  vectors-trg      The output file to write the target language" << endl;
        cerr << "                      representations to" << endl;
        return -1;
    }

    unordered_map<string,int> words;
    vector<double> doc_freqs;

    ifstream testCorpus1(argv[1+optind]);
    if(testCorpus1.fail()) {
        cerr << "Could not access " << argv[1+optind] << endl;
        return -1;
    }
    ifstream testCorpus2(argv[2+optind]);
    if(testCorpus2.fail()) {
        cerr << "Could not access " << argv[2+optind] << endl;
        return -1;
    }

    int W = 0;

    int K = atoi(argv[3+optind]);

    int N = 0;

    if(K <= 0) {
        cerr << "Invalid kernel value" << endl;
    }

    cout << "First scan of training data" << endl;
    ifstream corpus1(argv[optind]);
    string line;
    while(getline(corpus1,line)) {
        set<int> inDoc;
        stringstream tokens(line);
        string token;
        while(getline(tokens,token,' ')) {
            if(words.find(token) == words.end()) {
                words[token] = W;
                doc_freqs.push_back(0);
                inDoc.insert(W);
                W++;
            } else {
                inDoc.insert(words[token]);
            }
        }
        for(auto i = inDoc.begin(); i != inDoc.end(); ++i) {
            doc_freqs[*i]++;
        }
        N++;
    }
    corpus1.close();
    cout << "W = " << W << endl << "N = " << N << endl;

    cout << "Building matrix";

    auto mat = make_shared<SparseMat>(W,N);

    int n = 0;
    ifstream corpus2(argv[optind]);
    while(getline(corpus2,line)) {
        if(n % 1000 == 999) {
            cout << ".";
            cout.flush();
        }
        map<int,double> colData;
        stringstream tokens(line);
        string token;
        while(getline(tokens,token,' ')) {
            int tkId = words[token];
            if(colData.find(tkId) == colData.end()) {
                colData[tkId] = 1;//log(N / doc_freqs[tkId]);
            } else {
                colData[tkId] += 1;//log(N / doc_freqs[tkId]);
            }
        }
        mat->add_col(n++,colData);
    }
    cout << endl;
    corpus2.close();
    //mat->print();

    cout << "Finding eigenvectors" << endl;
    char mmap_file[256];
    if(use_mmap) {
        tmpnam(mmap_file);
    }
    double *eval = new double[K];
    double *evec = arpack_sym_eig(mat,K,eval,false,use_mmap ? mmap_file : NULL);
    mat.reset();

    for(int i = 0; i < K; i++) {
        eval[i] = sqrt(eval[i]);
    }

    DenseMat U(evec,K,W);
    
    projectVectors(argv[4+optind], testCorpus1, W, U, words,eval,K);
    projectVectors(argv[5+optind], testCorpus2, W, U, words,eval,K);

    if(use_mmap) {
        remove(mmap_file);
    }
    delete[] evec;
    delete[] eval;
}
