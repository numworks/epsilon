#include <poincare/sign.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/rational.h>
#include <poincare/src/expression/systematic_reduction.h>
#include <quiz.h>

#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

static bool integer_handler_same_absolute_value(IntegerHandler a,
                                                IntegerHandler b) {
  a.setSign(NonStrictSign::Positive);
  b.setSign(NonStrictSign::Positive);
  return IntegerHandler::Compare(a, b) == 0;
}

static void assert_properties(const Tree* numerator, const Tree* denominator,
                              Type expectedType, Sign sign) {
  Tree* r = Rational::Push(numerator, denominator);
  quiz_assert(r->type() == expectedType);
  quiz_assert(sign == Rational::Sign(r));
  integer_handler_same_absolute_value(Integer::Handler(numerator),
                                      Rational::Numerator(r));
  integer_handler_same_absolute_value(Integer::Handler(denominator),
                                      Rational::Denominator(r));
  r->removeTree();
}

QUIZ_CASE(pcj_rational_properties) {
  assert_properties(3_e, 8_e, Type::RationalPosShort,
                    Sign::FiniteStrictlyPositive());
  assert_properties(-1_e, 255_e, Type::RationalNegShort,
                    Sign::FiniteStrictlyNegative());
  assert_properties(1_e, -1_e, Type::MinusOne,
                    Sign::FiniteStrictlyNegativeInteger());
  assert_properties(-1_e, -2_e, Type::Half, Sign::FiniteStrictlyPositive());
  assert_properties(127_e, -255_e, Type::RationalNegShort,
                    Sign::FiniteStrictlyNegative());
  assert_properties(0_e, 5_e, Type::Zero, Sign::Zero());
  assert_properties(32134123_e, 812312312_e, Type::RationalPosBig,
                    Sign::FiniteStrictlyPositive());
  assert_properties(32134123_e, -812312312_e, Type::RationalNegBig,
                    Sign::FiniteStrictlyNegative());
  assert_properties(-32134123_e, 812312312_e, Type::RationalNegBig,
                    Sign::FiniteStrictlyNegative());
  assert_properties(-32134123_e, -812312312_e, Type::RationalPosBig,
                    Sign::FiniteStrictlyPositive());
  assert_properties(0_e, 812312312_e, Type::Zero, Sign::Zero());
  assert_properties(0_e, -812312312_e, Type::Zero, Sign::Zero());
}

static void assert_set_sign(const Tree* iNumerator, const Tree* iDenominator,
                            NonStrictSign sign, const Tree* resNumerator,
                            const Tree* resDenominator) {
  Tree* i = Rational::Push(iNumerator, iDenominator);
  Rational::SetSign(i, sign);
  Tree* expected = Rational::Push(resNumerator, resDenominator);
  quiz_assert(i->treeIsIdenticalTo(expected));
  expected->removeTree();
  i->removeTree();
}

QUIZ_CASE(pcj_rational_set_sign) {
  assert_set_sign(0_e, 1_e, NonStrictSign::Negative, 0_e, 1_e);
  assert_set_sign(0_e, 1_e, NonStrictSign::Positive, 0_e, 1_e);
  assert_set_sign(2_e, 1_e, NonStrictSign::Negative, -2_e, 1_e);
  assert_set_sign(-3_e, 4_e, NonStrictSign::Positive, 3_e, 4_e);
}

static void assert_irreducible_form(const Tree* iNumerator,
                                    const Tree* iDenominator,
                                    const Tree* resNumerator,
                                    const Tree* resDenominator) {
  Tree* i = Rational::Push(iNumerator, iDenominator);
  Tree* expected = Rational::Push(resNumerator, resDenominator);
  Tree* result = i->cloneTree();
  quiz_assert(result->treeIsIdenticalTo(expected));
  result->removeTree();
  expected->removeTree();
  i->removeTree();
}

QUIZ_CASE(pcj_rational_irreducible_form) {
  assert_irreducible_form(2_e, 6_e, 1_e, 3_e);
  assert_irreducible_form(-15170_e, 50061_e, -10_e, 33_e);
  assert_irreducible_form(123169_e, 123191_e, 123169_e, 123191_e);
}

