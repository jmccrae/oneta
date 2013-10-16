#include "../src/arpack.h"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace std;

#define ERROR 1e-2

TEST_CASE("Test AAT multiplication", "[SparseMat AAT]") {
    auto M = make_shared<SparseMat>(2,2);

    map<int,double> col1;
    col1[0] = 1;
    col1[1] = 2;
    M->add_col(0,col1);
    map<int,double> col2;
    col2[0] = 3;
    col2[1] = 4;
    M->add_col(1,col2);

    double v_in[] = { 1.0, 1.0 };
    double v_out[] = { 0.0, 0.0 };

    M->ATA_mult(v_in,v_out);

    REQUIRE(v_out[0] == 16);
    REQUIRE(v_out[1] == 36);
}

TEST_CASE("Test symmetric eigen problem", "[Arpack]") {
    auto M = make_shared<SparseMat>(2,2);

    map<int,double> col1;
    col1[0] = 1;
    col1[1] = 2;
    M->add_col(0,col1);
    map<int,double> col2;
    col2[0] = 3;
    col2[1] = 4;
    M->add_col(1,col2);

    double *eval = new double[1];

    double *evec = arpack_sym_eig(M,1,eval,false,NULL);


    SECTION("eigenvalues") {
        REQUIRE(abs(eval[0] - 29.86) < ERROR);
    }

    SECTION("eigenvectors") {
        REQUIRE(abs(evec[0] - 0.576) < ERROR);
        REQUIRE(abs(evec[1] - 0.817) < ERROR);
    }
}
