#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/sign.h>
#include <poincare_junior/src/expression/simplification.h>

#include "helper.h"
using namespace PoincareJ;

bool check_sign(const char* input, Sign::Sign expectedSign,
                bool shouldBeInteger = false) {
  expectedSign.isInteger = shouldBeInteger;
  Tree* expression = TextToTree(input);
  Projection::DeepSystemProjection(expression);
  bool result = Sign::GetSign(expression) == expectedSign;
  expression->removeTree();
  return result;
}

QUIZ_CASE(pcj_sign) {
  QUIZ_ASSERT(check_sign("2", Sign::Positive, true));
  QUIZ_ASSERT(check_sign("2+π", Sign::Positive));
  QUIZ_ASSERT(check_sign("2-4", Sign::Unknown, true));
  QUIZ_ASSERT(check_sign("3 * abs(cos(x)) * -2", Sign::NegativeOrNull));

  QUIZ_ASSERT(SharedEditionPool->numberOfTrees() == 0);
}
