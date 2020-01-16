#include <poincare_layouts.h>
#include "helper.h"

using namespace Poincare;

void assert_parsed_expression_layouts_to(const char * expression, Layout l) {
  Expression e = parse_expression(expression, nullptr, true);
  Layout el = e.createLayout(DecimalMode, PrintFloat::k_numberOfStoredSignificantDigits);
  quiz_assert_print_if_failure(el.isIdenticalTo(l), expression);
}

QUIZ_CASE(poincare_expression_to_layout) {
  assert_parsed_expression_layouts_to("abs(1)", AbsoluteValueLayout::Builder(CodePointLayout::Builder('1')));
  assert_parsed_expression_layouts_to("binomial(1,2)", BinomialCoefficientLayout::Builder(CodePointLayout::Builder('1'), CodePointLayout::Builder('2')));
  assert_parsed_expression_layouts_to("[[1,2][3,4]]", MatrixLayout::Builder(CodePointLayout::Builder('1'), CodePointLayout::Builder('2'), CodePointLayout::Builder('3'), CodePointLayout::Builder('4')));
  assert_parsed_expression_layouts_to("1+2", HorizontalLayout::Builder(CodePointLayout::Builder('1'), CodePointLayout::Builder('+'), CodePointLayout::Builder('2')));
  assert_parsed_expression_layouts_to("ceil(1)", CeilingLayout::Builder(CodePointLayout::Builder('1')));
  assert_parsed_expression_layouts_to("conj(1)", ConjugateLayout::Builder(CodePointLayout::Builder('1')));
  assert_parsed_expression_layouts_to("floor(1)", FloorLayout::Builder(CodePointLayout::Builder('1')));
  assert_parsed_expression_layouts_to("1/2", FractionLayout::Builder(CodePointLayout::Builder('1'), CodePointLayout::Builder('2')));
  assert_parsed_expression_layouts_to("int(1,x,2,3)", IntegralLayout::Builder(CodePointLayout::Builder('1'), CodePointLayout::Builder('x'), CodePointLayout::Builder('2'), CodePointLayout::Builder('3')));
  assert_parsed_expression_layouts_to("(1)", HorizontalLayout::Builder(LeftParenthesisLayout::Builder(), CodePointLayout::Builder('1'), RightParenthesisLayout::Builder()));
  assert_parsed_expression_layouts_to("√(1)", NthRootLayout::Builder(CodePointLayout::Builder('1')));
  assert_parsed_expression_layouts_to("root(1,2)", NthRootLayout::Builder(CodePointLayout::Builder('1'), CodePointLayout::Builder('2')));
  assert_parsed_expression_layouts_to("sum(1,n,2,3)", SumLayout::Builder(CodePointLayout::Builder('1'), CodePointLayout::Builder('n'), CodePointLayout::Builder('2'), CodePointLayout::Builder('3')));
  assert_parsed_expression_layouts_to("product(1,n,2,3)", ProductLayout::Builder(CodePointLayout::Builder('1'), CodePointLayout::Builder('n'), CodePointLayout::Builder('2'), CodePointLayout::Builder('3')));
  assert_parsed_expression_layouts_to("1^2", HorizontalLayout::Builder(CodePointLayout::Builder('1'), VerticalOffsetLayout::Builder(CodePointLayout::Builder('2'), VerticalOffsetLayoutNode::Position::Superscript)));
}

void assert_expression_layouts_and_serializes_to(Expression expression, const char * serialization) {
  Layout layout = expression.createLayout(DecimalMode, PrintFloat::k_numberOfStoredSignificantDigits);
  assert_layout_serialize_to(layout, serialization);
}

