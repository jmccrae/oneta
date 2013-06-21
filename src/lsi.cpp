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
#ifdef DEBUG
#define BACKWARD_HAS_BFD 1
#include "backward.hpp"
namespace backward {
    backward::SignalHandling sh;
}
#endif

using namespace std;


//--------interfaces to arpack (fortran) routines---------
extern "C" void dsaupd_(int *ido, char *bmat, int *n, char *which, int *nev, double *tol, double *resid, int *ncv, double *v, int *ldv, int *iparam, 
			int *ipntr, double *workd, double *workl, int *lworkl, int *info);

extern "C" void dseupd_(int *rvec, const char *howmany, int *select, double *d,
			double *z, int *ldz, double *sigma, char *bmat, 
			int *n, char *which, int *nev, double *tol, 
			double *resid, int *ncv, double *v, int *ldv, 
			int *iparam, int *ipntr, double *workd, double *workl, 
			int *lworkl, int *info);
//-----------------------------------------------		

class SparseArrayElem {
    public:
        int idx;
        double val;
        SparseArrayElem() : idx(0), val(0.0){

        }
        SparseArrayElem(int i, double v) : idx(i), val(v) {

        }
};

class SparseMat {
    public:
        const int M,N;
        int *cp;
        vector<SparseArrayElem> data;
        SparseMat(const int rows, const int cols) : M(rows), N(cols) {            
            cp = new int[cols+1];
            cp[0] = 0;
        }
        ~SparseMat() {
            delete[] cp;
        }
        void add_col(int n, map<int,double>& colData) {
            for(auto it = colData.begin(); it != colData.end(); ++it) {
                data.push_back(SparseArrayElem(it->first,it->second));
            }
            cp[n+1] = cp[n] + colData.size();
        }
        /**
         * If this matrix is A calculate v_out = AA^T v_in
         */
        void AAT_mult(double *v_in, double *v_out) {
            double *v_tmp = new double[N];
            memset(v_tmp,0,N*sizeof(double));
            memset(v_out,0,M*sizeof(double));
            auto it = data.begin();
            for(int i = 0; i < N; i++) {
                for(int j = cp[i]; j < cp[i+1]; j++) {
                    v_tmp[i] += v_in[it->idx] * it->val;
                    ++it;
                }
            }
            it = data.begin();
            for(int i = 0; i < N; i++) {
                for(int j = cp[i]; j < cp[i+1]; j++) {
                    v_out[it->idx] += v_tmp[i] * it->val;
                    ++it;
                }
            }           
            delete[] v_tmp;
        }


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
        void print() {
            int j = 0;
            int i = 0;
            for(auto it = data.begin(); it != data.end(); ++it) {
                while(i >= cp[j]) {
                    j++;
                }
                cout << it->val << "@(" << it->idx << "," << (j-1) << ") ";
                i++;
            }
            cout << endl;
        }
};


class DenseMat {
    public:
        double *data;
        int M,N;
        DenseMat(double *d, int rows, int cols) : data(d), M(rows), N(cols) {

        }
        shared_ptr<vector<double>> operator*(shared_ptr<vector<double>> v) {
            auto v2 = make_shared<vector<double>>(M,0);
            for(int i = 0; i < M; i++) {
                for(int j = 0; j < N; j++) {
                    (*v2)[i] += data[i*N+j] * (*v)[j];
                }
            }
            return v2;
        }
};

void divide(shared_ptr<vector<double>> v1, double *v2) {
    for(unsigned i = 0; i < v1->size(); i++) {
        (*v1)[i] /= v2[i];
    }
}


/**
 * Calculate the top K eigendecompsition of AAT
 * @param A sparse matrix
 * @param The number of LSI topics 0 < K <= M.N
 * @param The vector to write the eigenvectors to. evec = new double[K*M.N]
 * @param The vector to write the eigenvalues to. eval = new double[K];
 */
