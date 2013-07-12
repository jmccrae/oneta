#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <string>
#include <cfloat>
#include <cstring>
#include <cstdlib>
#include <iomanip>
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

/**
 * The PLDA model - Solved by Gibbs sampling
 */
class Model {
    public:
        /** Dimensionality of the model */
        unsigned W,K,J,L;
        /** The document-topic correspondence */
        float *N_jk;
        /** The language-word-topic correspondence */
        float *N_lwk;
        /** The language-topic counts */            
        float *N_lk;
        /** Current topic assignment */
        MultilingCorpus z;
        /** Other model parameters */
        const float alpha, beta, betaW;
    private:
        /** Temporary array used for sampling */
        float *P;
    public:
        Model(unsigned vocab, unsigned topic, unsigned docs, unsigned langs) :
           W(vocab), K(topic), J(docs), L(langs), alpha(2.0/K), beta(0.01), betaW(0.01*vocab) {
               N_jk = new float[K*J];               
               N_lwk = new float[L*K*W];
               N_lk = new float[L*K];
               P = new float[K+1];
        } 
        ~Model() {
            delete[] N_jk;
            delete[] N_lwk;
            delete[] N_lk;
            delete[] P;
        }

        /**
         * Initialize the model at random
         */
        void initialize(MultilingCorpus& x) {
            memset(N_jk,0,K*J*sizeof(float));
            memset(N_lwk,0,L*K*W*sizeof(float));
            memset(N_lk,0,L*K*sizeof(float));
            unsigned l = 0;
            for(auto mc = x.begin(); mc != x.end(); ++mc) {
                MonolingCorpus mz;
                unsigned j = 0;
                for(auto d = mc->begin(); d != mc->end(); ++d) {
                    Doc zd;
                    for(auto it = d->begin(); it != d->end(); ++it) {
                        unsigned w = *it;
                        unsigned k = rand() % K;
                        zd.push_back(k);
                        N_jk[j*K + k]++;
                        N_lwk[l * K * W + w * K + k]++;
                        N_lk[l * K + k]++;
                    }
                    j++;
                    mz.push_back(zd);
                }
                l++;                
                z.push_back(mz);
            }  
        }

        /**
         * Perform one iteration
         */
        void iterate(MultilingCorpus& x) {
            unsigned l = 0;
            for(auto mc = x.begin(); mc != x.end(); ++mc) {
                MonolingCorpus& mz = z[l];
                unsigned j = 0;
                for(auto d = mc->begin(); d != mc->end(); ++d) {
                    Doc& dz = mz[j];
                    unsigned i = 0;
                    for(auto it = d->begin(); it != d->end(); ++it) {
                        unsigned w = *it;
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

        /**
         * Copy values from another model
         */
        void import(Model& m, unsigned l) {
            for(unsigned i = 0; i < K * W; i++) {
                N_lwk[i] += m.N_lwk[l * K * W + i];
            }
            for(unsigned i = 0; i < K; i++) {
                N_lk[i] += m.N_lk[l * K + i];
            }
        }

        void print() {
            for(unsigned j = 0; j < J; j++) {
                for(unsigned k = 0; k < K; k++) {
                    cout << N_jk[j * K + k] << " ";
                }
                cout << endl;
            }
            int l = 0;
            for(auto mc = z.begin(); mc != z.end(); ++mc) {
                cout << "Language " << ++l << endl;
                for(auto d = mc->begin(); d != mc->end(); ++d) {
                    for(auto it = d->begin(); it != d->end(); ++it) {
                        unsigned k = *it;
                        cout << k << " ";
                    }
                    cout << endl;
                }
            }
            cout << endl;
        }
    private:
        unsigned sample(unsigned l, unsigned j, unsigned w, unsigned prevK) {
            float u = rand() / (float)RAND_MAX;
            float sum = 0.0;
            unsigned jkIdx = j * K;
            unsigned lwkIdx = l * K * W + w * K;
            unsigned lkIdx = l * K;
            for (unsigned k = 0; k < K; k++) {
                const float dec = prevK == k ? 1 : 0;
                //P[k] = a_kj(k, j, dec) * b_lwk(l, w, k, dec) / c_lk(l, k, dec);
                //P[k] = (N_kj[k * J + j] + alpha - dec) * (N_lkw[l * K * W + k * W + w] + beta - dec) / (N_lk[l * K + k] + W * beta - dec);
                P[k] = (N_jk[jkIdx] + alpha - dec) * (N_lwk[lwkIdx] + beta - dec) / (N_lk[lkIdx] + betaW - dec);
                jkIdx++;
                lwkIdx++;
                lkIdx++;
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
                N_jk[j * K + oldK]--;
                N_lwk[l * K * W + w * K + oldK]--;
                N_lk[l * K + oldK]--;
                N_jk[j * K + k]++;
                N_lwk[l * K * W + w * K + k]++;
                N_lk[l * K + k]++;
            }
        }
};

/**
 * Read the corpus and build a monolingual corpus object
 * @param fname The file to read
 * @param x The corpus object to write to
 * @param W The number of unique tokens in the file
 * @param words The word index max
 * @param add_vocab OOV: extend word map or discard?
 */
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
    srand(time(0));
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
        int eta = (clock() - begin) / CLOCKS_PER_SEC * (N-n-1);
        cout << "Iteration " << (n+1) << " ETA " << (eta / 3600) << ":" << setfill('0') << setw(2) << ((eta % 3600) / 60) << ":" << setfill('0') << setw(2) << (eta % 60) << endl;
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
        int eta = (clock() - begin) / CLOCKS_PER_SEC * (N-n-1);
        cout << "Test Iteration " << (n+1) << " ETA " << (eta / 3600) << ":" << setfill('0') << setw(2) << ((eta % 3600) / 60) << ":" << setfill('0') << setw(2) << (eta % 60) << endl;
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
            out1 << testModel1.N_jk[j * K + k] << " ";
            out2 << testModel2.N_jk[j * K + k] << " ";
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
