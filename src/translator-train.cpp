#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include "sparse_mat.h"
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


bool countCorpus(char *fName, int *N, unordered_map<string,int>& words) {
    ifstream corpus(fName);
    
    if(corpus.fail()) {
        cerr << "Could not access " << fName << endl;
        return false;
    }
    string line;

    int W = 0;

    while(getline(corpus,line)) {
        stringstream tokens(line);
        string token;
        while(getline(tokens,token,' ')) {
            if(words.find(token) == words.end()) {
                words[token] = W++;
            }
        }
        (*N)++;
    }
    corpus.close();
    return true;
}

shared_ptr<SparseMat> buildTDM(char *fName, int N, unordered_map<string,int>& words) {
    ifstream corpus(fName);
    string line;
    auto mat = make_shared<SparseMat>(words.size(),N);

    int n = 0;

    while(getline(corpus,line)) {
        map<int,double> colData;
        stringstream tokens(line);
        string token;
        while(getline(tokens,token,' ')) {
            if(words.find(token) == words.end()) {
                cerr << "not found: " << token << endl;
            }
            const int w = words[token];
            if(colData.find(w) == colData.end()) {
                colData[w] = 1;
            } else {
                colData[w] += 1;
            }
        }
        mat->add_col(n++,colData);
        if(n % 100 == 0) {
            cerr << ".";
            cerr.flush();
        }
    }
    cerr << endl;
    corpus.close();
    return mat;
}

typedef map<string,vector<pair<string,double>>> TranslationMap;
typedef shared_ptr<map<string,vector<pair<string,double>>>> TranslationMapPtr;

TranslationMapPtr buildTranslationMap(char *fName) {
    ifstream csvFile(fName);
    auto transMap = make_shared<TranslationMap>();
    string line;
    getline(csvFile,line); // discard header
    while(getline(csvFile,line)) {
        string f,t,scoreStr;
        stringstream tokens(line);
        getline(tokens,f,',');
        getline(tokens,t,',');
        getline(tokens,scoreStr,',');
        double score = atof(scoreStr.c_str());
        if(transMap->find(f) != transMap->end()) {
            transMap->at(f).push_back(pair<string,double>(t,score));
        } else {
            vector<pair<string,double>> l;
            l.push_back(pair<string,double>(t,score));
            transMap->insert(pair<string,vector<pair<string,double>>>(f,l));
        }
    }
    return transMap;
}


double *arpack_sym_eig(shared_ptr<SparseMat> M, int K, double *eval) {
    cout << "Preparing ARPACK" << endl;
    int ido = 0; 
    int n = M->M;
    char bmat[2] = "I"; /* standard symmetric eigenvalue problem*/
    char which[3] = "LA"; /* Largest eigenvalue*/

    double tol = 0.0; /* Machine precision*/
    double *resid=new double[n];
    int ncv = 2*K; 
    if(ncv>n){
        ncv = n;
    }
    int ldv=n;
    cout << "Alloc v (" << (ldv*ncv) << " elements)" << endl;
    double *v=new double[ldv*ncv];
    cout << "ok" << endl;
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
    cout << "Alloc workl (" << (ncv*(ncv+8)) << " elements)" << endl;
    double *workl=new double[ncv*(ncv+8)];
    cout << "ok" << endl;
    int lworkl = ncv*(ncv+8); /* Length of the workl array */
    int info = 0; /* Passes convergence information out of the iteration
					routine. */
    int rvec = 1; /* Specifies that eigenvectors should be calculated */
    int *select=new int[ncv];
    double *d=new double[2*ncv];
    cout << "OK" << endl;

    do {
        dsaupd_(&ido, bmat, &n, which, &K, &tol, resid, &ncv, v, &ldv, iparam, ipntr, workd, workl, &lworkl, &info);
        if ((ido==1)||(ido==-1)){
            M->ATA_mult(workd+ipntr[0]-1,workd+ipntr[1]-1);
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
        //memcpy(evec, v, sizeof(double)*K*n);
    }
    cout << "ARPACK done" << endl;
    delete[] resid;
    delete[] iparam;
    delete[] ipntr;
    delete[] workd;
    delete[] workl;
    delete[] select;
    delete[] d;
    return v;
}

double *word2vec(shared_ptr<SparseMat> C, double *eval, double *evectors, int K, int W, int N) {
    double *X = new double[K * W];
    memset(X,0,K * W * sizeof(double));
    for(int i = 0; i < K; i++) {
        for(int j = 0; j < N; j++) {
            for(int y = C->cp[j]; y < C->cp[j+1]; y++) {
                auto d = C->data[y];
                X[i,d.idx] += eval[i] * evectors[j * N + i] * d.val;
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

    auto C1 = buildTDM(argv[1],N1,words1);
    auto C2 = buildTDM(argv[2],N2,words2);

    auto T = buildTranslationMap(argv[3]);

    double eval[K];

    auto v = arpack_sym_eig(C1,K,eval);

    for(int i = 0; i <= K; i++) {
        for(int j = i*N1; j <= (i+1)*N1; j++) {
            cerr << v[j] << endl;
        }
        cerr << endl;
    }

    auto X = word2vec(C1,eval,v,K,words1.size(),N1);

    /*for(int i = 0; i < words1.size() * K; i++) {
        cerr << X[i] << endl;
    }*/

    return 0;
}
