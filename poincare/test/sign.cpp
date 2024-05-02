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
  assert(RelaxIntegerProperty(Sign::StrictlyPositive()) ==
         Sign::StrictlyPositive());
  assert(RelaxIntegerProperty(Sign::Positive()) == Sign::Positive());
  assert(RelaxIntegerProperty(Sign::StrictlyNegative()) ==
         Sign::StrictlyNegative());
  assert(RelaxIntegerProperty(Sign::Negative()) == Sign::Negative());
  assert(RelaxIntegerProperty(Sign::Unknown()) == Sign::Unknown());
  assert(RelaxIntegerProperty(Sign::StrictlyPositiveInteger()) ==
         Sign::StrictlyPositive());
  assert(RelaxIntegerProperty(Sign::PositiveInteger()) == Sign::Positive());
  assert(RelaxIntegerProperty(Sign::StrictlyNegativeInteger()) ==
         Sign::StrictlyNegative());
  assert(RelaxIntegerProperty(Sign::NegativeInteger()) == Sign::Negative());
  assert(RelaxIntegerProperty(Sign::NonNullInteger()) == Sign::NonNull());
  assert(RelaxIntegerProperty(Sign::Integer()) == Sign::Unknown());

  // Ceil
  assert(DecimalFunction(Sign::Zero(), Type::Ceil) == Sign::Zero());
  assert(DecimalFunction(Sign::NonNull(), Type::Ceil) == Sign::Integer());
  assert(DecimalFunction(Sign::StrictlyPositive(), Type::Ceil) ==
         Sign::StrictlyPositiveInteger());
  assert(DecimalFunction(Sign::Positive(), Type::Ceil) ==
         Sign::PositiveInteger());
  assert(DecimalFunction(Sign::StrictlyNegative(), Type::Ceil) ==
         Sign::NegativeInteger());
  assert(DecimalFunction(Sign::Negative(), Type::Ceil) ==
         Sign::NegativeInteger());
  assert(DecimalFunction(Sign::Unknown(), Type::Ceil) == Sign::Integer());
  assert(DecimalFunction(Sign::StrictlyPositiveInteger(), Type::Ceil) ==
         Sign::StrictlyPositiveInteger());
  assert(DecimalFunction(Sign::PositiveInteger(), Type::Ceil) ==
         Sign::PositiveInteger());
  assert(DecimalFunction(Sign::StrictlyNegativeInteger(), Type::Ceil) ==
         Sign::StrictlyNegativeInteger());
  assert(DecimalFunction(Sign::NegativeInteger(), Type::Ceil) ==
         Sign::NegativeInteger());
  assert(DecimalFunction(Sign::NonNullInteger(), Type::Ceil) ==
         Sign::NonNullInteger());
  assert(DecimalFunction(Sign::Integer(), Type::Ceil) == Sign::Integer());

  // Floor
  assert(DecimalFunction(Sign::Zero(), Type::Floor) == Sign::Zero());
  assert(DecimalFunction(Sign::NonNull(), Type::Floor) == Sign::Integer());
  assert(DecimalFunction(Sign::StrictlyPositive(), Type::Floor) ==
         Sign::PositiveInteger());
  assert(DecimalFunction(Sign::Positive(), Type::Floor) ==
         Sign::PositiveInteger());
  assert(DecimalFunction(Sign::StrictlyNegative(), Type::Floor) ==
         Sign::StrictlyNegativeInteger());
  assert(DecimalFunction(Sign::Negative(), Type::Floor) ==
         Sign::NegativeInteger());
  assert(DecimalFunction(Sign::Unknown(), Type::Floor) == Sign::Integer());
  assert(DecimalFunction(Sign::StrictlyPositiveInteger(), Type::Floor) ==
         Sign::StrictlyPositiveInteger());
  assert(DecimalFunction(Sign::PositiveInteger(), Type::Floor) ==
         Sign::PositiveInteger());
  assert(DecimalFunction(Sign::StrictlyNegativeInteger(), Type::Floor) ==
         Sign::StrictlyNegativeInteger());
  assert(DecimalFunction(Sign::NegativeInteger(), Type::Floor) ==
         Sign::NegativeInteger());
  assert(DecimalFunction(Sign::NonNullInteger(), Type::Floor) ==
         Sign::NonNullInteger());
  assert(DecimalFunction(Sign::Integer(), Type::Floor) == Sign::Integer());

  // Frac
  assert(DecimalFunction(Sign::Zero(), Type::Frac) == Sign::Zero());
  assert(DecimalFunction(Sign::NonNull(), Type::Frac) == Sign::Positive());
  assert(DecimalFunction(Sign::StrictlyPositive(), Type::Frac) ==
         Sign::Positive());
  assert(DecimalFunction(Sign::Positive(), Type::Frac) == Sign::Positive());
  assert(DecimalFunction(Sign::StrictlyNegative(), Type::Frac) ==
         Sign::Positive());
  assert(DecimalFunction(Sign::Negative(), Type::Frac) == Sign::Positive());
  assert(DecimalFunction(Sign::Unknown(), Type::Frac) == Sign::Positive());
  assert(DecimalFunction(Sign::StrictlyPositiveInteger(), Type::Frac) ==
         Sign::Zero());
  assert(DecimalFunction(Sign::PositiveInteger(), Type::Frac) == Sign::Zero());
  assert(DecimalFunction(Sign::StrictlyNegativeInteger(), Type::Frac) ==
         Sign::Zero());
  assert(DecimalFunction(Sign::NegativeInteger(), Type::Frac) == Sign::Zero());
  assert(DecimalFunction(Sign::NonNullInteger(), Type::Frac) == Sign::Zero());
  assert(DecimalFunction(Sign::Integer(), Type::Frac) == Sign::Zero());

  // Round
  assert(DecimalFunction(Sign::Zero(), Type::Round) == Sign::Zero());
  assert(DecimalFunction(Sign::NonNull(), Type::Round) == Sign::Unknown());
  assert(DecimalFunction(Sign::StrictlyPositive(), Type::Round) ==
         Sign::Positive());
  assert(DecimalFunction(Sign::Positive(), Type::Round) == Sign::Positive());
  assert(DecimalFunction(Sign::StrictlyNegative(), Type::Round) ==
         Sign::Negative());
  assert(DecimalFunction(Sign::Negative(), Type::Round) == Sign::Negative());
  assert(DecimalFunction(Sign::Unknown(), Type::Round) == Sign::Unknown());
  assert(DecimalFunction(Sign::StrictlyPositiveInteger(), Type::Round) ==
         Sign::PositiveInteger());
  assert(DecimalFunction(Sign::PositiveInteger(), Type::Round) ==
         Sign::PositiveInteger());
  assert(DecimalFunction(Sign::StrictlyNegativeInteger(), Type::Round) ==
         Sign::NegativeInteger());
  assert(DecimalFunction(Sign::NegativeInteger(), Type::Round) ==
         Sign::NegativeInteger());
  assert(DecimalFunction(Sign::NonNullInteger(), Type::Round) ==
         Sign::Integer());
  assert(DecimalFunction(Sign::Integer(), Type::Round) == Sign::Integer());

  // Opposite
  assert(Opposite(Sign::Zero()) == Sign::Zero());
  assert(Opposite(Sign::NonNull()) == Sign::NonNull());
  assert(Opposite(Sign::StrictlyPositive()) == Sign::StrictlyNegative());
  assert(Opposite(Sign::Positive()) == Sign::Negative());
  assert(Opposite(Sign::StrictlyNegative()) == Sign::StrictlyPositive());
  assert(Opposite(Sign::Negative()) == Sign::Positive());
  assert(Opposite(Sign::Unknown()) == Sign::Unknown());
  assert(Opposite(Sign::StrictlyPositiveInteger()) ==
         Sign::StrictlyNegativeInteger());
  assert(Opposite(Sign::PositiveInteger()) == Sign::NegativeInteger());
  assert(Opposite(Sign::StrictlyNegativeInteger()) ==
         Sign::StrictlyPositiveInteger());
  assert(Opposite(Sign::NegativeInteger()) == Sign::PositiveInteger());
  assert(Opposite(Sign::NonNullInteger()) == Sign::NonNullInteger());
  assert(Opposite(Sign::Integer()) == Sign::Integer());

  // Mult(..., Zero)
  assert(Mult(Sign::Zero(), Sign::Zero()) == Sign::Zero());
  assert(Mult(Sign::NonNull(), Sign::Zero()) == Sign::Zero());
  assert(Mult(Sign::StrictlyPositive(), Sign::Zero()) == Sign::Zero());
  assert(Mult(Sign::Positive(), Sign::Zero()) == Sign::Zero());
  assert(Mult(Sign::StrictlyNegative(), Sign::Zero()) == Sign::Zero());
  assert(Mult(Sign::Negative(), Sign::Zero()) == Sign::Zero());
  assert(Mult(Sign::Unknown(), Sign::Zero()) == Sign::Zero());
  // Mult(..., NonNull)
  assert(Mult(Sign::NonNull(), Sign::NonNull()) == Sign::NonNull());
  assert(Mult(Sign::StrictlyPositive(), Sign::NonNull()) == Sign::NonNull());
  assert(Mult(Sign::Positive(), Sign::NonNull()) == Sign::Unknown());
  assert(Mult(Sign::StrictlyNegative(), Sign::NonNull()) == Sign::NonNull());
  assert(Mult(Sign::Negative(), Sign::NonNull()) == Sign::Unknown());
  assert(Mult(Sign::Unknown(), Sign::NonNull()) == Sign::Unknown());
  // Mult(..., Positive)
  assert(Mult(Sign::StrictlyPositive(), Sign::StrictlyPositive()) ==
         Sign::StrictlyPositive());
  assert(Mult(Sign::Positive(), Sign::StrictlyPositive()) == Sign::Positive());
  assert(Mult(Sign::StrictlyNegative(), Sign::StrictlyPositive()) ==
         Sign::StrictlyNegative());
  assert(Mult(Sign::Negative(), Sign::StrictlyPositive()) == Sign::Negative());
  assert(Mult(Sign::Unknown(), Sign::StrictlyPositive()) == Sign::Unknown());
  // Mult(..., PositiveOrNull)
  assert(Mult(Sign::Positive(), Sign::Positive()) == Sign::Positive());
  assert(Mult(Sign::StrictlyNegative(), Sign::Positive()) == Sign::Negative());
  assert(Mult(Sign::Negative(), Sign::Positive()) == Sign::Negative());
  assert(Mult(Sign::Unknown(), Sign::Positive()) == Sign::Unknown());
  // Mult(..., Negative)
  assert(Mult(Sign::StrictlyNegative(), Sign::StrictlyNegative()) ==
         Sign::StrictlyPositive());
  assert(Mult(Sign::Negative(), Sign::StrictlyNegative()) == Sign::Positive());
  assert(Mult(Sign::Unknown(), Sign::StrictlyNegative()) == Sign::Unknown());
  // Mult(..., NegativeOrNull)
  assert(Mult(Sign::Negative(), Sign::Negative()) == Sign::Positive());
  assert(Mult(Sign::Unknown(), Sign::Negative()) == Sign::Unknown());
  // Mult(..., Unknown)
  assert(Mult(Sign::Unknown(), Sign::Unknown()) == Sign::Unknown());

  // Add(..., Zero)
  assert(Add(Sign::Zero(), Sign::Zero()) == Sign::Zero());
  assert(Add(Sign::NonNull(), Sign::Zero()) == Sign::NonNull());
  assert(Add(Sign::StrictlyPositive(), Sign::Zero()) ==
         Sign::StrictlyPositive());
  assert(Add(Sign::Positive(), Sign::Zero()) == Sign::Positive());
  assert(Add(Sign::StrictlyNegative(), Sign::Zero()) ==
         Sign::StrictlyNegative());
  assert(Add(Sign::Negative(), Sign::Zero()) == Sign::Negative());
  assert(Add(Sign::Unknown(), Sign::Unknown()) == Sign::Unknown());
  // Add(..., NonNull)
  assert(Add(Sign::NonNull(), Sign::NonNull()) == Sign::Unknown());
  assert(Add(Sign::StrictlyPositive(), Sign::NonNull()) == Sign::Unknown());
  assert(Add(Sign::Positive(), Sign::NonNull()) == Sign::Unknown());
  assert(Add(Sign::StrictlyNegative(), Sign::NonNull()) == Sign::Unknown());
  assert(Add(Sign::Negative(), Sign::NonNull()) == Sign::Unknown());
  assert(Add(Sign::Unknown(), Sign::NonNull()) == Sign::Unknown());
  // Add(..., Positive)
  assert(Add(Sign::StrictlyPositive(), Sign::StrictlyPositive()) ==
         Sign::StrictlyPositive());
  assert(Add(Sign::Positive(), Sign::StrictlyPositive()) ==
         Sign::StrictlyPositive());
  assert(Add(Sign::StrictlyNegative(), Sign::StrictlyPositive()) ==
         Sign::Unknown());
  assert(Add(Sign::Negative(), Sign::StrictlyPositive()) == Sign::Unknown());
  assert(Add(Sign::Unknown(), Sign::StrictlyPositive()) == Sign::Unknown());
  // Add(..., PositiveOrNull)
  assert(Add(Sign::Positive(), Sign::Positive()) == Sign::Positive());
  assert(Add(Sign::StrictlyNegative(), Sign::Positive()) == Sign::Unknown());
  assert(Add(Sign::Negative(), Sign::Positive()) == Sign::Unknown());
  assert(Add(Sign::Unknown(), Sign::Positive()) == Sign::Unknown());
  // Add(..., Negative)
  assert(Add(Sign::StrictlyNegative(), Sign::StrictlyNegative()) ==
         Sign::StrictlyNegative());
  assert(Add(Sign::Negative(), Sign::StrictlyNegative()) ==
         Sign::StrictlyNegative());
  assert(Add(Sign::Unknown(), Sign::StrictlyNegative()) == Sign::Unknown());
  // Add(..., NegativeOrNull)
  assert(Add(Sign::Negative(), Sign::Negative()) == Sign::Negative());
  assert(Add(Sign::Unknown(), Sign::Negative()) == Sign::Unknown());
  // Add(..., Unknown)
  assert(Add(Sign::Unknown(), Sign::Unknown()) == Sign::Unknown());
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
  assert_sign("2", Sign::StrictlyPositiveInteger());
  assert_sign("2+π", Sign::StrictlyPositive());
  assert_sign("2-π", Sign::Unknown());
  assert_sign("3 * abs(cos(x)) * -2", Sign::Negative());

  assert_sign("5+i*x", ComplexSign::Unknown());
  assert_sign("5+i*im(x)",
              ComplexSign(Sign::StrictlyPositiveInteger(), Sign::Unknown()));
  assert_sign("5+i*x", ComplexSign::Unknown());
  assert_sign("re(x)^2", ComplexSign(Sign::Positive(), Sign::Zero()));
  assert_sign("re(x)^2+im(x)^2", ComplexSign(Sign::Positive(), Sign::Zero()));
  assert_sign("0.5*ln(re(x)^2+im(x)^2)",
              ComplexSign(Sign::Unknown(), Sign::Zero()));
  assert_sign("e^(0.5*ln(re(x)^2+im(x)^2))",
              ComplexSign(Sign::StrictlyPositive(), Sign::Zero()));
  assert_sign("(abs(x)+i)*abs(abs(x)-i)",
              ComplexSign(Sign::Positive(), Sign::StrictlyPositive()));
  assert_sign("(5+i)^3",
              ComplexSign(Sign::NonNullInteger(), Sign::NonNullInteger()));
  assert_sign("(5-i)^(-1)", ComplexSign(Sign::NonNull(), Sign::NonNull()));
  assert_sign("e^(0.5*ln(12))+i*re(ln(2+i))",
              ComplexSign(Sign::StrictlyPositive(), Sign::Unknown()));
  assert_sign("re(abs(x)-i)+i*arg(2+i)",
              ComplexSign(Sign::Positive(), Sign::StrictlyPositive()));

  // cos
  assert_sign("cos(3)", Sign::Unknown());
  assert_sign("cos(2i)", ComplexSign(Sign::StrictlyPositive(), Sign::Zero()));
  assert_sign("cos(-2i)", ComplexSign(Sign::StrictlyPositive(), Sign::Zero()));
  assert_sign("cos(3+2i)", ComplexSign::Unknown());

  // sin
  assert_sign("sin(3)", Sign::Unknown());
  assert_sign("sin(2i)", ComplexSign(Sign::Zero(), Sign::StrictlyPositive()));
  assert_sign("sin(-2i)", ComplexSign(Sign::Zero(), Sign::StrictlyNegative()));
  assert_sign("sin(3+2i)", ComplexSign::Unknown());

  // ln
  assert_sign("ln(3)", ComplexSign(Sign::Unknown(), Sign::Zero()));
  assert_sign("ln(-3)", ComplexSign(Sign::Unknown(), Sign::StrictlyPositive()));
  assert_sign("ln(ln(3))", ComplexSign(Sign::Unknown(), Sign::Positive()));
  assert_sign("ln(4+i)",
              ComplexSign(Sign::Unknown(), Sign::StrictlyPositive()));
  assert_sign("ln(4-i)",
              ComplexSign(Sign::Unknown(), Sign::StrictlyNegative()));
  assert_sign("ln(ln(x)i)", ComplexSign::Unknown());
}
