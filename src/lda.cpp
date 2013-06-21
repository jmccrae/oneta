#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <string>
#include <random>
#include <cfloat>
#ifdef DEBUG
#define BACKWARD_HAS_BFD 1
#include "backward.hpp"
namespace backward {
    backward::SignalHandling sh;
}
#endif

using namespace std;

typedef vector<vector<vector<unsigned>>> MultilingCorpus;
typedef vector<vector<unsigned>> MonolingCorpus;
typedef vector<unsigned> Doc;

default_random_engine generator(time(0));

class Model {
    public:
        unsigned W,K,J,L;
        unsigned *N_kj;
        unsigned *N_lkw;
        unsigned *N_lk;
        MultilingCorpus z;
        uniform_int_distribution<unsigned> kgen;
        uniform_real_distribution<double> pgen;
        const double alpha, beta;
    private:
        double *P;
    public:
        Model(unsigned vocab, unsigned topic, unsigned docs, unsigned langs) :
           W(vocab), K(topic), J(docs), L(langs), kgen(0,K-1), pgen(0,1), alpha(2.0/K), beta(0.01) {
               N_kj = new unsigned[K*J];               
               N_lkw = new unsigned[L*K*W];
               N_lk = new unsigned[L*K];
               P = new double[K+1];
        } 
        ~Model() {
            delete[] N_kj;
            delete[] N_lkw;
            delete[] N_lk;
            delete[] P;
        }

        void initialize(MultilingCorpus& x) {
            memset(N_kj,0,K*J*sizeof(unsigned));
            memset(N_lkw,0,L*K*W*sizeof(unsigned));
            memset(N_lk,0,L*K*sizeof(unsigned));
            unsigned l = 0;
            for(MonolingCorpus& mc : x) {
                MonolingCorpus mz;
                unsigned j = 0;
                for(Doc& d : mc) {
                    Doc zd;
                    for(unsigned w : d) {
                        unsigned k = kgen(generator);
                        zd.push_back(k);
                        N_kj[k*J + j]++;
                        N_lkw[l * K * W + k * W + w]++;
                        N_lk[l * K + k]++;
                    }
                    j++;
                    mz.push_back(zd);
                }
                l++;                
                z.push_back(mz);
            }  
        }

        void iterate(MultilingCorpus& x) {
            unsigned l = 0;
            for(MonolingCorpus& mc : x) {
                MonolingCorpus& mz = z[l];
                unsigned j = 0;
                for(Doc& d : mc) {
                    Doc& dz = mz[j];
                    unsigned i = 0;
                    for(unsigned w : d) {
                        auto oldK = dz[i];
                        auto k = sample(l,j,w,oldK);
                        assignZ(l,j,w,k,oldK);
                        dz[i] = k;
                        i++;
                    }
                    j++;
                }
                l++;
            }
        }

        void import(Model& m, unsigned l) {
            for(unsigned i = 0; i < K * W; i++) {
                N_lkw[i] += m.N_lkw[l * K * W + i];
            }
            for(unsigned i = 0; i < K; i++) {
                N_lk[i] += m.N_lk[l * K + i];
            }
        }

        void print() {
            for(unsigned j = 0; j < J; j++) {
                for(unsigned k = 0; k < K; k++) {
                    cout << N_kj[k * J + j] << " ";
                }
                cout << endl;
            }
            int l = 0;
            for(MonolingCorpus& mc : z) {
                cout << "Language " << ++l << endl;
                for(Doc& d : mc) {
                    for(unsigned k : d) {
                        cout << k << " ";
                    }
                    cout << endl;
                }
            }
            cout << endl;
        }
    private:
        unsigned sample(unsigned l, unsigned j, unsigned w, unsigned prevK) {
            double u = pgen(generator);
            double sum = 0.0;
            double bestPk = -DBL_MAX;
            for (unsigned k = 0; k < K; k++) {
                const unsigned dec = prevK == k ? 1 : 0;
                P[k] = a_kj(k, j, dec) * b_lwk(l, w, k, dec) / c_lk(l, k, dec);
                if (P[k] > bestPk) {
                    bestPk = P[k];
                }
                sum += P[k];
            }
            for (unsigned k = 0; k < K; k++) {
                if (u < (P[k] / sum)) {
                    return k;
                }
                P[k + 1] += P[k];
            }
            cerr << "Sampling failed" << endl;
            return 0;
        } 