typedef Tree* (*BinaryOperation)(const Tree* i, const Tree* j);

static void assert_operation(const Tree* iNumerator, const Tree* iDenominator,
                             const Tree* jNumerator, const Tree* jDenominator,
                             BinaryOperation operation,
                             const Tree* resNumerator,
                             const Tree* resDenominator) {
  Tree* i = Rational::Push(iNumerator, iDenominator);
  Tree* j = Rational::Push(jNumerator, jDenominator);
  Tree* expected = Rational::Push(resNumerator, resDenominator);
  Tree* result = operation(i, j);
  SystematicReduction::ShallowReduce(result);
  quiz_assert(result->treeIsIdenticalTo(expected));
  result->removeTree();
  expected->removeTree();
  j->removeTree();
  i->removeTree();
}

typedef Tree* (*TernaryOperation)(const Tree* i, const Tree* j, const Tree* k);

static void assert_operation(const Tree* iNumerator, const Tree* iDenominator,
                             const Tree* jNumerator, const Tree* jDenominator,
                             const Tree* kNumerator, const Tree* kDenominator,
                             TernaryOperation operation,
                             const Tree* resNumerator,
                             const Tree* resDenominator) {
  Tree* i = Rational::Push(iNumerator, iDenominator);
  Tree* j = Rational::Push(jNumerator, jDenominator);
  Tree* k = Rational::Push(kNumerator, kDenominator);
  Tree* expected = Rational::Push(resNumerator, resDenominator);
  Tree* result = operation(i, j, k);
  SystematicReduction::ShallowReduce(result);
  quiz_assert(result->treeIsIdenticalTo(expected));
  result->removeTree();
  expected->removeTree();
  k->removeTree();
  j->removeTree();
  i->removeTree();
}

QUIZ_CASE(pcj_rational_addition) {
  assert_operation(1_e, 2_e, 1_e, 2_e, Rational::Addition, 1_e, 1_e);
  assert_operation(1237_e, 5257_e, -3_e, 4_e, Rational::Addition, -10823_e,
                   21028_e);
  assert_operation(1_e, 2_e, 1_e, 4_e, 2_e, 8_e, Rational::Addition, 1_e, 1_e);
}

QUIZ_CASE(pcj_rational_multiplication) {
  assert_operation(1_e, 2_e, 1_e, 2_e, Rational::Multiplication, 1_e, 4_e);
  assert_operation(23515_e, 7_e, 2_e, 23515_e, Rational::Multiplication, 2_e,
                   7_e);
  assert_operation(1_e, 2_e, 1_e, 2_e, 1_e, -2_e, Rational::Multiplication,
                   -1_e, 8_e);
}

QUIZ_CASE(pcj_rational_integer_power) {
  assert_operation(3_e, 2_e, 3_e, 1_e, Rational::IntegerPower, 27_e, 8_e);
  assert_operation(1_e, 2_e, 10_e, 1_e, Rational::IntegerPower, 1_e, 1024_e);
  assert_operation(7123_e, 3_e, 2_e, 1_e, Rational::IntegerPower, 50737129_e,
                   9_e);
}

QUIZ_CASE(pcj_rational_create_mixed_fraction) {
  assert_trees_are_equal(Rational::CreateMixedFraction(7_e / 5_e, true),
                         KMixedFraction(1_e, 2_e / 5_e));
  assert_trees_are_equal(Rational::CreateMixedFraction(7_e / 5_e, false),
                         KAdd(1_e, 2_e / 5_e));
  assert_trees_are_equal(Rational::CreateMixedFraction(-7_e / 5_e, true),
                         KOpposite(KMixedFraction(1_e, 2_e / 5_e)));
  assert_trees_are_equal(Rational::CreateMixedFraction(-7_e / 5_e, false),
                         KAdd(KOpposite(1_e), KOpposite(2_e / 5_e)));
  assert_trees_are_equal(Rational::CreateMixedFraction(2_e / 3_e, true),
                         KMixedFraction(0_e, 2_e / 3_e));
  assert_trees_are_equal(Rational::CreateMixedFraction(-2_e / 3_e, false),
                         KAdd(0_e, KOpposite(2_e / 3_e)));
}
