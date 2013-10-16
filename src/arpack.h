// Define the interface to ARPACK
#pragma once
#include <memory>
#include "sparse_mat.h"

/**
 * Solve the eigenproblem
 * @param M A sparse symmetric matrix
 * @param K The number of eigenvectors/values to calculate
 * @param eval The vector write the eigenvalues to, should be of length K
 * @param inner If true solve the problem ATAx = lx, otherwise AATx = lx
 * @param mmap_file File to use for mmap or NULL for no mmap
 * @return The eigenvalues in ?x? form
 */
double *arpack_sym_eig(std::shared_ptr<SparseMat> M, int K, double *eval, bool inner, char *mmap_file);