        void assignZ(unsigned l, unsigned j, unsigned w, unsigned k, unsigned oldK) {
            if(k != oldK) {
                N_kj[oldK * J + j]--;
                N_lkw[l * K * W + oldK * W + w]--;
                N_lk[l * K + oldK]--;
                N_kj[k * J + j]++;
                N_lkw[l * K * W + k * W + w]++;
                N_lk[l * K + k]++;
            }
        }

        inline double a_kj(unsigned k, unsigned j, unsigned dec) {
            return (double) N_kj[k * J +j] + alpha - dec;
        }

        inline double b_lwk(unsigned l, unsigned w, unsigned k, unsigned dec) {
            return ((double) N_lkw[l * K * W + k * W  + w] + beta - dec);
        }

        inline double c_lk(unsigned l, unsigned k, unsigned dec) {
            return ((double) N_lk[l * K + k] + W * beta - dec);
        }
};

void readcorpus(const char *fname, MonolingCorpus& x, unsigned& W, unordered_map<string,unsigned>& words, bool add_vocab = true) {
    ifstream train(fname);
    string line;
    while(getline(train,line)) {
        Doc doc;
        stringstream tokens(line);
        string token;
        while(getline(tokens,token,' ')) {
            if(words.find(token) == words.end()) {
                if(add_vocab) {
                    words[token] = W;
                    doc.push_back(W);
                    W++;
                }
            } else {
                doc.push_back(words[token]);
            } 
        }
        x.push_back(doc);
    }
 
}

int main(int argc, char **argv) {
    if(argc != 9) {
        cerr << "Usage: ./lda train-corpus1 train-corpus2 test-corpus1 test-corpus2 K N out1 out2" << endl;
        return -1;
    }

    unordered_map<string,unsigned> words;
    unsigned K = atoi(argv[5]);
    if(K <= 0) {
        cerr << "Bad K value" << endl;
        return -1;
    }
    unsigned N = atoi(argv[6]);
    if(N <= 0) {
        cerr << "Bad N value" << endl;
        return -1;
    }
    unsigned W = 0;
    MultilingCorpus x_lj(2);

    cout << "Loading corpus" << endl;
    readcorpus(argv[1],x_lj[0],W,words);
    readcorpus(argv[2],x_lj[1],W,words);
    cout << "W = " << W << endl;
    if(x_lj[0].size() != x_lj[1].size()) {
        cerr << "Two document sets differ in document count" << endl;
        return -1;
    }
    unsigned J = x_lj[0].size();

    Model model(W,K,J,2);

    cout << "Initializing" << endl;
    model.initialize(x_lj);

    cout << "Training" << endl;
    for(unsigned n = 0; n < N; n++) {
        auto begin = clock();
        model.iterate(x_lj);
        int t = (clock() - begin);
        int eta = (N - n - 1) * t;
        cout << "Iteration " << (n+1) << " ETA " << (eta / 3600) << ":" << ((eta % 3600) / 60) << ":" << (eta % 60) << endl;
    } 

    cout << "Loading test" << endl;
    MultilingCorpus test1(1), test2(1);
    readcorpus(argv[3],test1[0],W,words,false);
    readcorpus(argv[4],test2[0],W,words,false);

    if(test1[0].size() != test2[0].size()) {
        cerr << "Two document sets differ in document count" << endl;
        return -1;
    }
    unsigned Jt = test1[0].size();


    cout << "Evaluating" << endl;
    Model testModel1(W,K,Jt,1);
    Model testModel2(W,K,Jt,1);
    testModel1.initialize(test1);
    testModel2.initialize(test2);
    testModel1.import(model,0);
    testModel2.import(model,1);

    for(unsigned n = 0; n < N; n++) {
        auto begin = clock();
        testModel1.iterate(test1);
        testModel2.iterate(test2);
        int t = (clock() - begin);
        int eta = (N - n - 1) * t;
        cout << "Test Iteration " << (n+1) << " ETA " << (eta / 3600) << ":" << ((eta % 3600) / 60) << ":" << (eta % 60) << endl;
    }

    cout << "Writing" << endl;

    ofstream out1(argv[7]);
    ofstream out2(argv[8]);
    for(unsigned j = 0; j < Jt; j++) {
        for(unsigned k = 0; k < K; k++) {
            out1 << k << " ";
            out2 << k << " ";
        }
        out1 << "||| ";
        out2 << "||| ";
        for(unsigned k = 0; k < K; k++) {
            out1 << testModel1.N_kj[k * J + j] << " ";
            out2 << testModel2.N_kj[k * J + j] << " ";
        }
        out1 << endl;
        out2 << endl;
    }
    out1.flush();
    out1.close();
    out2.flush();
    out2.close();
    
    return 0;
}
