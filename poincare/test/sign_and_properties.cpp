#include <poincare/src/expression/number.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/properties.h>
#include <poincare/src/expression/sign.h>
#include <poincare/src/expression/systematic_reduction.h>
#include <poincare/src/expression/variables.h>
#include <poincare/symbol_context.h>

#include "helper.h"
#include "helpers/symbol_store.h"
#include "poincare/properties.h"
#include "poincare/sign.h"

using namespace Poincare;
using namespace Poincare::Internal;

static_assert(Properties::Zero().sign().isNull() &&
              Properties::Zero().isInteger());
static_assert(ComplexProperties::FromValue(
                  ComplexProperties::RealInteger().getRealValue(),
                  ComplexProperties::RealInteger().getImagValue()) ==
              ComplexProperties::RealInteger());
static_assert(
    ComplexProperties::FromValue(ComplexProperties::Real().getRealValue(),
                                 ComplexProperties::Real().getImagValue()) ==
    ComplexProperties::Real());
static_assert(
    ComplexProperties::FromValue(ComplexProperties::Unknown().getRealValue(),
                                 ComplexProperties::Unknown().getImagValue()) ==
    ComplexProperties::Unknown());
static_assert(ComplexProperties::Unknown().isUnknown());
static_assert(ComplexProperties::Real().isReal());
static_assert(ComplexProperties::RealInteger().isReal() &&
              ComplexProperties::RealInteger().isInteger());

namespace Poincare {
extern Properties RelaxIntegerProperty(Properties p);
extern Properties DecimalFunction(Properties p, Type type);
extern Sign Opposite(Sign s);
extern Properties Opposite(Properties p);
extern Sign Mult(Sign s1, Sign s2);
extern Properties Mult(Properties p1, Properties p2);
extern Sign Add(Sign s1, Sign s2);
extern Properties Add(Properties p1, Properties p2);
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

  // Opposite
  quiz_assert(Opposite(Sign::Zero()) == Sign::Zero());
  quiz_assert(Opposite(Sign::NonNull()) == Sign::NonNull());
  quiz_assert(Opposite(Sign::StrictlyPositive()) == Sign::StrictlyNegative());
  quiz_assert(Opposite(Sign::Positive()) == Sign::Negative());
  quiz_assert(Opposite(Sign::StrictlyNegative()) == Sign::StrictlyPositive());
  quiz_assert(Opposite(Sign::Negative()) == Sign::Positive());
  quiz_assert(Opposite(Sign::Unknown()) == Sign::Unknown());

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

  // Add(..., Zero)
  quiz_assert(Add(Sign::Zero(), Sign::Zero()) == Sign::Zero());
  quiz_assert(Add(Sign::NonNull(), Sign::Zero()) == Sign::NonNull());
  quiz_assert(Add(Sign::StrictlyPositive(), Sign::Zero()) ==
              Sign::StrictlyPositive());
  quiz_assert(Add(Sign::Positive(), Sign::Zero()) == Sign::Positive());
  quiz_assert(Add(Sign::StrictlyNegative(), Sign::Zero()) ==
              Sign::StrictlyNegative());
  quiz_assert(Add(Sign::Negative(), Sign::Zero()) == Sign::Negative());
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
  quiz_assert(Add(Sign::Unknown(), Sign::Negative()) == Sign::Unknown());
  // Add(..., Unknown)
  quiz_assert(Add(Sign::Unknown(), Sign::Unknown()) == Sign::Unknown());
}

