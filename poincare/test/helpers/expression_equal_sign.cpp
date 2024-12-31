#include <poincare/src/expression/equal_sign.h>
#include <poincare/src/expression/k_tree.h>

#include "../helper.h"

using namespace Poincare::Internal;

QUIZ_CASE(pcj_exact_and_approximate_are_equal) {
  QUIZ_ASSERT(ExactAndApproximateExpressionsAreStrictlyEqual(2_e, 2.0_fe));
  QUIZ_ASSERT(ExactAndApproximateExpressionsAreStrictlyEqual(-2_e, -2.0_fe));
  QUIZ_ASSERT(
      ExactAndApproximateExpressionsAreStrictlyEqual(1_e / 2_e, 0.5_fe));
  QUIZ_ASSERT(!ExactAndApproximateExpressionsAreStrictlyEqual(
      1_e / 3_e, 0.3333333333333_fe));
  QUIZ_ASSERT(ExactAndApproximateExpressionsAreStrictlyEqual(
      KAdd(2_e, KMult(1_e / 2_e, i_e)), KAdd(2.0_fe, KMult(0.5_fe, i_e))));
  QUIZ_ASSERT(!ExactAndApproximateExpressionsAreStrictlyEqual(
      KAdd(1_e, π_e, i_e), KAdd(4.141592654_fe, i_e)));
  // TODO: add tests with large decimals, floats with E etc
}
