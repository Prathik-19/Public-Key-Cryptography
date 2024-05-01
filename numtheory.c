#include "numtheory.h"
#include "randstate.h"

void gcd(mpz_t g, const mpz_t a, const mpz_t b) {
    if (mpz_cmp_ui(b, 0) == 0) {
        mpz_set(g, a);
        return;
    }
    mpz_t a_copy, b_copy, t;
    mpz_inits(a_copy, b_copy, t, NULL);
    mpz_set(a_copy, a);
    mpz_set(b_copy, b);
    while (mpz_cmp_ui(b_copy, 0) != 0) {
        mpz_set(t, b_copy);
        mpz_mod(b_copy, a_copy, b_copy);
        mpz_set(a_copy, t);
    }
    mpz_set(g, a_copy);
    mpz_clears(a_copy, b_copy, t, NULL);
}

void mod_inverse(mpz_t o, const mpz_t a, const mpz_t n) {
    mpz_t r1, r2, t1, t2, q, q_mul_r, q_mul_t, temp;
    mpz_init_set(r1, n);
    mpz_init_set(r2, a);
    mpz_init_set_ui(t1, 0);
    mpz_init_set_ui(t2, 1);
    mpz_init(q);
    mpz_init(q_mul_r);
    mpz_init(q_mul_t);
    mpz_init(temp);

    while (mpz_sgn(r2) != 0) {
        mpz_fdiv_q(q, r1, r2);
        mpz_set(temp, r1);
        mpz_set(r1, r2);
        mpz_mul(q_mul_r, q, r2);
        mpz_sub(r2, temp, q_mul_r);
        mpz_set(temp, t1);
        mpz_set(t1, t2);
        mpz_mul(q_mul_t, q, t2);
        mpz_sub(t2, temp, q_mul_t);
    }

    if (mpz_cmp_ui(r1, 1) > 0) {
        mpz_set_ui(o, 0);
    } else {
        if (mpz_sgn(t1) < 0) {
            mpz_add(t1, t1, n);
        }
        mpz_set(o, t1);
    }

    mpz_clear(r1);
    mpz_clear(r2);
    mpz_clear(t1);
    mpz_clear(t2);
    mpz_clear(q);
    mpz_clear(q_mul_r);
    mpz_clear(q_mul_t);
    mpz_clear(temp);
}

void pow_mod(mpz_t o, const mpz_t a, const mpz_t d, const mpz_t n) {
    mpz_t v, p, d_copy, d_copy_div_2, temp;
    mpz_inits(v, p, d_copy, d_copy_div_2, temp, NULL);
    mpz_set_ui(v, 1);
    mpz_set(p, a);
    mpz_set(d_copy, d);
    while (mpz_cmp_ui(d_copy, 0) > 0) {
        if (mpz_odd_p(d_copy)) {
            mpz_mul(temp, v, p);
            mpz_mod(v, temp, n);
        }
        mpz_mul(temp, p, p);
        mpz_mod(p, temp, n);
        mpz_tdiv_q_2exp(d_copy_div_2, d_copy, 1);
        mpz_set(d_copy, d_copy_div_2);
    }
    mpz_set(o, v);
    mpz_clears(v, p, d_copy, d_copy_div_2, temp, NULL);
}

bool is_prime(const mpz_t n, uint64_t iters) {
    mpz_t a, y, j, i, r, pow_mod_exp, n1, value_1;

    mpz_inits(a, y, j, i, r, pow_mod_exp, n1, value_1, NULL);

    if (mpz_cmp_ui(n, 2) == 0 || mpz_cmp_ui(n, 3) == 0) {

        return true;
    }

    if (mpz_divisible_ui_p(n, 2) != 0) {
        return false;
    }

    if (mpz_cmp_ui(n, 1) == 0) {
        return false;
    }

    if (mpz_cmp_ui(n, 0) == 0) {
        return false;
    }

    mp_bitcnt_t s, s1;

    s = 1;

    mpz_sub_ui(n1, n, 1);
    int ab;

    do {
        ab = mpz_divisible_2exp_p(n1, s);
        s++;
    } while (ab == 0);

    mpz_set_ui(value_1, 1);

    mpz_mul_2exp(value_1, value_1, s);

    mpz_cdiv_q(r, n1, value_1);

    for (mpz_set_ui(i, 1); mpz_cmp_ui(i, iters) < 0; mpz_add_ui(i, i, 1)) {

        mpz_urandomm(a, state, n);
        mpz_add_ui(a, a, 2);
        pow_mod(y, a, r, n);

        if ((mpz_cmp_ui(y, 1) != 0) && mpz_cmp(y, n1) != 0) {

            mpz_set_ui(j, 1);

            s1 = s - 1;

            while ((mpz_cmp_ui(j, s1) <= 0) && mpz_cmp(y, n1) != 0) {

                mpz_set_ui(pow_mod_exp, 2);
                pow_mod(y, y, pow_mod_exp, n);

                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(a, y, j, i, r, pow_mod_exp, n1, value_1, NULL);
                    return false;
                }

                mpz_add_ui(j, j, 1);
            }

            if (mpz_cmp(y, n1) != 0) {
                mpz_clears(a, y, j, i, r, pow_mod_exp, n1, value_1, NULL);
                return false;
            }
        }
    }

    mpz_clears(a, y, j, i, r, pow_mod_exp, n1, value_1, NULL);
    return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_t candidate;
    mpz_init(candidate);

    do {
        // Generate a random number with the specified number of bits
        mpz_urandomb(candidate, state, bits);

        // Set the first and last bits to 1 to ensure that the number is of the specified size
        mpz_setbit(candidate, 0);
        mpz_setbit(candidate, bits - 1);

        // Check if the candidate is a prime number using the specified number of iterations
        if (is_prime(candidate, iters)) {
            mpz_set(p, candidate); // Set the prime number
            break;
        }

    } while (true);

    mpz_clear(candidate);
}
