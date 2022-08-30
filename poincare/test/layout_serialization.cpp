#include <poincare_layouts.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_layout_serialization) {
  quiz_assert(UCodePointLeftSystemParenthesis == 0x12);
  quiz_assert(UCodePointRightSystemParenthesis == 0x13);

  // AbsoluteValueLayout
  assert_layout_serialize_to(
      AbsoluteValueLayout::Builder(CodePointLayout::Builder('8')),
      "abs\u00128\u0013");

  // BinomialCoefficientLayout
  assert_layout_serialize_to(
      BinomialCoefficientLayout::Builder(CodePointLayout::Builder('7'), CodePointLayout::Builder('6')),
      "binomial\u0012\u00127\u0013,\u00126\u0013\u0013");

  // Bracket and BracketPairLayout -> Tested by other layouts

  // CeilingLayout
  assert_layout_serialize_to(
      CeilingLayout::Builder(CodePointLayout::Builder('8')),
      "ceil\u00128\u0013");

  // CodePointLayout -> Tested by most other layouts

  // CondensedSumLayout -> No serialization

  // ConjugateLayout
  assert_layout_serialize_to(
      ConjugateLayout::Builder(CodePointLayout::Builder('1')),
      "conj\u00121\u0013");

  // CurlyBraceLayout
  assert_layout_serialize_to(
      CurlyBraceLayout::Builder(),
      "{}");

  // DerivativeLayout
  assert_layout_serialize_to(
      FirstOrderDerivativeLayout::Builder(CodePointLayout::Builder('f'), CodePointLayout::Builder('x'), CodePointLayout::Builder('a')),
      "diff\u0012\u0012f\u0013,\u0012x\u0013,\u0012a\u0013\u0013");
  assert_layout_serialize_to(
      HigherOrderDerivativeLayout::Builder(CodePointLayout::Builder('f'), CodePointLayout::Builder('x'), CodePointLayout::Builder('a'), CodePointLayout::Builder('n')),
      "diff\u0012\u0012f\u0013,\u0012x\u0013,\u0012a\u0013,\u0012n\u0013\u0013");

  // EmptyLayout
  assert_layout_serialize_to(
      EmptyLayout::Builder(),
      "");

  // FloorLayout
  assert_layout_serialize_to(
      FloorLayout::Builder(CodePointLayout::Builder('8')),
      "floor\u00128\u0013");

  // Fraction layout: 1/(2+3)
  assert_layout_serialize_to(
      FractionLayout::Builder(
        CodePointLayout::Builder('1'),
        LayoutHelper::StringToCodePointsLayout("2+3", 3)),
      "\u0012\u00121\u0013/\u00122+3\u0013\u0013");

  // GridPairLayout -> Tested by MatrixLayout

  // HorizontalLayout
  assert_layout_serialize_to(
      HorizontalLayout::Builder(
        CodePointLayout::Builder('a'),
        CodePointLayout::Builder('b'),
        CodePointLayout::Builder('c'),
        CodePointLayout::Builder('d')),
      "abcd");

  // IntegralLayout
  assert_layout_serialize_to(
      IntegralLayout::Builder(
        CodePointLayout::Builder('1'),
        CodePointLayout::Builder('x'),
        CodePointLayout::Builder('2'),
        CodePointLayout::Builder('3')),
      "int\u0012\u00121\u0013,\u0012x\u0013,\u00122\u0013,\u00123\u0013\u0013");

  // MatrixLayout
  assert_layout_serialize_to(
      MatrixLayout::Builder(
        CodePointLayout::Builder('a'),
        CodePointLayout::Builder('b'),
        CodePointLayout::Builder('c'),
        CodePointLayout::Builder('d')),
      "[[a,b][c,d]]");


  // Nth root layout
  assert_layout_serialize_to(
      NthRootLayout::Builder(
        CodePointLayout::Builder('7'),
        CodePointLayout::Builder('6')),
      "root\u0012\u00127\u0013,\u00126\u0013\u0013");

  // ParenthesisLayout
  assert_layout_serialize_to(
      ParenthesisLayout::Builder(),
      "()");

  // ProductLayout
  assert_layout_serialize_to(
      ProductLayout::Builder(
        CodePointLayout::Builder('1'),
        CodePointLayout::Builder('x'),
        CodePointLayout::Builder('2'),
        CodePointLayout::Builder('3')),
      "product\u0012\u00121\u0013,\u0012x\u0013,\u00122\u0013,\u00123\u0013\u0013");

  // SequenceLayout -> Tested by SumLayout and ProductLayout

  // SumLayout
  assert_layout_serialize_to(
      SumLayout::Builder(
        LayoutHelper::StringToCodePointsLayout("1+1", 3),
        CodePointLayout::Builder('x'),
        CodePointLayout::Builder('2'),
        CodePointLayout::Builder('3')),
      "sum\u0012\u00121+1\u0013,\u0012x\u0013,\u00122\u0013,\u00123\u0013\u0013");


  // Vertical offset layout
  assert_layout_serialize_to(
      HorizontalLayout::Builder(
        CodePointLayout::Builder('2'),
        VerticalOffsetLayout::Builder(
          LayoutHelper::StringToCodePointsLayout("x+5", 3),
          VerticalOffsetLayoutNode::Position::Superscript)),
      "2^\x12x+5\x13");
}
