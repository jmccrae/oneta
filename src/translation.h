#pragma once
#include "sparse_mat.h"
#include <unordered_map>
#include <set>

typedef std::map<std::string,std::vector<std::pair<std::string,double>>> TranslationMap;
typedef std::shared_ptr<std::map<std::string,std::vector<std::pair<std::string,double>>>> TranslationMapPtr;

/**
 * Count the number of unique tokens and documents in the corpus and build the word map
 * @param fName The file name of the corpus
 * @param N The value to set for the number of documents
 * @param words The map to fill with the indices of words
 * @return False if the method did not succeed (i.e., the file name did not exist)
 */
bool countCorpus(const char *fName, int *N, std::unordered_map<std::string,int>& words);

/**
 * Count the number of unique tokens and documents in the corpus given a filter on words
 * @param fName  The file name of the corpus
 * @param N The value to set for the number of documents
 * @param words The map to fill with the indices of words
 * @param filter The set of words to accept
 * @return False if the method did not succeed (i.e., the file name did not exist)
 */
bool countCorpusFiltered(const char *fName, int *N, std::unordered_map<std::string,int>& words, std::shared_ptr<std::set<std::string>> filter);

/**
 * Build a term-document sparse matrix from the corpus
 * @param fName The file name of the corpus
 * @param N The number of documents (lines) in the corpus
 * @param words The word map
 * @return The matrix
 */
std::shared_ptr<SparseMat> buildTDM(const char *fName, int N, std::unordered_map<std::string,int>& words);

/**
 * Build the translation function from the CSV file of the format
 *   srcWord,trgWord,weight
 * @param fName The file name
 * @return The translation map: a map where each source word is linked to a list of 
 * translation/score pairs
 */
TranslationMapPtr buildTranslationMap(const char *fName);

/**
 * Reduce a translation map to just the source words
 * @param The translation words
 * @return The set of source words
 */
std::shared_ptr<std::set<std::string>> transSourceWords(TranslationMapPtr transMap);

/**
 * Reduce the translation map to just the target words
 * @param The translation words
 * @return The set of target words
 */
std::shared_ptr<std::set<std::string>> transTargetWords(TranslationMapPtr transMap);

/**
 * Convert the word map to a list of words (where the ith element is that with idx i)
 * @param wordMap The word map to read from
 * @param invMap The list to write, must already be initialized to correct size
 */
void invertWordMap(std::unordered_map<std::string,int>& wordMap, std::vector<std::string>& invMap);
