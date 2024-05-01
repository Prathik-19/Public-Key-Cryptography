#include <stdio.h>

#include "numtheory.h"
#include "randstate.h"
#include "ss.h"

#include <getopt.h>
#include <gmp.h>
#include <stdio.h> // Including standard libraries and gmp library
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define OPTIONS "i:o:n:vh"

void v(mpz_t pq, mpz_t d) {
    gmp_fprintf(stderr, "pq  (%zu bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq);
    gmp_fprintf(stderr, "d  (%zu bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
}

void usage(void) {
    fprintf(stderr, "SYNOPSIS\n");
    fprintf(stderr, "   Decrypts data using SS decryption.\n");
    fprintf(stderr, "   Encrypted data is decrypted by the decrypt program.\n\n");
    fprintf(stderr, "USAGE\n");
    fprintf(stderr, "   ./decrypt [OPTIONS]\n\n");
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "   -h              Display program help and usage.\n");
    fprintf(stderr, "   -v              Display verbose program output.\n");
    fprintf(stderr, "   -i in_file       Input file of data to encrypt (default: stdin).\n");
    fprintf(stderr, "   -o out_file      Output file for encrypted data (default: stdout).\n");
    fprintf(stderr, "   -n pbfile       Public key file (default: ss.pub).\n");
}

int main(int argc, char **argv) {
    int opt = 0; // variable for getting option form array
    char *private_key_file = "ss.priv"; // public key name

    FILE *in_file = stdin;
    FILE *out_file = stdout;
    FILE *private_key = NULL;
    bool verbose = false;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'o':
            out_file = fopen(optarg, "w");
            if (!out_file) {
                fprintf(stderr, "Error opening the file %s or it is not present in the directory\n",
                    optarg);
                fclose(in_file);
                fclose(out_file);
                return EXIT_FAILURE;
            }

            break;
        case 'i':
            in_file = fopen(optarg, "r");
            if (!in_file) {
                fprintf(stderr, "Error opening the file %s or it is not present in the directory\n",
                    optarg);
                fclose(in_file);
                return EXIT_FAILURE;
            }
            break;
        case 'v': verbose = true; break;
        case 'n': private_key_file = optarg; break;
        case 'h': usage(); return EXIT_SUCCESS;
        default: usage(); return EXIT_FAILURE;
        }
    }
    private_key = fopen(private_key_file, "r");
    if (!private_key) {
        fprintf(stderr, "Error opening the file %s or it is not present in the directory\n",
            private_key_file);
        fclose(in_file);
        fclose(out_file);
        fclose(private_key);
        return EXIT_FAILURE;
    }

    mpz_t pq, d;
    mpz_inits(pq, d, NULL);

    ss_read_priv(pq, d, private_key);
    ss_decrypt_file(in_file, out_file, d, pq);

    if (verbose) {
        v(pq, d);
    }

    fclose(out_file);
    fclose(private_key);
    fclose(in_file);

    mpz_clears(pq, d, NULL);
    return EXIT_SUCCESS;
}
