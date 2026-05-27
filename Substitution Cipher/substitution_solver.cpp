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
    
    // Initially all characters map to '~'
    map<char, char> mapping;

    cout << "Interactive Substitution Tool\n";
    cout << "=============================\n";
    cout << "Commands:\n";
    cout << "  add <cipher_char> <plain_char>  - Add/update substitution\n";
    cout << "  remove <cipher_char>            - Remove substitution (reset to ~)\n";
    cout << "  show                            - Show current mappings\n";
    cout << "  generate                        - Generate interface.txt\n";
    cout << "  quit                            - Exit\n\n";
    
    string command;
    while (true) {
        cout << "> ";
        cin >> command;
        
        if (command == "quit" || command == "q") {
            break;
        }
        else if (command == "add") {
            string line;
            getline(cin, line);
            if (line.length() != 4) {
                cout << "Error: format is 'add <char1> <char2>'\n";
                continue;
            }
            // Skip leading space, then take next char, skip one space, take next char
            int pos = 1;
            char cipher_char = line[pos];
            char plain_char = line[pos+2];
            mapping[cipher_char] = plain_char;
            cout << "Added: '" << cipher_char << "' -> '" << plain_char << "'\n";
        }
        else if (command == "remove") {
            string line;
            getline(cin, line);
            int pos = 1;
            if (line.length() != 2) {
                cout << "Error: format is 'remove <char>'\n";
                continue;
            }
            char cipher_char = line[pos];
            mapping.erase(cipher_char);
            cout << "Removed mapping for '" << cipher_char << "'\n";
        }
        else if (command == "show") {
            cout << "\nCurrent mappings:\n";
            for (auto& p : mapping) {
                cout << "  '" << p.first << "' -> '" << p.second << "'\n";
            }
            cout << "  (all others) -> '~'\n\n";
        }
        else if (command == "generate" || command == "gen") {
            string plaintext = content;
            for (char& ch : plaintext) {
                if (ch == '\n') {
                    ch = '\n';
                } else if (mapping.count(ch)) {
                    ch = mapping[ch];
                } else {
                    ch = '~';
                }
            }

            ofstream fout("output.txt");
            if (!fout) {
                cerr << "Failed to open output.txt for writing\n";
                continue;
            }
            
            int pos = 0;
            while (pos < content.length()) {
                // Find next 100 characters or until newline
                int line_end = pos;
                int char_count = 0;
                
                while (line_end < content.length() && char_count < 100) {
                    if (content[line_end] == '\n') {
                        line_end++;
                        break;
                    }
                    line_end++;
                    char_count++;
                }
                
                // Write cipher line
                fout << "C: ";
                for (int i = pos; i < line_end; i++) {
                    fout << content[i];
                }
                if (line_end < content.length() && content[line_end - 1] != '\n') {
                    fout << "\n";
                }
                
                // Write plain line
                fout << "P: ";
                for (int i = pos; i < line_end; i++) {
                    fout << plaintext[i];
                }
                if (line_end < content.length() && plaintext[line_end - 1] != '\n') {
                    fout << "\n";
                }
                
                fout << "\n";
                
                pos = line_end;
            }
            
            fout.close();
            cout << "Generated output.txt\n";
        }
        else {
            cout << "Unknown command. Try: add, remove, show, generate, quit\n";
        }
    }
    
    return 0;
}
