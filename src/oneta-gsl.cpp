#include <cmath>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <memory>
#include <cstring>
#include <sstream>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include "sparse_mat.h"

using namespace std;
/**
 * Calculate the result of v1/v2 and updates v1
 */
void divide(shared_ptr<vector<double>> v1, shared_ptr<vector<double>> v2) {
    if(v1->size() != v2->size()) {
        cerr << v1->size() << " != " << v2->size() << endl;
        throw "Vectors do not match";
    }
    for(unsigned i = 0; i < v1->size(); i++) {
        (*v1)[i] /= (*v2)[i];
    }
}    

/**
 * Calculate v1 - v2 and stores it in v2 (!)
 */
void sub(shared_ptr<vector<double>> v1, shared_ptr<vector<double>> v2) {
    if(v1->size() != v2->size()) {
        cerr << v1->size() << " != " << v2->size() << endl;
        throw "Vectors do not match";
    }
    for(unsigned i = 0; i < v1->size(); i++) {
        (*v2)[i] = (*v1)[i] - (*v2)[i];
    }

}

/**
 * Print a vector
 */
void print_vec(shared_ptr<vector<double>> v) {
    for(unsigned i = 0; i < v->size(); i++) {
        cout << (*v)[i] << " ";
    }
    cout << endl;
}

/**
 * Normalizes v1 and v2 as if they were appended i.e., 
 *  v1 = v1 / ||(v1 v2)||
 *  v2 = v2 / ||(v1 v2)||
 */
void normalize(shared_ptr<vector<double>> v1, shared_ptr<vector<double>> v2) {
    double v = 0;
    for(auto x = v1->begin(); x != v1->end(); ++x) {
        v += (*x)*(*x);
    }
    for(auto x = v2->begin(); x != v2->end(); ++x) {
        v += (*x)*(*x);
    }
    v = sqrt(v);
    if(v != 0) {
        for(unsigned i = 0; i < v1->size(); i++) {
            (*v1)[i] /= v;
        }
        for(unsigned i = 0; i < v2->size(); i++) {
            (*v2)[i] /= v;
        }
    }
}

// Refactor of common code for matrix building
double update_corpus_build_col(string& token, unordered_map<string,int>& words, map<int,double>& c, vector<double>& freqs) {
    int tkId = words[token];
    if(c.find(tkId) != c.end()) {
        double val = c[tkId];
        double delta = freqs[tkId];
        c[tkId] += delta;
        return (val + delta) * (val + delta) - val * val;
    } else {
        double delta = freqs[tkId];
        c[tkId] = delta;
        return delta * delta;
    }
}

