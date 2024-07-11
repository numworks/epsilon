#include <poincare/src/layout/k_tree.h>
#include <poincare/src/memory/tree_stack.h>

#include "helper.h"

using namespace Poincare::Internal;

QUIZ_CASE(poincare_layout_serialization) {
  quiz_assert(UCodePointLeftSystemParenthesis == 0x12);
  quiz_assert(UCodePointRightSystemParenthesis == 0x13);

  // Abs
  assert_layout_serializes_to(KRackL(KAbsL("8"_l))->cloneTree(),
                              "abs\u00128\u0013");

  // Binomial
  assert_layout_serializes_to(KRackL(KBinomialL("7"_l, "6"_l))->cloneTree(),
                              "binomial\u00127,6\u0013");

  // Bracket and BracketPairLayout -> Tested by other layouts

  // Ceil
  assert_layout_serializes_to(KRackL(KCeilL("8"_l))->cloneTree(),
                              "ceil\u00128\u0013");

  // CodePoint -> Tested by most other layouts

  // CondensedSum -> No serialization

  // Conj
  assert_layout_serializes_to(KRackL(KConjL("1"_l))->cloneTree(),
                              "conj\u00121\u0013");

  // CurlyBraces
  assert_layout_serializes_to(KRackL(KCurlyBracesL(KRackL()))->cloneTree(),
                              "{}");

  // Diff
  assert_layout_serializes_to(KRackL(KDiffL("x"_l, "a"_l, "f"_l))->cloneTree(),
                              "diff\u0012f,x,a\u0013");
  assert_layout_serializes_to(
      KRackL(KNthDiffL("x"_l, "a"_l, "f"_l, "n"_l))->cloneTree(),
      "diff\u0012f,x,a,n\u0013");

  // Empty
  assert_layout_serializes_to(KRackL()->cloneTree(), "");

  // Floor
  assert_layout_serializes_to(KRackL(KFloorL("8"_l))->cloneTree(),
                              "floor\u00128\u0013");

  // Fraction
  assert_layout_serializes_to(KRackL(KFracL("1"_l, "2+3"_l))->cloneTree(),
                              "1/\u00122+3\u0013");

  // GridPair -> Tested by Matrix

  // Rack
  assert_layout_serializes_to(
      KRackL("a"_cl, "b"_cl, "c"_cl, "d"_cl)->cloneTree(), "abcd");

  // Integral
  assert_layout_serializes_to(
      KRackL(KIntegralL("x"_l, "2"_l, "3"_l, "f"_l))->cloneTree(),
      "int\u0012f,x,2,3\u0013");

  // Matrix
  assert_layout_serializes_to(
      KMatrix2x2L("a"_l, "b"_l, "c"_l, "d"_l)->cloneTree(), "[[a,b][c,d]]");

  // Nth root
  assert_layout_serializes_to(KRootL("7"_l, "6"_l)->cloneTree(),
                              "root\u00127,6\u0013");

  // Parenthesis
  assert_layout_serializes_to(KRackL(KParenthesesL(KRackL()))->cloneTree(),
                              "()");

  // Product
  assert_layout_serializes_to(
      KRackL(KProductL("x"_l, "2"_l, "3"_l, "f"_l))->cloneTree(),
      "product\u0012f,x,2,3\u0013");

  // Sequence -> Tested by Sum and Product

  // Sum
  assert_layout_serializes_to(
      KRackL(KSumL("x"_l, "2"_l, "3"_l, "1+1"_l))->cloneTree(),
      "sum\u00121+1,x,2,3\u0013");

  // Vertical offset
  assert_layout_serializes_to(("2"_cl ^ KSuperscriptL("x+5"_l))->cloneTree(),
                              "2^\x12x+5\x13");

  // Piecewise
  Tree* p = SharedTreeStack->pushPiecewiseLayout(4, 2);
  "3"_l->cloneTree();
  "2>3"_l->cloneTree();
  "2"_l->cloneTree();
  "2<3"_l->cloneTree();
  "1"_l->cloneTree();
  KRackL()->cloneTree();
  KRackL()->cloneTree();  // last empty row for input
  KRackL()->cloneTree();  // last empty row for input
  assert_layout_serializes_to(p, "piecewise(3,2>3,2,2<3,1)");
}