QUIZ_CASE(pcj_properties_methods) {
  // OR operator
  quiz_assert((Properties::Zero() || Properties::NonNull()) ==
              Properties::Unknown());
  quiz_assert((Properties::Zero() || Properties::StrictlyPositive()) ==
              Properties::Positive());
  quiz_assert((Properties::Zero() || Properties::StrictlyNegative()) ==
              Properties::Negative());
  quiz_assert((Properties::StrictlyPositive() ||
               Properties::StrictlyNegative()) == Properties::NonNull());
  quiz_assert((Properties::Positive() || Properties::StrictlyNegative()) ==
              Properties::Unknown());
  quiz_assert((Properties::StrictlyPositive() || Properties::Negative()) ==
              Properties::Unknown());
  quiz_assert((Properties::Positive() || Properties::Negative()) ==
              Properties::Unknown());
  quiz_assert((Properties::Finite() || Properties::Unknown()) ==
              Properties::Unknown());

  // Opposite
  quiz_assert(Opposite(Properties::Zero()) == Properties::Zero());
  quiz_assert(Opposite(Properties::NonNull()) == Properties::NonNull());
  quiz_assert(Opposite(Properties::StrictlyPositive()) ==
              Properties::StrictlyNegative());
  quiz_assert(Opposite(Properties::Positive()) == Properties::Negative());
  quiz_assert(Opposite(Properties::StrictlyNegative()) ==
              Properties::StrictlyPositive());
  quiz_assert(Opposite(Properties::Negative()) == Properties::Positive());
  quiz_assert(Opposite(Properties::Unknown()) == Properties::Unknown());
  quiz_assert(Opposite(Properties::StrictlyPositiveInteger()) ==
              Properties::StrictlyNegativeInteger());
  quiz_assert(Opposite(Properties::PositiveInteger()) ==
              Properties::NegativeInteger());
  quiz_assert(Opposite(Properties::StrictlyNegativeInteger()) ==
              Properties::StrictlyPositiveInteger());
  quiz_assert(Opposite(Properties::NegativeInteger()) ==
              Properties::PositiveInteger());
  quiz_assert(Opposite(Properties::NonNullInteger()) ==
              Properties::NonNullInteger());
  quiz_assert(Opposite(Properties::Integer()) == Properties::Integer());
  quiz_assert(Opposite(Properties::FiniteInteger()) ==
              Properties::FiniteInteger());
  quiz_assert(Opposite(Properties::FiniteStrictlyPositive()) ==
              Properties::FiniteStrictlyNegative());

  // Mult(..., Zero)
  quiz_assert(Mult(Properties::Zero(), Properties::Zero()) ==
              Properties::Zero());
  quiz_assert(Mult(Properties::NonNull(), Properties::Zero()) ==
              Properties::Zero());
  quiz_assert(Mult(Properties::StrictlyPositive(), Properties::Zero()) ==
              Properties::Zero());
  quiz_assert(Mult(Properties::Positive(), Properties::Zero()) ==
              Properties::Zero());
  quiz_assert(Mult(Properties::StrictlyNegative(), Properties::Zero()) ==
              Properties::Zero());
  quiz_assert(Mult(Properties::Negative(), Properties::Zero()) ==
              Properties::Zero());
  quiz_assert(Mult(Properties::Unknown(), Properties::Zero()) ==
              Properties::Zero());
  // Mult(..., NonNull)
  quiz_assert(Mult(Properties::NonNull(), Properties::NonNull()) ==
              Properties::NonNull());
  quiz_assert(Mult(Properties::StrictlyPositive(), Properties::NonNull()) ==
              Properties::NonNull());
  quiz_assert(Mult(Properties::Positive(), Properties::NonNull()) ==
              Properties::Unknown());
  quiz_assert(Mult(Properties::StrictlyNegative(), Properties::NonNull()) ==
              Properties::NonNull());
  quiz_assert(Mult(Properties::Negative(), Properties::NonNull()) ==
              Properties::Unknown());
  quiz_assert(Mult(Properties::Unknown(), Properties::NonNull()) ==
              Properties::Unknown());
  quiz_assert(Mult(Properties::Finite(), Properties::NonNull()) ==
              Properties::Unknown());
  //  Mult(..., StrictlyPositive)
  quiz_assert(
      Mult(Properties::StrictlyPositive(), Properties::StrictlyPositive()) ==
      Properties::StrictlyPositive());
  quiz_assert(Mult(Properties::Positive(), Properties::StrictlyPositive()) ==
              Properties::Positive());
  quiz_assert(
      Mult(Properties::StrictlyNegative(), Properties::StrictlyPositive()) ==
      Properties::StrictlyNegative());
  quiz_assert(Mult(Properties::Negative(), Properties::StrictlyPositive()) ==
              Properties::Negative());
  quiz_assert(Mult(Properties::Unknown(), Properties::StrictlyPositive()) ==
              Properties::Unknown());
  // Mult(..., Positive)
  quiz_assert(Mult(Properties::Positive(), Properties::Positive()) ==
              Properties::Positive());
  quiz_assert(Mult(Properties::StrictlyNegative(), Properties::Positive()) ==
              Properties::Negative());
  quiz_assert(Mult(Properties::Negative(), Properties::Positive()) ==
              Properties::Negative());
  quiz_assert(Mult(Properties::Unknown(), Properties::Positive()) ==
              Properties::Unknown());
  // Mult(..., StrictlyNegative)
  quiz_assert(
      Mult(Properties::StrictlyNegative(), Properties::StrictlyNegative()) ==
      Properties::StrictlyPositive());
  quiz_assert(Mult(Properties::Negative(), Properties::StrictlyNegative()) ==
              Properties::Positive());
  quiz_assert(Mult(Properties::Unknown(), Properties::StrictlyNegative()) ==
              Properties::Unknown());
  // Mult(..., Negative)
  quiz_assert(Mult(Properties::Negative(), Properties::Negative()) ==
              Properties::Positive());
  quiz_assert(Mult(Properties::Unknown(), Properties::Negative()) ==
              Properties::Unknown());
  // Mult(..., Unknown)
  quiz_assert(Mult(Properties::Unknown(), Properties::Unknown()) ==
              Properties::Unknown());
  quiz_assert(Mult(Properties::Finite(), Properties::Unknown()) ==
              Properties::Unknown());

  // Add(..., Zero)
  quiz_assert(Add(Properties::Zero(), Properties::Zero()) ==
              Properties::Zero());
  quiz_assert(Add(Properties::NonNull(), Properties::Zero()) ==
              Properties::NonNull());
  quiz_assert(Add(Properties::StrictlyPositive(), Properties::Zero()) ==
              Properties::StrictlyPositive());
  quiz_assert(Add(Properties::Positive(), Properties::Zero()) ==
              Properties::Positive());
  quiz_assert(Add(Properties::StrictlyNegative(), Properties::Zero()) ==
              Properties::StrictlyNegative());
  quiz_assert(Add(Properties::Negative(), Properties::Zero()) ==
              Properties::Negative());
  quiz_assert(Add(Properties::Finite(), Properties::Zero()) ==
              Properties::Finite());
  quiz_assert(Add(Properties::Unknown(), Properties::Zero()) ==
              Properties::Unknown());
  // Add(..., NonNull)
  quiz_assert(Add(Properties::NonNull(), Properties::NonNull()) ==
              Properties::Unknown());
  quiz_assert(Add(Properties::StrictlyPositive(), Properties::NonNull()) ==
              Properties::Unknown());
  quiz_assert(Add(Properties::Positive(), Properties::NonNull()) ==
              Properties::Unknown());
  quiz_assert(Add(Properties::StrictlyNegative(), Properties::NonNull()) ==
              Properties::Unknown());
  quiz_assert(Add(Properties::Negative(), Properties::NonNull()) ==
              Properties::Unknown());
  quiz_assert(Add(Properties::Unknown(), Properties::NonNull()) ==
              Properties::Unknown());
  // Add(..., StrictlyPositive)
  quiz_assert(
      Add(Properties::StrictlyPositive(), Properties::StrictlyPositive()) ==
      Properties::StrictlyPositive());
  quiz_assert(Add(Properties::Positive(), Properties::StrictlyPositive()) ==
              Properties::StrictlyPositive());
  quiz_assert(Add(Properties::StrictlyNegative(),
                  Properties::StrictlyPositive()) == Properties::Unknown());
  quiz_assert(Add(Properties::Negative(), Properties::StrictlyPositive()) ==
              Properties::Unknown());
  quiz_assert(Add(Properties::Unknown(), Properties::StrictlyPositive()) ==
              Properties::Unknown());
  // Add(..., Positive)
  quiz_assert(Add(Properties::Positive(), Properties::Positive()) ==
              Properties::Positive());
  quiz_assert(Add(Properties::StrictlyNegative(), Properties::Positive()) ==
              Properties::Unknown());
  quiz_assert(Add(Properties::Negative(), Properties::Positive()) ==
              Properties::Unknown());
  quiz_assert(Add(Properties::FinitePositive(), Properties::Positive()) ==
              Properties::Positive());
  quiz_assert(Add(Properties::Unknown(), Properties::Positive()) ==
              Properties::Unknown());
  // Add(..., StrictlyNegative)
  quiz_assert(
      Add(Properties::StrictlyNegative(), Properties::StrictlyNegative()) ==
      Properties::StrictlyNegative());
  quiz_assert(Add(Properties::Negative(), Properties::StrictlyNegative()) ==
              Properties::StrictlyNegative());
  quiz_assert(Add(Properties::Unknown(), Properties::StrictlyNegative()) ==
              Properties::Unknown());
  // Add(..., Negative)
  quiz_assert(Add(Properties::Negative(), Properties::Negative()) ==
              Properties::Negative());
  quiz_assert(Add(Properties::FiniteNegative(), Properties::Negative()) ==
              Properties::Negative());
  quiz_assert(Add(Properties::Unknown(), Properties::Negative()) ==
              Properties::Unknown());
  // Add(.., Finite)
  quiz_assert(Add(Properties::Finite(), Properties::Finite()) ==
              Properties::Finite());
  quiz_assert(Add(Properties::Unknown(), Properties::Finite()) ==
              Properties::Unknown());
  // Add(..., Unknown)
  quiz_assert(Add(Properties::Unknown(), Properties::Unknown()) ==
              Properties::Unknown());

  // RelaxIntegerProperty
  quiz_assert(RelaxIntegerProperty(Properties::Zero()) == Properties::Zero());
  quiz_assert(RelaxIntegerProperty(Properties::NonNull()) ==
              Properties::NonNull());
  quiz_assert(RelaxIntegerProperty(Properties::StrictlyPositive()) ==
              Properties::StrictlyPositive());
  quiz_assert(RelaxIntegerProperty(Properties::Positive()) ==
              Properties::Positive());
  quiz_assert(RelaxIntegerProperty(Properties::StrictlyNegative()) ==
              Properties::StrictlyNegative());
  quiz_assert(RelaxIntegerProperty(Properties::Negative()) ==
              Properties::Negative());
  quiz_assert(RelaxIntegerProperty(Properties::Unknown()) ==
              Properties::Unknown());
  quiz_assert(RelaxIntegerProperty(Properties::StrictlyPositiveInteger()) ==
              Properties::StrictlyPositive());
  quiz_assert(RelaxIntegerProperty(Properties::PositiveInteger()) ==
              Properties::Positive());
  quiz_assert(RelaxIntegerProperty(Properties::StrictlyNegativeInteger()) ==
              Properties::StrictlyNegative());
  quiz_assert(RelaxIntegerProperty(Properties::NegativeInteger()) ==
              Properties::Negative());
  quiz_assert(RelaxIntegerProperty(Properties::NonNullInteger()) ==
              Properties::NonNull());
  quiz_assert(RelaxIntegerProperty(Properties::Integer()) ==
              Properties::Unknown());
  quiz_assert(RelaxIntegerProperty(Properties::FiniteInteger()) ==
              Properties::Finite());
  quiz_assert(
      RelaxIntegerProperty(Properties::FiniteStrictlyPositiveInteger()) ==
      Properties::FiniteStrictlyPositive());

  // Ceil
  quiz_assert(DecimalFunction(Properties::Zero(), Type::Ceil) ==
              Properties::Zero());
  quiz_assert(DecimalFunction(Properties::NonNull(), Type::Ceil) ==
              Properties::Integer());
  quiz_assert(DecimalFunction(Properties::StrictlyPositive(), Type::Ceil) ==
              Properties::StrictlyPositiveInteger());
  quiz_assert(DecimalFunction(Properties::Positive(), Type::Ceil) ==
              Properties::PositiveInteger());
  quiz_assert(DecimalFunction(Properties::StrictlyNegative(), Type::Ceil) ==
              Properties::NegativeInteger());
  quiz_assert(DecimalFunction(Properties::Negative(), Type::Ceil) ==
              Properties::NegativeInteger());
  quiz_assert(DecimalFunction(Properties::Unknown(), Type::Ceil) ==
              Properties::Integer());
  quiz_assert(
      DecimalFunction(Properties::StrictlyPositiveInteger(), Type::Ceil) ==
      Properties::StrictlyPositiveInteger());
  quiz_assert(DecimalFunction(Properties::PositiveInteger(), Type::Ceil) ==
              Properties::PositiveInteger());
  quiz_assert(
      DecimalFunction(Properties::StrictlyNegativeInteger(), Type::Ceil) ==
      Properties::StrictlyNegativeInteger());
  quiz_assert(DecimalFunction(Properties::NegativeInteger(), Type::Ceil) ==
              Properties::NegativeInteger());
  quiz_assert(DecimalFunction(Properties::NonNullInteger(), Type::Ceil) ==
              Properties::NonNullInteger());
  quiz_assert(DecimalFunction(Properties::Integer(), Type::Ceil) ==
              Properties::Integer());

  // Floor
  quiz_assert(DecimalFunction(Properties::Zero(), Type::Floor) ==
              Properties::Zero());
  quiz_assert(DecimalFunction(Properties::NonNull(), Type::Floor) ==
              Properties::Integer());
  quiz_assert(DecimalFunction(Properties::StrictlyPositive(), Type::Floor) ==
              Properties::PositiveInteger());
  quiz_assert(DecimalFunction(Properties::Positive(), Type::Floor) ==
              Properties::PositiveInteger());
  quiz_assert(DecimalFunction(Properties::StrictlyNegative(), Type::Floor) ==
              Properties::StrictlyNegativeInteger());
  quiz_assert(DecimalFunction(Properties::Negative(), Type::Floor) ==
              Properties::NegativeInteger());
  quiz_assert(DecimalFunction(Properties::Unknown(), Type::Floor) ==
              Properties::Integer());
  quiz_assert(
      DecimalFunction(Properties::StrictlyPositiveInteger(), Type::Floor) ==
      Properties::StrictlyPositiveInteger());
  quiz_assert(DecimalFunction(Properties::PositiveInteger(), Type::Floor) ==
              Properties::PositiveInteger());
  quiz_assert(
      DecimalFunction(Properties::StrictlyNegativeInteger(), Type::Floor) ==
      Properties::StrictlyNegativeInteger());
  quiz_assert(DecimalFunction(Properties::NegativeInteger(), Type::Floor) ==
              Properties::NegativeInteger());
  quiz_assert(DecimalFunction(Properties::NonNullInteger(), Type::Floor) ==
              Properties::NonNullInteger());
  quiz_assert(DecimalFunction(Properties::Integer(), Type::Floor) ==
              Properties::Integer());

  // Frac
  quiz_assert(DecimalFunction(Properties::Zero(), Type::Frac) ==
              Properties::Zero());
  quiz_assert(DecimalFunction(Properties::NonNull(), Type::Frac) ==
              Properties::FinitePositive());
  quiz_assert(DecimalFunction(Properties::StrictlyPositive(), Type::Frac) ==
              Properties::FinitePositive());
  quiz_assert(DecimalFunction(Properties::Positive(), Type::Frac) ==
              Properties::FinitePositive());
  quiz_assert(DecimalFunction(Properties::StrictlyNegative(), Type::Frac) ==
              Properties::FinitePositive());
  quiz_assert(DecimalFunction(Properties::Negative(), Type::Frac) ==
              Properties::FinitePositive());
  quiz_assert(DecimalFunction(Properties::Unknown(), Type::Frac) ==
              Properties::FinitePositive());
  quiz_assert(DecimalFunction(Properties::StrictlyPositiveInteger(),
                              Type::Frac) == Properties::Zero());
  quiz_assert(DecimalFunction(Properties::PositiveInteger(), Type::Frac) ==
              Properties::Zero());
  quiz_assert(DecimalFunction(Properties::StrictlyNegativeInteger(),
                              Type::Frac) == Properties::Zero());
  quiz_assert(DecimalFunction(Properties::NegativeInteger(), Type::Frac) ==
              Properties::Zero());
  quiz_assert(DecimalFunction(Properties::NonNullInteger(), Type::Frac) ==
              Properties::Zero());
  quiz_assert(DecimalFunction(Properties::Integer(), Type::Frac) ==
              Properties::Zero());

  // Round
  quiz_assert(DecimalFunction(Properties::Zero(), Type::Round) ==
              Properties::Zero());
  quiz_assert(DecimalFunction(Properties::NonNull(), Type::Round) ==
              Properties::Unknown());
  quiz_assert(DecimalFunction(Properties::StrictlyPositive(), Type::Round) ==
              Properties::Positive());
  quiz_assert(DecimalFunction(Properties::Positive(), Type::Round) ==
              Properties::Positive());
  quiz_assert(DecimalFunction(Properties::StrictlyNegative(), Type::Round) ==
              Properties::Negative());
  quiz_assert(DecimalFunction(Properties::Negative(), Type::Round) ==
              Properties::Negative());
  quiz_assert(DecimalFunction(Properties::Unknown(), Type::Round) ==
              Properties::Unknown());
  quiz_assert(DecimalFunction(Properties::StrictlyPositiveInteger(),
                              Type::Round) == Properties::PositiveInteger());
  quiz_assert(DecimalFunction(Properties::PositiveInteger(), Type::Round) ==
              Properties::PositiveInteger());
  quiz_assert(DecimalFunction(Properties::StrictlyNegativeInteger(),
                              Type::Round) == Properties::NegativeInteger());
  quiz_assert(DecimalFunction(Properties::NegativeInteger(), Type::Round) ==
              Properties::NegativeInteger());
  quiz_assert(DecimalFunction(Properties::NonNullInteger(), Type::Round) ==
              Properties::Integer());
  quiz_assert(DecimalFunction(Properties::Integer(), Type::Round) ==
              Properties::Integer());
}

