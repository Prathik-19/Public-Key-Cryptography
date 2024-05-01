#include <stdio.h>

#include "numtheory.h"
#include "randstate.h"
#include "ss.h"

#include <getopt.h>
#include <gmp.h>
// Including standard libraries and gmp library
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define OPTIONS "i:o:n:vh"

void v(char *user, mpz_t n) {
    fprintf(stderr, "user = %s\n", user);
    gmp_fprintf(stderr, "n = %Zd\n", n);
}

void usage(void) {
    fprintf(stderr, "SYNOPSIS\n");
    fprintf(stderr, "   Encrypts data using SS encryption.\n");
    fprintf(stderr, "   Encrypted data is decrypted by the decrypt program.\n\n");
    fprintf(stderr, "USAGE\n");
    fprintf(stderr, "   ./encrypt [OPTIONS]\n\n");
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "   -h              Display program help and usage.\n");
    fprintf(stderr, "   -v              Display verbose program output.\n");
    fprintf(stderr, "   -i in_file       Input file of data to encrypt (default: stdin).\n");
    fprintf(stderr, "   -o out_file      Output file for encrypted data (default: stdout).\n");
    fprintf(stderr, "   -n pbfile       Public key file (default: ss.pub).\n");
}

int main(int argc, char **argv) {
    int opt = 0; // variable for getting option form array
    char *public_key_file = "ss.pub"; // public key name

    FILE *in_file = stdin;
    FILE *out_file = stdout;
    FILE *public_key = NULL;
    bool verbose = false;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'v': verbose = true; break;
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
        case 'h': usage(); return EXIT_SUCCESS;
        case 'n': public_key_file = optarg; break;

        default: usage(); return EXIT_FAILURE;
        }
    }
    public_key = fopen(public_key_file, "r");
    if (!public_key) {
        fprintf(stderr, "Error opening the file %s or it is not present in the directory\n",
            public_key_file);
        fclose(in_file);
        fclose(out_file);
        fclose(public_key);
        return EXIT_FAILURE;
    }
    mpz_t n;
    mpz_init(n);

    char *user_name = malloc(256 * sizeof(char));

    ss_read_pub(n, user_name, public_key);
    ss_encrypt_file(in_file, out_file, n);
    if (verbose) {
        v(user_name, n);
    }

    fclose(out_file);
    fclose(public_key);
    fclose(in_file);
    mpz_clear(n);
    free(user_name);

    return EXIT_SUCCESS;
}
