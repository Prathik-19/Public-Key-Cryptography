#include <stdio.h> // Including standard libraries and gmp library	// clang-format off
// clang-format on
#include "numtheory.h" // Including numtheory.h file to use function from the file
#include "randstate.h" // Including randstate.h header file to use the state variable
#include "ss.h"

#include <gmp.h>
#include <stdbool.h> // Including standard libraries and gmp library
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#define EPSILON 1e-14
/*
static inline double absolute(double x) {
    return x < 0.0 ? -x : x;
}

static inline double sqrt_newton(double x) {
    double z = 0.0;
    double y = 1.0;
    while (absolute(y - z) > EPSILON) {
        z = y;
        y = 0.5 * (z + (x / z));
    }
    return y;
}

static inline uint64_t log_base_2(double x) {
    uint64_t k = -1;
    uint64_t n = absolute(x);
    while (n > 0) {
        n /= 2.0;
        k += 1;
    }
    return k;
}

*/
void ss_make_pub(mpz_t p, mpz_t q, mpz_t n, uint64_t nbits, uint64_t iters) {
    // Set upper and lower limits for the number of bits in p and q
    uint64_t upper_limit = (2 * nbits) / 5;
    uint64_t lower_limit = nbits / 5;

    // Choose the number of bits for p and calculate the number of bits for q
    uint64_t p_bits = (random() % (upper_limit - lower_limit + 1)) + lower_limit;
    uint64_t q_bits = (nbits - (2 * p_bits));

    // Generate a random odd number of p_bits and check if it is prime
    while (mpz_sizeinbase(p, 2) != p_bits) {
        mpz_urandomb(p, state, p_bits);
        mpz_setbit(p, p_bits - 1); // Ensure that p has p_bits bits
        make_prime(p, p_bits, iters);
    }

    // Generate a random odd number of q_bits and check if it is prime
    while (mpz_sizeinbase(q, 2) != q_bits) {
        mpz_urandomb(q, state, q_bits);
        mpz_setbit(q, q_bits - 1); // Ensure that q has q_bits bits
        make_prime(q, q_bits, iters);
    }

    // Calculate n = p^2 * q
    mpz_mul(n, p, p);
    mpz_mul(n, n, q);
}

void ss_write_pub(const mpz_t n, const char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n%s\n", n, username);
    return;
}

void ss_read_pub(mpz_t n, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx", n);
    gmp_fscanf(pbfile, "%s", username);
    return;
}

void ss_make_priv(mpz_t d, mpz_t pq, const mpz_t p, const mpz_t q) {
    mpz_t gcd_val, n, p_minus_1, q_minus_1, temp;
    mpz_inits(gcd_val, n, p_minus_1, q_minus_1, temp, NULL);
    mpz_sub_ui(p_minus_1, p, 1);
    mpz_sub_ui(q_minus_1, q, 1);
    gcd(gcd_val, p_minus_1, q_minus_1);
    mpz_mul(temp, p_minus_1, q_minus_1);
    mpz_div(temp, temp, gcd_val);
    mpz_mul(pq, p, q);
    mpz_mul(n, p, pq);
    mod_inverse(d, n, temp);
    mpz_clears(gcd_val, n, p_minus_1, q_minus_1, temp, NULL);
    return;
}

void ss_write_priv(const mpz_t pq, const mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n%Zx\n", pq, d);
    return;
}

void ss_read_priv(mpz_t pq, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx", pq);
    gmp_fscanf(pvfile, "%Zx", d);
    return;
}

void ss_encrypt(mpz_t c, const mpz_t m, const mpz_t n) {
    pow_mod(c, m, n, n);
    return;
}

void ss_encrypt_file(FILE *infile, FILE *outfile, const mpz_t n) {
    mpz_t m, out;
    mpz_inits(m, out, NULL);

    size_t k = ((0.5 * mpz_sizeinbase(n, 2) - 1) / 8);

    uint8_t *block = (uint8_t *) (calloc(k, sizeof(uint8_t)));
    block[0] = 0XFF;
    size_t j = fread(&block[1], 1, k - 1, infile);
    while (j > 0) {

        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, block);

        ss_encrypt(out, m, n);

        gmp_fprintf(outfile, "%Zx\n", out);
        j = fread(&block[1], 1, k - 1, infile);
    }
    free(block);

    mpz_clears(m, out, NULL);
}

void ss_decrypt(mpz_t m, const mpz_t c, const mpz_t d, const mpz_t pq) {
    pow_mod(m, c, d, pq);
    return;
}

void ss_decrypt_file(FILE *infile, FILE *outfile, const mpz_t d, const mpz_t pq) {
    size_t k = ((mpz_sizeinbase(pq, 2) - 1) / 8);
    uint8_t *block = (uint8_t *) calloc(k, sizeof(uint8_t));

    size_t j;
    mpz_t c;
    mpz_t m;
    mpz_init(c);
    mpz_init(m);
    bool flag = gmp_fscanf(infile, "%Zx\n", c) != EOF;
    while (flag) {

        ss_decrypt(m, c, d, pq);
        mpz_export(block, &j, 1, sizeof(uint8_t), 1, 0, m);
        fwrite(&block[1], sizeof(uint8_t), j - 1, outfile);
        flag = gmp_fscanf(infile, "%Zx\n", c) != EOF;
    }

    free(block);
    mpz_clear(c);
    mpz_clear(m);
}