void assert_properties(const char* input, ComplexProperties expectedProperties,
                       ComplexFormat complexFormat = ComplexFormat::Cartesian,
                       Strategy strategy = Strategy::Default) {
  Tree* expression = parse(input);
  ProjectionContext ctx = {.m_complexFormat = complexFormat,
                           .m_strategy = strategy,
                           .m_advanceReduce = false};
  Simplification::ToSystem(expression, &ctx);
  bool result = GetComplexProperties(expression) == expectedProperties;
#if POINCARE_TREE_LOG
  if (!result) {
    std::cout << input << " -> ";
    expression->logSerialize();
    std::cout << "\t\t\tWrong Properties: ";
    GetComplexProperties(expression).log();
    std::cout << "\t\t\tInstead of: ";
    expectedProperties.log();
  }
#endif
  quiz_assert(result);
  expression->removeTree();
}

void assert_properties(const char* input, Properties expectedProperties) {
  assert_properties(input,
                    ComplexProperties(expectedProperties, Properties::Zero()));
}

QUIZ_CASE(pcj_properties) {
  assert_properties("2", Properties::FiniteStrictlyPositiveInteger());
  assert_properties("-2.5", Properties::FiniteStrictlyNegative());
  assert_properties("π",
                    ComplexProperties(Properties::FiniteStrictlyPositive(),
                                      Properties::Zero()),
                    ComplexFormat::Cartesian, Strategy::ApproximateToFloat);
  assert_properties(
      "inf",
      ComplexProperties(Properties::StrictlyPositive(), Properties::Zero()),
      ComplexFormat::Cartesian, Strategy::ApproximateToFloat);

  assert_properties("2+π", Properties::FiniteStrictlyPositive());
  assert_properties("√(2)-2", Properties::FiniteStrictlyNegative());
  assert_properties("π-2*ln(π)", Properties::FiniteStrictlyPositive());
  assert_properties("√(535)-e^π+log(2)", Properties::FiniteStrictlyPositive());
  assert_properties("π-22/7", Properties::FiniteStrictlyNegative());
  assert_properties("3 * abs(cos(x)) * -2", Properties::FiniteNegative());

  assert_properties("x", ComplexProperties::RealFinite());
  assert_properties("5+i*(x+i*y)", ComplexProperties::Finite());
  assert_properties(
      "5+i*y", ComplexProperties(Properties::FiniteStrictlyPositiveInteger(),
                                 Properties::Finite()));
  assert_properties("5+i*(x+i*y)", ComplexProperties::Finite());
  assert_properties("x^2", Properties::FinitePositive());
  assert_properties("x^2+y^2", Properties::FinitePositive());
  assert_properties("0.5*ln(x^2+y^2)", Properties::Unknown());
  assert_properties("e^(0.5*ln(x^2+y^2))", Properties::Positive());
  assert_properties("(abs(x)+i)*abs(abs(x)-i)",
                    ComplexProperties(Properties::FinitePositive(),
                                      Properties::FiniteStrictlyPositive()));
  assert_properties("e^(0.5*ln(12))+i*re(ln(2+i))",
                    ComplexProperties(Properties::FiniteStrictlyPositive(),
                                      Properties::Finite()));
  assert_properties("re(abs(x)-i)+i*arg(2+i)",
                    ComplexProperties(Properties::FinitePositive(),
                                      Properties::FiniteStrictlyPositive()));

  // cos
  assert_properties("cos(3)", Properties::Finite());
  assert_properties("cos(2i)", Properties::StrictlyPositive());
  assert_properties("cos(-2i)", Properties::StrictlyPositive());
  assert_properties("cos(3+2i)", ComplexProperties::Unknown());

  // sin
  assert_properties("sin(3)", Properties::Finite());
  assert_properties("sin(2i)",
                    ComplexProperties(Properties::Zero(),
                                      Properties::FiniteStrictlyPositive()));
  assert_properties("sin(-2i)",
                    ComplexProperties(Properties::Zero(),
                                      Properties::FiniteStrictlyNegative()));
  assert_properties("sin(3+2i)", ComplexProperties::Unknown());

  // ln
  assert_properties("ln(0)", ComplexProperties::Unknown());
  assert_properties("ln(3)", Properties::FiniteStrictlyPositive());
  assert_properties("ln(-3)",
                    ComplexProperties(Properties::Finite(),
                                      Properties::FiniteStrictlyPositive()));
  assert_properties(
      "ln(ln(3))", ComplexProperties(Properties::Finite(), Properties::Zero()));
  assert_properties("ln(4+i)",
                    ComplexProperties(Properties::Finite(),
                                      Properties::FiniteStrictlyPositive()));
  assert_properties("ln(4-i)",
                    ComplexProperties(Properties::Finite(),
                                      Properties::FiniteStrictlyNegative()));
  assert_properties("ln(ln(x+i*y)i)", ComplexProperties(Properties::Unknown(),
                                                        Properties::Finite()));

  // power
  assert_properties("0^5", Properties::Zero());
  assert_properties("(1+3*i)^0", Properties::FiniteStrictlyPositiveInteger());
  assert_properties("(1+i)^4", ComplexProperties(Properties::FiniteInteger(),
                                                 Properties::FiniteInteger()));
  assert_properties("(5+i)^3", ComplexProperties(Properties::FiniteInteger(),
                                                 Properties::FiniteInteger()));
  assert_properties("(5-i)^(-1)", ComplexProperties(Properties::Finite(),
                                                    Properties::Finite()));

  // arg
  assert_properties("arg(5)", Properties::Zero());
  assert_properties("arg(-5)", Properties::FiniteStrictlyPositive());
  assert_properties("arg(ln(3+i*inf))", Properties::FiniteStrictlyPositive());
  assert_properties("arg(3+i)", Properties::FiniteStrictlyPositive());
  assert_properties("arg(3 - i)", Properties::FiniteStrictlyNegative());

  // inf
  assert_properties("e^(arg(x+i*y)×i)", ComplexProperties::Finite());
  assert_properties("inf", Properties::StrictlyPositive());
  assert_properties("-inf", Properties::StrictlyNegative());
  /* This case has been carefully crafted to enforce the following order in the
   * addition: integer + unknown integer + non integer */
  assert_properties(
      "1+floor(x)*(1+i)+floor(y)*(1+π+i)",
      ComplexProperties(Properties::Finite(), Properties::FiniteInteger()));
  assert_properties("1+floor(x)*(1+i)",
                    ComplexProperties(Properties::FiniteInteger(),
                                      Properties::FiniteInteger()));

  // euclidean division
  assert_properties("quo(5, 2)", Properties::FinitePositiveInteger());
  assert_properties("rem(5, 2)", Properties::FinitePositiveInteger());
  assert_properties("quo(5, -2)", Properties::FiniteNegativeInteger());
  assert_properties("rem(5, -2)", Properties::FinitePositiveInteger());
  assert_properties("quo(-5, -2)", Properties::FinitePositiveInteger());
  assert_properties("rem(-5, -2)", Properties::FinitePositiveInteger());
  assert_properties("quo(-5, 2)", Properties::FiniteNegativeInteger());
  assert_properties("rem(-5, 2)", Properties::FinitePositiveInteger());

  // binomial
  assert_properties("binomial(4,3)", Properties::Integer());
  assert_properties("binomial(-2,-1)", Properties::Integer());
  assert_properties("binomial(0.25,1)", Properties::Unknown());
}

