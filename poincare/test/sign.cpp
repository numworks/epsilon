#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/sign.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/variables.h>

#include "helper.h"
using namespace Poincare::Internal;

static_assert(Sign::Zero().isZero() && !Sign::Zero().canBeNonInteger());
static_assert(ComplexSign(ComplexSign::RealInteger().getValue()) ==
              ComplexSign::RealInteger());
static_assert(ComplexSign(ComplexSign::RealInteger().getValue()) ==
              ComplexSign::RealInteger());
static_assert(ComplexSign(ComplexSign::RealUnknown().getValue()) ==
              ComplexSign::RealUnknown());
static_assert(ComplexSign(ComplexSign::Unknown().getValue()) ==
              ComplexSign::Unknown());
static_assert(ComplexSign::Unknown().isUnknown());
static_assert(ComplexSign::RealUnknown().isReal());
static_assert(ComplexSign::RealInteger().isReal() &&
              !ComplexSign::RealInteger().canBeNonInteger());

namespace Poincare::Internal {
extern Sign RelaxIntegerProperty(Sign s);
extern Sign DecimalFunction(Sign s, Type type);
extern Sign Opposite(Sign s);
extern Sign Mult(Sign s1, Sign s2);
extern Sign Add(Sign s1, Sign s2);
}  // namespace Poincare::Internal

