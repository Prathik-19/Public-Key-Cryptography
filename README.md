# Public Key Cryptography 

This program is an implementation of the ss algorithm that is used to encrypt and decrypt the cyphertext given the Public Modulus(n) and Private exponent(e).

## File Description

decrypt.c - This file contains the code to run the decrypt functions from ss.c file and parses the command line options and store the decrypted text in the outfile.

encrypt.c - This file contains the code to run the encrypt functions from ss.c file and parses the command line options and store the encrypt text in the outfile.

keygen.c - This file contains the code for generating public and private keys using functions from ss.c file and parses the command line options.

numtheory.c - This file contains the code for implementing functions like gcd, power mod, mod inverse, is prime and make prime.

numtheory.h - This is a header file that contains function declarations from the numtheory.c file.

randstate.c - This file contains the code for generating random numbers given the seed. 

randstate.h - This is a header file that contains function declarations from randstate.c file.

ss.c - This file contains the code for ss functions that generates public and private keys, reads and writes them into a new file, and encrypts and decrypts files.

ss.h - This is a header file that contains function declarations from ss.c file.

Makefile - This file is responsible for automating and does the compilation process for all the .c files. 

DESIGN.pdf - This is the design document for the assignment, it contains pseudocode and design process of the .c files and generates the output files(./keygen, ./encrypt, ./decrypt).

WRITEUP.pdf - This is the writeup document for the assignment, it contains the graphs and discussion to the questions posed on the dreidel files.

## Building

No build permissions needed for the files in the directory.

## Running
To compile the c files, use the following command that generates the output files(./keygen, ./encrypt, ./decrypt).
```
make
```
After compiling, you can use the following commands to generate output.
```
./keygen -b 		To specify the number of bits (Default: 1024) you want the numbers to be. Range: 50-4096
./keygen -i 		To specify the number of iterations (Default: 50) you want.
./keygen -n		    To specify the public key file (Default: ss.pub).
./keygen -d		    To specify the private key file (Default: ss.priv).
./keygen -s 		To specify the seed for random state (Default: current time).
./keygen -v		    Enables the verbose output.
./keygen -h 		Displays the program usage and synopsis.

./encrypt -i 		To specify the input file to encrypt (Default: stdin).
./encrypt -o 		To specify the output file to encrypt (Default: stdout).
./encrypt -n 		To specify the public key file (Default: ss.pub).
./encrypt -v 		Enables the verbose output
./encrypt -h 		Displays the program usage and synopsis.

./decrypt -i 		To specify the input file to decrypt (Default: stdin).
./decrypt -o 		To specify the output file to decrypt (Default: stdout).
./decrypt -n 		To specify the private key file (Default: ss.priv).
./decrypt -v 		Enables the verbose output.
./decrypt -h		Displays the program usage and synopsis.
```

