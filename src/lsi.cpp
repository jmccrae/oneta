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
#include "sparse_mat.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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

//--------MMap----
extern "C" void *mmap(void *addr, size_t length, int prot, int flags,
                  int fd, off_t offset);
extern "C" int munmap(void *addr, size_t length); 
//----------------

void divide(shared_ptr<vector<double>> v1, const double *v2) {
    for(unsigned i = 0; i < v1->size(); i++) {
        (*v1)[i] /= v2[i];
    }
}


/**
 * Calculate the top K eigendecompsition of AAT
 * @param M A sparse matrix
 * @param K The number of LSI topics 0 < K <= M.N
 * @param eval The vector to write the eigenvalues to. eval = new double[K];
 * @return The array of vectors
 */
double *arpack_sym_eig(SparseMat& M, int K, double *eval, char *tmpname) {
    cout << "Preparing ARPACK" << endl;
    char tmpname2[256];
    tmpnam(tmpname2);
    int ido = 0; 
    int n = M.M;
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
    double *v;
    if(tmpname) {
        int fd = open(tmpname, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
        cout << tmpname << endl;
        if(fd == -1) {
            cerr << "Could not allocate" << endl;
            return 0;
        }
        v = (double*)mmap(0,ldv*ncv*sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(v == MAP_FAILED) {
            cerr << "map failed" << endl;
            return 0;
        }
    } else {
        v=new double[ldv*ncv];
    }
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
    double *workl;
    if(tmpname) {
        int fd = open(tmpname2, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
        cout << tmpname2 << endl;
        if(fd == -1) {
            cerr << "Could not allocate" << endl;
            return 0;
        }
        workl = (double*)mmap(0,ncv*(ncv+8)*sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    } else {
        workl=new double[ncv*(ncv+8)];
    }
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
    if(tmpname) {
        remove(tmpname2);
    }
    return v;
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

    SparseMat *mat = new SparseMat(W,N);

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
    double *evec = arpack_sym_eig(*mat,K,eval,use_mmap ? mmap_file : NULL);
    delete mat;

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
