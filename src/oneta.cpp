#include <cmath>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <memory>
#include <armadillo>

using namespace std;
using namespace arma;

/**
 * Sparse Array element is simply an idx and a value
 */
class SparseArrayElem {
    public:
        int idx;
        double val;
        SparseArrayElem() : idx(0), val(0.0){

        }
        SparseArrayElem(int i, double v) : idx(i), val(v) {

        }
};

/**
 * A sparse matrix in compressed column format
 */
class SparseMat {
    public:
        /**
         * Number of rows and columns
         */
        const int M,N;
        /**
         * The column pointer (offset where the nth column starts in the data)
         */
        int *cp;
        /**
         * The non-zero values
         */
        vector<SparseArrayElem> data;
        /**
         * Create a sparse matrix of a given dimension
         */
        SparseMat(const int rows, const int cols) : M(rows), N(cols) {            
            cp = new int[cols+1];
            cp[0] = 0;
        }
        ~SparseMat() {
            delete[] cp;
        }
        /**
         * Calculate Av
         */
        shared_ptr<vector<double>> operator*(shared_ptr<vector<double>>& v) {
            if(v->size() != N) {
                throw "Wrong vector size";
            }
            auto v2 = make_shared<vector<double>>(M,0.0);
            auto it = data.begin();
            for(int i = 0; i < N; i++) {
                for(int j = cp[i]; j < cp[i+1]; j++) {
                    (*v2)[it->idx] += (*v)[i] * it->val;
                    ++it;
                }
            }
            return v2;
        }
        /**
         * Calculate A^T v
         */
        shared_ptr<vector<double>> multt(shared_ptr<vector<double>>& v) {
            if(v->size() != M) {
                cerr << v->size() << " != " << M << endl;
                throw "Wrong vector size";
            }
            auto v2 = make_shared<vector<double>>(N,0.0);
            auto it = data.begin();
            for(int i = 0; i < N; i++) {
                for(int j = cp[i]; j < cp[i+1]; j++) {
                    (*v2)[i] += (*v)[it->idx] * it->val;
                    ++it;
                }
            }
            return v2;
        }
        /**
         * Used when building the matrix, no checks for double calls
         */
        void add_col(int n, map<int,double>& colData) {
            for(auto it : colData)  {
                data.push_back(SparseArrayElem(it.first,it.second));
            }
            cp[n+1] = cp[n] + colData.size();
        }
        /**
         * For the i-th column compute <a_i,a_i>
         */
        double col_inner(int i) {
            double sq = 0;
            for(int j = cp[i]; j < cp[i+1]; j++) {
                sq += data[j].val * data[j].val;
            }
            return sq;
        }
        /**
         * Compute the inner product of <a_i,a_j>
         */
        double inner(int i, int j) {
            double sq = 0;
            int a = cp[i], b = cp[j];
            while(a < cp[i+1] && b < cp[j+1]) {
               if(data[a].idx < data[b].idx) {
                   a++;
               } else if(data[b].idx < data[a].idx) {
                   b++;
               } else {
                   sq += data[a].val * data[b].val;
                   a++;
                   b++;
               }
            }
            return sq;
        }
        /**
         * Print this matrix
         */
        void print() {
            for(auto it : data) {
                cout << it.val << "@" << it.idx << " ";
            }
            cout << endl;
        }

};

/**
 * Calculate the result of v1/v2 and updates v1
 */
void divide(shared_ptr<vector<double>> v1, shared_ptr<vector<double>> v2) {
    if(v1->size() != v2->size()) {
        cerr << v1->size() << " != " << v2->size() << endl;
        throw "Vectors do not match";
    }
    for(int i = 0; i < v1->size(); i++) {
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
    for(int i = 0; i < v1->size(); i++) {
        (*v2)[i] = (*v1)[i] - (*v2)[i];
    }

}

/**
 * Print a vector
 */
void print_vec(shared_ptr<vector<double>> v) {
    for(int i = 0; i < v->size(); i++) {
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
    for(auto x : *v1) {
        v += x*x;
    }
    for(auto x : *v2) {
        v += x*x;
    }
    v = sqrt(v);
    for(int i = 0; i < v1->size(); i++) {
        (*v1)[i] /= v;
    }
    for(int i = 0; i < v2->size(); i++) {
        (*v2)[i] /= v;
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

    cout << "First scan of training data" << endl;
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

    if(sqnorm) {
        cout << "Calculating norms" << endl;
        for(int i = 0; i < freqs1.size(); i++) {
            freqs1[i] = 1.0 / freqs1[i];
        }
        for(int i = 0; i < freqs2.size(); i++) {
            freqs2[i] = 1.0 / freqs2[i];
        }
    }

    cout << "W1 = " << w1Words.size() << endl;
    cout << "W2 = " << w2Words.size() << endl;

    const int D2 = J - D1;

    if(D2 < 0) {
        cerr << "D1 value is too large" << endl;
        return -1;
    }

    cout << "D2 = " << D2 << endl;

    cout << "Building matrices";
    SparseMat A(W1,D1), B(W1,D2), C(W2,D2);

    ifstream corpus2(argc[1]);
    if(corpus2.fail()) {
        cerr << "Could not read " << argc[1] << endl;
        return -1;
    }
    int j = 0;
    while(!corpus2.eof()) {
        if(j % 10 == 9) {
            cout << ".";
        }
        cout.flush();
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
            for(auto it : c1) {
                c1[it.first] = it.second / docsq;
            }
            A.add_col(j,c1);
        } else {
            for(auto it : c1) {
                c1[it.first] = it.second / docsq;
            }
            for(auto it : c2) {
                c2[it.first] = it.second / docsq;
            }
            B.add_col(j-D1,c1);
            C.add_col(j-D1,c2);
        }
        j++;
    }
    cout << endl;
    corpus2.close();

    cout << "Build Cn" << endl;
    auto Cn = make_shared<vector<double>>(D2,0);
    for(int i = 0; i < D2; i++) {
        double d = C.col_inner(i);
        (*Cn)[i] = d > 0 ? d : 1;
    }

    cout << "Calculating ATA";
    Mat<double> ATA(D1,D1);
    for(int i = 0; i < D1; i++) {
        cout << ".";
        cout.flush();
        for(int i2 = 0; i2 < D1; i2++) {
            ATA(i,i2) = A.inner(i,i2);
        }
    }
    cout << endl;

    cout << "Solve Inverse..." << endl;
    Mat<double> ATAi = inv(ATA);

    cout << "Calculating projected vectors";
    ofstream out(argc[4]);
    if(out.fail()) {
        cerr << "Could not write to " << argc[4] << endl;
        return -1;
    }
    ifstream test(argc[2]);
    if(test.fail()) {
        cerr << "Could not read " << argc[2] << endl;
        return -1;
    }
    while(getline(test,line)) {
        cout << ".";
        cout.flush();
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
        Col<double> v1c(*v1b);
        Col<double> v1 = ATAi * v1c;
        for(int i = 0; i < D1; i++) {
            out << i << " ";
        }
        for(int i = 0; i < D2; i++) {
            if((*v2)[i] != 0.0) {
                out << i << " ";
            }
        }
        out << "||| ";
        for(int i = 0; i < D1; i++) {
            out << v1[i] << " ";
        }
        for(int i = 0; i < D2; i++) {
            if((*v2)[i] != 0.0) {
                out << (*v2)[i] << " ";
            }
        }
        out << endl;
    }
    out.flush();
    out.close();
    test.close();
    cout << endl;
    return 0;
}
