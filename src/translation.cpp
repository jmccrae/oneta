#include "translation.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

/**
 * Count the number of unique tokens and documents in the corpus and build the word map
 * @param fName The file name of the corpus
 * @param N The value to set for the number of documents
 * @param words The map to fill with the indices of words
 * @return False if the method did not succeed (i.e., the file name did not exist)
 */
bool countCorpus(const char *fName, int *N, unordered_map<string,int>& words) {
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

/**
 * Count the number of unique tokens and documents in the corpus given a filter on words
 * @param fName  The file name of the corpus
 * @param N The value to set for the number of documents
 * @param words The map to fill with the indices of words
 * @param filter The set of words to accept
 * @return False if the method did not succeed (i.e., the file name did not exist)
 */
bool countCorpusFiltered(const char *fName, int *N, unordered_map<string,int>& words, shared_ptr<set<string>> filter) {
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
        bool tokenFound = false;
        while(getline(tokens,token,' ')) {
            if(words.find(token) == words.end() && filter->find(token) != filter->end()) {
                words[token] = W++;
                tokenFound = true;
            }
        }
        if(!tokenFound) {
            cerr << "A line was empty after filtering" << endl;
        }
        (*N)++;
    }
    corpus.close();
    return true;
}



/**
 * Build a term-document sparse matrix from the corpus
 * @param fName The file name of the corpus
 * @param N The number of documents (lines) in the corpus
 * @param words The word map
 * @return The matrix
 */
shared_ptr<SparseMat> buildTDM(const char *fName, int N, unordered_map<string,int>& words) {
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

/**
 * Build the translation function from the CSV file of the format
 *   srcWord,trgWord,weight
 * @param fName The file name
 * @return The translation map: a map where each source word is linked to a list of 
 * translation/score pairs
 */
TranslationMapPtr buildTranslationMap(const char *fName) {
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

/**
 * Reduce a translation map to just the source words
 * @param The translation words
 * @return The set of source words
 */
shared_ptr<set<string>> transSourceWords(TranslationMapPtr transMap) {
    auto words = make_shared<set<string>>();
    for(auto it = transMap->begin(); it != transMap->end(); ++it) {
        words->insert(it->first);
    }
    return words;
}

/**
 * Reduce the translation map to just the target words
 * @param The translation words
 * @return The set of target words
 */
shared_ptr<set<string>> transTargetWords(TranslationMapPtr transMap) {
    auto words = make_shared<set<string>>();
    for(auto it = transMap->begin(); it != transMap->end(); ++it) {
        for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
            words->insert(it2->first);
        }
    }
    return words;
}

/**
 * Convert the word map to a list of words (where the ith element is that with idx i)
 * @param wordMap The word map to read from
 * @param invMap The list to write, must already be initialized to correct size
 */
void invertWordMap(unordered_map<string,int>& wordMap, vector<string>& invMap) {
    for(auto it = wordMap.begin(); it != wordMap.end(); ++it) {
        invMap[it->second] = it->first;
    }
}


shared_ptr<gsl_matrix> monoKernel(double *evectors, int K,
        TranslationMapPtr translationMap, TranslationMapPtr invTranslationMap, unordered_map<string,int>& wordMap) {
    auto words = transSourceWords(translationMap);
    auto M = shared_ptr<gsl_matrix>(gsl_matrix_alloc(K,K));

    for(int k = 0; k < K; k++) {
        for(int k2 = 0; k2< K; k2++) {
            double v = 0.0;
            for(auto word : *words) {
                auto w = wordMap[word];
                auto translations = &translationMap->at(word);
                for(auto it = translations->begin(); it != translations->end(); ++it) {
                    auto invTranslations = &invTranslationMap->at(it->first);
                    for(auto it2 = invTranslations->begin(); it2 != invTranslations->end(); ++it2) {
                        auto w2 = wordMap[it2->first];
                        v += evectors[w * K + k] * it->second * it2->second * evectors[w2 * K + k2];
                    }
                }
            }
            gsl_matrix_set(M.get(),k,k2,v);
        }
    }
    return M;
}

shared_ptr<gsl_matrix> biKernel(double *evectorsSrc, double *evectorsTrg, int K,
        TranslationMapPtr translationMap, unordered_map<string,int>& wordMap1,
        unordered_map<string,int>& wordMap2) {
    auto M = shared_ptr<gsl_matrix>(gsl_matrix_alloc(K,K));

    for(int k = 0; k < K; k++) {
        for(int k2 = 0; k2 < K; k2++) {
            double v = 0.0;
            for(auto it = translationMap->begin(); it != translationMap->end(); ++it) {
                auto w1 = wordMap1[it->first];
                for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
                    auto w2 = wordMap2[it2->first];
                    v += evectorsSrc[w1 * K + k] * it2->second * evectorsTrg[w2 * K + k2];
                }
            }
            gsl_matrix_set(M.get(),k,k2,v);
        }
    }
    return M;
}


/** Invert a translation map
 * @param The translation map
 * @return The inverted map
 */
TranslationMapPtr inverseMap(TranslationMapPtr map) {
    auto invMap = make_shared<TranslationMap>();

    for(auto it = map->begin(); it != map->end(); ++it) {
        auto f = it->first;
        for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
            if(invMap->find(it2->first) != invMap->end()) {
                invMap->at(it2->first).push_back(pair<string,double>(f,it2->second));
            } else {
                vector<pair<string,double>> vec;
                vec.push_back(pair<string,double>(f,it2->second));
                invMap->insert(pair<string,vector<pair<string,double>>>(it2->first,vec));
            }
        }
    }

    return invMap;
}
