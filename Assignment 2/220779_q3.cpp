#include <bits/stdc++.h>
#include <gmp.h>
#include <gmpxx.h>

// Compile with -lgmp -lgmpxx

using namespace std;

// Initialize global random engine
gmp_randclass rnd(gmp_randinit_default);

// modular exponentiation (res = base^exp % mod)
mpz_class mod_exp(const mpz_class& base, const mpz_class& exp, const mpz_class& mod) {
    mpz_class res;
    mpz_powm(res.get_mpz_t(), base.get_mpz_t(), exp.get_mpz_t(), mod.get_mpz_t());
    return res;
}

// The Miller-Rabin primality test.
bool miller_rabin(const mpz_class& n, int k = 40) {
    // Base cases
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0) return false;

    // d and r such that n-1 = d*(2^r)
    mpz_class d = n-1;
    int r = 0;
    while (d%2 == 0) {
        d /= 2;
        r++;
    }

    for (int i=0; i<k; i++) {
        // Pick a random integer 'a' in the range [2, n-2]
        mpz_class a = rnd.get_z_range(n-3) + 2; 
        mpz_class x = mod_exp(a, d, n); // x = a^d mod n

        if (x == 1 || x == n-1) continue;

        bool composite = true;
        for (int j=1; j<r; j++) {
            x = mod_exp(x, 2, n);
            if (x == n-1) {
                composite = false;
                break;
            }
        }

        if (composite) return false;
    }
    return true; // Probably prime
}

// Generate random prime of size bits.
mpz_class generate_prime(int bits = 512) {
    mpz_class p;
    while (true) {
        p = rnd.get_z_bits(bits);
        // Make the number to be exactly 'bits' long and odd
        mpz_setbit(p.get_mpz_t(), bits-1); // Set most significant bit
        mpz_setbit(p.get_mpz_t(), 0);        // Set least significant bit

        if (miller_rabin(p, 40)) return p;
    }
}

int main() {
    // Seed the random number generator
    rnd.seed(time(NULL));

    cout << "\n============= Question 3: Diffie-Hellman Key Exchange ================" << endl;

    // Part (b) Generate a random Sophie-Germain prime p of size 512 bits.
    cout << "\n[Part b] Generating a 512-bit Sophie-Germain prime p..." << endl;
    mpz_class p_prime, p;
    
    // Since p = 2 * p_prime + 1, p_prime should be exactly 511 bits long.
    while (true) {
        p_prime = generate_prime(511);
        p = 2*p_prime+1;        
        if (miller_rabin(p, 40)) break;
    }

    cout << "Sophie-Germain prime p (512 bits):\n" << p.get_str(16) << endl << endl;
    cout << "Value of p' (where p = 2p' + 1):\n" << p_prime.get_str(16) << endl;

    // Part (c) Find a generator of the group Z_p*
    cout << "\n\n[Part c] Finding a generator g of Z_p*..." << endl;
    mpz_class g;
    
    while (true) {
        g = rnd.get_z_range(p-3) + 2;

        // Since p is a Sophie-Germain prime, the order of the multiplicative group Z_p* is p-1 = 2*p_prime.
        // Therefore, the prime factors of the order (p-1) are only 2 and p_prime.
        // To verify g is a generator, we just need to ensure g^2 != 1 and g^p_prime != 1.
        
        mpz_class test1 = mod_exp(g, 2, p);
        mpz_class test2 = mod_exp(g, p_prime, p);

        if (test1 != 1 && test2 != 1) break; 
    }

    cout << "Found generator g:\n" << g.get_str(16) << endl;

    // Part (d) Implement Diffie-Hellman Key Exchange
    cout << "\n\n[Part d] Simulating Diffie-Hellman Key Exchange..." << endl;

    cout << "Alice and Bob agree on public prime p and generator g.\n" << endl;

    // 1. Generate private keys (random numbers less than p-1)
    mpz_class private_a = rnd.get_z_range(p-2)+1;
    mpz_class private_b = rnd.get_z_range(p-2)+1;

    // 2. Compute public keys: A = g^a mod p, B = g^b mod p
    mpz_class public_A = mod_exp(g, private_a, p);
    mpz_class public_B = mod_exp(g, private_b, p);

    cout << "Alice's private key 'a' (secret):\n" << private_a.get_str(16) << endl;
    cout << "Bob's private key 'b' (secret):\n" << private_b.get_str(16) << endl;
    
    cout << "Alice's public key A = g^a mod p (sent to Bob):\n" << public_A.get_str(16) << endl;
    cout << "Bob's public key B = g^b mod p (sent to Alice):\n" << public_B.get_str(16) << endl;

    // 3. Compute shared secrets: S_A = B^a mod p, S_B = A^b mod p
    mpz_class shared_secret_Alice = mod_exp(public_B, private_a, p);
    mpz_class shared_secret_Bob = mod_exp(public_A, private_b, p);

    cout << "Alice computes shared secret (B^a mod p):\n" << shared_secret_Alice.get_str(16) << endl;
    cout << "Bob computes shared secret (A^b mod p):\n" << shared_secret_Bob.get_str(16) << endl;

    if (shared_secret_Alice == shared_secret_Bob) 
        cout << "\nSuccess! Both parties computed the same common secret." << endl;
    else 
        cout << "\nFailure! Secrets do not match." << endl;

    return 0;
}