void assert_projected_is_null(const char* input, OMG::Troolean isNull) {
  Tree* e = parse(input);
  ProjectionContext context;
  Simplification::ToSystem(e, &context);
  ComplexProperties properties = GetComplexProperties(e);
  quiz_assert_print_if_failure(
      OMG::TrooleanAnd(properties.imagSign().trooleanIsNull(),
                       properties.realSign().trooleanIsNull()) == isNull,
      input);
  e->removeTree();
}

QUIZ_CASE(pcj_sign_is_zero) {
  assert_projected_is_null("2", OMG::Troolean::False);
  assert_projected_is_null("0b10", OMG::Troolean::False);
  assert_projected_is_null("0x2", OMG::Troolean::False);
  assert_projected_is_null("0", OMG::Troolean::True);
  assert_projected_is_null("0b0", OMG::Troolean::True);
  assert_projected_is_null("0x0", OMG::Troolean::True);
  assert_projected_is_null("2.3", OMG::Troolean::False);
  assert_projected_is_null("0.0", OMG::Troolean::True);
  assert_projected_is_null("π", OMG::Troolean::False);
  assert_projected_is_null("inf", OMG::Troolean::False);
  assert_projected_is_null("undef", OMG::Troolean::Unknown);
  assert_projected_is_null("2/3", OMG::Troolean::False);
  assert_projected_is_null("0/1", OMG::Troolean::True);
  assert_projected_is_null("a", OMG::Troolean::Unknown);
  assert_projected_is_null("1*0", OMG::Troolean::True);
  assert_projected_is_null("1-1", OMG::Troolean::Unknown);
  assert_projected_is_null("abs(0)", OMG::Troolean::True);
  assert_projected_is_null("arcsin(1/7)", OMG::Troolean::False);
  assert_projected_is_null("0+3/2*i", OMG::Troolean::False);
  assert_projected_is_null("0+0i", OMG::Troolean::True);
  assert_projected_is_null("2/3+3/2*i", OMG::Troolean::False);
  assert_projected_is_null("factor(0)", OMG::Troolean::True);
  assert_projected_is_null("0!", OMG::Troolean::False);
  assert_projected_is_null("im(14)", OMG::Troolean::True);
  assert_projected_is_null("re(0)", OMG::Troolean::True);
  assert_projected_is_null("(-7)", OMG::Troolean::False);
  assert_projected_is_null("sign(0)", OMG::Troolean::True);
  // TODO: Should be false
  assert_projected_is_null("_W", OMG::Troolean::Unknown);
  assert_projected_is_null("0/(3/7)", OMG::Troolean::True);
  assert_projected_is_null("sqrt(2/5)", OMG::Troolean::False);
  assert_projected_is_null("0+1%", OMG::Troolean::Unknown);
  assert_projected_is_null("1+1%", OMG::Troolean::Unknown);
  assert_projected_is_null("1-1%", OMG::Troolean::Unknown);
}

