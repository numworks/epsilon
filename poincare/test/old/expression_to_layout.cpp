#include <poincare/src/expression/units/k_units.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/serialize.h>

#include "../helper.h"

using namespace Poincare;
using namespace Poincare::Internal;
using namespace Poincare::Internal::KTrees;

void assert_parsed_expression_layouts_to(const char* expression,
                                         const Tree* expectedLayout) {
  Tree* e = parse(expression, nullptr);
  Tree* l = Layouter::LayoutExpression(e);
  quiz_assert_print_if_failure(l->treeIsIdenticalTo(expectedLayout),
                               expression);
}

QUIZ_CASE(poincare_expression_to_layout) {
  assert_parsed_expression_layouts_to("abs(1)", KRackL(KAbsL("1"_l)));
  assert_parsed_expression_layouts_to("binomial(1,2)",
                                      KRackL(KBinomialL("1"_l, "2"_l)));
  assert_parsed_expression_layouts_to(
      "[[1,2][3,4]]", KRackL(KMatrix2x2L("1"_l, "2"_l, "3"_l, "4"_l)));
  assert_parsed_expression_layouts_to("1+2", "1+2"_l);
  assert_parsed_expression_layouts_to("ceil(1)", KRackL(KCeilL("1"_l)));
  assert_parsed_expression_layouts_to("conj(1)", KRackL(KConjL("1"_l)));
  assert_parsed_expression_layouts_to("floor(1)", KRackL(KFloorL("1"_l)));
  assert_parsed_expression_layouts_to("1/2", KRackL(KFracL("1"_l, "2"_l)));
  assert_parsed_expression_layouts_to(
      "int(1,x,2,3)", KRackL(KIntegralL("x"_l, "2"_l, "3"_l, "1"_l)));
  // PCJ expressions do not preserve parentheses
  assert_parsed_expression_layouts_to("(1)", KRackL(KParenthesesL("1"_l)));
  assert_parsed_expression_layouts_to("√(1)", KRackL(KSqrtL("1"_l)));
  assert_parsed_expression_layouts_to("root(1,2)",
                                      KRackL(KRootL("1"_l, "2"_l)));
  assert_parsed_expression_layouts_to(
      "sum(1,n,2,3)", KRackL(KSumL("n"_l, "2"_l, "3"_l, "1"_l)));
  assert_parsed_expression_layouts_to(
      "product(1,n,2,3)", KRackL(KProductL("n"_l, "2"_l, "3"_l, "1"_l)));
  assert_parsed_expression_layouts_to("1^2", "1"_l ^ KSuperscriptL("2"_l));
  assert_parsed_expression_layouts_to(
      "-1^2", "-"_l ^ KParenthesesL("1"_l ^ KSuperscriptL("2"_l)));
  assert_parsed_expression_layouts_to("-x^2", "-x"_l ^ KSuperscriptL("2"_l));
}

void assert_expression_layouts_and_serializes_to(const Tree* expression,
                                                 const char* serialization,
                                                 bool linearMode) {
  Tree* layout =
      Layouter::LayoutExpression(expression->cloneTree(), linearMode);
  quiz_assert(layout);
  constexpr size_t bufferSize = 256;
  char buffer[bufferSize];
  *Serialize(layout, buffer, buffer + bufferSize) = 0;
  quiz_assert(strcmp(serialization, buffer) == 0);
  layout->removeTree();
}

void assert_expression_layouts_and_serializes_to(const Tree* expression,
                                                 const char* serialization1D,
                                                 const char* serialization2D) {
  assert_expression_layouts_and_serializes_to(expression, serialization1D,
                                              true);
  assert_expression_layouts_and_serializes_to(expression, serialization2D,
                                              false);
}

