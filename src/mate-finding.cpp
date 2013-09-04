#include <vector>
#include <cmath>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <sstream>

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
        return 1.0;
    } else {
        //cout << "ab=" << ab << " a2=" << a2 << " b2=" << b2 << endl;
        return ab / sqrt(a2*b2);
    }
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
        stringstream ss(line);
        ss.exceptions(ifstream::failbit);	
        if(linesRead % 10 == 0) {
	   cerr << ".";
        }
        cerr.flush();
        linesRead++;
        SparseArray arr;
        while(ss.peek() != '|') {
            SparseArrayElem e;
            ss >> e.idx;
            arr.push_back(e);
            while(ss.peek() == ' ') {
                ss.get();
            }
        }
        while(ss.peek() == '|' || ss.peek() == ' ') {
            ss.get();
        }
        unsigned vals = 0;
        for(auto it = arr.begin(); it != arr.end(); ++it) {
            if(++vals > arr.size()) {
                throw runtime_error("Index and value length differ (too many values)");
            }
            ss >> it->value;
        }   
        if(vals != arr.size()) {
            throw runtime_error("Index and value length differ (too few values)");
        }
 
        sort(arr.begin(),arr.end());
        t.push_back(arr);
    }
    cerr << "OK" << endl;
}
     
int main(int argc, char **argv) {
    if(argc != 3) {
        cerr << "Usage: mate-finding vectors1 vectors2" << endl;
        cerr << endl;
        cerr << "Compare two sets of translingual representations." << endl;
        cerr << "It is assumed the files correspond line-by-line" << endl;
        cerr << endl;
        cerr << " vectors1   The first set of vectors" << endl;
        cerr << " vectors2   The second set of vectors" << endl;

        return -1;
    }
    
    vector<SparseArray> t1;
    try {
        readTopicFile(t1,argv[1]);
    } catch(ifstream::failure e) {
        cerr << "Failed: " << e.what() << endl;
        return -1;
    }
    
    vector<SparseArray> t2;
    try {
        readTopicFile(t2,argv[2]);
    } catch(ifstream::failure e) {
        cerr << "Failed: " << e.what() << endl;
        return -1;
    }
    double top1prec = 0, top5prec = 0, top10prec = 0, mrr =0;
    int n = 0;

    if(t1.size() != t2.size()) {
        cerr << "Topic sets differ in length" << endl;
        return -1;
    }

    int N = t1.size();

    for(int i = 0; i < N; i++) {
        double *sim = new double[N];
        for(int j = 0; j < N; j++) {
            sim[j] = cosSim(t1[i],t2[j]);
        }
        int rank = 1;
        int ties = 0;
        for(int j = 0; j < N; j++) {
            if(sim[j] > sim[i]) {
                rank++;
            }
            if(sim[j] == sim[i]) {
                ties++;
            }
        }
        if(ties != 1) {
            cout << "!";
        }
        if(rank == 1) {
            top1prec++;
            cout << "+";
        } else if(rank < 10) {
            cout << rank;
        } else {
            cout << "-";
        }
        cout.flush();
        
        if(rank <= 5) {
            top5prec += 1;
        }
	if(rank <= 10) {
            top10prec += 1;
        }
        mrr += 1.0/rank;
        n++;
        delete[] sim;
    }
    cout << endl;
    cout << "Top-1  Precision     : " << (top1prec/n) << endl;
    cout << "Top-5  Precision     : " << (top5prec/n) << endl;
    cout << "Top-10 Precision     : " << (top10prec/n) << endl;
    cout << "Mean Reciprocal Rank : " << (mrr / n) << endl;
        
    return 0;
}      
