#include "arpack.h"
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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


double *arpack_sym_eig(shared_ptr<SparseMat> M, int K, double *eval, bool inner, char *tmpname) {
    cout << "Preparing ARPACK" << endl;
    char tmpname2[256];
    tmpnam(tmpname2);
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

    cerr << "ncv=" << ncv << endl;
    cerr << "K=" << K << endl;
    cerr << "N=" << n << endl;

    do {
        dsaupd_(&ido, bmat, &n, which, &K, &tol, resid, &ncv, v, &ldv, iparam, ipntr, workd, workl, &lworkl, &info);
        if ((ido==1)||(ido==-1)){
            if(inner) {
                M->ATA_mult(workd+ipntr[0]-1,workd+ipntr[1]-1);
            } else {
                M->AAT_mult(workd+ipntr[0]-1,workd+ipntr[1]-1);
            }
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


