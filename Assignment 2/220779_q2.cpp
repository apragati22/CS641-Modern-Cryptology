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
    if (n%2 == 0) return false;

    // d and r such that n-1 = d*(2^r)
    mpz_class d = n-1;
    int r = 0;
    while (d%2 == 0) {
        d /= 2;
        r++;
    }

    for (int i=0; i<k; i++) {
        // Pick a random integer 'a' in the range [2, n-2]
        mpz_class a = rnd.get_z_range(n-3)+2; 
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

// Generate random primes of size bits.
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

// Extended Euclidean Algorithm (EEA)
// Returns gcd(a,b) and finds x, y such that a*x + b*y = gcd(a,b)
mpz_class extended_gcd(mpz_class a, mpz_class b, mpz_class& x, mpz_class& y) {
    mpz_class x0 = 1, y0 = 0;
    mpz_class x1 = 0, y1 = 1;

    while (b != 0) {
        mpz_class q = a / b;
        mpz_class rem = a % b;
        
        a = b;
        b = rem;

        mpz_class x_temp = x1;
        x1 = x0 - q * x1;
        x0 = x_temp;

        mpz_class y_temp = y1;
        y1 = y0 - q * y1;
        y0 = y_temp;
    }
    
    x = x0;
    y = y0;
    return a;
}

int main() {
    // Seed our random number generator
    rnd.seed(time(NULL));

    cout << "\n============= Question 2: RSA Implementation ================" << endl;
    
    // Part (b) Use part(a) to generate random primes p and q (512 bits)
    cout << "\n[Part a & b] Generating 512-bit prime p and q..." << endl;
    mpz_class p = generate_prime(512);
    cout << "p = " << p.get_str(16) << endl;

    mpz_class q = generate_prime(512);
    while (p == q) {
        q = generate_prime(512);
    }
    cout << "q = " << q.get_str(16) << endl;

    // Part (d) Set n = p*q and compute d = e^{-1} mod phi(n) using EEA
    mpz_class n = p * q;
    mpz_class phi_n = (p - 1) * (q - 1);
    
    mpz_class e = 65537; // Standard public exponent used commonly in RSA
    
    cout << "\n[Part d] Setting n = p*q and computing d = e^-1 mod phi(n)..." << endl;
    mpz_class x, y;
    mpz_class gcd = extended_gcd(e, phi_n, x, y);
    
    if (gcd != 1) {
        cout << "Error: e and phi(n) are not coprime. Run again." << endl;
        return 1;
    }
    
    // Ensure the private key d is positive
    mpz_class d = x;
    if (d < 0) d += phi_n;

    cout << "n = " << n.get_str(16) << endl;
    cout << "e = " << e.get_str(10) << endl;
    cout << "d = " << d.get_str(16) << endl;

    // Part (e) Generate a random message of 1024 bits, encrypt, decrypt, verify
    cout << "\n[Part e] Textbook RSA Encryption & Decryption..." << endl;
    
    // Generating 1024-bit message less than n
    mpz_class m = rnd.get_z_bits(1024);
    while (m >= n) {
        m = rnd.get_z_bits(1024);
    }

    cout << "Original Message (m):\n" << m.get_str(16) << endl;

    // Encryption: c = m^e mod n
    mpz_class c = mod_exp(m, e, n);
    cout << "\nCiphertext (c = m^e mod n):\n" << c.get_str(16) << endl;

    // Decryption: m_decrypted = c^d mod n
    mpz_class m_decrypted = mod_exp(c, d, n);
    cout << "\nDecrypted Message (m' = c^d mod n):\n" << m_decrypted.get_str(16) << endl;

    if (m == m_decrypted) 
        cout << "\nSuccess! The decrypted message matches the original message." << endl;
    else 
        cout << "\nFailure! The decrypted message differs." << endl;

    return 0;
}
