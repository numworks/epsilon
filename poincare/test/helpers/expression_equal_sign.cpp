#include <poincare/src/expression/equal_sign.h>
#include <poincare/src/expression/k_tree.h>

#include "../helper.h"

using namespace Poincare::Internal;

QUIZ_CASE(pcj_exact_and_approximate_are_equal) {
  QUIZ_ASSERT(IsCalculationOutputStrictEquality(2_e, 2.0_fe, 10));
  QUIZ_ASSERT(IsCalculationOutputStrictEquality(-2_e, -2.0_fe, 10));
  QUIZ_ASSERT(IsCalculationOutputStrictEquality(1_e / 2_e, 0.5_fe, 10));
  QUIZ_ASSERT(
      !IsCalculationOutputStrictEquality(1_e / 3_e, 0.3333333333333_fe, 10));
  QUIZ_ASSERT(IsCalculationOutputStrictEquality(
      KAdd(2_e, KMult(1_e / 2_e, i_e)), KAdd(2.0_fe, KMult(0.5_fe, i_e)), 10));
  QUIZ_ASSERT(!IsCalculationOutputStrictEquality(
      KAdd(1_e, π_e, i_e), KAdd(4.141592654_fe, i_e), 10));

  QUIZ_ASSERT(!IsCalculationOutputStrictEquality(0.9999999999_e, 1_e, 3));

  // TODO: add tests with large decimals, floats with E etc
}
