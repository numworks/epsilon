#include <quiz.h>
#include <assert.h>
#include <poincare_layouts.h>
#include "helper.h"
#include "tree/helpers.h"

using namespace Poincare;

// TODO: ADD TESTS
void assert_parsed_expression_layouts_to(const char * expression, Layout l) {
  Expression e = parse_expression(expression, true);
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

void assert_parsed_expression_layout_serialize_to_self(const char * expressionLayout) {
  Expression e = parse_expression(expressionLayout, true);
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
