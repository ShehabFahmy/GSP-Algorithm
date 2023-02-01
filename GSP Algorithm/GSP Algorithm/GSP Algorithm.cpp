#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <unordered_map>
#include <regex>
#include <fstream>

using namespace std;

vector<string> sequence0; // Database
long long minCount;
void supportPruning(vector<string>& newSeq, unordered_map<string, int>& supportCount) {
    for (auto i = newSeq.begin(); i != newSeq.end(); i++) {
        if (supportCount[*i] < minCount) {
            supportCount.erase(*i);
            newSeq.erase(i);
            i--;
        }
    }
    //    cout << newSeq.size() << endl;
    //    for (auto j : newSeq) {
    //        cout << j << " " << supportCountNS[j] << endl;
    //    }
}
void initSequenceChars(vector<string>& sequence0, vector<string>& sequence1, unordered_map<string, int>& supportCount) {
    set<string> sequenceChars;
    for (int i = 0; i < sequence0.size(); i++) {
        string s = sequence0[i];
        regex r(R"([^{}<>()])"); // [^{}]+ for data between braces // (\{([^{}]*)}) for data with braces
        // [^{}<>] for characters
        smatch m;
        unordered_map<string, bool> local;
        while (regex_search(s, m, r)) {
            string c = m[0];
            c.insert(c.begin(), '{');
            c.insert(c.end(), '}');
            if (!local[c]) {
                supportCount[c]++;
            }
            local[c] = true;
            int before = sequenceChars.size();
            sequenceChars.insert(c);
            int after = sequenceChars.size();
            if (before != after) {
                sequence1.push_back(c);
            }
            s = m.suffix().str();
        }
    }
    supportPruning(sequence1, supportCount);
    /** FOR TESTING 1-Sequence Table: **/
//    cout << sequence1.size() << endl;
//    for (auto it : sequence1) {
//        cout << it << " " << supportCount1S[it] << endl;
//    }
}
void initSequence2(vector<string>& sequence1, vector<string>& sequence2, unordered_map<string, int>& supportCount) {
    for (auto e1 = sequence1.begin(); e1 != sequence1.end(); e1++) {
        for (auto e2 = sequence1.begin(); e2 != sequence1.end(); e2++) { // TEMPORAL JOINS
            sequence2.push_back(*e1 + *e2);
        }
        for (auto e2 = e1; e2 != sequence1.end(); e2++) { // NON-TEMPORAL JOINS
            if (*e2 != *e1) {
                string x = "{";
                x += e1->at(1);
                x += e2->at(1);
                x += "}";
                sequence2.push_back(x);
            }
        }
    }
    for (auto j : sequence2) {
        /*********** HOW TO CALCULATE THE SUPPORT FOR 2-SEQUENCE ??!! ***********/
        supportCount[j] = 0;
        string s = j;
        int size = s.size();
        vector<string> v;
        regex r(R"([^{}<>()])");
        smatch m;
        while (regex_search(s, m, r)) {
            v.push_back(m[0]);
            s = m.suffix().str();
        }
        for (int i = 0; i < sequence0.size(); i++) {
            map<string, set<int>> indecies; // positions for each string
            int index = 0;
            string row = sequence0[i];
            regex r1(R"([^{}<>()]+)");
            smatch m1;
            while (regex_search(row, m1, r1)) {
                string x = m1[0];
                for (int k = 0; k < v.size(); ++k) {
                    if (x.find(v[k]) != -1) {
                        indecies[v[k]].insert(index);
                    }
                }
                index++;
                row = m1.suffix().str();
            }
            if (indecies[v[0]].size() == 0 || indecies[v[1]].size() == 0) {
                continue;
            }
            else {
                if (size == 6) {
                    if (*indecies[v[0]].begin() < *indecies[v[1]].rbegin()) {
                        supportCount[j]++;
                    }
                }
                else {
                    set<int>::iterator l;
                    for (l = indecies[v[0]].begin(); l != indecies[v[0]].end(); l++) {
                        if (indecies[v[1]].find(*l) != indecies[v[1]].end()) {
                            supportCount[j]++;
                            break;
                        }
                    }
                }
            }
        }
    }
    supportPruning(sequence2, supportCount);
    /** FOR TESTING 2-Sequence Table: **/
//    cout << sequence2.size() << endl;
//    for (auto j : sequence2) {
//        cout << j << " " << supportCount[j] << endl;
//    }
}
void generateSequence(vector<string>& prevSeq, vector<string>& newSeq) {
    for (int i = 0; i < prevSeq.size(); i++) {
        string swith = prevSeq[i];
        string swithout = "";
        string l1;
        regex r(R"([^{}<>()]+)");
        smatch m;
        int n = 0;
        while (regex_search(swith, m, r)) {
            bool flag = false;
            if (((string)m[0]).size() > 1 && n == 0) {
                flag = true;
            }
            if (swithout.size() != 0 || flag) {
                l1 += "{";
                l1 += m[0];
                l1 += "}";
            }
            n++;
            swithout += m[0];
            swith = m.suffix().str();
        }
        if (l1.empty()) {
            l1 += "{" + swithout + "}";
        }
        for (int j = 0; j < prevSeq.size(); j++) {
            if (i != j) {
                string twith = prevSeq[j];
                string twithout = "";
                string l2;
                regex r1(R"([^{}<>()]+)");
                smatch m1;
                int pos;
                while (regex_search(twith, m1, r1)) {
                    l2 += "{";
                    l2 += m1[0];
                    l2 += "}";
                    pos = ((string)m1[0]).size();
                    twithout += m1[0];
                    twith = m1.suffix().str();
                }
                pos += 2;
                if (pos == 3) {
                    l2.erase((l2.end() - pos), l2.end());
                }
                if (l2.empty()) {
                    l2 += "{" + twithout + "}";
                }
                if (swithout.substr(1, swithout.size() - 1) == twithout.substr(0, twithout.size() - 1)) {
                    if (l1.size() > l2.size()) {
                        string string1 = prevSeq[j];
                        newSeq.push_back(prevSeq[i] + string1.erase(0, l2.size()));
                    }
                    else {
                        string string1 = prevSeq[i];
                        string1.erase(string1.end() - l1.size(), string1.end());
                        newSeq.push_back(string1 + prevSeq[j]);
                    }
                }
            }
        }
    }
    /** FOR TESTING n-Sequence Table: **/
//    cout << newSeq.size() << endl;
//    for (auto i : newSeq){
//        cout << i << endl;
//    }
}
void calculateSupport(vector<string>& newSeq, unordered_map<string, int>& supportCount) {
    for (auto j : newSeq) {
        supportCount[j] = 0;
        string s = j;
        vector<string> v; // each string between braces in sequence
        regex r(R"([^{}<>()]+)");
        smatch m;
        while (regex_search(s, m, r)) {
            v.push_back(m[0]);
            s = m.suffix().str();
        }
        for (int i = 0; i < sequence0.size(); i++) {
            map<string, set<int>> indecies; // positions for each string
            int index = 0;
            string row = sequence0[i];
            regex r1(R"([^{}<>()]+)");
            smatch m1;
            while (regex_search(row, m1, r1)) {
                string x = m1[0]; // string between braces in database sequence
                for (int k = 0; k < v.size(); ++k) {
                    if (x.find(v[k]) != -1) {
                        indecies[v[k]].insert(index);
                    }
                }
                index++;
                row = m1.suffix().str();
            }
            bool fail = false;
            for (int k = 0; k < v.size(); k++) {
                if (indecies[v[k]].size() == 0) {
                    fail = true;
                    break;
                }
            }
            if (fail) {
                continue;
            }
            else {
                bool fail2 = false;
                int ind = -1;
                for (int k = 0; k < v.size(); ++k) {
                    bool flag = false;
                    for (auto l : indecies[v[k]]) {
                        if (l > ind) {
                            ind = l;
                            flag = true;
                            break;
                        }
                    }
                    if (!flag) {
                        fail2 = true;
                    }
                }
                if (!fail2) {
                    supportCount[j]++;
                }
            }
        }
    }
    /** FOR TESTING N-Sequence Table: **/
//    cout << newSeq.size() << endl;
//    for (auto j : newSeq) {
//        cout << j << " " << supportCountNS[j] << endl;
//    }
}
int main() {
    vector<vector<string>> freqSequences; // Carries all the Frequent Sequences
    unordered_map<string, int> supportCount;
    ifstream Data("data.txt");
    string line;
    while (getline(Data, line)) {
        sequence0.push_back(line);
    }
    Data.close();
    int minSupport;
    cout << "Enter Min_Support Percentage (%): ";
    cin >> minSupport;
    cout << endl;
    minCount = (minSupport * sequence0.size()) / 100;

    vector<string> sequence1; // 1-Sequence
    initSequenceChars(sequence0, sequence1, supportCount);
    freqSequences.push_back(sequence1);

    vector<string> sequence2; // 2-Sequence
    initSequence2(sequence1, sequence2, supportCount);
    freqSequences.push_back(sequence2);

    while (true) { // n-Sequence
        vector<string> prevSeq = freqSequences.back();
        vector<string> newSeq;
        generateSequence(prevSeq, newSeq);
        if (newSeq.size() < 1) {
            break;
        }
        calculateSupport(newSeq, supportCount);
        supportPruning(newSeq, supportCount);
        freqSequences.push_back(newSeq);
    }
    int i = 0;
    for (auto j : freqSequences) {
        i++;
        cout << i << "-Sequence of Size " << j.size() << ":" << endl;
        for (auto k : j) {
            cout << k << " " << supportCount[k] << endl;
        }
        cout << endl;
    }
    system("pause");
    return 0;
}