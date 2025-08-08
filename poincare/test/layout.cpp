#include <ion/display.h>
#include <omg/unicode_helper.h>
#include <poincare/expression.h>
#include <poincare/old/empty_context.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/units/k_units.h>
#include <poincare/src/layout/grid.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/layout_serializer.h>
#include <poincare/src/layout/layout_span_decoder.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/multiplication_symbol.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/layout/rack_layout_decoder.h>
#include <poincare/src/layout/render.h>
#include <poincare/src/memory/pattern_matching.h>

#include "helper.h"

using namespace Poincare::Internal;

void assert_expression_layouts_as(const Tree* expression, const Tree* layout,
                                  bool linearMode = false,
                                  bool compactMode = false,
                                  int numberOfSignificantDigits = -1,
                                  Preferences::PrintFloatMode floatMode =
                                      Preferences::PrintFloatMode::Decimal,
                                  OMG::Base base = OMG::Base::Decimal) {
  Tree* l = Layouter::LayoutExpression(expression->cloneTree(), linearMode,
                                       compactMode, numberOfSignificantDigits,
                                       floatMode, base);
  assert_trees_are_equal(l, layout);
  l->removeTree();
}

void assert_parsed_expression_layouts_to(const char* expression,
                                         const Tree* expectedLayout) {
  Tree* e = parse(expression);
  Tree* l = Layouter::LayoutExpression(e);
  quiz_assert_print_if_failure(l->treeIsIdenticalTo(expectedLayout),
                               expression);
}

