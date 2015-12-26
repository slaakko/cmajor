#include "gmp/gmp.h"
#include <unordered_map>
#include <forward_list>

std::unordered_map<int, mpz_t> mpz_map;
std::forward_list<int> free_mpz;
int next_mpz_handle = 0;

extern "C" int create_mpz()
{
    int mpz_handle = 0;
    if (!free_mpz.empty())
    {
        mpz_handle = free_mpz.front();
        free_mpz.pop_front();
    }
    else
    {
        mpz_handle = next_mpz_handle++;
    }
    mpz_t& mpz = mpz_map[mpz_handle];
    mpz_init(mpz);
    return mpz_handle;
}

extern "C" void destroy_mpz(int mpz_handle)
{
    mpz_t& mpz = mpz_map[mpz_handle];
    mpz_clear(mpz);
    free_mpz.push_front(mpz_handle);
}

extern "C" void assign_mpz(int mpz_left, int mpz_right)
{
    mpz_t& left = mpz_map[mpz_left];
    mpz_t& right = mpz_map[mpz_right];
    mpz_set(left, right);
}

extern "C" void neg_mpz(int mpz_left, int mpz_right)
{
    mpz_t& left = mpz_map[mpz_left];
    mpz_t& right = mpz_map[mpz_right];
    mpz_neg(left, right);
}

extern "C" void abs_mpz(int mpz_left, int mpz_right)
{
    mpz_t& left = mpz_map[mpz_left];
    mpz_t& right = mpz_map[mpz_right];
    mpz_abs(left, right);
}

extern "C" void assign_mpz_ui(int mpz_left, unsigned long right)
{
    mpz_t& left = mpz_map[mpz_left];
    mpz_set_ui(left, right);
}

extern "C" void assign_mpz_si(int mpz_left, int right)
{
    mpz_t& left = mpz_map[mpz_left];
    mpz_set_si(left, right);
}

extern "C" int assign_mpz_str(int mpz_handle, const char* str, int base)
{
    mpz_t& mpz = mpz_map[mpz_handle];
    return mpz_set_str(mpz, str, base);
}

extern "C" void swap_mpz(int mpz_left, int mpz_right)
{
    mpz_t& left = mpz_map[mpz_left];
    mpz_t& right = mpz_map[mpz_right];
    mpz_swap(left, right);
}

extern "C" char* get_mpz_str(int mpz_handle, int base)
{
    mpz_t& mpz = mpz_map[mpz_handle];
    return mpz_get_str(NULL, base, mpz);
}

extern "C" void add_mpz(int mpz_target, int mpz_left, int mpz_right)
{
    mpz_t& target = mpz_map[mpz_target];
    mpz_t& left = mpz_map[mpz_left];
    mpz_t& right = mpz_map[mpz_right];
    mpz_add(target, left, right);
}

extern "C" void sub_mpz(int mpz_target, int mpz_left, int mpz_right)
{
    mpz_t& target = mpz_map[mpz_target];
    mpz_t& left = mpz_map[mpz_left];
    mpz_t& right = mpz_map[mpz_right];
    mpz_sub(target, left, right);
}

extern "C" void mul_mpz(int mpz_target, int mpz_left, int mpz_right)
{
    mpz_t& target = mpz_map[mpz_target];
    mpz_t& left = mpz_map[mpz_left];
    mpz_t& right = mpz_map[mpz_right];
    mpz_mul(target, left, right);
}

extern "C" void div_mpz(int mpz_target, int mpz_left, int mpz_right)
{
    mpz_t& target = mpz_map[mpz_target];
    mpz_t& left = mpz_map[mpz_left];
    mpz_t& right = mpz_map[mpz_right];
    mpz_fdiv_q(target, left, right);
}

extern "C" void rem_mpz(int mpz_target, int mpz_left, int mpz_right)
{
    mpz_t& target = mpz_map[mpz_target];
    mpz_t& left = mpz_map[mpz_left];
    mpz_t& right = mpz_map[mpz_right];
    mpz_fdiv_r(target, left, right);
}

extern "C" int cmp_mpz(int mpz_left, int mpz_right)
{
    mpz_t& left = mpz_map[mpz_left];
    mpz_t& right = mpz_map[mpz_right];
    return mpz_cmp(left, right);
}
