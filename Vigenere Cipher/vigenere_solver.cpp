#include <bits/stdc++.h>
using namespace std;

const double EN_FREQ[26] = {
    0.082, 0.015, 0.028, 0.043, 0.127, 0.022, 0.020,
    0.061, 0.070, 0.002, 0.008, 0.040, 0.024, 0.067,
    0.075, 0.019, 0.001, 0.060, 0.063, 0.091, 0.028,
    0.010, 0.023, 0.001, 0.020, 0.001
};

string read_cipher(const string &path) {
    ifstream f(path);
    if (!f) {
        cerr << "Failed to open " << path << "\n";
        exit(1);
    }
    string s, all;
    while (getline(f, s)) {
        for (char c: s) 
            if (isalpha((unsigned char)c)) all.push_back(tolower(c));
    }
    return all;
}

double index_of_coincidence(const string &s) {
    int n = s.size();
    if (n <= 1) return 0.0;
    vector<int> cnt(26,0);
    for (char c: s) 
        cnt[c-'a']++;
    double sum = 0.0;
    for (int i=0;i<26;i++) 
        sum += cnt[i]*(cnt[i]-1);
    return sum / (double)(n*(n-1));
}

vector<double> avg_ic_for_keylen(const string &ct, int keylen) {
    vector<string> groups(keylen);
    for (size_t i=0;i<ct.size();i++) 
        groups[i%keylen].push_back(ct[i]);
    vector<double> ics;
    for (int i=0;i<keylen;i++) 
        ics.push_back(index_of_coincidence(groups[i]));
    return ics;
}

double average(const vector<double>&v){ 
    if(v.empty()) return 0.0; 
    double s=0; 
    for(double x:v) 
        s+=x; 
    return s/v.size(); 
}

string decipher_with_key(const string &ct, const string &key){
    string pt; pt.reserve(ct.size());
    int klen = key.size();
    for (size_t i=0;i<ct.size();i++){
        int kc = key[i%klen]-'a';
        int pc = (ct[i]-'a' - kc + 26) % 26;
        pt.push_back('a' + pc);
    }
    return pt;
}

string reduce_key(const string &key){
    int len = key.size();
    for (int d=1; d<=len/2; d++){
        if (len % d == 0){
            bool isRepeat = true;
            for (int i=d; i<len; i++){
                if (key[i] != key[i%d]){
                    isRepeat = false;
                    break;
                }
            }
            if (isRepeat) return key.substr(0, d);
        }
    }
    return key;
}

string find_key_by_freq(const string &ct, int keylen){
    string key; key.resize(keylen);
    vector<string> groups(keylen);
    for (size_t i=0;i<ct.size();i++) 
        groups[i%keylen].push_back(ct[i]);

    for (int i=0;i<keylen;i++){
        const string &g = groups[i];
        int n = g.size();
        if (n==0) { key[i]='a'; continue; }
        vector<int> cnt(26,0);
        for (char c: g) cnt[c-'a']++;

        double bestScore = 1e100; int bestShift = 0;
        for (int shift=0; shift<26; shift++){
            double chi2 = 0.0;
            for (int l=0;l<26;l++){
                int shifted = (l + shift) % 26;
                double observed = cnt[shifted];
                double expected = EN_FREQ[l] * n;
                double diff = observed - expected;
                if (expected > 0) chi2 += diff*diff/expected;
            }
            if (chi2 < bestScore){ bestScore = chi2; bestShift = shift; }
        }
        key[i] = 'a' + bestShift;
    }
    return reduce_key(key);
}

int main(int argc, char** argv){
    string infile = "cipher.txt";
    if (argc >= 2) infile = argv[1];

    string ct = read_cipher(infile);
    if (ct.empty()){
        cerr << "Ciphertext is empty or contains no letters.\n";
        return 1;
    }

    int maxlen = 40;
    int maxTry = min((int)ct.size(), maxlen);
    vector<pair<double,int>> scores;
    for (int L=1; L<=maxTry; L++){
        auto ics = avg_ic_for_keylen(ct, L);
        double avg = average(ics);
        scores.push_back({avg, L});
    }
    sort(scores.begin(), scores.end(), greater<>());

    int tryCount = min((int)scores.size(), 6);
    string bestPlain;
    string bestKey;
    double bestEnglishness = 1e100;

    for (int t=0;t<tryCount;t++){
        int L = scores[t].second;
        string key = find_key_by_freq(ct, L);
        string pt = decipher_with_key(ct, key);

        vector<int> cnt(26,0);
        for (char c: pt) cnt[c-'a']++;
        int n = pt.size();
        double chi2 = 0.0;
        for (int i=0;i<26;i++){
            double expected = EN_FREQ[i] * n;
            double diff = cnt[i] - expected;
            if (expected>0) chi2 += diff*diff/expected;
        }
        if (chi2 < bestEnglishness){ bestEnglishness = chi2; bestPlain = pt; bestKey = key; }
    }

    ofstream outf("plain.txt");
    if (!outf) {
        cerr << "Failed to open plain.txt for writing" << endl;
    } else {
        outf << bestPlain;
        outf.close();
    }

    cout << "Guessed key: " << bestKey << "\n";
    cout << "Plaintext written to plain.txt\n";
    return 0;
}
