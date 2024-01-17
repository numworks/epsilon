#include <poincare_junior/src/expression/projection.h>
#include <poincare_junior/src/expression/sign.h>
#include <poincare_junior/src/expression/simplification.h>

#include "helper.h"
using namespace PoincareJ;

void assert_sign(const char* input, Sign::Sign expectedSign,
                 bool shouldBeInteger = false) {
  expectedSign.isInteger = shouldBeInteger;
  Tree* expression = TextToTree(input);
  Projection::DeepSystemProjection(expression);
  QUIZ_ASSERT(Sign::GetSign(expression) == expectedSign);
  expression->removeTree();
}

QUIZ_CASE(pcj_sign) {
  assert_sign("2", Sign::PositiveInteger, true);
  assert_sign("2+π", Sign::Positive);
  assert_sign("2-4", Sign::Integer, true);
  assert_sign("3 * abs(cos(x)) * -2", Sign::NegativeOrNull);
}
