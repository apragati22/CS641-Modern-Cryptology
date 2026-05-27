#include <bits/stdc++.h>
#include <openssl/sha.h>
using namespace std;

// Compile with -lcrypto

#define EMAIL "apragati22@iitk.ac.in"
#define SEARCH_LIMIT 100000000
#define NUM_TRIALS 50
#define DESIRED_MATCH 16
#define COL_DESIRED_MATCH 32

void compute_sha256(const string& str, unsigned char* hash) {
    SHA256(reinterpret_cast<const unsigned char*>(str.c_str()), str.length(), hash);
}

void print_hash(const unsigned char* hash) {
    for (int i=0; i<SHA256_DIGEST_LENGTH; i++) {
        cout << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    cout << dec << endl;
}

int count_matching_bits(const unsigned char* h1, const unsigned char* h2) {
    int match_count = 0;
    for (int i=0; i<SHA256_DIGEST_LENGTH; i++) {
        unsigned int xor_val = (int)h1[i] ^ (int)h2[i];
        if(xor_val == 0) {
            match_count += 8;
            continue;
        }
        else{
            for(int j=0; j<8; j++) {
                if(((xor_val >> (7-j)) & 1) == 0) 
                    match_count++;
                else 
                    return match_count;
            }
        }
    }
    return match_count;
}

int main() {

    string my_email = EMAIL;
    unsigned char h1[SHA256_DIGEST_LENGTH];
    compute_sha256(my_email, h1);

    cout << "\n================ Part a ========================================" << endl;
    cout << "   Hash of " << my_email << " (h1): "<<endl;
    print_hash(h1);

    cout << "\n================ Part b ========================================" << endl;
    long long counter = 0;
    string matched_email;
    unsigned char h2[SHA256_DIGEST_LENGTH];
    int max_match = 0;
    while (counter < SEARCH_LIMIT) {
        matched_email = "random" + to_string(counter) + "@iitk.ac.in";
        compute_sha256(matched_email, h2);
        
        int current_match = count_matching_bits(h1, h2);
        if (current_match>=16 && current_match > max_match) {
            max_match = current_match;
            cout << "    Found a " << max_match << "-bit match in "<< counter <<" trials" << endl;
            cout << "    Matching email: " << matched_email << endl;
            cout << "    Hash (h2): ";
            print_hash(h2);
            cout << "    --------------------------" << endl;
        }
        counter++;
    }
    cout << " Searched " << SEARCH_LIMIT<< " hashes. Longest match found was " << max_match << " bits." << endl;
    
    cout << "\n================ Part c ========================================" << endl;
    long long total_calls = 0;
    long long successful_trials = 0;
    for (int i=0; i<NUM_TRIALS; i++) {
        counter = 0;
        string candidate_email;
        unsigned char h3[SHA256_DIGEST_LENGTH];
        while (counter < SEARCH_LIMIT) {
            candidate_email = "test" + to_string(counter) + "@iitk.ac.in";
            compute_sha256(candidate_email, h3);
            counter++;
            if (count_matching_bits(h1, h3) >= DESIRED_MATCH) {
                total_calls += counter;
                successful_trials++;
                break;
            }
        }
    }
    double average_t = (double)total_calls / successful_trials;
    double log2_t = log2(average_t);
    cout << "    Average calls (t) for " << DESIRED_MATCH << "-bit match = " << average_t << endl;
    cout << "    log2(t) = " << log2_t << endl;

    cout << "\n================ Part d ========================================" << endl;
    cout << "    The log2(t) is close to " << DESIRED_MATCH << ". This is expected because finding a specific " << DESIRED_MATCH << "-bit match" << endl;
    cout << "    is equivalent to a partial preimage search. The probability of randomly matching " << DESIRED_MATCH << " bits" << endl;
    cout << "    is 1/2^" << DESIRED_MATCH << ". Therefore, it requires roughly 2^" << DESIRED_MATCH << " average trials." << endl;

    cout << "\n================ Part e ========================================" << endl;

    unordered_map<uint64_t, string> seen_hashes;
    unordered_map<uint64_t, string> seen_hashes_fine; // another map for search between 8-bit lengths
    unsigned char h4[SHA256_DIGEST_LENGTH];
    int bytes = 2; // start from 16 bits (2 bytes)

    while(bytes < SHA256_DIGEST_LENGTH){
        int trials = 0;
        seen_hashes.clear();
        bool found = false;

        while(trials < SEARCH_LIMIT){
            string candidate = "random" + to_string(trials) + "@iitk.ac.in";
            compute_sha256(candidate, h4);
            uint64_t prefix = 0;
            for(int i = 0; i < bytes; i++){
                prefix = (prefix << 8) | h4[i];
            }
            if(seen_hashes.find(prefix) != seen_hashes.end()){
                cout << "    Found a " << 8*bytes << "-bit collision in " << trials << " trials" << endl;
                cout << "    Email 1: " << seen_hashes[prefix] << endl;
                // cout<< "Hash of Email 1: ";
                // print_hash(h4);
                cout << "    Email 2: " << candidate << endl;
                // cout<< "Hash of Email 2: ";
                // compute_sha256(seen_hashes[prefix], h4);
                // print_hash(h4);
                cout << "    --------------------------" << endl;
                found = true;
                break;
            } else {
                seen_hashes[prefix] = candidate;
            }
            trials++;
        }

        // FINE SEARCH BETWEEN 8-BIT LENGTHS (UNCOMMENT TO RUN, BUT IT TAKES TIME)
        // if(!found){
        //     int found_bits = 8*(bytes-1);
        //     for(int target_bits = found_bits + 1; target_bits < 8 * bytes; target_bits++){
        //         seen_hashes_fine.clear();
        //         bool fine_found = false;
        //         int fine_trials = 0;

        //         while(fine_trials < SEARCH_LIMIT){
        //             string candidate = "random" + to_string(fine_trials) + "@iitk.ac.in";
        //             compute_sha256(candidate, h4);
        //             uint64_t prefix = 0;
        //             int full_bytes = target_bits / 8;
        //             int extra_bits = target_bits % 8;

        //             for(int i = 0; i < full_bytes; i++){
        //                 prefix = (prefix << 8) | h4[i];
        //             }
        //             if(extra_bits > 0){
        //                 prefix = (prefix << extra_bits) | (h4[full_bytes] >> (8 - extra_bits));
        //             }

        //             if(seen_hashes_fine.find(prefix) != seen_hashes_fine.end()){
        //                 fine_found = true;
        //                 found_bits = target_bits;
        //                 cout << "    Found a " << target_bits << "-bit collision in "
        //                     << fine_trials << " trials" << endl;
        //                 cout << "    Email 1: " << seen_hashes_fine[prefix] << endl;
        //                 cout << "    Email 2: " << candidate << endl;
        //                 cout << "    --------------------------" << endl;
        //                 break;
        //             } else {
        //                 seen_hashes_fine[prefix] = candidate;
        //             }
        //             fine_trials++;
        //         }

        //         if(!fine_found){
        //             cout << "    No collision found at " << target_bits << " bits." << endl;
        //             break; 
        //         }
        //     }

        //     cout << "  Longest collision found: " << found_bits << " bits." << endl;
        //     break;
        // }

        if (!found) {
            cout << "  Longest collision found: " << 8*(bytes-1) << " bits." << endl;
            break;
        }
        bytes++;
        counter++;
    }

    cout << "\n================ Part f ========================================" << endl;
    unsigned char h5[SHA256_DIGEST_LENGTH];
    total_calls = 0;
    successful_trials = 0;

    for(int i=0; i<NUM_TRIALS; i++){
        int trials = 0;
        seen_hashes.clear();

        while(trials < SEARCH_LIMIT){
            string candidate = "random" + to_string(trials) + "@iitk.ac.in";
            compute_sha256(candidate, h5);
            uint64_t prefix = 0;
            for(int i = 0; i < COL_DESIRED_MATCH/8; i++){
                prefix = (prefix << 8) | h5[i];
            }
            if(seen_hashes.find(prefix) != seen_hashes.end()){
                total_calls += trials;
                successful_trials++;
                break;
            } else {
                seen_hashes[prefix] = candidate;
            }
            trials++;
        }
    }

    average_t = (double)total_calls / successful_trials;
    log2_t = log2(average_t);
    cout << "    Average calls to find " << COL_DESIRED_MATCH <<"-bit collision is (t) = " << average_t << endl;
    cout << "    log2(t) = " << log2_t << endl;

    cout << "\n================ Part g ========================================" << endl;
    cout << "    The log2(t) is close to " << log2_t << ". This happens because of the Birthday Paradox." << endl;
    cout << "    To find any two hashes that collide in a space of size 2^" << COL_DESIRED_MATCH << ", we only need to test" << endl;
    cout << "    roughly sqrt(2^" << COL_DESIRED_MATCH << ") = 2^" << COL_DESIRED_MATCH/2 << " candidates on average before a collision occurs." << endl;

    cout <<" Collision search is much faster than preimage search because of this paradox. For a 32-bit collision, we only need around 2^16 = 65,000 checks on average, while for a 16-bit preimage match we needed around 2^16 = 65,000 checks. This illustrates how the complexity of finding collisions grows much slower than finding specific matches." << endl;
    return 0;
}


