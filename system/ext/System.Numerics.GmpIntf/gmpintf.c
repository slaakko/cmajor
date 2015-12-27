#ifdef _WIN32
    #include "gmp/windows/gmp.h"
#elif defined(__linux) || defined(__unix) || defined(__posix)
    #include "gmp/linux/gmp.h"
#else
    #error unknown platform
#endif
#include <stdlib.h>

void* create_mpz()
{
    mpz_t* mpz = (mpz_t*)malloc(sizeof(mpz_t));
    mpz_init(*mpz);
    return mpz;
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
    mpz_fdiv_r(*target, *left, *right);
}

int cmp_mpz(void* mpz_left, void* mpz_right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    return mpz_cmp(*left, *right);
}

void and_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_and(*target, *left, *right);
}

void or_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_ior(*target, *left, *right);
}

void xor_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_xor(*target, *left, *right);
}

void cpl_mpz(void* mpz_left, void* mpz_right)
{
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_com(*left, *right);
}

void setbit_mpz(void* mpz_handle, unsigned long bit_index)
{
    mpz_t* mpz = (mpz_t*)mpz_handle;
    mpz_setbit(*mpz, (mp_bitcnt_t)bit_index);
}

void clrbit_mpz(void* mpz_handle, unsigned long bit_index)
{
    mpz_t* mpz = (mpz_t*)mpz_handle;
    mpz_clrbit(*mpz, (mp_bitcnt_t)bit_index);
}

void cplbit_mpz(void* mpz_handle, unsigned long bit_index)
{
    mpz_t* mpz = (mpz_t*)mpz_handle;
    mpz_combit(*mpz, (mp_bitcnt_t)bit_index);
}

int tstbit_mpz(void* mpz_handle, unsigned long bit_index)
{
    mpz_t* mpz = (mpz_t*)mpz_handle;
    return mpz_tstbit(*mpz, (mp_bitcnt_t)bit_index);
}
