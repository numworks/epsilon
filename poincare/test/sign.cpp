#include <apps/shared/global_context.h>
#include <poincare/src/expression/number.h>
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
static_assert(ComplexSign::FromValue(ComplexSign::Real().getRealValue(),
                                     ComplexSign::Real().getImagValue()) ==
              ComplexSign::Real());
static_assert(ComplexSign::FromValue(ComplexSign::Unknown().getRealValue(),
                                     ComplexSign::Unknown().getImagValue()) ==
              ComplexSign::Unknown());
static_assert(ComplexSign::Unknown().isUnknown());
static_assert(ComplexSign::Real().isReal());
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
                           .m_strategy = strategy,
                           .m_advanceReduce = false};
  Simplification::ToSystem(expression, &ctx);
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
  assert_sign("√(2)-2", Sign::FiniteStrictlyNegative());
  assert_sign("π-2*ln(π)", Sign::FiniteStrictlyPositive());
  assert_sign("√(535)-e^π+log(2)", Sign::FiniteStrictlyPositive());
  assert_sign("π-22/7", Sign::FiniteStrictlyNegative());
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
              ComplexSign(Sign::FiniteStrictlyPositive(), Sign::Finite()));
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
  assert_sign("ln(3)", Sign::FiniteStrictlyPositive());
  assert_sign("ln(-3)",
              ComplexSign(Sign::Finite(), Sign::FiniteStrictlyPositive()));
  assert_sign("ln(ln(3))", ComplexSign(Sign::Finite(), Sign::Zero()));
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
  assert_sign("arg(ln(3+i*inf))", Sign::FiniteStrictlyPositive());
  assert_sign("arg(3+i)", Sign::FiniteStrictlyPositive());
  assert_sign("arg(3 - i)", Sign::FiniteStrictlyNegative());

  // inf
  assert_sign("e^(arg(x+i*y)×i)", ComplexSign::Finite());
  assert_sign("inf", Sign::StrictlyPositive());
  assert_sign("-inf", Sign::StrictlyNegative());
  /* This case has been carefully crafted to enforce the following order in the
   * addition: integer + unknown integer + non integer */
  assert_sign("1+floor(x)*(1+i)+floor(y)*(1+π+i)",
              ComplexSign(Sign::Finite(), Sign::FiniteInteger()));
  assert_sign("1+floor(x)*(1+i)",
              ComplexSign(Sign::FiniteInteger(), Sign::FiniteInteger()));

  // euclidean division
  assert_sign("quo(5, 2)", Sign::FinitePositiveInteger());
  assert_sign("rem(5, 2)", Sign::FinitePositiveInteger());
  assert_sign("quo(5, -2)", Sign::FiniteNegativeInteger());
  assert_sign("rem(5, -2)", Sign::FinitePositiveInteger());
  assert_sign("quo(-5, -2)", Sign::FinitePositiveInteger());
  assert_sign("rem(-5, -2)", Sign::FinitePositiveInteger());
  assert_sign("quo(-5, 2)", Sign::FiniteNegativeInteger());
  assert_sign("rem(-5, 2)", Sign::FinitePositiveInteger());

  // binomial
  assert_sign("binomial(4,3)", Sign::Integer());
  assert_sign("binomial(-2,-1)", Sign::Integer());
  assert_sign("binomial(0.25,1)", Sign::Unknown());
}

