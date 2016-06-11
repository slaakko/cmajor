#ifdef _WIN32
    #if defined(__MINGW64__)
        #include "gmp/windows/x64/gmp.h"
    #else
        #include "gmp/windows/x86/gmp.h"
    #endif
#elif defined(__linux) || defined(__unix) || defined(__posix)
    #if defined(__x86_64__)
        #include "gmp/linux/x86_64/gmp.h"
    #else
        #include "gmp/linux/i686/gmp.h"
    #endif
#else
    #error unknown platform
#endif
#include <stdlib.h>

// integer functions:

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
    mpz_tdiv_q(*target, *left, *right);
}

void rem_mpz(void* mpz_target, void* mpz_left, void* mpz_right)
{
    mpz_t* target = (mpz_t*)mpz_target;
    mpz_t* left = (mpz_t*)mpz_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpz_tdiv_r(*target, *left, *right);
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

// rational functions:

void* create_mpq()
{
    mpq_t* mpq = (mpq_t*)malloc(sizeof(mpq_t));
    mpq_init(*mpq);
    return mpq;
}

void destroy_mpq(void* mpq)
{
    mpq_t* m = (mpq_t*)mpq;
    mpq_clear(*m);
    free(mpq);
}

void canonicalize_mpq(void* mpq)
{
    mpq_t* subject = (mpq_t*)mpq;
    mpq_canonicalize(*subject);
}

void assign_mpq(void* mpq_left, void* mpq_right)
{
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    mpq_set(*left, *right);
}

int assign_mpq_str(void* mpq_handle, const char* str, int base)
{
    mpq_t* mpq = (mpq_t*)mpq_handle;
    int result = mpq_set_str(*mpq, str, base);
    if (result == 0)
    {
        canonicalize_mpq(mpq);
    }
    return result;
}

char* get_mpq_str(void* mpq_handle, int base)
{
    mpq_t* mpq = (mpq_t*)mpq_handle;
    return mpq_get_str(NULL, base, *mpq);
}

void add_mpq(void* mpq_target, void* mpq_left, void* mpq_right)
{
    mpq_t* target = (mpq_t*)mpq_target;
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    mpq_add(*target, *left, *right);
    canonicalize_mpq(target);
}

void sub_mpq(void* mpq_target, void* mpq_left, void* mpq_right)
{
    mpq_t* target = (mpq_t*)mpq_target;
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    mpq_sub(*target, *left, *right);
    canonicalize_mpq(target);
}

void mul_mpq(void* mpq_target, void* mpq_left, void* mpq_right)
{
    mpq_t* target = (mpq_t*)mpq_target;
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    mpq_mul(*target, *left, *right);
    canonicalize_mpq(target);
}

void div_mpq(void* mpq_target, void* mpq_left, void* mpq_right)
{
    mpq_t* target = (mpq_t*)mpq_target;
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    mpq_div(*target, *left, *right);
    canonicalize_mpq(target);
}

void neg_mpq(void* mpq_left, void* mpq_right)
{
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    mpq_neg(*left, *right);
    canonicalize_mpq(left);
}

void abs_mpq(void* mpq_left, void* mpq_right)
{
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    mpq_abs(*left, *right);
    canonicalize_mpq(left);
}

int cmp_mpq(void* mpq_left, void* mpq_right)
{
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    return mpq_cmp(*left, *right);
}

int equal_mpq(void* mpq_left, void* mpq_right)
{
    mpq_t* left = (mpq_t*)mpq_left;
    mpq_t* right = (mpq_t*)mpq_right;
    return mpq_equal(*left, *right);
}

void get_numerator_mpq(void* mpz_numerator, void* mpq_rational)
{
    mpz_t* numerator = (mpz_t*)mpz_numerator;
    mpq_t* rational = (mpq_t*)mpq_rational;
    mpq_get_num(*numerator, *rational);
}

void get_denominator_mpq(void* mpz_denominator, void* mpq_rational)
{
    mpz_t* denominator = (mpz_t*)mpz_denominator;
    mpq_t* rational = (mpq_t*)mpq_rational;
    mpq_get_den(*denominator, *rational);
}

// float functions:

void set_default_prec_mpf(unsigned int prec)
{
    mpf_set_default_prec(prec);
}

unsigned int get_default_prec_mpf()
{
    return mpf_get_default_prec();
}

void* create_mpf()
{
    mpf_t* mpf = (mpf_t*)malloc(sizeof(mpf_t));
    mpf_init(*mpf);
    return mpf;
}

void create_mpf_prec(unsigned int prec)
{    
    mpf_t* mpf = (mpf_t*)malloc(sizeof(mpf_t));
    mpf_init2(*mpf, prec);
    return mpf;
}

void destroy_mpf(void* mpf)
{
    mpf_t* m = (mpf_t*)mpf;
    mpf_clear(*m);
    free(mpf);
}

unsigned int get_prec_mpf(void* mpf)
{
    mpf_t* m = (mpf_t*)mpf;
    return mpf_get_prec(*m);
}

void set_prec_mpf(void* mpf, unsigned int prec)
{    
    mpf_t* m = (mpf_t*)mpf;
    mpf_set_prec(*m, prec);
}

void set_mpf(void* mpf_left, void* mpf_right)
{
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_t* right = (mpf_t*)mpf_right;
    mpf_set(*left, *right);
}

void set_mpf_ui(void* mpf_left, unsigned long int right)
{
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_set_ui(*left, right);
}

void set_mpf_si(void* mpf_left, signed long int right)
{
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_set_si(*left, right);
}

void set_mpf_d(void* mpf_left, double right)
{
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_set_d(*left, right);
}

void set_mpf_z(void* mpf_left, void* mpz_right)
{
    mpf_t* left = (mpf_t*)mpf_left;
    mpz_t* right = (mpz_t*)mpz_right;
    mpf_set_z(*left, *right);
}

void set_mpf_q(void* mpf_left, void* mpq_right)
{
    mpf_t* left = (mpf_t*)mpf_left;
    mpq_t* right = (mpq_t*)mpq_right;
    mpf_set_q(*left, *right);
}

int set_mpf_str(void* mpf_left, const char* str, int base_)
{
    mpf_t* left = (mpf_t*)mpf_left;
    return mpf_set_str(*left, str, base_);
}

char* get_mpf_str(void* mpf_handle, int base_, unsigned int numDigits, int* exponent)
{
    mpf_t* subject = (mpf_t*)mpf_handle;
    return mpf_get_str(NULL, exponent, base_, numDigits, *subject);
}

void add_mpf(void* mpf_target, void* mpf_left, void* mpf_right)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_t* right = (mpf_t*)mpf_right;
    mpf_add(*target, *left, *right);
}