int main(int argv, char **argc) {
    if(argv != 5 && argv != 6) {
        cerr << "Usage: ./oneta train_corpus test_corpus kernel_size output [-sqnorm]" << endl;
        return -1;
    }
    unordered_map<string,int> w1Words;
    unordered_map<string,int> w2Words;

    int W1 = 0, W2 = 0;

    const int D1 = atoi(argc[3]);

    if(D1 < 0) {
        cerr << "Invalid value for kernel_size" << endl;
        return -1;
    }

    cerr << "First scan of training data" << endl;
    // The goal here is to map the words and find the dimensions of our matrices

    int J = 0;
    ifstream corpus(argc[1]);
    if(corpus.fail()) {
        cerr << "Could not read " << argc[1] << endl;
        return -1;
    }
    bool sqnorm = argv == 6 && (strcmp(argc[5],"-sqnorm") == 0);
    bool norm = sqnorm;

    string line;
    vector<double> freqs1;
    vector<double> freqs2;
    while(!corpus.eof()) {
        getline(corpus,line);
        if(line.length() == 0) {
            continue;
        }
        string token;
        stringstream ss(line);
        while(getline(ss,token,' ')) {
            bool inW1Words = (w1Words.find(token) != w1Words.end());
            if(J < D1 && !inW1Words) {
                w1Words[token] = W1;
                W1++;
                freqs1.push_back(1);
            } else if(inW1Words && norm) {
                freqs1[w1Words[token]]++;
            } else if(J >= D1 && w1Words.find(token) == w1Words.end()) {
                if(w2Words.find(token) == w2Words.end()) {
                    w2Words[token] = W2;
                    W2++;
                    freqs2.push_back(1);
                } else if(norm) {
                    freqs2[w2Words[token]]++;
                }
            }
        }
        J++;
    }
    corpus.close();

    // If we are calculating SQNorm we do it here
    if(sqnorm) {
        cerr << "Calculating norms" << endl;
        for(unsigned i = 0; i < freqs1.size(); i++) {
            freqs1[i] = 1.0 / freqs1[i];
        }
        for(unsigned i = 0; i < freqs2.size(); i++) {
            freqs2[i] = 1.0 / freqs2[i];
        }
    }

    cerr << "W1 = " << w1Words.size() << endl;
    cerr << "W2 = " << w2Words.size() << endl;

    const int D2 = J - D1;

    if(D2 < 0) {
        cerr << "D1 value is too large" << endl;
        return -1;
    }

    cerr << "D2 = " << D2 << endl;

    // We use L-Solve procedure, that is we first compute a term-document matrix of
    // the form 
    //   X = ( A B )
    //     = ( 0 C )
    cerr << "Building matrices";
    SparseMat A(W1,D1), B(W1,D2), C(W2,D2);

    ifstream corpus2(argc[1]);
    if(corpus2.fail()) {
        cerr << "Could not read " << argc[1] << endl;
        return -1;
    }
    int j = 0;
    while(!corpus2.eof()) {
        if(j % 10 == 9) {
            cerr << ".";
        }
        cerr.flush();
        map<int,double> c1;
        map<int,double> c2;

        getline(corpus2,line);
        if(line.length() == 0) {
            continue;
        }
        string token;
        stringstream ss(line);
        double docsq = 0.0;
        while(getline(ss,token,' ')) {
            if(j < D1) {
                docsq += update_corpus_build_col(token,w1Words,c1,freqs1);
            } else if(w1Words.find(token) != w1Words.end()) {
                docsq += update_corpus_build_col(token,w1Words,c1,freqs1);
            } else {
                docsq += update_corpus_build_col(token,w2Words,c2,freqs2);
            }
        }
        docsq = sqrt(docsq);
        if(j < D1) {
            for(auto it = c1.begin(); it != c1.end(); ++it) {
                c1[it->first] = it->second / docsq;
            }
            A.add_col(j,c1);
        } else {
            for(auto it = c1.begin(); it != c1.end(); ++it) {
                c1[it->first] = it->second / docsq;
            }
            for(auto it = c2.begin(); it != c2.end(); ++it) {
                c2[it->first] = it->second / docsq;
            }
            B.add_col(j-D1,c1);
            C.add_col(j-D1,c2);
        }
        j++;
    }
    cerr << endl;
    corpus2.close();

    // Next we look to compute the L-Solve defined as 
    // L^T = ( (A^T A)^-1 A^T -(A^T A)^-1 A^T B C' ) 
    //       (        0                 C'         )
    //
    // Where  C' = C^T / Cn

    cerr << "Build Cn" << endl;
    auto Cn = make_shared<vector<double>>(D2,0);
    int non_zeroes = 0;
    for(int i = 0; i < D2; i++) {
        double d = C.col_inner(i);
        if(d > 0) {
            (*Cn)[i] = d;
        } else {
            (*Cn)[i] = 1;
            non_zeroes++;
        }
    }
    cerr << "Discarded Documents: " << non_zeroes << endl;

    cerr << "Calculating ATA";
    gsl_matrix* ATA = 0;
    gsl_permutation *p = 0;
    // If we are doing standard ESA we skip calculating any orthonormalization
    if(D1 > 0) {
        ATA = gsl_matrix_alloc(D1,D1);
        for(int i = 0; i < D1; i++) {
            cerr << ".";
            cerr.flush();
            for(int i2 = 0; i2 < D1; i2++) {
                gsl_matrix_set(ATA,i,i2,A.inner(i,i2));
            }
        }
        cerr << endl;
   
        cerr << "Solve Inverse..." << endl;
        p = gsl_permutation_alloc(D1);
        int signum;
        gsl_linalg_LU_decomp(ATA,p,&signum);
    } else {
        cerr << endl;
    }

    cerr << "Calculating projected vectors";
    // For each of our input vectors we first calculate the term frequency as 
    // two vectors d1 and d2. We then multiply by L^T so we get
    // v2 = C^T * d2 / Cn
    // P(d1,d2) = ( ATAi * A^T * (d1 - B * v2) , v2 )

    ostream *out;
    if(strcmp("-",argc[4]) == 0) {
        out = &cout;
    } else {
        out = new ofstream(argc[4]);
    }
    if(out->fail()) {
        cerr << "Could not write to " << argc[4] << endl;
        return -1;
    }
    ifstream test(argc[2]);
    if(test.fail()) {
        cerr << "Could not read " << argc[2] << endl;
        return -1;
    }
    while(getline(test,line)) {
        cerr << ".";
        cerr.flush();
        string token;
        stringstream ss(line);
        auto d1 = make_shared<vector<double>>(W1,0.0),
             d2 = make_shared<vector<double>>(W2,0.0);        
        while(getline(ss,token,' ')) { 
            if(w1Words.find(token) != w1Words.end()) {
                int tkId = w1Words[token];
                (*d1)[tkId] += freqs1[tkId];
            } else if(w2Words.find(token) != w2Words.end()) {
                int tkId = w2Words[token];
                (*d2)[tkId] += freqs2[tkId];
            }
        }
        normalize(d1,d2);
        auto v2 = C.multt(d2);
        divide(v2,Cn);
        auto v1a = B * v2;
        sub(d1,v1a);
        auto v1b = A.multt(v1a);
        gsl_vector *v1c, *v1 = 0;
        if(D1 > 0) {
           v1c = gsl_vector_alloc(D1);
           memcpy(v1c->data,v1b->data(),sizeof(double)*D1);
           v1 = gsl_vector_alloc(D1);
           gsl_linalg_LU_solve(ATA,p,v1c,v1);
        }
        for(int i = 0; i < D1; i++) {
            *out << i << " ";
        }
        for(int i = 0; i < D2; i++) {
            if((*v2)[i] != 0.0) {
                *out << i << " ";
            }
        }
        *out << "||| ";
        for(int i = 0; i < D1; i++) {
            *out << v1->data[i] << " ";
        }
        for(int i = 0; i < D2; i++) {
            if((*v2)[i] != 0.0) {
                *out << (*v2)[i] << " ";
            }
        }
        *out << endl;
    }
    out->flush();
//    out.close();
    test.close();
    cerr << endl;
    return 0;
}