QUIZ_CASE(pcj_sign_methods) {
  // RelaxIntegerProperty
  assert(RelaxIntegerProperty(Sign::Zero()) == Sign::Zero());
  assert(RelaxIntegerProperty(Sign::NonNull()) == Sign::NonNull());
  assert(RelaxIntegerProperty(Sign::Positive()) == Sign::Positive());
  assert(RelaxIntegerProperty(Sign::PositiveOrNull()) ==
         Sign::PositiveOrNull());
  assert(RelaxIntegerProperty(Sign::Negative()) == Sign::Negative());
  assert(RelaxIntegerProperty(Sign::NegativeOrNull()) ==
         Sign::NegativeOrNull());
  assert(RelaxIntegerProperty(Sign::Unknown()) == Sign::Unknown());
  assert(RelaxIntegerProperty(Sign::PositiveInteger()) == Sign::Positive());
  assert(RelaxIntegerProperty(Sign::PositiveOrNullInteger()) ==
         Sign::PositiveOrNull());
  assert(RelaxIntegerProperty(Sign::NegativeInteger()) == Sign::Negative());
  assert(RelaxIntegerProperty(Sign::NegativeOrNullInteger()) ==
         Sign::NegativeOrNull());
  assert(RelaxIntegerProperty(Sign::NonNullInteger()) == Sign::NonNull());
  assert(RelaxIntegerProperty(Sign::Integer()) == Sign::Unknown());

  // Ceil
  assert(DecimalFunction(Sign::Zero(), Type::Ceil) == Sign::Zero());
  assert(DecimalFunction(Sign::NonNull(), Type::Ceil) == Sign::Integer());
  assert(DecimalFunction(Sign::Positive(), Type::Ceil) ==
         Sign::PositiveInteger());
  assert(DecimalFunction(Sign::PositiveOrNull(), Type::Ceil) ==
         Sign::PositiveOrNullInteger());
  assert(DecimalFunction(Sign::Negative(), Type::Ceil) ==
         Sign::NegativeOrNullInteger());
  assert(DecimalFunction(Sign::NegativeOrNull(), Type::Ceil) ==
         Sign::NegativeOrNullInteger());
  assert(DecimalFunction(Sign::Unknown(), Type::Ceil) == Sign::Integer());
  assert(DecimalFunction(Sign::PositiveInteger(), Type::Ceil) ==
         Sign::PositiveInteger());
  assert(DecimalFunction(Sign::PositiveOrNullInteger(), Type::Ceil) ==
         Sign::PositiveOrNullInteger());
  assert(DecimalFunction(Sign::NegativeInteger(), Type::Ceil) ==
         Sign::NegativeInteger());
  assert(DecimalFunction(Sign::NegativeOrNullInteger(), Type::Ceil) ==
         Sign::NegativeOrNullInteger());
  assert(DecimalFunction(Sign::NonNullInteger(), Type::Ceil) ==
         Sign::NonNullInteger());
  assert(DecimalFunction(Sign::Integer(), Type::Ceil) == Sign::Integer());

  // Floor
  assert(DecimalFunction(Sign::Zero(), Type::Floor) == Sign::Zero());
  assert(DecimalFunction(Sign::NonNull(), Type::Floor) == Sign::Integer());
  assert(DecimalFunction(Sign::Positive(), Type::Floor) ==
         Sign::PositiveOrNullInteger());
  assert(DecimalFunction(Sign::PositiveOrNull(), Type::Floor) ==
         Sign::PositiveOrNullInteger());
  assert(DecimalFunction(Sign::Negative(), Type::Floor) ==
         Sign::NegativeInteger());
  assert(DecimalFunction(Sign::NegativeOrNull(), Type::Floor) ==
         Sign::NegativeOrNullInteger());
  assert(DecimalFunction(Sign::Unknown(), Type::Floor) == Sign::Integer());
  assert(DecimalFunction(Sign::PositiveInteger(), Type::Floor) ==
         Sign::PositiveInteger());
  assert(DecimalFunction(Sign::PositiveOrNullInteger(), Type::Floor) ==
         Sign::PositiveOrNullInteger());
  assert(DecimalFunction(Sign::NegativeInteger(), Type::Floor) ==
         Sign::NegativeInteger());
  assert(DecimalFunction(Sign::NegativeOrNullInteger(), Type::Floor) ==
         Sign::NegativeOrNullInteger());
  assert(DecimalFunction(Sign::NonNullInteger(), Type::Floor) ==
         Sign::NonNullInteger());
  assert(DecimalFunction(Sign::Integer(), Type::Floor) == Sign::Integer());

  // Frac
  assert(DecimalFunction(Sign::Zero(), Type::Frac) == Sign::Zero());
  assert(DecimalFunction(Sign::NonNull(), Type::Frac) ==
         Sign::PositiveOrNull());
  assert(DecimalFunction(Sign::Positive(), Type::Frac) ==
         Sign::PositiveOrNull());
  assert(DecimalFunction(Sign::PositiveOrNull(), Type::Frac) ==
         Sign::PositiveOrNull());
  assert(DecimalFunction(Sign::Negative(), Type::Frac) ==
         Sign::PositiveOrNull());
  assert(DecimalFunction(Sign::NegativeOrNull(), Type::Frac) ==
         Sign::PositiveOrNull());
  assert(DecimalFunction(Sign::Unknown(), Type::Frac) ==
         Sign::PositiveOrNull());
  assert(DecimalFunction(Sign::PositiveInteger(), Type::Frac) == Sign::Zero());
  assert(DecimalFunction(Sign::PositiveOrNullInteger(), Type::Frac) ==
         Sign::Zero());
  assert(DecimalFunction(Sign::NegativeInteger(), Type::Frac) == Sign::Zero());
  assert(DecimalFunction(Sign::NegativeOrNullInteger(), Type::Frac) ==
         Sign::Zero());
  assert(DecimalFunction(Sign::NonNullInteger(), Type::Frac) == Sign::Zero());
  assert(DecimalFunction(Sign::Integer(), Type::Frac) == Sign::Zero());

  // Round
  assert(DecimalFunction(Sign::Zero(), Type::Round) == Sign::Zero());
  assert(DecimalFunction(Sign::NonNull(), Type::Round) == Sign::Unknown());
  assert(DecimalFunction(Sign::Positive(), Type::Round) ==
         Sign::PositiveOrNull());
  assert(DecimalFunction(Sign::PositiveOrNull(), Type::Round) ==
         Sign::PositiveOrNull());
  assert(DecimalFunction(Sign::Negative(), Type::Round) ==
         Sign::NegativeOrNull());
  assert(DecimalFunction(Sign::NegativeOrNull(), Type::Round) ==
         Sign::NegativeOrNull());
  assert(DecimalFunction(Sign::Unknown(), Type::Round) == Sign::Unknown());
  assert(DecimalFunction(Sign::PositiveInteger(), Type::Round) ==
         Sign::PositiveOrNullInteger());
  assert(DecimalFunction(Sign::PositiveOrNullInteger(), Type::Round) ==
         Sign::PositiveOrNullInteger());
  assert(DecimalFunction(Sign::NegativeInteger(), Type::Round) ==
         Sign::NegativeOrNullInteger());
  assert(DecimalFunction(Sign::NegativeOrNullInteger(), Type::Round) ==
         Sign::NegativeOrNullInteger());
  assert(DecimalFunction(Sign::NonNullInteger(), Type::Round) ==
         Sign::Integer());
  assert(DecimalFunction(Sign::Integer(), Type::Round) == Sign::Integer());
}

void assert_sign(const char* input, ComplexSign expectedSign) {
  Tree* expression = TextToTree(input);
  /* TODO: Factorize this with SimplifyLastTree to have properly projected
   * variables, random trees, ... */
  Projection::DeepSystemProject(expression,
                                {.m_complexFormat = ComplexFormat::Cartesian});
  Simplification::DeepSystematicReduce(expression);
  bool result = ComplexSign::Get(expression) == expectedSign;
#if POINCARE_TREE_LOG
  if (!result) {
    std::cout << input << " -> ";
    expression->logSerialize();
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

  assert_sign("5+i*x", ComplexSign::Unknown());
  assert_sign("5+i*im(x)",
              ComplexSign(Sign::PositiveInteger(), Sign::Unknown()));
  assert_sign("5+i*x", ComplexSign::Unknown());
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
