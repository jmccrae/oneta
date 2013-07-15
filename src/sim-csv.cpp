#include <vector>
#include <cmath>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <sstream>
#include <memory>
#include <cstring>
#ifdef DEBUG
#define BACKWARD_HAS_BFD 1
#include "backward.hpp"
namespace backward {
    backward::SignalHandling sh;
}
#endif


using namespace std;

/**
 * Just a value index pair
 */
class SparseArrayElem {
    public:
        int idx;
        double value;
	SparseArrayElem() : idx(0), value (0.0) {

	}
        SparseArrayElem(const int i, const double v) : idx(i), value(v) {

        }
        bool operator<(const SparseArrayElem& e) const {
            return idx < e.idx;
        }
};

typedef vector<SparseArrayElem> SparseArray;

double cosSim(const SparseArray& a, const SparseArray& b) {
    double ab = 0, a2 = 0, b2 = 0;
    auto i = a.begin(), j = b.begin();
    
    while(i != a.end() || j != b.end()) {
        if(i == a.end()) {
            b2 += j->value * j->value;
            ++j;
        } else if(j == b.end()) {
            a2 += i->value * i->value;
            ++i;
        } else if((*i) < (*j)) {
            a2 += i->value * i->value;
            ++i;
        } else if((*j) < (*i)) {
            b2 += j->value * j->value;
            ++j;
        } else {
            a2 += i->value * i->value;
            b2 += j->value * j->value;
            ab += i->value * j->value;
            ++i;
            ++j;
        }
    }
 
    if(a2 == 0 || b2 == 0) {
        return 0.0;
    } else {
        //cout << "ab=" << ab << " a2=" << a2 << " b2=" << b2 << endl;
        return ab / sqrt(a2*b2);
    }
}

unique_ptr<SparseArray> readTopicLine(string& line, int& linesRead) {
    stringstream ss(line);
    ss.exceptions(ifstream::failbit);	
    if(linesRead % 10 == 0) {
       cerr << ".";
    }
    cerr.flush();
    linesRead++;
    auto arr = unique_ptr<SparseArray>(new SparseArray());
    while(ss.peek() != '|') {
        SparseArrayElem e;
        ss >> e.idx;
        arr->push_back(e);
        while(ss.peek() == ' ') {
            ss.get();
        }
    }
    if(!arr->empty()) {
        while(ss.peek() == '|' || ss.peek() == ' ') {
            ss.get();
        }
        unsigned vals = 0;
        for(auto it = arr->begin(); it != arr->end(); ++it) {
            if(++vals > arr->size()) {
                throw runtime_error("Index and value length differ (too many values)");
            }
            ss >> it->value;
        }   
        if(vals != arr->size()) {
            throw runtime_error("Index and value length differ (too few values)");
        }
    }
 
    sort(arr->begin(),arr->end());

    return arr;
}
 
/**
 * Read a topic vector file
 * The format of these files are such that each line is:
 * idx1 idx2 ... idxN ||| val1 val2 ... valN
 * Which represents a coordinate form sparse array
 * @param t Vector to write to
 * @param fname The file name to read from
 */
void readTopicFile(vector<SparseArray>& t, const char *fname) {
    cerr << "Reading " << fname << endl;
    int linesRead = 1;
    ifstream topic1file(fname,ifstream::in);
    if(topic1file.fail()) {
        cerr << "Failed to read " << fname << endl;
        return;
    }
    string line;
    while(getline(topic1file,line)) {
        t.push_back(*readTopicLine(line,linesRead));
    }
    cerr << "OK" << endl;
}

   
int main(int argc, char **argv) {
    if(argc != 3 && argc != 5) {
        cerr << "Usage: sim-csv topics1 topics2 [topic1-names topic2-names] > scores" << endl;
        return -1;
    }
    
    bool namedTopics = argc == 5;
    bool use_stdin = strcmp(argv[1],"-") == 0;

    vector<string> topic1names;
    vector<string> topic2names;
    if(namedTopics) {
        ifstream topic1namesFile(argv[3]);
        string line;
        while(getline(topic1namesFile,line)) {
            topic1names.push_back(line);
        }
        topic1namesFile.close();
        ifstream topic2namesFile(argv[4]);
        while(getline(topic2namesFile,line)) {
            topic2names.push_back(line);
        }
    }

    vector<SparseArray> t1;
    if(!use_stdin) {
        try {        
            readTopicFile(t1,argv[1]);
        } catch(ifstream::failure e) {
            cerr << "Failed: " << e.what() << endl;
            return -1;
        }
    }
    
    vector<SparseArray> t2;
    try {
        readTopicFile(t2,argv[2]);
    } catch(ifstream::failure e) {
        cerr << "Failed: " << e.what() << endl;
        return -1;
    }

    if(t1.size() != t2.size() && !use_stdin) {
        cerr << "Topic sets differ in length" << endl;
        return -1;
    }

    unsigned N = t2.size();

    if(namedTopics && (N != topic1names.size() || N != topic2names.size())) {
        cerr << "Named topics size mismatch: " << N << ", " << topic1names.size() << ", " << topic2names.size() << endl;
        return -1;
    }

    int linesRead = 0;

    cerr << "Writing CSV" << endl;
    cout << (use_stdin ? "STDIN" : argv[1]) << "," << argv[2] << ",SIM" << endl;
    for(unsigned i = 0; i < N || use_stdin; i++) {
        unique_ptr<SparseArray> t1i;
        if(use_stdin) {
            string line;
            if(!getline(cin,line)) {
                break;
            }
            t1i = readTopicLine(line,linesRead);
        }        
        for(unsigned j = 0; j < N; j++) {
            double sim;
            if(use_stdin) {
               sim = cosSim(*t1i,t2[j]);
            } else {
                sim = cosSim(t1[i],t2[j]);
            }
            if(namedTopics) {
                cout << "\"" << topic1names[i] << "\",\"" << topic2names[j] << "\"," << sim << endl;
            } else {
                cout << i << "," << j << "," << sim << endl;
            }
        }
        if(i % 10 == 9) {
            cerr << ".";
            cerr.flush();
        }
    }
    cerr << endl;
        
    return 0;
}      