QUIZ_CASE(pcj_expression_to_layout) {
  assert_expression_layouts_as(KAdd(1_e, 2_e, 3_e), "1+2+3"_l);
  assert_expression_layouts_as(KSub(KAdd(1_e, 2_e), 3_e), "1+2-3"_l);
  assert_expression_layouts_as(KSub(1_e, KAdd(2_e, 3_e)),
                               "1-"_l ^ KParenthesesL("2+3"_l));
  assert_expression_layouts_as(
      KPow(KAdd("x"_e, "y"_e), 2_e),
      KRackL(KParenthesesL("x+y"_l), KSuperscriptL("2"_l)));
  assert_expression_layouts_as(
      KPow(KMult("x"_e, "y"_e), 2_e),
      KRackL(KParenthesesL("x·y"_l), KSuperscriptL("2"_l)));
  assert_expression_layouts_as(KAdd(KMixedFraction(2_e, KDiv(1_e, 3_e)), 4_e),
                               "2 1/3+4"_l, true);
  // 12 345 - 54 321
  const Tree* expected = "12"_l ^ KThousandsSeparatorL ^ "345"_l ^
                         KOperatorSeparatorL ^ "-"_l ^ KOperatorSeparatorL ^
                         "54"_l ^ KThousandsSeparatorL ^ "321"_l;
  assert_expression_layouts_as(KAdd(12345_e, KOpposite(54321_e)), expected,
                               false);
  assert_expression_layouts_as(KAdd(12345_de, -54321_de), expected, false);

  assert_expression_layouts_as(9_e, "0b1001"_l, false, false, -1,
                               Preferences::PrintFloatMode::Decimal,
                               OMG::Base::Binary);
  assert_expression_layouts_as(54321_e, "0b1101010000110001"_l, false, false,
                               -1, Preferences::PrintFloatMode::Decimal,
                               OMG::Base::Binary);
  assert_expression_layouts_as(54321_e, "0xD431"_l, false, false, -1,
                               Preferences::PrintFloatMode::Decimal,
                               OMG::Base::Hexadecimal);
  assert_expression_layouts_as(-123_e, "-123"_l, false, false, -1,
                               Preferences::PrintFloatMode::Decimal,
                               OMG::Base::Decimal);
  assert_expression_layouts_as(KAdd("x"_e, KOpposite(KAdd("y"_e, "z"_e))),
                               "x-"_l ^ KParenthesesL("y+z"_l));
  assert_expression_layouts_as(KListSlice("L"_e, 2_e, 3_e),
                               "L"_l ^ KParenthesesL("2,3"_l));

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
  LayoutSerializer::Serialize(layout, buffer);
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

QUIZ_CASE(pcj_expression_to_layout_multiplication_operator) {
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

QUIZ_CASE(pcj_expression_to_layout_implicit_addition) {
  assert_expression_layouts_and_serializes_to(
      KAdd(KMult(2_e, KUnits::hour), KMult(3_e, KUnits::minute),
           KMult(4.5_e, KUnits::second)),
      "2×_h+3×_min+4.5×_s", "2h3min4.5s");
  assert_expression_layouts_and_serializes_to(
      KAdd(KMult(2_e, KUnits::hour), KMult(3_e, KUnits::minute),
           KMult(KDecimal(4_e, -30_e), KUnits::second)),
      "2×_h+3×_min+4ᴇ30×_s", "2h+3min+4ᴇ30s");
}

QUIZ_CASE(pcj_expression_to_layout_mixed_fraction) {
  assert_expression_layouts_and_serializes_to(
      KMixedFraction(1_e, KDiv(2_e, 3_e)), "1 2/3", "1(2/3)");
  assert_expression_layouts_and_serializes_to(
      KDiv(KMixedFraction(1_e, KDiv(2_e, 3_e)), 4_e), "(1 2/3)/4",
      "(1(2/3))/4");
}

void assert_parsed_expression_layout_serialize_to_self(
    const char* expressionLayout) {
  Tree* e = parse(expressionLayout);
  Tree* l = Layouter::LayoutExpression(e);
  constexpr size_t bufferSize = 256;
  char buffer[bufferSize];
  LayoutSerializer::Serialize(l, buffer);
  quiz_assert_print_if_failure(strcmp(expressionLayout, buffer) == 0,
                               expressionLayout);
}

QUIZ_CASE(pcj_expression_to_layout_serializes_to_self) {
  assert_parsed_expression_layout_serialize_to_self("binomial((7),(6))");
  assert_parsed_expression_layout_serialize_to_self("root((7),(3))");
}

QUIZ_CASE(pcj_layout_decoder) {
  const Tree* l = "123"_l;
  CPLayoutDecoder d(l->child(0), 0, 3);
  quiz_assert(d.nextCodePoint() == '1');
  quiz_assert(d.nextCodePoint() == '2');
  quiz_assert(d.nextCodePoint() == '3');
  quiz_assert(d.nextCodePoint() == 0);
}

QUIZ_CASE(pcj_omg_code_point) {
  LayoutSpan l(MakeSpan(Rack::From("123"_l)));
  quiz_assert(CodePointSearch(l, '2') == 1);
  quiz_assert(CodePointSearch(l, '4') == l.size());
}

#if 0
QUIZ _CASE(pcj_layout_render) {
  KDContext* ctx = Ion::Display::Context::SharedContext;
  // TODO layoutCursor is nullptr and expected
  Layout l = Layout(
      KRackL("1+"_l,
             KParenthesesL(KRackL(
                 "2×"_l, KParenthesesL(KRackL("1+"_l, KFracL("1"_l, "2"_l))))),
             /*KSuperscriptL("2"_l),*/ "-2"_l));
  l.draw(ctx, KDPoint(10, 100), KDFont::Size::Large);
}
#endif

QUIZ_CASE(pcj_layout_multiplication_symbol) {
  quiz_assert(MultiplicationSymbol(KMult(2_e, 3_e), false) == u'×');
  quiz_assert(MultiplicationSymbol(KMult(2_e, "a"_e), false) == UCodePointNull);
  quiz_assert(MultiplicationSymbol(KMult(2_e, KCos(π_e), KSqrt(2_e)), false) ==
              u'·');
}

QUIZ_CASE(pcj_k_matrix_l) {
  const Grid* mat = Grid::From(KMatrix1x1L("a"_l));
  quiz_assert(mat->numberOfColumns() == 2);
  quiz_assert(mat->numberOfRows() == 2);
  quiz_assert(mat->childAt(0, 0)->treeIsIdenticalTo("a"_l));

  mat = Grid::From(KMatrix2x2L("a"_l, "b"_l, "c"_l, "d"_l));
  quiz_assert(mat->numberOfColumns() == 3);
  quiz_assert(mat->numberOfRows() == 3);
  quiz_assert(mat->childAt(1, 1)->treeIsIdenticalTo("d"_l));
}

QUIZ_CASE(pcj_layout_serialization) {
  // Log
  assert_layout_serializes_to(KRackL(KAbsL("42log"_l ^ KSubscriptL("123"_l) ^
                                           KParenthesesLeftTempL("x"_l))),
                              "abs(42log(x,123))");
  assert_layout_serializes_to("1+"_l ^ KPrefixSuperscriptL("abc"_l) ^ "log"_l ^
                                  KParenthesesLeftTempL("x"_l),
                              "1+log(x,abc)");

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
  assert_layout_serializes_to(("2"_cl ^ KSuperscriptL("x+5"_l)), "2^(x+5)");

  // Piecewise
  assert_layout_serializes_to(
      KRackL(KPiecewise3L("3"_l, "2>3"_l, "2"_l, "2<3"_l, "1"_l, ""_l)),
      "piecewise(3,2>3,2,2<3,1)");
}

void assert_layout_is_not_parsed(Poincare::Layout layout) {
  using namespace Poincare;
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  LayoutSerializer::Serialize(layout, buffer);
  UserExpression e = UserExpression::Parse(buffer, EmptyContext{});
  quiz_assert_print_if_failure(e.isUninitialized(), buffer);
}

QUIZ_CASE(pcj_layout_to_expression_unparsable) {
  {
    /*   d     |
     * -----(x)|
     * d 1+x   |1+x=3
     */
    Poincare::Layout l = KRackL(KDiffL("1+x"_l, "3"_l, "1"_l, "x"_l));
    assert_layout_is_not_parsed(l);
  }
  {
    /*   d^2     |
     * -------(x)|
     * d 1+x^2   |1+x=3
     */
    Poincare::Layout l = KRackL(KNthDiffL("1+x"_l, "3"_l, "2"_l, "x"_l));
    assert_layout_is_not_parsed(l);
  }
}

void assert_parsed_layout_is(Poincare::Layout layout,
                             const Poincare::Internal::Tree* rj) {
  using namespace Poincare;
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  LayoutSerializer::Serialize(layout, buffer);
  Internal::Tree* ej = Internal::Parser::Parse(layout.tree()->cloneTree());
  quiz_assert_print_if_failure(ej, buffer);
  quiz_assert_print_if_failure(ej->treeIsIdenticalTo(rj), buffer);
  Internal::TreeStack::SharedTreeStack->flush();
}

QUIZ_CASE(pcj_layout_to_expression_parsable) {
  Poincare::Layout l;
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

QUIZ_CASE(pcj_layout_constructors) {
  using namespace Poincare::Internal::KTrees;
  Poincare::Layout e0 = KRackL();
  Poincare::Layout e1 = Poincare::Layout::Create(KRackL(KAbsL(KA)), {.KA = e0});
  Poincare::Layout e2 = "a"_l;
  Poincare::Layout e3 = Poincare::Layout::Create(KRackL(KBinomialL(KA, KB)),
                                                 {.KA = e1, .KB = e2});
  Poincare::Layout e4 =
      Poincare::Layout::Create(KRackL(KCeilL(KA)), {.KA = e3});
  Poincare::Layout e5 = KRackL(KParenthesesL(KRackL()));
  Poincare::Layout e7 = Poincare::Layout::Create(
      KRackL(KCondensedSumL(KA, KB, KC)), {.KA = e4, .KB = e5, .KC = e3});
  Poincare::Layout e8 =
      Poincare::Layout::Create(KRackL(KConjL(KA)), {.KA = e7});
  Poincare::Layout e10 = KRackL(KCurlyBracesL(KRackL()));
  Poincare::Layout e11 =
      Poincare::Layout::Create(KRackL(KFloorL(KA)), {.KA = e10});
  Poincare::Layout e12 = Poincare::Layout::Create(KRackL(KBinomialL(KA, KB)),
                                                  {.KA = e8, .KB = e11});
  Poincare::Layout e13 = KRackL();
  Poincare::Layout e15 =
      Poincare::Layout::Create(KRackL(KIntegralL(KA, KB, KC, KD)),
                               {.KA = e11, .KB = e12, .KC = e13, .KD = e10});
  Poincare::Layout e16 =
      Poincare::Layout::Create(KRackL(KSqrtL(KA)), {.KA = e15});
  Poincare::Layout e17 = KRackL(KEmptyMatrixL);
  Poincare::Layout e18 = KRackL();
  Poincare::Layout e19 = KRackL();
  Poincare::Layout e20 = KRackL();
  Poincare::Layout e21 =
      Poincare::Layout::Create(KRackL(KProductL(KA, KB, KC, KD)),
                               {.KA = e17, .KB = e18, .KC = e19, .KD = e20});
  Poincare::Layout e22 = KRackL();
  Poincare::Layout e23 = KRackL();
  Poincare::Layout e24 = KRackL();
  Poincare::Layout e25 =
      Poincare::Layout::Create(KRackL(KSumL(KA, KB, KC, KD)),
                               {.KA = e21, .KB = e22, .KC = e23, .KD = e24});
  Poincare::Layout e26 =
      Poincare::Layout::Create(KRackL(KSuperscriptL(KA)), {.KA = e25});
  Poincare::Layout e27 = "t"_l;
  [[maybe_unused]] Poincare::Layout e28 =
      Poincare::Layout::Create(KRackL(KDiffL(KA, KB, KC, KD)),
                               {.KA = e15, .KB = e27, .KC = e0, .KD = e26});
  [[maybe_unused]] Poincare::Layout e29 =
      Poincare::Layout::Create(KRackL(KNthDiffL(KA, KB, KC, KD)),
                               {.KA = e15, .KB = e27, .KC = e26, .KD = e21});
  [[maybe_unused]] Poincare::Layout e30 = "Hé"_l;
  // TODO: Layout e31 = KPiecewiseL();
}

QUIZ_CASE(pcj_layout_comparison) {
  Poincare::Layout e0 = "a"_l;
  Poincare::Layout e1 = "a"_l;
  Poincare::Layout e2 = "b"_l;
  quiz_assert(e0.isIdenticalTo(e1));
  quiz_assert(!e0.isIdenticalTo(e2));

  Poincare::Layout e3 = KRackL();
  Poincare::Layout e4 = KRackL();
  quiz_assert(e3.isIdenticalTo(e4));
  quiz_assert(!e3.isIdenticalTo(e0));

  Poincare::Layout e5 =
      Poincare::Layout::Create(KRackL(KSqrtL(KA)), {.KA = e0});
  Poincare::Layout e6 =
      Poincare::Layout::Create(KRackL(KSqrtL(KA)), {.KA = e1});
  Poincare::Layout e7 =
      Poincare::Layout::Create(KRackL(KSqrtL(KA)), {.KA = e2});
  quiz_assert(e5.isIdenticalTo(e6));
  quiz_assert(!e5.isIdenticalTo(e7));
  quiz_assert(!e5.isIdenticalTo(e0));

  Poincare::Layout e8 =
      Poincare::Layout::Create(KRackL(KSuperscriptL(KA)), {.KA = e5});
  Poincare::Layout e9 =
      Poincare::Layout::Create(KRackL(KSuperscriptL(KA)), {.KA = e6});
  Poincare::Layout e10 = KRackL(KSubscriptL(KRackL(KSqrtL("a"_l))));
  quiz_assert(e8.isIdenticalTo(e9));
  quiz_assert(!e8.isIdenticalTo(e10));
  quiz_assert(!e8.isIdenticalTo(e0));

  Poincare::Layout e11 = Poincare::Layout::Create(
      KRackL(KSumL(KA, KB, KC, KD)), {.KA = e0, .KB = e3, .KC = e6, .KD = e2});
  Poincare::Layout e12 =
      KRackL(KSumL("a"_l, KRackL(), KRackL(KSqrtL("a"_l)), "b"_l));
  Poincare::Layout e13 =
      KRackL(KProductL("a"_l, KRackL(), KRackL(KSqrtL("a"_l)), "b"_l));
  quiz_assert(e11.isIdenticalTo(e12));
  quiz_assert(!e11.isIdenticalTo(e13));
}
