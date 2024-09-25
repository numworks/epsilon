#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/sign.h>
#include <poincare/src/expression/systematic_reduction.h>
#include <poincare/src/expression/variables.h>

#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

static_assert(Sign::Zero().isNull() && Sign::Zero().isInteger());
static_assert(
    ComplexSign::FromValue(ComplexSign::RealInteger().getRealValue(),
                           ComplexSign::RealInteger().getImagValue()) ==
    ComplexSign::RealInteger());
static_assert(
    ComplexSign::FromValue(ComplexSign::RealUnknown().getRealValue(),
                           ComplexSign::RealUnknown().getImagValue()) ==
    ComplexSign::RealUnknown());
static_assert(ComplexSign::FromValue(ComplexSign::Unknown().getRealValue(),
                                     ComplexSign::Unknown().getImagValue()) ==
              ComplexSign::Unknown());
static_assert(ComplexSign::Unknown().isUnknown());
static_assert(ComplexSign::RealUnknown().isReal());
static_assert(ComplexSign::RealInteger().isReal() &&
              ComplexSign::RealInteger().isInteger());

namespace Poincare {
extern Sign RelaxIntegerProperty(Sign s);
extern Sign DecimalFunction(Sign s, Type type);
extern Sign Opposite(Sign s);
extern Sign Mult(Sign s1, Sign s2);
extern Sign Add(Sign s1, Sign s2);
}  // namespace Poincare