void assert_projected_is_positive(const char* input, OMG::Troolean isPositive) {
  Tree* e = parse(input);
  ProjectionContext context;
  Simplification::ToSystem(e, &context);
  ComplexProperties properties = GetComplexProperties(e);
  quiz_assert_print_if_failure(
      OMG::TrooleanAnd(properties.imagSign().trooleanIsNull(),
                       properties.realSign().trooleanIsPositive()) ==
          isPositive,
      input);
  e->removeTree();
}

QUIZ_CASE(pcj_sign_is_positive) {
  assert_projected_is_positive("-0.002", OMG::Troolean::False);
  assert_projected_is_positive("-2000", OMG::Troolean::False);
  assert_projected_is_positive("0.002", OMG::Troolean::True);
  assert_projected_is_positive("2000", OMG::Troolean::True);
  assert_projected_is_positive("0", OMG::Troolean::True);
  assert_projected_is_positive("2", OMG::Troolean::True);
  assert_projected_is_positive("0b10", OMG::Troolean::True);
  assert_projected_is_positive("0x2", OMG::Troolean::True);
  assert_projected_is_positive("-2/3", OMG::Troolean::False);
  assert_projected_is_positive("2/3", OMG::Troolean::True);
  assert_projected_is_positive("0/3", OMG::Troolean::True);
  // TODO: Should be true
  assert_projected_is_positive("arccos(-1/7)", OMG::Troolean::Unknown);
  assert_projected_is_positive("arccos(a)", OMG::Troolean::Unknown);
  assert_projected_is_positive("arcsin(-1/7)", OMG::Troolean::False);
  assert_projected_is_positive("arctan(1/7)", OMG::Troolean::True);
  assert_projected_is_positive("ceil(7/3)", OMG::Troolean::True);
  assert_projected_is_positive("floor(7/3)", OMG::Troolean::True);
  assert_projected_is_positive("round(7/3,1)", OMG::Troolean::True);
  assert_projected_is_positive("conj(2/3+0*i)", OMG::Troolean::True);
  assert_projected_is_positive("rem(2.0,3.0)", OMG::Troolean::True);
  assert_projected_is_positive("abs(-14)", OMG::Troolean::True);
  // TODO: Should be true
  assert_projected_is_positive("frac(-7/3)", OMG::Troolean::Unknown);
  assert_projected_is_positive("gcd(-7,-7)", OMG::Troolean::True);
  assert_projected_is_positive("lcm(-7,-7)", OMG::Troolean::True);
  assert_projected_is_positive("-7", OMG::Troolean::False);
  assert_projected_is_positive("(-7)", OMG::Troolean::False);
  assert_projected_is_positive("permute(7,8)", OMG::Troolean::True);
  assert_projected_is_positive("re(-7)", OMG::Troolean::False);
  assert_projected_is_positive("sign(-7)", OMG::Troolean::False);
  assert_projected_is_positive("_W", OMG::Troolean::True);
  assert_projected_is_positive("norm([[1]])", OMG::Troolean::True);
  // TODO: Should be false
  assert_projected_is_positive("(7/3)/(-1)", OMG::Troolean::Unknown);
  assert_projected_is_positive("quo(-7,-1)", OMG::Troolean::True);
  // TODO: Should be true
  assert_projected_is_positive("arcsin(arctan(-re(arccos(π))))",
                               OMG::Troolean::Unknown);
}

