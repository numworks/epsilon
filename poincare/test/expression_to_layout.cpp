#include <quiz.h>
#include <assert.h>
#include "helper.h"
#include "tree/helpers.h"

using namespace Poincare;

// TODO: ADD TESTS

void assert_parsed_expression_layout_serialize_to_self(const char * expressionLayout) {
  Expression e = parse_expression(expressionLayout, true);
  Layout el = e.createLayout(DecimalMode, PrintFloat::k_numberOfStoredSignificantDigits);
  constexpr int bufferSize = 255;
  char buffer[bufferSize];
  el.serializeForParsing(buffer, bufferSize);
  quiz_assert_print_if_failure(strcmp(expressionLayout, buffer) == 0, expressionLayout);
}

QUIZ_CASE(poincare_expression_to_layout) {
  assert_parsed_expression_layout_serialize_to_self("binomial\u0012\u00127\u0013,\u00126\u0013\u0013");
  assert_parsed_expression_layout_serialize_to_self("root\u0012\u00127\u0013,\u00123\u0013\u0013");
}

