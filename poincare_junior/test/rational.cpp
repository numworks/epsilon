#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <quiz.h>

#include "helper.h"

using namespace PoincareJ;

static bool integer_handler_same_absolute_value(IntegerHandler a,
                                                IntegerHandler b) {
  a.setSign(NonStrictSign::Positive);
  b.setSign(NonStrictSign::Positive);
  return IntegerHandler::Compare(a, b) == 0;
}

static void assert_properties(const Tree* numerator, const Tree* denominator,
                              BlockType expectedBlockType,
                              StrictSign strictSign) {
  Tree* r = Rational::Push(numerator, denominator);
  quiz_assert(r->type() == expectedBlockType);
  quiz_assert(strictSign == Rational::StrictSign(r));
  integer_handler_same_absolute_value(Integer::Handler(numerator),
                                      Rational::Numerator(r));
  integer_handler_same_absolute_value(Integer::Handler(denominator),
                                      Rational::Denominator(r));
  r->removeTree();
}

QUIZ_CASE(pcj_rational_properties) {
  assert_properties(3_e, 8_e, BlockType::RationalShort, StrictSign::Positive);
  assert_properties(-1_e, 255_e, BlockType::RationalShort,
                    StrictSign::Negative);
  assert_properties(1_e, -1_e, BlockType::MinusOne, StrictSign::Negative);
  assert_properties(-1_e, -2_e, BlockType::Half, StrictSign::Positive);
  assert_properties(127_e, -255_e, BlockType::RationalShort,
                    StrictSign::Negative);
  assert_properties(0_e, 5_e, BlockType::Zero, StrictSign::Null);
  assert_properties(32134123_e, 812312312_e, BlockType::RationalPosBig,
                    StrictSign::Positive);
  assert_properties(32134123_e, -812312312_e, BlockType::RationalNegBig,
                    StrictSign::Negative);
  assert_properties(-32134123_e, 812312312_e, BlockType::RationalNegBig,
                    StrictSign::Negative);
  assert_properties(-32134123_e, -812312312_e, BlockType::RationalPosBig,
                    StrictSign::Positive);
  assert_properties(0_e, 812312312_e, BlockType::Zero, StrictSign::Null);
  assert_properties(0_e, -812312312_e, BlockType::Zero, StrictSign::Null);
}

static void assert_set_sign(const Tree* iNumerator, const Tree* iDenominator,
                            NonStrictSign sign, const Tree* resNumerator,
                            const Tree* resDenominator) {
  Tree* i = Rational::Push(iNumerator, iDenominator);
  i = Rational::SetSign(i, sign);
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
  Tree* result = i->clone();
  Rational::MakeIrreducible(result);
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

typedef Tree* (*Operation)(const Tree* i, const Tree* j);

static void assert_operation(const Tree* i, const Tree* j, Operation operation,
                             const Tree* expected) {
  Tree* result = operation(i, j);
  Rational::MakeIrreducible(result);
  Simplification::ShallowSystematicReduce(result);
  quiz_assert(result->treeIsIdenticalTo(expected));
  result->removeTree();
}

static void assert_add_or_mult(const Tree* iNumerator, const Tree* iDenominator,
                               const Tree* jNumerator, const Tree* jDenominator,
                               Operation operation, const Tree* resNumerator,
                               const Tree* resDenominator) {
  assert(operation == Rational::Addition ||
         operation == Rational::Multiplication);
  Tree* i = Rational::Push(iNumerator, iDenominator);
  Tree* j = Rational::Push(jNumerator, jDenominator);
  Tree* expected = Rational::Push(resNumerator, resDenominator);
  Rational::MakeIrreducible(expected);
  assert_operation(i, j, operation, expected);
  expected->removeTree();
  j->removeTree();
  i->removeTree();
}

QUIZ_CASE(pcj_rational_addition) {
  assert_add_or_mult(1_e, 2_e, 1_e, 2_e, Rational::Addition, 1_e, 1_e);
  assert_add_or_mult(1237_e, 5257_e, -3_e, 4_e, Rational::Addition, -10823_e,
                     21028_e);
}

QUIZ_CASE(pcj_rational_multiplication) {
  assert_add_or_mult(1_e, 2_e, 1_e, 2_e, Rational::Multiplication, 1_e, 4_e);
  assert_add_or_mult(23515_e, 7_e, 2_e, 23515_e, Rational::Multiplication, 2_e,
                     7_e);
}

static void assert_power(const Tree* iNumerator, const Tree* iDenominator,
                         const Tree* j, const Tree* resNumerator,
                         const Tree* resDenominator) {
  Tree* i = Rational::Push(iNumerator, iDenominator);
  Tree* expected = Rational::Push(resNumerator, resDenominator);
  Rational::MakeIrreducible(expected);
  assert_operation(i, j, Rational::IntegerPower, expected);
  expected->removeTree();
  i->removeTree();
}

QUIZ_CASE(pcj_rational_integer_power) {
  assert_power(3_e, 2_e, 3_e, 27_e, 8_e);
  assert_power(1_e, 2_e, 10_e, 1_e, 1024_e);
  assert_power(7123_e, 3_e, 2_e, 50737129_e, 9_e);
}
