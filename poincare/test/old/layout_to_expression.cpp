#include <poincare/src/expression/conversion.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/parser.h>

#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal::KTrees;

void assert_layout_is_not_parsed(Layout l) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  l.serialize(buffer, bufferSize);
  Expression e = Expression::Parse(buffer, nullptr, false);
  quiz_assert_print_if_failure(e.isUninitialized(), buffer);
}

QUIZ_CASE(poincare_layout_to_expression_unparsable) {
  {
    /*   d     |
     * -----(x)|
     * d 1+x   |1+x=3
     */
    Layout l = KRackL(KDiffL("1+x"_l, "3"_l, "1"_l, "x"_l));
    assert_layout_is_not_parsed(l);
  }
  {
    /*   d^2     |
     * -------(x)|
     * d 1+x^2   |1+x=3
     */
    Layout l = KRackL(KNthDiffL("1+x"_l, "3"_l, "2"_l, "x"_l));
    assert_layout_is_not_parsed(l);
  }
}

void assert_parsed_layout_is(Layout l, const Poincare::Internal::Tree* rj) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  l.serialize(buffer, bufferSize);

  Internal::Tree* ej = Internal::Parser::Parse(l.tree()->cloneTree(), nullptr);
  quiz_assert_print_if_failure(ej, buffer);
  quiz_assert_print_if_failure(ej->treeIsIdenticalTo(rj), buffer);
  Internal::TreeStack::SharedTreeStack->flush();
}

QUIZ_CASE(poincare_layout_to_expression_parsable) {
  Layout l;
  const Poincare::Internal::Tree* e;

  // 1+2
  l = "1+2"_l;
  e = KAdd(1_e, 2_e);
  assert_parsed_layout_is(l, e);

  // |3+3/6|
  l = KRackL(KAbsL("3+"_l ^ KFracL("3"_l, "6"_l)));
  e = KAbs(KAdd(3_e, KDiv(3_e, 6_e)));
  assert_parsed_layout_is(l, e);

  // binCoef(4,5)
  l = KRackL(KBinomialL("4"_l, "5"_l));
  e = KBinomial(4_e, 5_e);
  assert_parsed_layout_is(l, e);

  // ceil(4.6)
  l = KRackL(KCeilL("4.6"_l));
  e = KCeil(4.6_e);
  assert_parsed_layout_is(l, e);

  // floor(7.2)
  l = KRackL(KFloorL("7.2"_l));
  e = KFloor(7.2_e);
  assert_parsed_layout_is(l, e);

  // 2^(3+4)
  l = "2"_l ^ KSuperscriptL("3+4"_l);
  e = KPow(2_e, KAdd(3_e, 4_e));
  assert_parsed_layout_is(l, e);

  // 2^(3+4)!
  l = "2"_l ^ KSuperscriptL("3+4"_l) ^ "!"_l;
  e = KFact(KPow(2_e, KAdd(3_e, 4_e)));
  assert_parsed_layout_is(l, e);

  // 2^3^4
  l = "2"_l ^ KSuperscriptL("3"_l) ^ KSuperscriptL("4"_l);
  e = KPow(2_e, KPow(3_e, 4_e));
  assert_parsed_layout_is(l, e);

  // log_3(2)
  l = "log"_l ^ KSubscriptL("3"_l) ^ KParenthesesL("2"_l);
  e = KLogBase(2_e, 3_e);
  assert_parsed_layout_is(l, e);

  /* 3
   *  log(2) */
  l = KPrefixSuperscriptL("3"_l) ^ "log"_l ^ KParenthesesL("2"_l);
  e = KLogBase(2_e, 3_e);
  assert_parsed_layout_is(l, e);

  // root(5,3)
  l = KRackL(KRootL("5"_l, "3"_l));
  e = KRoot(5_e, 3_e);
  assert_parsed_layout_is(l, e);

  // int(7, x, 4, 5)
  l = KRackL(KIntegralL("x"_l, "4"_l, "5"_l, "7"_l));
  e = KIntegral("x"_e, 4_e, 5_e, 7_e);
  assert_parsed_layout_is(l, e);

  // 2^2 !
  l = "2"_l ^ KSuperscriptL("2"_l) ^ "!"_l;
  e = KFact(KPow(2_e, 2_e));
  assert_parsed_layout_is(l, e);

  // 5* 6/(7+5) *3
  l = "5"_l ^ KFracL("6"_l, "7+5"_l) ^ "3"_l;
  e = KMult(5_e, KDiv(6_e, KAdd(7_e, 5_e)), 3_e);
  assert_parsed_layout_is(l, e);

  // [[3^2!, 7][4,5]
  l = KRackL(
      KMatrix2x2L("3"_l ^ KSuperscriptL("2"_l) ^ "!"_l, "7"_l, "4"_l, "5"_l));
  e = KMatrix<2, 2>(KFact(KPow(3_e, 2_e)), 7_e, 4_e, 5_e);
  assert_parsed_layout_is(l, e);

  // 2^det([[3!, 7][4,5])
  l = "2"_l ^ KSuperscriptL("det"_l ^ KParenthesesL(KRackL(KMatrix2x2L(
                                          "3!"_l, "7"_l, "4"_l, "5"_l))));
  e = KPow(2_e, KDet(KMatrix<2, 2>(KFact(3_e), 7_e, 4_e, 5_e)));
  assert_parsed_layout_is(l, e);

  // 2e^3
  l = "2e"_l ^ KSuperscriptL("3"_l);
  e = KMult(2_e, KPow(e_e, 3_e));
  assert_parsed_layout_is(l, e);

  // x|2|
  l = "x"_l ^ KAbsL("2"_l);
  e = KMult("x"_e, KAbs(2_e));
  assert_parsed_layout_is(l, e);

  // x int(7, x, 4, 5)
  l = "x"_l ^ KIntegralL("x"_l, "4"_l, "5"_l, "7"_l);
  e = KMult("x"_e, KIntegral("x"_e, 4_e, 5_e, 7_e));
  assert_parsed_layout_is(l, e);

  // diff(1/Var, Var, cos(2))
  l = KRackL(
      KDiffL("Var"_l, "cos(2)"_l, "1"_l, KRackL(KFracL("1"_l, "Var"_l))));
  e = KDiff("Var"_e, KCos(2_e), 1_e, KDiv(1_e, "Var"_e));
  assert_parsed_layout_is(l, e);

  // diff(1/Var, Var, cos(2), 2)
  l = KRackL(
      KNthDiffL("Var"_l, "cos(2)"_l, "2"_l, KRackL(KFracL("1"_l, "Var"_l))));
  e = KDiff("Var"_e, KCos(2_e), 2_e, KDiv(1_e, "Var"_e));
  assert_parsed_layout_is(l, e);

  // Piecewise
  l = KRackL(KPiecewise3L("3"_l, "2>3"_l, "2"_l, "2<3"_l, "1"_l, ""_l));
  e = KPiecewise(3_e, KSuperior(2_e, 3_e), 2_e, KInferior(2_e, 3_e), 1_e);
  assert_parsed_layout_is(l, e);
}