QUIZ_CASE(pcj_sign_methods) {
  // OR operator
  quiz_assert((Sign::Zero() || Sign::NonNull()) == Sign::Unknown());
  quiz_assert((Sign::Zero() || Sign::StrictlyPositive()) == Sign::Positive());
  quiz_assert((Sign::Zero() || Sign::StrictlyNegative()) == Sign::Negative());
  quiz_assert((Sign::StrictlyPositive() || Sign::StrictlyNegative()) ==
              Sign::NonNull());
  quiz_assert((Sign::Positive() || Sign::StrictlyNegative()) ==
              Sign::Unknown());
  quiz_assert((Sign::StrictlyPositive() || Sign::Negative()) ==
              Sign::Unknown());
  quiz_assert((Sign::Positive() || Sign::Negative()) == Sign::Unknown());
  quiz_assert((Sign::Finite() || Sign::Unknown()) == Sign::Unknown());

  // RelaxIntegerProperty
  quiz_assert(RelaxIntegerProperty(Sign::Zero()) == Sign::Zero());
  quiz_assert(RelaxIntegerProperty(Sign::NonNull()) == Sign::NonNull());
  quiz_assert(RelaxIntegerProperty(Sign::StrictlyPositive()) ==
              Sign::StrictlyPositive());
  quiz_assert(RelaxIntegerProperty(Sign::Positive()) == Sign::Positive());
  quiz_assert(RelaxIntegerProperty(Sign::StrictlyNegative()) ==
              Sign::StrictlyNegative());
  quiz_assert(RelaxIntegerProperty(Sign::Negative()) == Sign::Negative());
  quiz_assert(RelaxIntegerProperty(Sign::Unknown()) == Sign::Unknown());
  quiz_assert(RelaxIntegerProperty(Sign::StrictlyPositiveInteger()) ==
              Sign::StrictlyPositive());
  quiz_assert(RelaxIntegerProperty(Sign::PositiveInteger()) ==
              Sign::Positive());
  quiz_assert(RelaxIntegerProperty(Sign::StrictlyNegativeInteger()) ==
              Sign::StrictlyNegative());
  quiz_assert(RelaxIntegerProperty(Sign::NegativeInteger()) ==
              Sign::Negative());
  quiz_assert(RelaxIntegerProperty(Sign::NonNullInteger()) == Sign::NonNull());
  quiz_assert(RelaxIntegerProperty(Sign::Integer()) == Sign::Unknown());
  quiz_assert(RelaxIntegerProperty(Sign::FiniteInteger()) == Sign::Finite());
  quiz_assert(RelaxIntegerProperty(Sign::FiniteStrictlyPositiveInteger()) ==
              Sign::FiniteStrictlyPositive());

  // Ceil
  quiz_assert(DecimalFunction(Sign::Zero(), Type::Ceil) == Sign::Zero());
  quiz_assert(DecimalFunction(Sign::NonNull(), Type::Ceil) == Sign::Integer());
  quiz_assert(DecimalFunction(Sign::StrictlyPositive(), Type::Ceil) ==
              Sign::StrictlyPositiveInteger());
  quiz_assert(DecimalFunction(Sign::Positive(), Type::Ceil) ==
              Sign::PositiveInteger());
  quiz_assert(DecimalFunction(Sign::StrictlyNegative(), Type::Ceil) ==
              Sign::NegativeInteger());
  quiz_assert(DecimalFunction(Sign::Negative(), Type::Ceil) ==
              Sign::NegativeInteger());
  quiz_assert(DecimalFunction(Sign::Unknown(), Type::Ceil) == Sign::Integer());
  quiz_assert(DecimalFunction(Sign::StrictlyPositiveInteger(), Type::Ceil) ==
              Sign::StrictlyPositiveInteger());
  quiz_assert(DecimalFunction(Sign::PositiveInteger(), Type::Ceil) ==
              Sign::PositiveInteger());
  quiz_assert(DecimalFunction(Sign::StrictlyNegativeInteger(), Type::Ceil) ==
              Sign::StrictlyNegativeInteger());
  quiz_assert(DecimalFunction(Sign::NegativeInteger(), Type::Ceil) ==
              Sign::NegativeInteger());
  quiz_assert(DecimalFunction(Sign::NonNullInteger(), Type::Ceil) ==
              Sign::NonNullInteger());
  quiz_assert(DecimalFunction(Sign::Integer(), Type::Ceil) == Sign::Integer());

  // Floor
  quiz_assert(DecimalFunction(Sign::Zero(), Type::Floor) == Sign::Zero());
  quiz_assert(DecimalFunction(Sign::NonNull(), Type::Floor) == Sign::Integer());
  quiz_assert(DecimalFunction(Sign::StrictlyPositive(), Type::Floor) ==
              Sign::PositiveInteger());
  quiz_assert(DecimalFunction(Sign::Positive(), Type::Floor) ==
              Sign::PositiveInteger());
  quiz_assert(DecimalFunction(Sign::StrictlyNegative(), Type::Floor) ==
              Sign::StrictlyNegativeInteger());
  quiz_assert(DecimalFunction(Sign::Negative(), Type::Floor) ==
              Sign::NegativeInteger());
  quiz_assert(DecimalFunction(Sign::Unknown(), Type::Floor) == Sign::Integer());
  quiz_assert(DecimalFunction(Sign::StrictlyPositiveInteger(), Type::Floor) ==
              Sign::StrictlyPositiveInteger());
  quiz_assert(DecimalFunction(Sign::PositiveInteger(), Type::Floor) ==
              Sign::PositiveInteger());
  quiz_assert(DecimalFunction(Sign::StrictlyNegativeInteger(), Type::Floor) ==
              Sign::StrictlyNegativeInteger());
  quiz_assert(DecimalFunction(Sign::NegativeInteger(), Type::Floor) ==
              Sign::NegativeInteger());
  quiz_assert(DecimalFunction(Sign::NonNullInteger(), Type::Floor) ==
              Sign::NonNullInteger());
  quiz_assert(DecimalFunction(Sign::Integer(), Type::Floor) == Sign::Integer());

  // Frac
  quiz_assert(DecimalFunction(Sign::Zero(), Type::Frac) == Sign::Zero());
  quiz_assert(DecimalFunction(Sign::NonNull(), Type::Frac) ==
              Sign::FinitePositive());
  quiz_assert(DecimalFunction(Sign::StrictlyPositive(), Type::Frac) ==
              Sign::FinitePositive());
  quiz_assert(DecimalFunction(Sign::Positive(), Type::Frac) ==
              Sign::FinitePositive());
  quiz_assert(DecimalFunction(Sign::StrictlyNegative(), Type::Frac) ==
              Sign::FinitePositive());
  quiz_assert(DecimalFunction(Sign::Negative(), Type::Frac) ==
              Sign::FinitePositive());
  quiz_assert(DecimalFunction(Sign::Unknown(), Type::Frac) ==
              Sign::FinitePositive());
  quiz_assert(DecimalFunction(Sign::StrictlyPositiveInteger(), Type::Frac) ==
              Sign::Zero());
  quiz_assert(DecimalFunction(Sign::PositiveInteger(), Type::Frac) ==
              Sign::Zero());
  quiz_assert(DecimalFunction(Sign::StrictlyNegativeInteger(), Type::Frac) ==
              Sign::Zero());
  quiz_assert(DecimalFunction(Sign::NegativeInteger(), Type::Frac) ==
              Sign::Zero());
  quiz_assert(DecimalFunction(Sign::NonNullInteger(), Type::Frac) ==
              Sign::Zero());
  quiz_assert(DecimalFunction(Sign::Integer(), Type::Frac) == Sign::Zero());

  // Round
  quiz_assert(DecimalFunction(Sign::Zero(), Type::Round) == Sign::Zero());
  quiz_assert(DecimalFunction(Sign::NonNull(), Type::Round) == Sign::Unknown());
  quiz_assert(DecimalFunction(Sign::StrictlyPositive(), Type::Round) ==
              Sign::Positive());
  quiz_assert(DecimalFunction(Sign::Positive(), Type::Round) ==
              Sign::Positive());
  quiz_assert(DecimalFunction(Sign::StrictlyNegative(), Type::Round) ==
              Sign::Negative());
  quiz_assert(DecimalFunction(Sign::Negative(), Type::Round) ==
              Sign::Negative());
  quiz_assert(DecimalFunction(Sign::Unknown(), Type::Round) == Sign::Unknown());
  quiz_assert(DecimalFunction(Sign::StrictlyPositiveInteger(), Type::Round) ==
              Sign::PositiveInteger());
  quiz_assert(DecimalFunction(Sign::PositiveInteger(), Type::Round) ==
              Sign::PositiveInteger());
  quiz_assert(DecimalFunction(Sign::StrictlyNegativeInteger(), Type::Round) ==
              Sign::NegativeInteger());
  quiz_assert(DecimalFunction(Sign::NegativeInteger(), Type::Round) ==
              Sign::NegativeInteger());
  quiz_assert(DecimalFunction(Sign::NonNullInteger(), Type::Round) ==
              Sign::Integer());
  quiz_assert(DecimalFunction(Sign::Integer(), Type::Round) == Sign::Integer());

  // Opposite
  quiz_assert(Opposite(Sign::Zero()) == Sign::Zero());
  quiz_assert(Opposite(Sign::NonNull()) == Sign::NonNull());
  quiz_assert(Opposite(Sign::StrictlyPositive()) == Sign::StrictlyNegative());
  quiz_assert(Opposite(Sign::Positive()) == Sign::Negative());
  quiz_assert(Opposite(Sign::StrictlyNegative()) == Sign::StrictlyPositive());
  quiz_assert(Opposite(Sign::Negative()) == Sign::Positive());
  quiz_assert(Opposite(Sign::Unknown()) == Sign::Unknown());
  quiz_assert(Opposite(Sign::StrictlyPositiveInteger()) ==
              Sign::StrictlyNegativeInteger());
  quiz_assert(Opposite(Sign::PositiveInteger()) == Sign::NegativeInteger());
  quiz_assert(Opposite(Sign::StrictlyNegativeInteger()) ==
              Sign::StrictlyPositiveInteger());
  quiz_assert(Opposite(Sign::NegativeInteger()) == Sign::PositiveInteger());
  quiz_assert(Opposite(Sign::NonNullInteger()) == Sign::NonNullInteger());
  quiz_assert(Opposite(Sign::Integer()) == Sign::Integer());
  quiz_assert(Opposite(Sign::FiniteInteger()) == Sign::FiniteInteger());
  quiz_assert(Opposite(Sign::FiniteStrictlyPositive()) ==
              Sign::FiniteStrictlyNegative());

  // Mult(..., Zero)
  quiz_assert(Mult(Sign::Zero(), Sign::Zero()) == Sign::Zero());
  quiz_assert(Mult(Sign::NonNull(), Sign::Zero()) == Sign::Zero());
  quiz_assert(Mult(Sign::StrictlyPositive(), Sign::Zero()) == Sign::Zero());
  quiz_assert(Mult(Sign::Positive(), Sign::Zero()) == Sign::Zero());
  quiz_assert(Mult(Sign::StrictlyNegative(), Sign::Zero()) == Sign::Zero());
  quiz_assert(Mult(Sign::Negative(), Sign::Zero()) == Sign::Zero());
  quiz_assert(Mult(Sign::Unknown(), Sign::Zero()) == Sign::Zero());
  // Mult(..., NonNull)
  quiz_assert(Mult(Sign::NonNull(), Sign::NonNull()) == Sign::NonNull());
  quiz_assert(Mult(Sign::StrictlyPositive(), Sign::NonNull()) ==
              Sign::NonNull());
  quiz_assert(Mult(Sign::Positive(), Sign::NonNull()) == Sign::Unknown());
  quiz_assert(Mult(Sign::StrictlyNegative(), Sign::NonNull()) ==
              Sign::NonNull());
  quiz_assert(Mult(Sign::Negative(), Sign::NonNull()) == Sign::Unknown());
  quiz_assert(Mult(Sign::Unknown(), Sign::NonNull()) == Sign::Unknown());
  quiz_assert(Mult(Sign::Finite(), Sign::NonNull()) == Sign::Unknown());
  //  Mult(..., StrictlyPositive)
  quiz_assert(Mult(Sign::StrictlyPositive(), Sign::StrictlyPositive()) ==
              Sign::StrictlyPositive());
  quiz_assert(Mult(Sign::Positive(), Sign::StrictlyPositive()) ==
              Sign::Positive());
  quiz_assert(Mult(Sign::StrictlyNegative(), Sign::StrictlyPositive()) ==
              Sign::StrictlyNegative());
  quiz_assert(Mult(Sign::Negative(), Sign::StrictlyPositive()) ==
              Sign::Negative());
  quiz_assert(Mult(Sign::Unknown(), Sign::StrictlyPositive()) ==
              Sign::Unknown());
  // Mult(..., Positive)
  quiz_assert(Mult(Sign::Positive(), Sign::Positive()) == Sign::Positive());
  quiz_assert(Mult(Sign::StrictlyNegative(), Sign::Positive()) ==
              Sign::Negative());
  quiz_assert(Mult(Sign::Negative(), Sign::Positive()) == Sign::Negative());
  quiz_assert(Mult(Sign::Unknown(), Sign::Positive()) == Sign::Unknown());
  // Mult(..., StrictlyNegative)
  quiz_assert(Mult(Sign::StrictlyNegative(), Sign::StrictlyNegative()) ==
              Sign::StrictlyPositive());
  quiz_assert(Mult(Sign::Negative(), Sign::StrictlyNegative()) ==
              Sign::Positive());
  quiz_assert(Mult(Sign::Unknown(), Sign::StrictlyNegative()) ==
              Sign::Unknown());
  // Mult(..., Negative)
  quiz_assert(Mult(Sign::Negative(), Sign::Negative()) == Sign::Positive());
  quiz_assert(Mult(Sign::Unknown(), Sign::Negative()) == Sign::Unknown());
  // Mult(..., Unknown)
  quiz_assert(Mult(Sign::Unknown(), Sign::Unknown()) == Sign::Unknown());
  quiz_assert(Mult(Sign::Finite(), Sign::Unknown()) == Sign::Unknown());

  // Add(..., Zero)
  quiz_assert(Add(Sign::Zero(), Sign::Zero()) == Sign::Zero());
  quiz_assert(Add(Sign::NonNull(), Sign::Zero()) == Sign::NonNull());
  quiz_assert(Add(Sign::StrictlyPositive(), Sign::Zero()) ==
              Sign::StrictlyPositive());
  quiz_assert(Add(Sign::Positive(), Sign::Zero()) == Sign::Positive());
  quiz_assert(Add(Sign::StrictlyNegative(), Sign::Zero()) ==
              Sign::StrictlyNegative());
  quiz_assert(Add(Sign::Negative(), Sign::Zero()) == Sign::Negative());
  quiz_assert(Add(Sign::Finite(), Sign::Zero()) == Sign::Finite());
  quiz_assert(Add(Sign::Unknown(), Sign::Zero()) == Sign::Unknown());
  // Add(..., NonNull)
  quiz_assert(Add(Sign::NonNull(), Sign::NonNull()) == Sign::Unknown());
  quiz_assert(Add(Sign::StrictlyPositive(), Sign::NonNull()) ==
              Sign::Unknown());
  quiz_assert(Add(Sign::Positive(), Sign::NonNull()) == Sign::Unknown());
  quiz_assert(Add(Sign::StrictlyNegative(), Sign::NonNull()) ==
              Sign::Unknown());
  quiz_assert(Add(Sign::Negative(), Sign::NonNull()) == Sign::Unknown());
  quiz_assert(Add(Sign::Unknown(), Sign::NonNull()) == Sign::Unknown());
  // Add(..., StrictlyPositive)
  quiz_assert(Add(Sign::StrictlyPositive(), Sign::StrictlyPositive()) ==
              Sign::StrictlyPositive());
  quiz_assert(Add(Sign::Positive(), Sign::StrictlyPositive()) ==
              Sign::StrictlyPositive());
  quiz_assert(Add(Sign::StrictlyNegative(), Sign::StrictlyPositive()) ==
              Sign::Unknown());
  quiz_assert(Add(Sign::Negative(), Sign::StrictlyPositive()) ==
              Sign::Unknown());
  quiz_assert(Add(Sign::Unknown(), Sign::StrictlyPositive()) ==
              Sign::Unknown());
  // Add(..., Positive)
  quiz_assert(Add(Sign::Positive(), Sign::Positive()) == Sign::Positive());
  quiz_assert(Add(Sign::StrictlyNegative(), Sign::Positive()) ==
              Sign::Unknown());
  quiz_assert(Add(Sign::Negative(), Sign::Positive()) == Sign::Unknown());
  quiz_assert(Add(Sign::FinitePositive(), Sign::Positive()) ==
              Sign::Positive());
  quiz_assert(Add(Sign::Unknown(), Sign::Positive()) == Sign::Unknown());
  // Add(..., StrictlyNegative)
  quiz_assert(Add(Sign::StrictlyNegative(), Sign::StrictlyNegative()) ==
              Sign::StrictlyNegative());
  quiz_assert(Add(Sign::Negative(), Sign::StrictlyNegative()) ==
              Sign::StrictlyNegative());
  quiz_assert(Add(Sign::Unknown(), Sign::StrictlyNegative()) ==
              Sign::Unknown());
  // Add(..., Negative)
  quiz_assert(Add(Sign::Negative(), Sign::Negative()) == Sign::Negative());
  quiz_assert(Add(Sign::FiniteNegative(), Sign::Negative()) ==
              Sign::Negative());
  quiz_assert(Add(Sign::Unknown(), Sign::Negative()) == Sign::Unknown());
  // Add(.., Finite)
  quiz_assert(Add(Sign::Finite(), Sign::Finite()) == Sign::Finite());
  quiz_assert(Add(Sign::Unknown(), Sign::Finite()) == Sign::Unknown());
  // Add(..., Unknown)
  quiz_assert(Add(Sign::Unknown(), Sign::Unknown()) == Sign::Unknown());
}

