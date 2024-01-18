#include <poincare_junior/src/expression/projection.h>
#include <poincare_junior/src/expression/sign.h>
#include <poincare_junior/src/expression/simplification.h>

#include "helper.h"
using namespace PoincareJ;

void assert_sign(const char* input, ComplexSign expectedSign) {
  Tree* expression = TextToTree(input);
  Projection::DeepSystemProjection(expression);
  Simplification::DeepSystematicReduce(expression);
  bool result = ComplexSign::Get(expression) == expectedSign;
  if (!result) {
    std::cout << "\t\t\tWrong Sign: ";
    ComplexSign::Get(expression).log();
    std::cout << "\t\t\tInstead of: ";
    expectedSign.log();
  }
  quiz_assert(result);
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
