UDP server working on port `2020` that checks how many words in given input are palindromes and return the result in `x/y` format

# Compilation and run
To compile run `make` and to run server type `make start`

# Usage
To send request use:

ncat -v -u 127.0.0.1 2020 <`some words to check`>