#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/rational.h>
#include <quiz.h>

#include "../helper.h"

using namespace Poincare::Internal;

static inline void assert_equal(const Tree* e1, const Tree* e2) {
  quiz_assert(Rational::Compare(e1, e2) == 0);
}

static inline void assert_not_equal(const Tree* e1, const Tree* e2) {
  quiz_assert(Rational::Compare(e1, e2) != 0);
}

static inline void assert_lower(const Tree* e1, const Tree* e2) {
  quiz_assert(Rational::Compare(e1, e2) < 0);
}

static inline void assert_greater(const Tree* e1, const Tree* e2) {
  quiz_assert(Rational::Compare(e1, e2) > 0);
}

QUIZ_CASE(poincare_rational_order) {
  Tree* e1 = Rational::Push(123_e, 324_e);
  Tree* e2 = Rational::Push(41_e, 108_e);
  assert_equal(e1, e2);
  Tree* e3 = Rational::Push(123_e, 234_e);
  Tree* e4 = Rational::Push(42_e, 108_e);
  assert_not_equal(e3, e4);
  Tree* e5 = Rational::Push(456_e, 567_e);
  assert_lower(e3, e5);
  Tree* e6 = Rational::Push(-123_e, 234_e);
  assert_lower(e6, e5);
  Tree* e7 = Rational::Push(-456_e, 567_e);
  assert_greater(e3, e7);
  Tree* i = parse("12345678912345678910");
  Tree* e8 = Rational::Push(123456789123456789_e, i);
  assert_greater(e3, e8);
}

static inline void assert_add_to(const Tree* e1, const Tree* e2,
                                 const Tree* e3) {
  quiz_assert(Rational::Compare(Rational::Addition(e1, e2), e3) == 0);
}

QUIZ_CASE(poincare_rational_addition) {
  Tree* e1 = Rational::Push(1_e, 2_e);
  Tree* e2 = Rational::Push(1_e, 1_e);
  Tree* e3 = Rational::Push(3_e, 2_e);
  assert_add_to(e1, e2, e3);
  Tree* i = parse("18446744073709551616");
  Tree* e4 = Rational::Push(i, 4294967296_e);
  Tree* e5 = Rational::Push(8_e, 9_e);
  Tree* e6 = Rational::Push(38654705672_e, 9_e);
  assert_add_to(e4, e5, e6);
  Tree* e7 = Rational::Push(-8_e, 9_e);
  Tree* e8 = Rational::Push(38654705656_e, 9_e);
  assert_add_to(e4, e7, e8);
}

static inline void assert_pow_to(const Tree* e1, const Tree* e2,
                                 const Tree* e3) {
  quiz_assert(Rational::Compare(Rational::IntegerPower(e1, e2), e3) == 0);
}

QUIZ_CASE(poincare_rational_power) {
  Tree* e1 = Rational::Push(4_e, 5_e);
  Tree* e2 = Rational::Push(3_e, 1_e);
  Tree* e3 = Rational::Push(64_e, 125_e);
  assert_pow_to(e1, e2, e3);
  Tree* e4 = Rational::Push(-3_e, 1_e);
  Tree* e5 = Rational::Push(125_e, 64_e);
  assert_pow_to(e1, e4, e5);
}