void assert_reduced_is_positive(
    const char* input, OMG::Troolean isPositive,
    const SymbolContext& symbolContext = Poincare::EmptySymbolContext{},
    ComplexFormat complexFormat = ComplexFormat::Cartesian,
    AngleUnit angleUnit = AngleUnit::Radian) {
  ProjectionContext projCtx = {
      .m_complexFormat = complexFormat,
      .m_angleUnit = angleUnit,
      .m_symbolic = SymbolicComputation::ReplaceDefinedSymbols,
      .m_context = symbolContext,
      .m_advanceReduce = false};
  Tree* e = parse(input, symbolContext);
  Simplification::ProjectAndReduce(e, &projCtx);
  ComplexProperties properties = GetComplexProperties(e);
  quiz_assert_print_if_failure(
      OMG::TrooleanAnd(properties.imagSign().trooleanIsNull(),
                       properties.realSign().trooleanIsPositive()) ==
          isPositive,
      input);
  e->removeTree();
}

QUIZ_CASE(pcj_sign_reduced_is_positive) {
  assert_reduced_is_positive("abs(-cos(2)+i)", OMG::Troolean::True);
  assert_reduced_is_positive("2.345ᴇ-23", OMG::Troolean::True);
  assert_reduced_is_positive("-2.345ᴇ-23", OMG::Troolean::False);
  assert_reduced_is_positive("2×(-3)×abs(-32)", OMG::Troolean::False);
  assert_reduced_is_positive("2×(-3)×abs(-32)×cos(3)", OMG::Troolean::Unknown);
  assert_reduced_is_positive("x", OMG::Troolean::Unknown);
  assert_reduced_is_positive("2^(-abs(3))", OMG::Troolean::True);
  assert_reduced_is_positive("(-2)^4", OMG::Troolean::True);
  assert_reduced_is_positive("(-2)^3", OMG::Troolean::False);
  assert_reduced_is_positive("random()", OMG::Troolean::True);
  assert_reduced_is_positive("42/3", OMG::Troolean::True);
  assert_reduced_is_positive("-23/32", OMG::Troolean::False);
  assert_reduced_is_positive("i", OMG::Troolean::False);
  assert_reduced_is_positive("-π", OMG::Troolean::False);
  assert_reduced_is_positive("π", OMG::Troolean::True);
  assert_reduced_is_positive("e", OMG::Troolean::True);
  assert_reduced_is_positive("0", OMG::Troolean::True);
  assert_reduced_is_positive("cos(π/2)", OMG::Troolean::True);
  assert_reduced_is_positive("cos(90)", OMG::Troolean::True,
                             EmptySymbolContext{}, ComplexFormat::Cartesian,
                             AngleUnit::Degree);
  assert_reduced_is_positive("√(-1)", OMG::Troolean::Unknown);
  assert_reduced_is_positive("√(-1)", OMG::Troolean::Unknown,
                             EmptySymbolContext{}, ComplexFormat::Real);
  assert_reduced_is_positive("sign(π)", OMG::Troolean::True);
  assert_reduced_is_positive("sign(-π)", OMG::Troolean::False);
  assert_reduced_is_positive("1%", OMG::Troolean::Unknown);
  assert_reduced_is_positive("-1+1%", OMG::Troolean::Unknown);
  assert_reduced_is_positive("1-1%", OMG::Troolean::Unknown);
  assert_reduced_is_positive("a", OMG::Troolean::Unknown);
  PoincareTest::SymbolStore symbolStore;
  PoincareTest::store("42→a", symbolStore);
  assert_reduced_is_positive("a", OMG::Troolean::True, symbolStore);
}

