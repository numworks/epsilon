#include <poincare_junior/src/expression/projection.h>
#include <poincare_junior/src/expression/sign.h>
#include <poincare_junior/src/expression/simplification.h>

#include "helper.h"
using namespace PoincareJ;

void assert_sign(const char* input, ComplexSign expectedSign) {
  Tree* expression = TextToTree(input);
  Projection::DeepSystemProjection(expression);
  Simplification::DeepSystematicReduce(expression);
  QUIZ_ASSERT(ComplexSign::Get(expression) == expectedSign);
  expression->removeTree();
}

void assert_sign(const char* input, Sign expectedSign) {
  assert_sign(input, ComplexSign(expectedSign, Sign::Zero()));
}

QUIZ_CASE(pcj_sign) {
  assert_sign("2", Sign::PositiveInteger());
  assert_sign("2+π", Sign::Positive());
  assert_sign("2-floor(π)", Sign::Integer());
  assert_sign("3 * abs(cos(x)) * -2", Sign::NegativeOrNull());
  // TODO : Add tests
}
