Orthonormal Explicit Topic Analysis
===================================

This package represents code for generating semantic translingual document representations.

The package implements the following methodologies

* Latent Semantic Indexing
* Latent Dirichlet Allocation
* (Orthonormal) Explicit Topic Analysis

Requirements
------------

* G++ (>= 4.4.5) or a similar C++11 compiler with
* Make
* ARPACK
* GNU Scientific Library
* CMake

Installation
------------

    cmake .
    make

Commands
--------

The input format of files is tokenized text files with one document per line. The output format is a sparse vector of the following form:

    i1 i2 ... in ||| v1 v2 ... vn

Where the `i1` element of the vector has value `v1` and omitted values are zero

LSI
---

     bin/lsi train-corpus test-corpus-src test-corpus-trg K vectors-src vectors-trg

Applies the Latent Semantic Indexing method

* `traincorpus`: The training corpus where documents consist of both languages appended (i.e., by using `scripts/join.py`)
* `test-corpus-src`: The source language evaluation corpus
* `test-corpus-trg`: The target language evaluation corpus
* `K`: The number of topics to calculate in the representation
* `vectors-src`: The output file to write the source language representations to
* `vectors-trg`: The output file to write the target language representations to

LDA
---

     bin/lda train-corpus-src train-corpus-trg test-corpus-src test-corpus-trg K N vectors-src vectors-trg

Applies the Polylingual Latent Dirichlet Allocation method

* `train-corpus-src`: The source language training corpus
* `train-corpus-trg`: The target language training corpus
* `test-corpus-src`: The source language evaluation corpus
* `test-corpus-trg`: The target language evaluation corpus
* `K`: The number of topics to calculate
* `N`: The number of iterations of Gibbs sampling to perform
* `vectors-src`: The output file to write the source language representations to
* `vectors-trg`: The output file to write the target language representations to   

ONETA
-----

     bin/oneta train-corpus test-corpus kernel-size vectors [-sqnorm]

Applies the (Orthonormal) explicit topic analysis method

* `train-corpus`: The training corpus (Note this command should be run once per language)
* `test-corpus`: The test corpus
* `kernel-size`: The degree of complexity in the calculation. At N = 0 this command
    runs no orthonormalization, if N is the number of lines (documents) in the _train_ corpus
    this is full orthonormalization.
* `vectors`: The output to write the translingual representations to
* `-sqnorm`: If this is set apply term frequency normalization (Note this must appear at the end of the command list)

Mate finding
------------

     bin/mate-finding topics1 topics2

Compare two sets of translingual representations. It is assumed the files correspond line-by-line

References
==========

J.P. McCrae, P. Cimiano & R. Klinger (2013). Orthonormal explicit topic analysis for cross-lingual document matching. In 
Proceedings of EMNLP 2013.

The data used in this paper is in [here](http://greententacle.techfak.uni-bielefeld.de/~jmccrae/oneta_emnlp_data.tar.gz)