void assert_sign_sets_to(const char* input,
                         NonStrictSign isPositive = NonStrictSign::Positive) {
  Tree* e = parse(input);
  ProjectionContext projCtx;
  Simplification::ProjectAndReduce(e, &projCtx);
  ComplexProperties eSign = GetComplexProperties(e);
  bool eIsPositive = OMG::TrooleanToBool(
      OMG::TrooleanAnd(eSign.imagSign().trooleanIsNull(),
                       eSign.realSign().trooleanIsPositive()));
  double eValue = Approximation::To<double>(e, Approximation::Parameters{});

  assert(e->isNumber());
  Number::SetSign(e, isPositive);
  ComplexProperties newProperties = GetComplexProperties(e);
  bool newIsPositive = OMG::TrooleanToBool(
      OMG::TrooleanAnd(newProperties.imagSign().trooleanIsNull(),
                       newProperties.realSign().trooleanIsPositive()));
  double eNewValue = Approximation::To<double>(e, Approximation::Parameters{});

  quiz_assert(newIsPositive == (isPositive == NonStrictSign::Positive));
  bool isOpposite = (isPositive == NonStrictSign::Positive) != eIsPositive;
  quiz_assert_print_if_failure(eNewValue == (isOpposite ? -eValue : eValue),
                               input);
  e->removeTree();
}

QUIZ_CASE(pcj_sign_set) {
  assert_sign_sets_to("-2");
  assert_sign_sets_to("3!");
  assert_sign_sets_to("rem(33,-5)");
  assert_sign_sets_to("(-2)^5");
  assert_sign_sets_to("-1.234");
  assert_sign_sets_to("-2.468");
  assert_sign_sets_to("2/7");
  assert_sign_sets_to("-3/2");
  assert_sign_sets_to("re(3/2+0*i)");
  assert_sign_sets_to("frac(-34/5)");
  assert_sign_sets_to("round(67/34,1)");
  assert_sign_sets_to("quo(-23,12)");
  assert_sign_sets_to("(-3/5)*2*(-7/4)");
  assert_sign_sets_to("sign(pi)");
  assert_sign_sets_to("sign(-e)");

  assert_sign_sets_to("2", NonStrictSign::Negative);
  assert_sign_sets_to("10/3", NonStrictSign::Negative);
  assert_sign_sets_to("abs(-3)", NonStrictSign::Negative);
  assert_sign_sets_to("0.123", NonStrictSign::Negative);
}

void assert_projected_is_real_or_not(const char* input, bool isReal = true) {
  Tree* e = parse(input);
  ProjectionContext context;
  Simplification::ToSystem(e, &context);
  ComplexProperties properties = GetComplexProperties(e);
  quiz_assert_print_if_failure(properties.isReal() == isReal, input);
  e->removeTree();
}

void assert_projected_is_real(const char* input) {
  assert_projected_is_real_or_not(input, true);
}

void assert_projected_is_not_real(const char* input) {
  assert_projected_is_real_or_not(input, false);
}

