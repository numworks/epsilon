#include <poincare_junior/src/expression/projection.h>
#include <poincare_junior/src/expression/sign.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/expression/variables.h>

#include "helper.h"
using namespace PoincareJ;

static_assert(ComplexSign(ComplexSign::RealInteger().getValue()) ==
              ComplexSign::RealInteger());
static_assert(ComplexSign(ComplexSign::RealInteger().getValue()) ==
              ComplexSign::RealInteger());
static_assert(ComplexSign(ComplexSign::RealUnknown().getValue()) ==
              ComplexSign::RealUnknown());
static_assert(ComplexSign(ComplexSign::ComplexUnknown().getValue()) ==
              ComplexSign::ComplexUnknown());
static_assert(ComplexSign::ComplexUnknown().isUnknown());
static_assert(ComplexSign::RealUnknown().isReal());
static_assert(ComplexSign::RealInteger().isReal() &&
              !ComplexSign::RealInteger().canBeNonInteger());

void assert_sign(const char* input, ComplexSign expectedSign) {
  Tree* expression = TextToTree(input);
  /* TODO: Factorize this with SimplifyLastTree to have properly projected
   * variables, random trees, ... */
  Projection::DeepSystemProjection(
      expression, {.m_complexFormat = ComplexFormat::Cartesian});
  Simplification::DeepSystematicReduce(expression);
  bool result = ComplexSign::Get(expression) == expectedSign;
#if POINCARE_MEMORY_TREE_LOG
  if (!result) {
    std::cout << "\t\t\tWrong Sign: ";
    ComplexSign::Get(expression).log();
    std::cout << "\t\t\tInstead of: ";
    expectedSign.log();
  }
#endif
  quiz_assert(result);
  expression->removeTree();
}

void assert_sign(const char* input, Sign expectedSign) {
  assert_sign(input, ComplexSign(expectedSign, Sign::Zero()));
}

QUIZ_CASE(pcj_sign) {
  assert_sign("2", Sign::PositiveInteger());
  assert_sign("2+π", Sign::Positive());
  assert_sign("2-π", Sign::Unknown());
  assert_sign("3 * abs(cos(x)) * -2", Sign::NegativeOrNull());

  assert_sign("5+i*x", ComplexSign::ComplexUnknown());
  assert_sign("5+i*im(x)",
              ComplexSign(Sign::PositiveInteger(), Sign::Unknown()));
  assert_sign("5+i*x", ComplexSign::ComplexUnknown());
  assert_sign("re(x)^2", ComplexSign(Sign::PositiveOrNull(), Sign::Zero()));
  assert_sign("re(x)^2+im(x)^2",
              ComplexSign(Sign::PositiveOrNull(), Sign::Zero()));
  assert_sign("0.5*ln(re(x)^2+im(x)^2)",
              ComplexSign(Sign::Unknown(), Sign::Zero()));
  assert_sign("e^(0.5*ln(re(x)^2+im(x)^2))",
              ComplexSign(Sign::Positive(), Sign::Zero()));
  assert_sign("(abs(x)+i)*abs(abs(x)-i)",
              ComplexSign(Sign::PositiveOrNull(), Sign::Positive()));
  assert_sign("(5+i)^3",
              ComplexSign(Sign::NonNullInteger(), Sign::NonNullInteger()));
  assert_sign("(5-i)^(-1)", ComplexSign(Sign::NonNull(), Sign::NonNull()));
  assert_sign("e^(0.5*ln(12))+i*re(ln(2+i))",
              ComplexSign(Sign::Positive(), Sign::Unknown()));
  assert_sign("re(abs(x)-i)+i*arg(2+i)",
              ComplexSign(Sign::PositiveOrNull(), Sign::Positive()));
}
