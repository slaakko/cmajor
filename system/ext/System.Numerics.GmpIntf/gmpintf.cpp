#include "gmp/gmp.h"

void* create_mpz()
{
    mpz_t* mpz = malloc(sizeof(mpz_t));
    mpz_init(*mpz);
    return mpz
}

void destroy_mpz(void* mpz)
{
    mpz_t* m = (mpz_t*)mpz;
    mpz_clear(*m);
    free(mpz);
}

void assign_mpz(void* mpz_left, void* mpz_right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_set(*left, *right);
}

void neg_mpz(void* mpz_left, void* mpz_right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_neg(*left, *right);
}

void abs_mpz(void* mpz_left, void* mpz_right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_abs(*left, *right);
}

void assign_mpz_ui(void* mpz_left, unsigned long right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_set_ui(*left, right);
}

void assign_mpz_si(void* mpz_left, int right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_set_si(*left, right);
}

int assign_mpz_str(void* mpz_handle, const char* str, int base)
{
    mpz_t* mpz = (mpz_t*)mpz_handle;
    return mpz_set_str(*mpz, str, base);
}

void swap_mpz(void* mpz_left, void* mpz_right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_swap(*left, *right);
}

char* get_mpz_str(void* mpz_handle, int base)
{
    mpz_t* mpz = (mpz_t*)mpz_handle;
    return mpz_get_str(NULL, base, *mpz);
}

void add_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_add(*target, *left, *right);
}

void sub_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_sub(*target, *left, *right);
}

void mul_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_mul(*target, *left, *right);
}

void div_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_fdiv_q(*target, *left, *right);
}

void rem_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_fdiv_r(target, left, right);
}

int cmp_mpz(void* mpz_left, void* mpz_right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    return mpz_cmp(*left, *right);
}
