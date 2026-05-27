# Substitution Cipher Decryption

### Step 1: Frequency Analysis

The foundation of breaking substitution cipher is **frequency analysis** - exploiting the fact that letters appear with different frequencies in natural language.

1. Count the frequency of each character in the ciphertext
2. Compare with expected English frequencies
3. Make initial guesses: most frequent cipher character likely represents space ' ', second likely 'e', etc.

`frequency_analysis.cpp` analyzes the ciphertext and outputs character frequencies to `frequency_analysis.txt`. According to it, the most frequent characters in the ciphertext are:

'+' : 16.74%   ---->    likely ' ' (space)

'2' : 10.99%   ---->    likely 'e'

**Usage:**
```bash
g++ -O3 frequency_analysis.cpp -o frequency_analysis
./frequency_analysis [input_file]
```

### Step 2: Pattern Recognition

After substituting the two most frequent characters, I looked for big size words which had multiple e's. After many hit and trials, I found a ciphertext word **\U232. 32** (at line 817 of `output.txt`) whose plaintext at that point was **--e-e---e** (on line 818 of `output.txt`). This 9 lettered word with 3e's also needed to have 4th and 8th letters same. The words which matched this pattern were **"therefore"**, **"wherefore"** and **"preterite"**, of which **"therefore"** is the most common. Using this, I got mappings for 

'\\'->'t', 'U'->'h', '3'->'r', '.'->'f' and  ' '->'o' (new mappings)
 
'+' -> ' ', '2' -> 'e' (earlier ones)

(all others) -> '~' (as '~' was not used in ciphertext)

### Step 3: Iterative Refinement

This process of guessing mappings, applying them, and looking for more patterns was repeatedly done using `substitution_solver.cpp`. The above mappings caused many words to become readable like "the", "there", "of", "for", "to", etc. This gave me a strong confidence on the existing mappings. 

Many more words were partially readable, and I could guess more mappings. For example, "were" gave 'x'->'w', "that" gave ':'->'a' and so on. Period (.), comma (,), 's' and some capital letters was discovered by line endings or plurals or list of items or sentence endings. After this I found the website from which the ciphertext was generated - [Secret sharing](https://en.wikipedia.org/wiki/Secret_sharing). This helped decode numbers and some other punctuations.

## Running Instructions 

#### Interactive Substitution Solver (`substitution_solver.cpp`)

- Add character mappings: `add <cipher_char> <plain_char>`
- Remove mappings: `remove <cipher_char>`
- View current mappings: `show`
- Generate output file: `generate` or `gen`
- Quit program: `quit` or `exit`
- Shows cipher and partial plaintext line-by-line in `output.txt`, Each line is prefixed with `C:` for ciphertext and `P:` for plaintext. Also, each line is constrained to 100 characters for readability. Unmapped characters in plaintext are shown as tildes `~` (as this was not used in ciphertext).

**Usage:**
```bash
g++ -O3 substitution_solver.cpp -o substitution_solver
./substitution_solver [cipher_file]
```

**Interactive Commands:**
- `add 2 e` - Map cipher character '2' to plaintext 'e'
- `remove 2` - Remove mapping for character '2'
- `show` - Display all current mappings
- `generate` - Create `output.txt` with cipher/plaintext comparison

## Files Structure

- `answer.txt`: Contains the decrypted random line from the ciphertext
- `cipher.txt`: Input ciphertext file to be decrypted
- `frequency_analysis.cpp`: C++ source code for frequency analysis
- `frequency_analysis.txt`: Output file containing character frequencies
- `substitution_solver.cpp`: C++ source code for interactive substitution solver
- `output.txt`: Output file showing ciphertext and fully decrypted plaintext
- `map.txt`: Final character mapping used for decryption
- `order.txt`: A log of the order in which characters were decrypted

