#include <omg/arithmetic.h>
#include <quiz.h>

using namespace OMG;

static inline void assert_gcd_is(size_t a, size_t b, size_t g) {
  quiz_assert(Arithmetic::Gcd(a, b) == g);
}

QUIZ_CASE(omg_arithmetic_gcd) {
  assert_gcd_is(1, 1, 1);
  assert_gcd_is(2, 3, 1);
  assert_gcd_is(7, 5, 1);
  assert_gcd_is(3, 9, 3);
}