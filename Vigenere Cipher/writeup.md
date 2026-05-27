# Vigenere Cipher Decryption

### Step 1: Key Length Detection

The attack relies on known English letter frequency distribution. The frequencies used were sourced from Wikipedia ([Letter frequency](https://en.wikipedia.org/wiki/Letter_frequency)):

```
A: 8.2%   B: 1.5%   C: 2.8%   D: 4.3%   E: 12.7%  F: 2.2%
G: 2.0%   H: 6.1%   I: 7.0%   J: 0.2%   K: 0.8%   L: 4.0%
M: 2.4%   N: 6.7%   O: 7.5%   P: 1.9%   Q: 0.1%   R: 6.0%
S: 6.3%   T: 9.1%   U: 2.8%   V: 1.0%   W: 2.3%   X: 0.1%
Y: 2.0%   Z: 0.1%
```

1. **Index of Coincidence**: This measures how likely two randomly selected letters from a text are to be the same. For English text, IC ≈ 0.065-0.068.

2. **Testing Key Lengths**: For each potential key length L (from 1 to 40):
   - Split the ciphertext into L groups (every L-th character)
   - Calculate the IC for each group
   - Average the IC values

 When we guess the correct key length, each group represents text encrypted with a single shift cipher. These groups will have IC values closer to English text (~0.065). Wrong key lengths produce more random-looking groups with IC closer to 0.038.

### Step 2: Frequency Analysis for Each Key Position

Once we have candidate key lengths, we recover each letter of the key independently:

1. **Group by Position**: Split ciphertext into groups based on the key length
2. **Frequency Analysis**: For each group:
   - Count the frequency of each letter
   - Try all 26 possible shift values (0-25)
   - For each shift, calculate the chi-squared statistic comparing the shifted frequencies to expected English letter frequencies
   - The shift with the lowest chi-squared value is most likely correct

**Chi-Squared Test**:
```
χ² = Σ [(observed - expected)² / expected]
```

Lower chi-squared values indicate better fit to English frequency distribution.

### Step 3: Decryption and Validation

1. **Generate Plaintext**: Try the top 6 candidate key lengths (based on IC scores)
2. **Evaluate Results**: For each candidate:
   - Recover the full key using frequency analysis
   - Decrypt the entire ciphertext
   - Calculate overall chi-squared for the plaintext
3. **Select Best Result**: Choose the plaintext with the lowest chi-squared value (best match to English)

## Running Instructions:

```
g++ -O3 vigenere_solver.cpp -o vigenere_solver
./vigenere_solver <ciphertext_file>
```
The output plaintext will be printed to ``plain.txt``.

## Key Used
The key used for encryption was **bdwmal**.

## File Structure
- `answer.txt` : Contains the last 10 characters of the decrypted plaintext 
- `vigenere_solver.cpp`: Main C++ source code implementing the attack
- `ciphertext.txt`: Input ciphertext file to be decrypted
- `plain.txt`: Output file containing the decrypted plaintext