QUIZ_CASE(poincare_expression_to_layout_multiplication_operator) {
  // Decimal x OneLetter
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Decimal::Builder("2", -4), Symbol::Builder('a')), "0.0002a");
  // Decimal x Decimal
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Decimal::Builder("2", -4), Decimal::Builder("2", -4)), "0.0002×0.0002");
  // Integer x Integer
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Rational::Builder(2), Rational::Builder(1)), "2×1");
  // Integer x MoreLetters
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Rational::Builder(2), Symbol::Builder("abc", 3)), "2·abc");
  // Integer x fraction
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Rational::Builder(2), Rational::Builder(2, 3)), "2×\u0012\u00122\u0013/\u00123\u0013\u0013");
  // BoundaryPunctuation x Integer
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(AbsoluteValue::Builder(Rational::Builder(2)), Rational::Builder(1)), "abs\u00122\u0013×1");
  // BoundaryPunctuation x OneLetter
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(BinomialCoefficient::Builder(Rational::Builder(2), Rational::Builder(3)), Symbol::Builder('a')), "binomial\u0012\u00122\u0013,\u00123\u0013\u0013a");
  // BoundaryPunctuation x Root
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Cosine::Builder(Rational::Builder(2)), SquareRoot::Builder(Rational::Builder(2))), "cos(2)√\u00122\u0013");

  // BasedInteger x ?
  // 0b101·0.23
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(BasedInteger::Builder("5", Integer::Base::Binary), Decimal::Builder("23", -1)), "0b101·0.23");
  // 0x2A3·23242
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(BasedInteger::Builder("675", Integer::Base::Hexadecimal), Rational::Builder(23242)), "0x2A3·23242");
  // 0b101·π
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(BasedInteger::Builder("5", Integer::Base::Binary), Symbol::Builder(UCodePointGreekSmallLetterPi)), "0b101·π");
  // 0x2A3·abc
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(BasedInteger::Builder("675", Integer::Base::Hexadecimal), Symbol::Builder("abc", 3)), "0x2A3·abc");
  // 0b101·(1+2)
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(BasedInteger::Builder("5", Integer::Base::Binary), Parenthesis::Builder(Addition::Builder(Rational::Builder(1), Rational::Builder(2)))), "0b101·(1+2)");
  // 0x2A3·√(2)
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(BasedInteger::Builder("675", Integer::Base::Hexadecimal), SquareRoot::Builder(Rational::Builder(2))), "0x2A3·√\u00122\u0013");
  // 0b101·root(2,3)
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(BasedInteger::Builder("5", Integer::Base::Binary), NthRoot::Builder(Rational::Builder(2), Rational::Builder(3))), "0b101·root\u0012\u00122\u0013,\u00123\u0013\u0013");
  // 0x2A3·2/3
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(BasedInteger::Builder("675", Integer::Base::Hexadecimal), Rational::Builder(2,3)), "0x2A3·\u0012\u00122\u0013/\u00123\u0013\u0013");

  // ? x BasedInteger
  // 0.23·0x2A3
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Decimal::Builder("23", -1), BasedInteger::Builder("675", Integer::Base::Hexadecimal)), "0.23·0x2A3");
  // 23242·0b101
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Rational::Builder(23242),BasedInteger::Builder("5", Integer::Base::Binary)), "23242·0b101");
  // π·0x2A3
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Symbol::Builder(UCodePointGreekSmallLetterPi), BasedInteger::Builder(675, Integer::Base::Hexadecimal)), "π·0x2A3");
  // abc·0b101
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Symbol::Builder("abc", 3), BasedInteger::Builder(5, Integer::Base::Binary)), "abc·0b101");
  // (1+2)·0x2A3
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Parenthesis::Builder(Addition::Builder(Rational::Builder(1), Rational::Builder(2))), BasedInteger::Builder(675, Integer::Base::Hexadecimal)), "(1+2)·0x2A3");
  // √(2)·0b101
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(SquareRoot::Builder(Rational::Builder(2)),BasedInteger::Builder(5, Integer::Base::Binary)), "√\u00122\u0013·0b101");
  // root(2,3)·0x2A3
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(NthRoot::Builder(Rational::Builder(2), Rational::Builder(3)), BasedInteger::Builder(675, Integer::Base::Hexadecimal)), "root\u0012\u00122\u0013,\u00123\u0013\u0013·0x2A3");
  // 2/3·0b101
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Rational::Builder(2,3),BasedInteger::Builder(5, Integer::Base::Binary)), "\u0012\u00122\u0013/\u00123\u0013\u0013·0b101");

  // 2√(2)
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Rational::Builder(2), SquareRoot::Builder(Rational::Builder(2))), "2√\u00122\u0013");
  // √(2)x2
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(SquareRoot::Builder(Rational::Builder(2)), Rational::Builder(2)), "√\u00122\u0013×2");
  // 2π
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Rational::Builder(2), Constant::Builder(UCodePointGreekSmallLetterPi)), "2π");
  // π·𝐢
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Constant::Builder(UCodePointGreekSmallLetterPi), Constant::Builder(UCodePointMathematicalBoldSmallI)), "π·𝐢");
  // conj(2)√(2)
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Conjugate::Builder(Rational::Builder(2)), SquareRoot::Builder(Rational::Builder(2))), "conj\u00122\u0013√\u00122\u0013");
  //√(2)a!
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(SquareRoot::Builder(Rational::Builder(2)), Factorial::Builder(Symbol::Builder('a'))), "√\u00122\u0013a!");
  // a 2/3
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Symbol::Builder('a'), Division::Builder(Rational::Builder(2), Rational::Builder(3))), "a\u0012\u00122\u0013/\u00123\u0013\u0013");
  // (1+π)a
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Parenthesis::Builder(Addition::Builder(Rational::Builder(1), Constant::Builder(UCodePointGreekSmallLetterPi))), Symbol::Builder('a')), "(1+π)a");
  // 2·root(2,a)
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Rational::Builder(2), NthRoot::Builder(Rational::Builder(2), Symbol::Builder('a'))), "2·root\u0012\u00122\u0013,\u0012a\u0013\u0013");

  // Operator contamination (if two operands needs x, all operands are separated by a x)
  // 1x2xa
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Rational::Builder(1), Rational::Builder(2), Symbol::Builder('a')), "1×2×a");
  // 2·π·a
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Rational::Builder(2), Constant::Builder(UCodePointGreekSmallLetterPi), Symbol::Builder('a')), "2·π·a");
  // 2(1+a)(1+π)
  assert_expression_layouts_and_serializes_to(Multiplication::Builder(Rational::Builder(2), Parenthesis::Builder(Addition::Builder(Rational::Builder(1), Symbol::Builder('a'))), Parenthesis::Builder(Addition::Builder(Rational::Builder(1), Constant::Builder(UCodePointGreekSmallLetterPi)))), "2(1+a)(1+π)");
}

void assert_parsed_expression_layout_serialize_to_self(const char * expressionLayout) {
  Expression e = parse_expression(expressionLayout, nullptr, true);
  Layout el = e.createLayout(DecimalMode, PrintFloat::k_numberOfStoredSignificantDigits);
  constexpr int bufferSize = 255;
  char buffer[bufferSize];
  el.serializeForParsing(buffer, bufferSize);
  quiz_assert_print_if_failure(strcmp(expressionLayout, buffer) == 0, expressionLayout);
}

QUIZ_CASE(poincare_expression_to_layout_serializes_to_self) {
  assert_parsed_expression_layout_serialize_to_self("binomial\u0012\u00127\u0013,\u00126\u0013\u0013");
  assert_parsed_expression_layout_serialize_to_self("root\u0012\u00127\u0013,\u00123\u0013\u0013");
}