QUIZ_CASE(poincare_expression_to_layout_multiplication_operator) {
  // Decimal x OneLetter
  assert_expression_layouts_and_serializes_to(KMult(KDecimal(2_e, 4_e), "a"_e),
                                              "2ᴇ-4×a", "2ᴇ-4a");
  // Decimal x Decimal
  assert_expression_layouts_and_serializes_to(
      KMult(KDecimal(2_e, 4_e), KDecimal(2_e, 4_e)), "2ᴇ-4×2ᴇ-4", "2ᴇ-4×2ᴇ-4");
  // Integer x Integer
  assert_expression_layouts_and_serializes_to(KMult(2_e, 1_e), "2×1", "2×1");
  // Integer x MoreLetters
  assert_expression_layouts_and_serializes_to(KMult(2_e, "abc"_e), "2×abc",
                                              "2·abc");
  // Integer x fraction
  assert_expression_layouts_and_serializes_to(KMult(2_e, KDiv(2_e, 3_e)),
                                              "2×2/3", "2×(2/3)");
  // BoundaryPunctuation x Integer
  assert_expression_layouts_and_serializes_to(KMult(KAbs(2_e), 1_e), "abs(2)×1",
                                              "abs(2)×1");
  // BoundaryPunctuation x OneLetter
  assert_expression_layouts_and_serializes_to(
      KMult(KBinomial(2_e, 3_e), "a"_e), "binomial(2,3)×a", "binomial(2,3)a");
  // BoundaryPunctuation x Root
  assert_expression_layouts_and_serializes_to(KMult(KCos(2_e), KSqrt(2_e)),
                                              "cos(2)×√(2)", "cos(2)√(2)");

  // 2√(2)
  assert_expression_layouts_and_serializes_to(KMult(2_e, KSqrt(2_e)), "2×√(2)",
                                              "2√(2)");
  // √(2)x2
  assert_expression_layouts_and_serializes_to(KMult(KSqrt(2_e), 2_e), "√(2)×2",
                                              "√(2)×2");
  // 2π
  assert_expression_layouts_and_serializes_to(KMult(2_e, π_e), "2×π", "2π");
  // π·i
  assert_expression_layouts_and_serializes_to(KMult(π_e, i_e), "π×i", "π·i");
  // conj(2)√(2)
  assert_expression_layouts_and_serializes_to(KMult(KConj(2_e), KSqrt(2_e)),
                                              "conj(2)×√(2)", "conj(2)√(2)");
  // √(2)a!
  assert_expression_layouts_and_serializes_to(KMult(KSqrt(2_e), KFact("a"_e)),
                                              "√(2)×a!", "√(2)a!");
  // a 2/3
  assert_expression_layouts_and_serializes_to(KMult("a"_e, KDiv(2_e, 3_e)),
                                              "a×2/3", "a(2/3)");
  // (1+π)a
  assert_expression_layouts_and_serializes_to(
      KMult(KParentheses(KAdd(1_e, π_e)), "a"_e), "(1+π)×a", "(1+π)a");
  // 2·root(2,a)
  assert_expression_layouts_and_serializes_to(KMult(2_e, KRoot(2_e, "a"_e)),
                                              "2×root(2,a)", "2·root(2,a)");

  // Operator contamination (if two operands needs x, all operands are separated
  // by a x) 1x2xa
  assert_expression_layouts_and_serializes_to(KMult(1_e, 2_e, "a"_e), "1×2×a",
                                              "1×2×a");
  // 2·π·a
  assert_expression_layouts_and_serializes_to(KMult(2_e, π_e, "a"_e), "2×π×a",
                                              "2·π·a");
  // 2(1+a)(1+π)
  assert_expression_layouts_and_serializes_to(
      KMult(2_e, KParentheses(KAdd(1_e, "a"_e)), KParentheses(KAdd(1_e, π_e))),
      "2×(1+a)×(1+π)", "2(1+a)(1+π)");

  /* Special case for units
   * When possible, do not display an operator between a value and its unit,
   * even if there is another operator defined for this multiplication. */
  // 2_m·3_m
  assert_expression_layouts_and_serializes_to(
      KMult(2_e, KUnits::meter, 3_e, KUnits::meter), "2×_m×3×_m", "2m·3m");
  // 2_m×3.5_m
  assert_expression_layouts_and_serializes_to(
      KMult(2_e, KUnits::meter, 3.5_fe, KUnits::meter), "2×_m×3.5×_m",
      "2m×3.5m");
  // Always put a dot between units
  // 2_s^-1·_m
  assert_expression_layouts_and_serializes_to(
      KMult(2_e, KPow(KUnits::second, -1_e), KUnits::meter), "2×_s^-1×_m",
      "2s^(-1)·m");
  // 2×3_m·_s^-1
  assert_expression_layouts_and_serializes_to(
      KMult(2_e, 3_e, KUnits::meter, KPow(KUnits::second, -1_e)),
      "2×3×_m×_s^-1", "2×3m·s^(-1)");
}

QUIZ_CASE(poincare_expression_to_layout_implicit_addition) {
  assert_expression_layouts_and_serializes_to(
      KAdd(KMult(2_e, KUnits::hour), KMult(3_e, KUnits::minute),
           KMult(4.5_e, KUnits::second)),
      "2×_h+3×_min+4.5×_s", "2h3min4.5s");
  assert_expression_layouts_and_serializes_to(
      KAdd(KMult(2_e, KUnits::hour), KMult(3_e, KUnits::minute),
           KMult(KDecimal(4_e, -30_e), KUnits::second)),
      "2×_h+3×_min+4ᴇ30×_s", "2h+3min+4ᴇ30s");
}

void assert_parsed_expression_layout_serialize_to_self(
    const char* expressionLayout) {
#if O
  OExpression e = parse_expression(expressionLayout, nullptr, true);
  OLayout el = e.createLayout(
      DecimalMode, PrintFloat::k_maxNumberOfSignificantDigits, nullptr);
  constexpr int bufferSize = 255;
  char buffer[bufferSize];
  el.serialize(buffer, bufferSize);
  quiz_assert_print_if_failure(strcmp(expressionLayout, buffer) == 0,
                               expressionLayout);
#endif
}

QUIZ_CASE(poincare_expression_to_layout_serializes_to_self) {
  assert_parsed_expression_layout_serialize_to_self("binomial((7),(6))");
  assert_parsed_expression_layout_serialize_to_self("root((7),(3))");
}

QUIZ_CASE(poincare_expression_to_layout_mixed_fraction) {
  assert_expression_layouts_and_serializes_to(
      KMixedFraction(1_e, KDiv(2_e, 3_e)), "1 2/3", "1(2/3)");
  assert_expression_layouts_and_serializes_to(
      KDiv(KMixedFraction(1_e, KDiv(2_e, 3_e)), 4_e), "(1 2/3)/4",
      "(1(2/3))/4");
}
