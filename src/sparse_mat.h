#pragma once
#include <vector>
#include <memory>
#include <map>
#include <iostream>

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
        bool operator<(const SparseArrayElem& e) const {
            return idx < e.idx;
        }
};

typedef std::vector<SparseArrayElem> SparseArray;

/**
 * A sparse matrix in compressed column format
 */
class SparseMat {
    public:
        /**
         * Number of rows and columns
         */
        const unsigned M,N;
        /**
         * The column pointer (offset where the nth column starts in the data)
         */
        unsigned *cp;
        /**
         * The non-zero values
         */
        std::vector<SparseArrayElem> data;
        /**
         * Create a sparse matrix of a given dimension
         */
        SparseMat(const unsigned rows, const unsigned cols) : M(rows), N(cols) {            
            cp = new unsigned[cols+1];
            cp[0] = 0;
        }
        ~SparseMat() {
            delete[] cp;
        }
        /**
         * Calculate Av
         */
        std::shared_ptr<std::vector<double>> operator*(std::shared_ptr<std::vector<double>>& v) {
            if(v->size() != N) {
                throw "Wrong vector size";
            }
            auto v2 = std::make_shared<std::vector<double>>(M,0.0);
            auto it = data.begin();
            for(unsigned i = 0; i < N; i++) {
                for(unsigned j = cp[i]; j < cp[i+1]; j++) {
                    (*v2)[it->idx] += (*v)[i] * it->val;
                    ++it;
                }
            }
            return v2;
        }
        /**
         * Calculate A^T v
         */
        std::shared_ptr<std::vector<double>> multt(std::shared_ptr<std::vector<double>>& v) {
            if(v->size() != M) {
                std::cerr << v->size() << " != " << M << std::endl;
                throw "Wrong vector size";
            }
            auto v2 = std::make_shared<std::vector<double>>(N,0.0);
            auto it = data.begin();
            for(unsigned i = 0; i < N; i++) {
                for(unsigned j = cp[i]; j < cp[i+1]; j++) {
                    (*v2)[i] += (*v)[it->idx] * it->val;
                    ++it;
                }
            }
            return v2;
        }
        /**
         * Used when building the matrix, no checks for double calls
         */
        void add_col(int n, std::map<int,double>& colData) {
            for(auto it = colData.begin(); it != colData.end(); ++it) {
                data.push_back(SparseArrayElem(it->first,it->second));
            }
            cp[n+1] = cp[n] + colData.size();
        }
        /**
         * For the i-th column compute <a_i,a_i>
         */
        double col_inner(int i) {
            double sq = 0;
            for(unsigned j = cp[i]; j < cp[i+1]; j++) {
                sq += data[j].val * data[j].val;
            }
            return sq;
        }

        /**
         * If this matrix is A calculate v_out = AA^T v_in
         */
        void AAT_mult(double *v_in, double *v_out) {
            double *v_tmp = new double[N];
            memset(v_tmp,0,N*sizeof(double));
            memset(v_out,0,M*sizeof(double));
            auto it = data.begin();
            for(unsigned i = 0; i < N; i++) {
                for(unsigned j = cp[i]; j < cp[i+1]; j++) {
                    v_tmp[i] += v_in[it->idx] * it->val;
                    ++it;
                }
            }
            it = data.begin();
            for(unsigned i = 0; i < N; i++) {
                for(unsigned j = cp[i]; j < cp[i+1]; j++) {
                    v_out[it->idx] += v_tmp[i] * it->val;
                    ++it;
                }
            }           
            delete[] v_tmp;
        }

        /**
         * Compute the inner product of <a_i,a_j>
         */
        double inner(int i, int j) {
            double sq = 0;
            unsigned a = cp[i], b = cp[j];
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
            for(auto it = data.begin(); it != data.end(); ++it) {
                std::cout << it->val << "@" << it->idx << " ";
            }
            std::cout << std::endl;
        }

};

/**
 * A simple dense matrix
 */
class DenseMat {
    public:
        double *data;
        int M,N;
        DenseMat(double *d, int rows, int cols) : data(d), M(rows), N(cols) {

        }
        std::shared_ptr<std::vector<double>> operator*(std::shared_ptr<std::vector<double>> v) {
            auto v2 = std::make_shared<std::vector<double>>(M,0);
            for(int i = 0; i < M; i++) {
                for(int j = 0; j < N; j++) {
                    (*v2)[i] += data[i*N+j] * (*v)[j];
                }
            }
            return v2;
        }
};


