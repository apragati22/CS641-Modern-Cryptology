#include <bits/stdc++.h>
using namespace std;

int main(int argc, char** argv) {
    string infile = "cipher.txt";
    if (argc >= 2) infile = argv[1];
    ifstream fin(infile);
    if (!fin) {
        cerr << "Failed to open " << infile << "\n";
        return 1;
    }
    
    string content((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
    fin.close();
    
    ofstream fout("frequency_analysis.txt");
    if (!fout) {
        cerr << "Failed to open frequency_analysis.txt for writing\n";
        return 1;
    }
    
    map<char, int> freq;
    for (char ch : content) {
        if (isalpha((unsigned char)ch)) {
            char lower = tolower((unsigned char)ch);
            freq[lower]++;
        } else {
            freq[ch]++;
        }
    }
    
    vector<pair<int, char>> sorted_freq;
    for (auto& p : freq) {
        sorted_freq.push_back({p.second, p.first});
    }
    sort(sorted_freq.begin(), sorted_freq.end(), greater<pair<int, char>>());

    fout << "Character frequencies in " << infile << " (descending order):\n";
    fout << "Character\tCount\tPercentage\n";
    fout << "=========\t=====\t==========\n";
    
    int total = 0;
    for (auto& p : sorted_freq) total += p.first;
    
    for (auto& p : sorted_freq) {
        double percentage = (double)p.first / total * 100.0;
        fout << p.second << "\t\t" << p.first << "\t" 
             << fixed << setprecision(2) << percentage << "%\n";
    }
    
    fout.close();
    cout << "Frequency analysis written to frequency_analysis.txt\n";
    
    return 0;
}