void assert_projected_is_null(const char* input, OMG::Troolean isNull) {
  ProjectionContext context;
  Tree* e = parse(input, context.m_context);
  Simplification::ToSystem(e, &context);
  ComplexSign sign = GetComplexSign(e);
  quiz_assert_print_if_failure(
      OMG::TrooleanAnd(sign.imagSign().trooleanIsNull(),
                       sign.realSign().trooleanIsNull()) == isNull,
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
  assert_projected_is_null("0+1%", OMG::Troolean::True);
  assert_projected_is_null("1+1%", OMG::Troolean::False);
  assert_projected_is_null("1-1%", OMG::Troolean::Unknown);
}

void assert_projected_is_positive(const char* input, OMG::Troolean isPositive) {
  ProjectionContext context;
  Tree* e = parse(input, context.m_context);
  Simplification::ToSystem(e, &context);
  ComplexSign sign = GetComplexSign(e);
  quiz_assert_print_if_failure(
      OMG::TrooleanAnd(sign.imagSign().trooleanIsNull(),
                       sign.realSign().trooleanIsPositive()) == isPositive,
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
    Shared::GlobalContext* globalContext,
    Preferences::ComplexFormat complexFormat =
        Preferences::ComplexFormat::Cartesian,
    Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Radian) {
  ProjectionContext projCtx = {
      .m_complexFormat = complexFormat,
      .m_angleUnit = angleUnit,
      .m_symbolic = SymbolicComputation::ReplaceDefinedSymbols,
      .m_context = globalContext,
      .m_advanceReduce = false};
  Tree* e = parse(input, globalContext);
  Simplification::ProjectAndReduce(e, &projCtx);
  ComplexSign sign = GetComplexSign(e);
  quiz_assert_print_if_failure(
      OMG::TrooleanAnd(sign.imagSign().trooleanIsNull(),
                       sign.realSign().trooleanIsPositive()) == isPositive,
      input);
  e->removeTree();
}

QUIZ_CASE(pcj_sign_reduced_is_positive) {
  Shared::GlobalContext globalContext;
  assert_reduced_is_positive("abs(-cos(2)+i)", OMG::Troolean::True,
                             &globalContext);
  assert_reduced_is_positive("2.345ᴇ-23", OMG::Troolean::True, &globalContext);
  assert_reduced_is_positive("-2.345ᴇ-23", OMG::Troolean::False,
                             &globalContext);
  assert_reduced_is_positive("2×(-3)×abs(-32)", OMG::Troolean::False,
                             &globalContext);
  assert_reduced_is_positive("2×(-3)×abs(-32)×cos(3)", OMG::Troolean::Unknown,
                             &globalContext);
  assert_reduced_is_positive("x", OMG::Troolean::Unknown, &globalContext);
  assert_reduced_is_positive("2^(-abs(3))", OMG::Troolean::True,
                             &globalContext);
  assert_reduced_is_positive("(-2)^4", OMG::Troolean::True, &globalContext);
  assert_reduced_is_positive("(-2)^3", OMG::Troolean::False, &globalContext);
  assert_reduced_is_positive("random()", OMG::Troolean::True, &globalContext);
  assert_reduced_is_positive("42/3", OMG::Troolean::True, &globalContext);
  assert_reduced_is_positive("-23/32", OMG::Troolean::False, &globalContext);
  assert_reduced_is_positive("i", OMG::Troolean::False, &globalContext);
  assert_reduced_is_positive("-π", OMG::Troolean::False, &globalContext);
  assert_reduced_is_positive("π", OMG::Troolean::True, &globalContext);
  assert_reduced_is_positive("e", OMG::Troolean::True, &globalContext);
  assert_reduced_is_positive("0", OMG::Troolean::True, &globalContext);
  assert_reduced_is_positive("cos(π/2)", OMG::Troolean::True, &globalContext);
  assert_reduced_is_positive("cos(90)", OMG::Troolean::True, &globalContext,
                             Preferences::ComplexFormat::Cartesian,
                             Preferences::AngleUnit::Degree);
  assert_reduced_is_positive("√(-1)", OMG::Troolean::Unknown, &globalContext);
  assert_reduced_is_positive("√(-1)", OMG::Troolean::Unknown, &globalContext,
                             Preferences::ComplexFormat::Real);
  assert_reduced_is_positive("sign(π)", OMG::Troolean::True, &globalContext);
  assert_reduced_is_positive("sign(-π)", OMG::Troolean::False, &globalContext);
  assert_reduced_is_positive("1%", OMG::Troolean::True, &globalContext);
  assert_reduced_is_positive("-1+1%", OMG::Troolean::False, &globalContext);
  assert_reduced_is_positive("1-1%", OMG::Troolean::Unknown, &globalContext);
  assert_reduced_is_positive("a", OMG::Troolean::Unknown, &globalContext);
  store("42→a", &globalContext);
  assert_reduced_is_positive("a", OMG::Troolean::True, &globalContext);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
}

void assert_sign_sets_to(const char* input,
                         NonStrictSign isPositive = NonStrictSign::Positive) {
  ProjectionContext projCtx;
  Tree* e = parse(input, projCtx.m_context);
  Simplification::ProjectAndReduce(e, &projCtx);
  ComplexSign eSign = GetComplexSign(e);
  bool eIsPositive = OMG::TrooleanToBool(
      OMG::TrooleanAnd(eSign.imagSign().trooleanIsNull(),
                       eSign.realSign().trooleanIsPositive()));
  double eValue = Approximation::To<double>(e, Approximation::Parameters{});

  assert(e->isNumber());
  Number::SetSign(e, isPositive);
  ComplexSign newSign = GetComplexSign(e);
  bool newIsPositive = OMG::TrooleanToBool(
      OMG::TrooleanAnd(newSign.imagSign().trooleanIsNull(),
                       newSign.realSign().trooleanIsPositive()));
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
  ProjectionContext context;
  Tree* e = parse(input, context.m_context);
  Simplification::ToSystem(e, &context);
  ComplexSign sign = GetComplexSign(e);
  quiz_assert_print_if_failure(sign.isReal() == isReal, input);
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
  assert_projected_is_not_real(Undefined::Name());

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