void assert_sign(const char* input, ComplexSign expectedSign,
                 ComplexFormat complexFormat = ComplexFormat::Cartesian,
                 Strategy strategy = Strategy::Default) {
  Tree* expression = parse(input);
  ProjectionContext ctx = {.m_complexFormat = complexFormat,
                           .m_strategy = strategy};
  Simplification::ProjectAndReduce(expression, &ctx, false);
  bool result = GetComplexSign(expression) == expectedSign;
#if POINCARE_TREE_LOG
  if (!result) {
    std::cout << input << " -> ";
    expression->logSerialize();
    std::cout << "\t\t\tWrong Sign: ";
    GetComplexSign(expression).log();
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
  assert_sign("2", Sign::FiniteStrictlyPositiveInteger());
  assert_sign("-2.5", Sign::FiniteStrictlyNegative());
  assert_sign("π", ComplexSign(Sign::FiniteStrictlyPositive(), Sign::Zero()),
              ComplexFormat::Cartesian, Strategy::ApproximateToFloat);
  assert_sign("inf", ComplexSign(Sign::StrictlyPositive(), Sign::Zero()),
              ComplexFormat::Cartesian, Strategy::ApproximateToFloat);

  assert_sign("2+π", Sign::FiniteStrictlyPositive());
  assert_sign("2-π", Sign::Finite());
  assert_sign("3 * abs(cos(x)) * -2", Sign::FiniteNegative());

  assert_sign("x", ComplexSign::RealFinite());
  assert_sign("5+i*(x+i*y)", ComplexSign::Finite());
  assert_sign("5+i*y", ComplexSign(Sign::FiniteStrictlyPositiveInteger(),
                                   Sign::Finite()));
  assert_sign("5+i*(x+i*y)", ComplexSign::Finite());
  assert_sign("x^2", Sign::FinitePositive());
  assert_sign("x^2+y^2", Sign::FinitePositive());
  assert_sign("0.5*ln(x^2+y^2)", Sign::Unknown());
  assert_sign("e^(0.5*ln(x^2+y^2))", Sign::Positive());
  assert_sign(
      "(abs(x)+i)*abs(abs(x)-i)",
      ComplexSign(Sign::FinitePositive(), Sign::FiniteStrictlyPositive()));
  assert_sign("e^(0.5*ln(12))+i*re(ln(2+i))",
              ComplexSign(Sign::StrictlyPositive(), Sign::Finite()));
  assert_sign(
      "re(abs(x)-i)+i*arg(2+i)",
      ComplexSign(Sign::FinitePositive(), Sign::FiniteStrictlyPositive()));

  // cos
  assert_sign("cos(3)", Sign::Finite());
  assert_sign("cos(2i)", Sign::StrictlyPositive());
  assert_sign("cos(-2i)", Sign::StrictlyPositive());
  assert_sign("cos(3+2i)", ComplexSign::Unknown());

  // sin
  assert_sign("sin(3)", Sign::Finite());
  assert_sign("sin(2i)",
              ComplexSign(Sign::Zero(), Sign::FiniteStrictlyPositive()));
  assert_sign("sin(-2i)",
              ComplexSign(Sign::Zero(), Sign::FiniteStrictlyNegative()));
  assert_sign("sin(3+2i)", ComplexSign::Unknown());

  // ln
  assert_sign("ln(0)", ComplexSign::Unknown());
  assert_sign("ln(3)", Sign::Finite());
  assert_sign("ln(-3)",
              ComplexSign(Sign::Finite(), Sign::FiniteStrictlyPositive()));
  assert_sign("ln(ln(3))",
              ComplexSign(Sign::Unknown(), Sign::FinitePositive()));
  assert_sign("ln(4+i)",
              ComplexSign(Sign::Finite(), Sign::FiniteStrictlyPositive()));
  assert_sign("ln(4-i)",
              ComplexSign(Sign::Finite(), Sign::FiniteStrictlyNegative()));
  assert_sign("ln(ln(x+i*y)i)", ComplexSign(Sign::Unknown(), Sign::Finite()));

  // power
  assert_sign("0^5", Sign::Zero());
  assert_sign("(1+3*i)^0", Sign::FiniteStrictlyPositiveInteger());
  assert_sign("(1+i)^4",
              ComplexSign(Sign::FiniteInteger(), Sign::FiniteInteger()));
  assert_sign("(5+i)^3",
              ComplexSign(Sign::FiniteInteger(), Sign::FiniteInteger()));
  assert_sign("(5-i)^(-1)", ComplexSign(Sign::Finite(), Sign::Finite()));

  // arg
  assert_sign("arg(5)", Sign::Zero());
  assert_sign("arg(-5)", Sign::FiniteStrictlyPositive());
  assert_sign("arg(ln(3+i*inf))", Sign::Finite());
  assert_sign("arg(3+i)", Sign::FiniteStrictlyPositive());
  assert_sign("arg(3 - i)", Sign::FiniteStrictlyNegative());

  // inf
  assert_sign("inf", Sign::StrictlyPositive());
  assert_sign("-inf", Sign::StrictlyNegative());
}
