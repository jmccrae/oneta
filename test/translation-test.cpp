#include "../src/translation.h"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <cstdio>

using namespace std;

#define ERROR 1e-2

string setUp() {
   char fname[L_tmpnam]; 
   tmpnam(fname);

   ofstream out(fname);
   out << "a a b" << endl;
   out << "b c d" << endl;
   out << "a b d d" << endl;
   out.flush();
   out.close();

   return string(fname);
}

void tearDown(string fname) {
    remove(fname.c_str());
}

TEST_CASE("Count Corpus", "[CountCorpus]") {
   auto fname = setUp();
   int N = 0;
   unordered_map<string,int> words;

   REQUIRE(countCorpus(fname.c_str(),&N,words));
   REQUIRE(N == 3);
   REQUIRE(words.size() == 4);

   tearDown(fname);
}

TEST_CASE("Count Corpus Filtered", "[CountCorpusFiltered]") {
   auto fname = setUp();
   int N = 0;
   unordered_map<string,int> words;
   auto filter = make_shared<set<string>>();
   filter->insert("b");
   filter->insert("c");

   REQUIRE(countCorpusFiltered(fname.c_str(),&N,words,filter));
   REQUIRE(N == 3);
   REQUIRE(words.size() == 2);

   tearDown(fname);
}


TEST_CASE("Build TDM", "[BuildTDM]") {
    auto fname = setUp();
    
    unordered_map<string,int> words;
    words["a"] = 0;
    words["b"] = 1;
    words["c"] = 2;
    words["d"] = 3;

    auto tdm = buildTDM(fname.c_str(),3,words);


    REQUIRE(tdm->data[0].val == 2);
    REQUIRE(tdm->data[1].val == 1);
    REQUIRE(tdm->data[2].val == 1);
    REQUIRE(tdm->data[3].val == 1);
    REQUIRE(tdm->data[4].val == 1);
    REQUIRE(tdm->data[5].val == 1);
    REQUIRE(tdm->data[6].val == 1);
    REQUIRE(tdm->data[7].val == 2);

    tearDown(fname);
}

TEST_CASE("Build translation map","[BuildTranslationMap]") {
    char fname[L_tmpnam];
    tmpnam(fname);

    ofstream out(fname);
    out << "F,T,S" << endl;
    out << "a,A,0.2" << endl;
    out << "a,C,0.3" << endl;
    out << "b,B,0.7" << endl;
    out.flush();
    out.close();

    auto transMap = buildTranslationMap(fname);

    REQUIRE(transMap->size() == 2);
    REQUIRE((*transMap)["a"].size() == 2);
    REQUIRE((*transMap)["a"][0].second == 0.2);

    remove(fname);
}
    
TEST_CASE("Translation Source Words","[TransSourceWords]") {
    auto transMap = make_shared<TranslationMap>();
    vector<pair<string,double>> v1;
    v1.push_back(pair<string,double>("A",0.2));
    v1.push_back(pair<string,double>("C",0.3));
    vector<pair<string,double>> v2;
    v2.push_back(pair<string,double>("B",0.7));

    (*transMap)["a"] = v1;
    (*transMap)["b"] = v2;

    auto srcWords = transSourceWords(transMap);

    REQUIRE(srcWords->size() == 2);
}    

TEST_CASE("Translation Target Words","[TransTargetWords]") {
    auto transMap = make_shared<TranslationMap>();
    vector<pair<string,double>> v1;
    v1.push_back(pair<string,double>("A",0.2));
    v1.push_back(pair<string,double>("C",0.3));
    vector<pair<string,double>> v2;
    v2.push_back(pair<string,double>("B",0.7));

    (*transMap)["a"] = v1;
    (*transMap)["b"] = v2;

    auto srcWords = transTargetWords(transMap);

    REQUIRE(srcWords->size() == 3);
}

TEST_CASE("Invert word map","[InvertWordMap]") {
    unordered_map<string,int> words;
    words["a"] = 0;
    words["b"] = 1;
    words["c"] = 2;
    words["d"] = 3;

    vector<string> invMap(4);
    invertWordMap(words,invMap);

    REQUIRE(invMap[0] == "a");
    REQUIRE(invMap[1] == "b");
    REQUIRE(invMap[2] == "c");
    REQUIRE(invMap[3] == "d");
}
