#include <quiz.h>
#include <stdint.h>
#include <assert.h>

long long __aeabi_llsl(long long value, int shift);
long long __aeabi_llsr(long long value, int shift);

QUIZ_CASE(liba_llsl) {
  quiz_assert(__aeabi_llsl((uint64_t)1, 1) == (uint64_t)0x2);
  quiz_assert(__aeabi_llsl((uint64_t)1, 2) == (uint64_t)0x4);
  quiz_assert(__aeabi_llsl((uint64_t)1, 10) == (uint64_t)0x400);
  quiz_assert(__aeabi_llsl((uint64_t)1, 20) == (uint64_t)0x100000);
  quiz_assert(__aeabi_llsl((uint64_t)1, 30) == (uint64_t)0x40000000);
  quiz_assert(__aeabi_llsl((uint64_t)1, 31) == (uint64_t)0x80000000);
  quiz_assert(__aeabi_llsl((uint64_t)1, 32) == (uint64_t)0x100000000);
  quiz_assert(__aeabi_llsl((uint64_t)1, 33) == (uint64_t)0x200000000);
  quiz_assert(__aeabi_llsl((uint64_t)1, 40) == (uint64_t)0x10000000000);
  quiz_assert(__aeabi_llsl((uint64_t)1, 50) == (uint64_t)0x4000000000000);
  quiz_assert(__aeabi_llsl((uint64_t)1, 60) == (uint64_t)0x1000000000000000);
  quiz_assert(__aeabi_llsl((uint64_t)1, 61) == (uint64_t)0x2000000000000000);
  quiz_assert(__aeabi_llsl((uint64_t)1, 62) == (uint64_t)0x4000000000000000);
  quiz_assert(__aeabi_llsl((uint64_t)1, 63) == (uint64_t)0x8000000000000000);
  quiz_assert(__aeabi_llsl((uint64_t)1, 64) == (uint64_t)0);
}

QUIZ_CASE(liba_llsr) {
  quiz_assert(__aeabi_llsr((uint64_t)0x8000000000000000, 1) == (uint64_t)0x4000000000000000);
  quiz_assert(__aeabi_llsr((uint64_t)0x8000000000000000, 2) == (uint64_t)0x2000000000000000);
  quiz_assert(__aeabi_llsr((uint64_t)0x8000000000000000, 10) == (uint64_t)0x20000000000000);
  quiz_assert(__aeabi_llsr((uint64_t)0x8000000000000000, 20) == (uint64_t)0x80000000000);
  quiz_assert(__aeabi_llsr((uint64_t)0x8000000000000000, 30) == (uint64_t)0x200000000);
  quiz_assert(__aeabi_llsr((uint64_t)0x8000000000000000, 31) == (uint64_t)0x100000000);
  quiz_assert(__aeabi_llsr((uint64_t)0x8000000000000000, 32) == (uint64_t)0x80000000);
  quiz_assert(__aeabi_llsr((uint64_t)0x8000000000000000, 33) == (uint64_t)0x40000000);
  quiz_assert(__aeabi_llsr((uint64_t)0x8000000000000000, 40) == (uint64_t)0x800000);
  quiz_assert(__aeabi_llsr((uint64_t)0x8000000000000000, 50) == (uint64_t)0x2000);
  quiz_assert(__aeabi_llsr((uint64_t)0x8000000000000000, 60) == (uint64_t)0x8);
  quiz_assert(__aeabi_llsr((uint64_t)0x8000000000000000, 61) == (uint64_t)0x4);
  quiz_assert(__aeabi_llsr((uint64_t)0x8000000000000000, 62) == (uint64_t)0x2);
  quiz_assert(__aeabi_llsr((uint64_t)0x8000000000000000, 63) == (uint64_t)0x1);
  quiz_assert(__aeabi_llsr((uint64_t)0x8000000000000000, 64) == (uint64_t)0);
}