void sub_mpf(void* mpf_target, void* mpf_left, void* mpf_right)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_t* right = (mpf_t*)mpf_right;
    mpf_sub(*target, *left, *right);
}

void mul_mpf(void* mpf_target, void* mpf_left, void* mpf_right)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_t* right = (mpf_t*)mpf_right;
    mpf_mul(*target, *left, *right);
}

void div_mpf(void* mpf_target, void* mpf_left, void* mpf_right)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_t* right = (mpf_t*)mpf_right;
    mpf_div(*target, *left, *right);
}

void sqrt_mpf(void* mpf_target, void* mpf_subject)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* subject = (mpf_t*)mpf_subject;
    mpf_sqrt(*target, *subject);
}

void neg_mpf(void* mpf_target, void* mpf_subject)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* subject = (mpf_t*)mpf_subject;
    mpf_neg(*target, *subject);
}

void abs_mpf(void* mpf_target, void* mpf_subject)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* subject = (mpf_t*)mpf_subject;
    mpf_abs(*target, *subject);
}

int cmp_mpf(void* mpf_left, void* mpf_right)
{
    mpf_t* left = (mpf_t*)mpf_left;
    mpf_t* right = (mpf_t*)mpf_right;
    return mpf_cmp(*left, *right);
}

void ceil_mpf(void* mpf_target, void* mpf_subject)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* subject = (mpf_t*)mpf_subject;
    mpf_ceil(*target, *subject);
}

void floor_mpf(void* mpf_target, void* mpf_subject)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* subject = (mpf_t*)mpf_subject;
    mpf_floor(*target, *subject);
}

void trunc_mpf(void* mpf_target, void* mpf_subject)
{
    mpf_t* target = (mpf_t*)mpf_target;
    mpf_t* subject = (mpf_t*)mpf_subject;
    mpf_trunc(*target, *subject);
}