QUIZ_CASE(pcj_sign_is_real) {
  // Numbers
  assert_projected_is_real("2.3");
  assert_projected_is_not_real("2.3i");
  assert_projected_is_real("random()");
  assert_projected_is_not_real("nonreal");
  assert_projected_is_not_real("undef");

  // Real if does not contain a matrix
  assert_projected_is_real("abs(2)");
  assert_projected_is_real("abs(2i)");
  assert_projected_is_real("abs({2,3})");
  assert_projected_is_not_real("abs([[2,3]])");
  assert_projected_is_real("abs(sum({0}×k,k,0,0))");
  assert_projected_is_real("binomial(2,3)");
  assert_projected_is_real("binomial(2i,3)");
  assert_projected_is_real("binomial(2,3i)");
  assert_projected_is_real("binomial({2,3},4)");
  assert_projected_is_real("binomial(2,{3,4})");
  assert_projected_is_not_real("binomial([[2,3]],4)");
  assert_projected_is_not_real("binomial(2,[[3,4]])");
  assert_projected_is_real("ceil(2)");
  assert_projected_is_real("ceil(2i)");
  assert_projected_is_real("ceil({2,3})");
  assert_projected_is_not_real("ceil([[2,3]])");
  assert_projected_is_real("arg(2)");
  assert_projected_is_real("arg(2i)");
  assert_projected_is_real("arg({2,3})");
  assert_projected_is_not_real("arg([[2,3]])");
  assert_projected_is_real("quo(2,3)");
  assert_projected_is_real("quo(2i,3)");
  assert_projected_is_real("quo(2,3i)");
  assert_projected_is_real("quo(2,3+a)");
  assert_projected_is_real("quo({2,3},4)");
  assert_projected_is_real("quo(2,{3,4})");
  assert_projected_is_not_real("quo([[2,3]],4)");
  assert_projected_is_not_real("quo(2,[[3,4]])");
  assert_projected_is_real("rem(2,3)");
  assert_projected_is_real("rem(2i,3)");
  assert_projected_is_real("rem(2,3i)");
  assert_projected_is_real("rem({2,3},4)");
  assert_projected_is_real("rem(2,{3,4})");
  assert_projected_is_not_real("rem([[2,3]],4)");
  assert_projected_is_not_real("rem(2,[[3,4]])");
  assert_projected_is_real("(2)!");
  assert_projected_is_real("(2i)!");
  assert_projected_is_real("({2,3})!");
  assert_projected_is_not_real("([[2,3]])!");
  assert_projected_is_real("floor(2)");
  assert_projected_is_real("floor(2i)");
  assert_projected_is_real("floor({2,3})");
  assert_projected_is_not_real("floor([[2,3]])");
  assert_projected_is_real("frac(2)");
  assert_projected_is_not_real(
      "frac(2i)");  // frac(2i) -> 2i - floor(2i) so sign is not real
  assert_projected_is_not_real("frac({2,3})");
  assert_projected_is_not_real("frac([[2,3]])");
  assert_projected_is_real("gcd(2,3)");
  assert_projected_is_real("gcd(2i,3)");
  assert_projected_is_real("gcd(2,3i)");
  assert_projected_is_real("gcd({2,3},4)");
  assert_projected_is_real("gcd(2,{3,4})");
  assert_projected_is_not_real("gcd([[2,3]],4)");
  assert_projected_is_not_real("gcd(2,[[3,4]])");
  assert_projected_is_real("im(2)");
  assert_projected_is_real("im(2i)");
  assert_projected_is_real("im({2,3})");
  assert_projected_is_not_real("im([[2,3]])");
  assert_projected_is_real("lcm(2,3)");
  assert_projected_is_real("lcm(2i,3)");
  assert_projected_is_real("lcm(2,3i)");
  assert_projected_is_real("lcm({2,3},4)");
  assert_projected_is_real("lcm(2,{3,4})");
  assert_projected_is_not_real("lcm([[2,3]],4)");
  assert_projected_is_not_real("lcm(2,[[3,4]])");
  assert_projected_is_real("permute(2,3)");
  assert_projected_is_real("permute(2i,3)");
  assert_projected_is_real("permute(2,3i)");
  assert_projected_is_real("permute({2,3},4)");
  assert_projected_is_real("permute(2,{3,4})");
  assert_projected_is_not_real("permute([[2,3]],4)");
  assert_projected_is_not_real("permute(2,[[3,4]])");
  assert_projected_is_real("randint(2,3)");
  assert_projected_is_real("randint(2i,3)");
  assert_projected_is_real("randint(2,3i)");
  assert_projected_is_real("randint({2,3},4)");
  assert_projected_is_real("randint(2,{3,4})");
  assert_projected_is_not_real("randint([[2,3]],4)");
  assert_projected_is_not_real("randint(2,[[3,4]])");
  assert_projected_is_real("randint(randintnorep(0,0,0)×i,0)");
  assert_projected_is_real("re(2)");
  assert_projected_is_real("re(2i)");
  assert_projected_is_real("re({2,3})");
  assert_projected_is_not_real("re([[2,3]])");
  assert_projected_is_real("round(2)");
  assert_projected_is_real("round(2i)");
  assert_projected_is_real("round({2,3})");
  assert_projected_is_not_real("round([[2,3]])");
  assert_projected_is_real("sign(2)");
  assert_projected_is_not_real("sign(2i)");
  assert_projected_is_not_real("sign({2,3})");
  assert_projected_is_not_real("sign([[2,3]])");
  assert_projected_is_real("diff(2x,x,1)");
  assert_projected_is_real("diff({2,3}x,x,1)");
  // Should be non-real, complex case is handled at simplification for now.
  assert_projected_is_real("diff(2*i*x,x,1)");
  assert_projected_is_not_real("diff([[2,3]]x,x,1)");

  // Depends on children
  assert_projected_is_real("2×_mg");
  assert_projected_is_not_real("2i×_mg");
  assert_projected_is_not_real("{2,3}×_mg");
  assert_projected_is_not_real("[[2,3]]×_mg");
  assert_projected_is_not_real("1+2+3+3×i");
  assert_projected_is_real("atan(4)");
  assert_projected_is_not_real("atan(i)");
  assert_projected_is_real("conj(4)");
  assert_projected_is_not_real("conj(i)");
  assert_projected_is_real("cos(4)");
  assert_projected_is_real("cos(i)");
  assert_projected_is_real("sin(4)");
  assert_projected_is_not_real("sin(i)");
  assert_projected_is_real("tan(4)");
  assert_projected_is_not_real("tan(i)");

  // Constant
  assert_projected_is_real("π");
  assert_projected_is_real("e");
  assert_projected_is_not_real("i");

  // Power
  /* TODO: Should be real
   * assert_projected_is_real("2^3.4"); */
  assert_projected_is_real("(-2)^(-3)");
  assert_projected_is_not_real("i^3.4");
  assert_projected_is_not_real("2^(3.4i)");
  assert_projected_is_not_real("(-2)^0.4");
}