void arpack_sym_eig(SparseMat& M, int K, double *evec, double *eval) {
    int ido = 0; 
    int n = M.M;
    char bmat[2] = "I"; /* standard symmetric eigenvalue problem*/
    char which[3] = "LA"; /* Largest eigenvalue*/

    double tol = 0.0; /* Machine precision*/
    double *resid=new double[n];
    int ncv = 4*K; 
    if(ncv>n){
        ncv = n;
    }
    int ldv=n;
    double *v=new double[ldv*ncv];
    int *iparam=new int[11];
    iparam[0] = 1;   // Specifies the shift strategy (1->exact)
    iparam[2] = 3*n; // Maximum number of iterations
    iparam[6] = 1;   /* Sets the mode of dsaupd.
		      1 is exact shifting,
                      2 is user-supplied shifts,
		      3 is shift-invert mode,
		      4 is buckling mode,
		      5 is Cayley mode. */

    int *ipntr=new int[11];
    double *workd=new double[3*n];
    double *workl=new double[ncv*(ncv+8)];
    int lworkl = ncv*(ncv+8); /* Length of the workl array */
    int info = 0; /* Passes convergence information out of the iteration
					routine. */
    int rvec = 1; /* Specifies that eigenvectors should be calculated */
    int *select=new int[ncv];
    double *d=new double[2*ncv];

    do {
        dsaupd_(&ido, bmat, &n, which, &K, &tol, resid, &ncv, v, &ldv, iparam, ipntr, workd, workl, &lworkl, &info);
        if ((ido==1)||(ido==-1)){
            M.AAT_mult(workd+ipntr[0]-1,workd+ipntr[1]-1);
        }
        cout << ".";
        cout.flush();
    } while((ido==1)||(ido==-1));

    cout << endl;
    if (info<0) {
        cerr << "Error [dsaupd], info = " << info << "\n";
        throw "ARPACK failed";
    } else {
        double sigma;
        int ierr;
        dseupd_(&rvec, "All", select, d, v, &ldv, &sigma, bmat, &n, which, &K, &tol, resid, &ncv, v, &ldv, iparam, ipntr, workd, workl, &lworkl, &ierr);

        if(ierr!=0){
            cout << "Error [dseupd], info = " << ierr << "\n";	
        }else if(info==1){
            cout << "Maximum number of iterations reached.\n\n";
        }else if(info==3){
            cout << "No shifts could be applied during implicit\n";
            cout << "Arnoldi update, try increasing NCV.\n\n";
        }
        memcpy(eval, d, sizeof(double)*K);
        memcpy(evec, v, sizeof(double)*K*n);
    }
    cout << "ARPACK done" << endl;
    delete[] resid;
    delete[] v;
    delete[] iparam;
    delete[] ipntr;
    delete[] workd;
    delete[] workl;
    delete[] select;
    delete[] d;
}


int main(int argv, char **argc) {
    if(argv != 5) {
        cerr << "Usage: ./lsi traincorpus testcorpus K output" << endl;
        return -1;
    }

    unordered_map<string,int> words;
    vector<double> doc_freqs;

    int W = 0;

    int K = atoi(argc[3]);

    int N = 0;

    if(K <= 0) {
        cerr << "Invalid kernel value" << endl;
    }

    cout << "First scan of training data" << endl;
    ifstream corpus1(argc[1]);
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

    SparseMat *mat = new SparseMat(W,N);

    int n = 0;
    ifstream corpus2(argc[1]);
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

    cout << "Finding eigenvectors";
    double *evec = new double[K*mat->M];
    double *eval = new double[K];
    arpack_sym_eig(*mat,K,evec,eval);
    delete mat;

    DenseMat U(evec,K,W);
    
    cout << "Projecting vectors" << endl;

    ifstream testCorpus(argc[2]);
    ofstream out(argc[4]);
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
    delete[] evec;
    delete[] eval;
}

