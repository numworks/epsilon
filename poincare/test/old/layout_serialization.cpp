#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/serialize.h>
#include <poincare/src/memory/tree_stack.h>
#include <quiz.h>

#include "helper.h"

using namespace Poincare::Internal;

QUIZ_CASE(poincare_layout_serialization) {
  quiz_assert(UCodePointLeftSystemParenthesis == 0x12);
  quiz_assert(UCodePointRightSystemParenthesis == 0x13);

  // Abs
  assert_layout_serializes_to(KRackL(KAbsL("8"_l)), "abs(8)");

  // Binomial
  assert_layout_serializes_to(KRackL(KBinomialL("7"_l, "6"_l)),
                              "binomial(7,6)");

  // Bracket and BracketPairLayout -> Tested by other layouts

  // Ceil
  assert_layout_serializes_to(KRackL(KCeilL("8"_l)), "ceil(8)");

  // CodePoint -> Tested by most other layouts

  // CondensedSum -> No serialization

  // Conj
  assert_layout_serializes_to(KRackL(KConjL("1"_l)), "conj(1)");

  // CurlyBraces
  assert_layout_serializes_to(KRackL(KCurlyBracesL(KRackL())), "{}");

  // Diff
  assert_layout_serializes_to(KRackL(KDiffL("x"_l, "a"_l, "1"_l, "f"_l)),
                              "diff(f,x,a)");
  assert_layout_serializes_to(KRackL(KNthDiffL("x"_l, "a"_l, "n"_l, "f"_l)),
                              "diff(f,x,a,n)");

  // Empty
  assert_layout_serializes_to(KRackL(), "");

  // Floor
  assert_layout_serializes_to(KRackL(KFloorL("8"_l)), "floor(8)");

  // Fraction
  assert_layout_serializes_to(KRackL(KFracL("1"_l, "2+3"_l)), "1/(2+3)");

  // Rack
  assert_layout_serializes_to(KRackL("a"_cl, "b"_cl, "c"_cl, "d"_cl), "abcd");

  // Integral
  assert_layout_serializes_to(KRackL(KIntegralL("x"_l, "2"_l, "3"_l, "f"_l)),
                              "int(f,x,2,3)");

  // Matrix
  assert_layout_serializes_to(KMatrix2x2L("a"_l, "b"_l, "c"_l, "d"_l),
                              "[[a,b][c,d]]");

  // Nth root
  assert_layout_serializes_to(KRootL("7"_l, "6"_l), "root(7,6)");

  // Parenthesis
  assert_layout_serializes_to(KRackL(KParenthesesL(KRackL())), "()");

  // Product
  assert_layout_serializes_to(KRackL(KProductL("x"_l, "2"_l, "3"_l, "f"_l)),
                              "product(f,x,2,3)");

  // Sum
  assert_layout_serializes_to(KRackL(KSumL("x"_l, "2"_l, "3"_l, "1+1"_l)),
                              "sum(1+1,x,2,3)");

  // Vertical offset
  assert_layout_serializes_to(("2"_cl ^ KSuperscriptL("x+5"_l)),
                              "2^\x12x+5\x13");

  // Piecewise
  assert_layout_serializes_to(
      KRackL(KPiecewise3L("3"_l, "2>3"_l, "2"_l, "2<3"_l, "1"_l, ""_l)),
      "piecewise(3,2>3,2,2<3,1)");
}
