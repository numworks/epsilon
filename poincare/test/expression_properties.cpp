#include <apps/shared/global_context.h>
#include <poincare/absolute_value.h>
#include <poincare/addition.h>
#include <poincare/arc_cosine.h>
#include <poincare/arc_sine.h>
#include <poincare/arc_tangent.h>
#include <poincare/based_integer.h>
#include <poincare/complex_cartesian.h>
#include <poincare/ceiling.h>
#include <poincare/conjugate.h>
#include <poincare/constant.h>
#include <poincare/derivative.h>
#include <poincare/division.h>
#include <poincare/division_quotient.h>
#include <poincare/division_remainder.h>
#include <poincare/factor.h>
#include <poincare/factorial.h>
#include <poincare/floor.h>
#include <poincare/frac_part.h>
#include <poincare/great_common_divisor.h>
#include <poincare/imaginary_part.h>
#include <poincare/infinity.h>
#include <poincare/integral.h>
#include <poincare/least_common_multiple.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
#include <poincare/percent.h>
#include <poincare/permute_coefficient.h>
#include <poincare/power.h>
#include <poincare/product.h>
#include <poincare/randint.h>
#include <poincare/random.h>
#include <poincare/rational.h>
#include <poincare/round.h>
#include <poincare/real_part.h>
#include <poincare/sign_function.h>
#include <poincare/square_root.h>
#include <poincare/sum.h>
#include <poincare/undefined.h>
#include <poincare/unit.h>
#include <poincare/vector_norm.h>

#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_properties_is_number) {
  quiz_assert(BasedInteger::Builder("2",Integer::Base::Binary).isNumber());
  quiz_assert(BasedInteger::Builder("2",Integer::Base::Decimal).isNumber());
  quiz_assert(BasedInteger::Builder("2",Integer::Base::Hexadecimal).isNumber());
  quiz_assert(Decimal::Builder("2",3).isNumber());
  quiz_assert(Float<float>::Builder(1.0f).isNumber());
  quiz_assert(Infinity::Builder(true).isNumber());
  quiz_assert(Undefined::Builder().isNumber());
  quiz_assert(Rational::Builder(2,3).isNumber());
  quiz_assert(!Symbol::Builder('a').isNumber());
  quiz_assert(!Multiplication::Builder(Rational::Builder(1), Rational::Builder(2)).isNumber());
  quiz_assert(!Addition::Builder(Rational::Builder(1), Rational::Builder(2)).isNumber());
}

QUIZ_CASE(poincare_properties_is_number_zero) {
  Shared::GlobalContext context;
  quiz_assert(BasedInteger::Builder("2",Integer::Base::Binary).isNull(&context) == TrinaryBoolean::False );
  quiz_assert(BasedInteger::Builder("2",Integer::Base::Decimal).isNull(&context) == TrinaryBoolean::False );
  quiz_assert(BasedInteger::Builder("2",Integer::Base::Hexadecimal).isNull(&context) == TrinaryBoolean::False );
  quiz_assert(BasedInteger::Builder("0",Integer::Base::Binary).isNull(&context) == TrinaryBoolean::True );
  quiz_assert(BasedInteger::Builder("0",Integer::Base::Decimal).isNull(&context) == TrinaryBoolean::True );
  quiz_assert(BasedInteger::Builder("0",Integer::Base::Hexadecimal).isNull(&context) == TrinaryBoolean::True );
  quiz_assert(Decimal::Builder("2",3).isNull(&context) == TrinaryBoolean::False );
  quiz_assert(Decimal::Builder("0",0).isNull(&context) == TrinaryBoolean::True );
  quiz_assert(Float<float>::Builder(1.0f).isNull(&context) == TrinaryBoolean::False );
  quiz_assert(Float<float>::Builder(0.0f).isNull(&context) == TrinaryBoolean::True );
  quiz_assert(Infinity::Builder(true).isNull(&context) == TrinaryBoolean::False );
  quiz_assert(Undefined::Builder().isNull(&context) == TrinaryBoolean::Unknown);
  quiz_assert(Rational::Builder(2,3).isNull(&context) == TrinaryBoolean::False );
  quiz_assert(Rational::Builder(0,1).isNull(&context) == TrinaryBoolean::True );
  quiz_assert(Symbol::Builder('a').isNull(&context) == TrinaryBoolean::Unknown);
  quiz_assert(Multiplication::Builder(Rational::Builder(1), Rational::Builder(0)).isNull(&context) == TrinaryBoolean::Unknown);
  quiz_assert(Addition::Builder(Rational::Builder(1), Rational::Builder(-1)).isNull(&context) == TrinaryBoolean::Unknown);

  quiz_assert(AbsoluteValue::Builder(Rational::Builder(0)).isNull(&context) == TrinaryBoolean::True);
  quiz_assert(ArcSine::Builder(Rational::Builder(1,7)).isNull(&context) == TrinaryBoolean::False);
  quiz_assert(ComplexCartesian::Builder(Rational::Builder(0), Rational::Builder(3, 2)).isNull(&context) == TrinaryBoolean::False);
  quiz_assert(ComplexCartesian::Builder(Rational::Builder(0), Rational::Builder(0)).isNull(&context) == TrinaryBoolean::True);
  quiz_assert(Conjugate::Builder(ComplexCartesian::Builder(Rational::Builder(2, 3), Rational::Builder(3, 2))).isNull(&context) == TrinaryBoolean::False);
  quiz_assert(Factor::Builder(Rational::Builder(0)).isNull(&context) == TrinaryBoolean::True);
  quiz_assert(Factorial::Builder(Rational::Builder(0)).isNull(&context) == TrinaryBoolean::False);
  quiz_assert(ImaginaryPart::Builder(Rational::Builder(14)).isNull(&context) == TrinaryBoolean::True);
  quiz_assert(RealPart::Builder(Rational::Builder(0)).isNull(&context) == TrinaryBoolean::True);
  quiz_assert(Parenthesis::Builder(Rational::Builder(-7)).isNull(&context) == TrinaryBoolean::False);
  quiz_assert(SignFunction::Builder(Rational::Builder(0)).isNull(&context) == TrinaryBoolean::True);
  quiz_assert(Unit::Builder(Unit::k_powerRepresentatives, Unit::Prefix::EmptyPrefix()).isNull(&context) == TrinaryBoolean::False);
  quiz_assert(Division::Builder(Rational::Builder(0), Rational::Builder(3,7)).isNull(&context) == TrinaryBoolean::True);
  quiz_assert(Power::Builder(Rational::Builder(0), Rational::Builder(3,7)).isNull(&context) == TrinaryBoolean::True);
  quiz_assert(SquareRoot::Builder(Rational::Builder(2,5)).isNull(&context) == TrinaryBoolean::False);
  quiz_assert(PercentAddition::Builder(Rational::Builder(0), Rational::Builder(1)).isNull(&context) == TrinaryBoolean::True);
  quiz_assert(PercentAddition::Builder(Rational::Builder(1), Rational::Builder(1)).isNull(&context) == TrinaryBoolean::False);
  quiz_assert(PercentAddition::Builder(Rational::Builder(1), Rational::Builder(-1)).isNull(&context) == TrinaryBoolean::Unknown);
}

QUIZ_CASE(poincare_properties_is_random) {
  quiz_assert(Random::Builder().isRandom());
  quiz_assert(Randint::Builder(Rational::Builder(1), Rational::Builder(2)).isRandom());
  quiz_assert(!Symbol::Builder('a').isRandom());
  quiz_assert(!Rational::Builder(2,3).isRandom());
}

QUIZ_CASE(poincare_properties_is_parametered_expression) {
  quiz_assert(Derivative::Builder(Rational::Builder(1), Symbol::Builder('x'), Rational::Builder(2)).isParameteredExpression());
  quiz_assert(Integral::Builder(Rational::Builder(1), Symbol::Builder('x'), Rational::Builder(2), Rational::Builder(2)).isParameteredExpression());
  quiz_assert(Sum::Builder(Rational::Builder(1), Symbol::Builder('n'), Rational::Builder(2), Rational::Builder(2)).isParameteredExpression());
  quiz_assert(Product::Builder(Rational::Builder(1), Symbol::Builder('n'), Rational::Builder(2), Rational::Builder(2)).isParameteredExpression());
  quiz_assert(!Symbol::Builder('a').isParameteredExpression());
  quiz_assert(!Rational::Builder(2,3).isParameteredExpression());
}

QUIZ_CASE(poincare_properties_is_rational_number) {
  quiz_assert(BasedInteger::Builder("2",Integer::Base::Binary).isAlternativeFormOfRationalNumber());
  quiz_assert(BasedInteger::Builder("2",Integer::Base::Decimal).isAlternativeFormOfRationalNumber());
  quiz_assert(BasedInteger::Builder("2",Integer::Base::Hexadecimal).isAlternativeFormOfRationalNumber());
  quiz_assert(Decimal::Builder("2",3).isAlternativeFormOfRationalNumber());
  quiz_assert(Rational::Builder(2,3).isAlternativeFormOfRationalNumber());
  quiz_assert(Opposite::Builder(Rational::Builder(2,3)).isAlternativeFormOfRationalNumber());
  quiz_assert(Division::Builder(BasedInteger::Builder(1), Rational::Builder(2)).isAlternativeFormOfRationalNumber());
  quiz_assert(Division::Builder(Opposite::Builder(BasedInteger::Builder(1)), Rational::Builder(2)).isAlternativeFormOfRationalNumber());
  quiz_assert(!Float<float>::Builder(1.0f).isAlternativeFormOfRationalNumber());
  quiz_assert(!Float<double>::Builder(1.0).isAlternativeFormOfRationalNumber());
  quiz_assert(!Infinity::Builder(true).isAlternativeFormOfRationalNumber());
  quiz_assert(!Undefined::Builder().isAlternativeFormOfRationalNumber());
  quiz_assert(!Symbol::Builder('a').isAlternativeFormOfRationalNumber());
  quiz_assert(!Multiplication::Builder(Rational::Builder(1), Rational::Builder(2)).isAlternativeFormOfRationalNumber());
  quiz_assert(!Addition::Builder(Rational::Builder(1), Rational::Builder(2)).isAlternativeFormOfRationalNumber());
}

void assert_expression_has_property(const char * expression, Context * context, Expression::ExpressionTest test) {
  Expression e = parse_expression(expression, context, false);
  quiz_assert_print_if_failure(e.recursivelyMatches(test, context), expression);
}

void assert_expression_has_not_property(const char * expression, Context * context, Expression::ExpressionTest test) {
  Expression e = parse_expression(expression, context, false);
  quiz_assert_print_if_failure(!e.recursivelyMatches(test, context), expression);
}

QUIZ_CASE(poincare_properties_is_approximate) {
  Shared::GlobalContext context;
  assert_expression_has_property("3.4", &context, Expression::IsApproximate);
  assert_expression_has_property("2.3+1", &context, Expression::IsApproximate);
  assert_expression_has_not_property("a", &context, Expression::IsApproximate);
}

QUIZ_CASE(poincare_properties_is_matrix) {
  Shared::GlobalContext context;
  assert_expression_has_property("[[1,2][3,4]]", &context, Expression::IsMatrix);
  assert_expression_has_property("dim([[1,2][3,4]])/3", &context, Expression::IsMatrix);
  assert_expression_has_property("[[1,2][3,4]]^(-1)", &context, Expression::IsMatrix);
  assert_expression_has_property("inverse([[1,2][3,4]])", &context, Expression::IsMatrix);
  assert_expression_has_property("3*identity(4)", &context, Expression::IsMatrix);
  assert_expression_has_property("transpose([[1,2][3,4]])", &context, Expression::IsMatrix);
  assert_expression_has_property("ref([[1,2][3,4]])", &context, Expression::IsMatrix);
  assert_expression_has_property("rref([[1,2][3,4]])", &context, Expression::IsMatrix);
  assert_expression_has_property("cross([[1][2][3]],[[3][4][5]])", &context, Expression::IsMatrix);
  assert_expression_has_not_property("2*3+1", &context, Expression::IsMatrix);
}

void assert_expression_is_deep_matrix(const char * expression) {
  Shared::GlobalContext context;
  Expression e = parse_expression(expression, &context, false);
  quiz_assert_print_if_failure(e.deepIsMatrix(&context), expression);
}

void assert_expression_is_not_deep_matrix(const char * expression) {
  Shared::GlobalContext context;
  Expression e = parse_expression(expression, &context, false);
  quiz_assert_print_if_failure(!e.deepIsMatrix(&context), expression);
}

QUIZ_CASE(poincare_properties_deep_is_matrix) {
  assert_expression_is_not_deep_matrix("diff([[1,2][3,4]],x,2)");
  assert_expression_is_not_deep_matrix("sign([[1,2][3,4]])");
  assert_expression_is_not_deep_matrix("trace([[1,2][3,4]])");
  assert_expression_is_not_deep_matrix("det([[1,2][3,4]])");
  assert_expression_is_not_deep_matrix("3");
  assert_expression_is_deep_matrix("2*dim(2)");
}

QUIZ_CASE(poincare_properties_is_infinity) {
  Shared::GlobalContext context;
  assert_expression_has_property("3.4+inf", &context, Expression::IsInfinity);
  assert_expression_has_not_property("2.3+1", &context, Expression::IsInfinity);
  assert_expression_has_not_property("a", &context, Expression::IsInfinity);
  assert_reduce("42.3+inf→a");
  assert_expression_has_property("a", &context, Expression::IsInfinity);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
}

void assert_reduced_expression_sign(const char * expression, Poincare::TrinaryBoolean isPositive, Preferences::ComplexFormat complexFormat = Cartesian, Preferences::AngleUnit angleUnit = Radian, Preferences::UnitFormat unitFormat = MetricUnitFormat) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  e = e.cloneAndReduce(ExpressionNode::ReductionContext(&globalContext, complexFormat, angleUnit, unitFormat, ExpressionNode::ReductionTarget::SystemForApproximation));
  quiz_assert_print_if_failure(e.isPositive(&globalContext) == isPositive, expression);
}

QUIZ_CASE(poincare_properties_decimal_sign) {
  quiz_assert(Decimal::Builder(-2, 3).isPositive() == TrinaryBoolean::False);
  quiz_assert(Decimal::Builder(-2, -3).isPositive() == TrinaryBoolean::False);
  quiz_assert(Decimal::Builder(2, -3).isPositive() == TrinaryBoolean::True);
  quiz_assert(Decimal::Builder(2, 3).isPositive() == TrinaryBoolean::True);
  quiz_assert(Decimal::Builder(0, 1).isPositive() == TrinaryBoolean::True);
}

QUIZ_CASE(poincare_properties_based_integer_sign) {
  quiz_assert(BasedInteger::Builder(2, Integer::Base::Binary).isPositive() == TrinaryBoolean::True);
  quiz_assert(BasedInteger::Builder(2, Integer::Base::Decimal).isPositive() == TrinaryBoolean::True);
  quiz_assert(BasedInteger::Builder(2, Integer::Base::Hexadecimal).isPositive() == TrinaryBoolean::True);
}

QUIZ_CASE(poincare_properties_rational_sign) {
  quiz_assert(Rational::Builder(-2).isPositive() == TrinaryBoolean::False);
  quiz_assert(Rational::Builder(-2, 3).isPositive() == TrinaryBoolean::False);
  quiz_assert(Rational::Builder(2, 3).isPositive() == TrinaryBoolean::True);
  quiz_assert(Rational::Builder(0, 3).isPositive() == TrinaryBoolean::True);
}

QUIZ_CASE(poincare_properties_expression_sign) {
  Shared::GlobalContext context;
  quiz_assert(ArcCosine::Builder(Rational::Builder(-1,7)).isPositive(&context) == TrinaryBoolean::True);
  quiz_assert(ArcCosine::Builder(Symbol::Builder('a')).isPositive(&context) == TrinaryBoolean::Unknown);
  quiz_assert(ArcSine::Builder(Rational::Builder(-1,7)).isPositive(&context) == TrinaryBoolean::False);
  quiz_assert(ArcTangent::Builder(Rational::Builder(1,7)).isPositive(&context) == TrinaryBoolean::True);
  quiz_assert(Ceiling::Builder(Rational::Builder(7,3)).isPositive(&context) == TrinaryBoolean::Unknown);
  quiz_assert(Floor::Builder(Rational::Builder(7,3)).isPositive(&context) == TrinaryBoolean::Unknown);
  quiz_assert(Round::Builder(Rational::Builder(7,3), Rational::Builder(1)).isPositive(&context) == TrinaryBoolean::True);
  quiz_assert(Conjugate::Builder(ComplexCartesian::Builder(Rational::Builder(2, 3), BasedInteger::Builder(0, Integer::Base::Binary))).isPositive(&context) == TrinaryBoolean::True);
  quiz_assert(DivisionRemainder::Builder(Decimal::Builder(2.0), Decimal::Builder(3.0)).isPositive(&context) == TrinaryBoolean::True);
  quiz_assert(AbsoluteValue::Builder(Rational::Builder(-14)).isPositive(&context) == TrinaryBoolean::True);
  quiz_assert(FracPart::Builder(Rational::Builder(-7,3)).isPositive(&context) == TrinaryBoolean::True);
  quiz_assert(GreatCommonDivisor::Builder({Rational::Builder(-7),Rational::Builder(-7)}).isPositive(&context) == TrinaryBoolean::True);
  quiz_assert(LeastCommonMultiple::Builder({Rational::Builder(-7),Rational::Builder(-7)}).isPositive(&context) == TrinaryBoolean::True);
  quiz_assert(Opposite::Builder(Rational::Builder(7)).isPositive(&context) == TrinaryBoolean::False);
  quiz_assert(Parenthesis::Builder(Rational::Builder(-7)).isPositive(&context) == TrinaryBoolean::False);
  quiz_assert(PermuteCoefficient::Builder(Rational::Builder(7),Rational::Builder(8)).isPositive(&context) == TrinaryBoolean::True);
  quiz_assert(RealPart::Builder(Rational::Builder(-7)).isPositive(&context) == TrinaryBoolean::False);
  quiz_assert(SignFunction::Builder(Rational::Builder(-7)).isPositive(&context) == TrinaryBoolean::False);
  quiz_assert(Unit::Builder(Unit::k_powerRepresentatives, Unit::Prefix::EmptyPrefix()).isPositive(&context) == TrinaryBoolean::True);
  quiz_assert(VectorNorm::Builder(BasedInteger::Builder(1)).isPositive(&context) == TrinaryBoolean::True);
  quiz_assert(Division::Builder(Rational::Builder(7,3), Rational::Builder(-1)).isPositive(&context) == TrinaryBoolean::False);
  quiz_assert(DivisionQuotient::Builder(Rational::Builder(-7), Rational::Builder(-1)).isPositive(&context) == TrinaryBoolean::True);
  quiz_assert(ArcSine::Builder(ArcTangent::Builder(Opposite::Builder(RealPart::Builder(ArcCosine::Builder(Constant::Builder("π")))))).isPositive(&context) == TrinaryBoolean::False);
}

constexpr Poincare::TrinaryBoolean Positive = Poincare::TrinaryBoolean::True;
constexpr Poincare::TrinaryBoolean Negative = Poincare::TrinaryBoolean::False;
constexpr Poincare::TrinaryBoolean Unknown = Poincare::TrinaryBoolean::Unknown;

QUIZ_CASE(poincare_properties_sign) {
  assert_reduced_expression_sign("abs(-cos(2)+I)", Positive);
  assert_reduced_expression_sign("2.345ᴇ-23", Positive);
  assert_reduced_expression_sign("-2.345ᴇ-23", Negative);
  assert_reduced_expression_sign("2×(-3)×abs(-32)", Negative);
  assert_reduced_expression_sign("2×(-3)×abs(-32)×cos(3)", Unknown);
  assert_reduced_expression_sign("x", Unknown);
  assert_reduced_expression_sign("2^(-abs(3))", Positive);
  assert_reduced_expression_sign("(-2)^4", Positive);
  assert_reduced_expression_sign("(-2)^3", Negative);
  assert_reduced_expression_sign("random()", Positive);
  assert_reduced_expression_sign("42/3", Positive);
  assert_reduced_expression_sign("-23/32", Negative);
  assert_reduced_expression_sign("i", Unknown);
  assert_reduced_expression_sign("-π", Negative);
  assert_reduced_expression_sign("π", Positive);
  assert_reduced_expression_sign("e", Positive);
  assert_reduced_expression_sign("0", Positive);
  assert_reduced_expression_sign("cos(π/2)", Positive);
  assert_reduced_expression_sign("cos(90)", Positive, Cartesian, Degree);
  assert_reduced_expression_sign("√(-1)", Unknown);
  assert_reduced_expression_sign("√(-1)", Unknown, Real);
  assert_reduced_expression_sign("sign(π)", Positive);
  assert_reduced_expression_sign("sign(-π)", Negative);
  assert_reduced_expression_sign("1%", Positive);
  assert_reduced_expression_sign("-1-1%", Negative);
  assert_reduced_expression_sign("1-1%", Unknown);
  assert_reduced_expression_sign("a", Unknown);
  assert_reduce("42→a");
  assert_reduced_expression_sign("a", Positive);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
}

void assert_sign_sets_to(Expression e, Poincare::TrinaryBoolean isPositive, Preferences::ComplexFormat complexFormat = Cartesian, Preferences::AngleUnit angleUnit = Radian, Preferences::UnitFormat unitFormat = MetricUnitFormat) {
  Shared::GlobalContext context;
  TrinaryBoolean eSign = e.isPositive(&context);
  assert(eSign == TrinaryBoolean::True || eSign == TrinaryBoolean::False);
  double eValue = e.approximateToScalar<double>(&context, complexFormat, angleUnit);
  Expression f = e.setSign(isPositive == TrinaryBoolean::True, ExpressionNode::ReductionContext(&context, complexFormat, angleUnit, unitFormat, User));
  quiz_assert(f.isPositive(&context) == isPositive);
  double fValue = f.approximateToScalar<double>(&context, complexFormat, angleUnit);
  quiz_assert(fValue == (eSign == isPositive ? eValue : -eValue) || (std::isnan(fValue) == std::isnan(eValue)));
}

QUIZ_CASE(poincare_properties_set_sign_positive) {
  assert_sign_sets_to(Factorial::Builder(Rational::Builder(3)), TrinaryBoolean::True);
  assert_sign_sets_to(DivisionRemainder::Builder(Rational::Builder(33), Rational::Builder(-5)), TrinaryBoolean::True);
  assert_sign_sets_to(Power::Builder(Rational::Builder(-2), Rational::Builder(5)), TrinaryBoolean::True);
  assert_sign_sets_to(Float<float>::Builder(-1.234f), TrinaryBoolean::True);
  assert_sign_sets_to(Float<double>::Builder(-2.468), TrinaryBoolean::True);
  assert_sign_sets_to(Rational::Builder(2, 7), TrinaryBoolean::True);
  assert_sign_sets_to(RealPart::Builder(ComplexCartesian::Builder(Rational::Builder(3, 2), Rational::Builder(0))), TrinaryBoolean::True);
  assert_sign_sets_to(Constant::Builder("π"), TrinaryBoolean::True);
  assert_sign_sets_to(FracPart::Builder(Rational::Builder(-34, 5)), TrinaryBoolean::True);
  assert_sign_sets_to(Round::Builder(Rational::Builder(67, 34), Rational::Builder(1)), TrinaryBoolean::True);
  assert_sign_sets_to(DivisionQuotient::Builder(Rational::Builder(-23), Rational::Builder(12)), TrinaryBoolean::True);
  assert_sign_sets_to(Unit::Builder(&Unit::k_massRepresentatives[Unit::k_poundRepresentativeIndex], &Unit::k_prefixes[Unit::k_emptyPrefixIndex]), TrinaryBoolean::True);
  assert_sign_sets_to(Multiplication::Builder(Rational::Builder(-3, 5), Rational::Builder(2), Rational::Builder(-7, 4)), TrinaryBoolean::True);
  assert_sign_sets_to(ArcSine::Builder(Rational::Builder(-1, 3)), TrinaryBoolean::True);
  assert_sign_sets_to(Factor::Builder(Rational::Builder(120)), TrinaryBoolean::True);
  assert_sign_sets_to(ArcCosine::Builder(Rational::Builder(1, 4)), TrinaryBoolean::True);
  assert_sign_sets_to(AbsoluteValue::Builder(Symbol::Builder("p", 1)), TrinaryBoolean::True);
  assert_sign_sets_to(SignFunction::Builder(Constant::Builder("π")), TrinaryBoolean::True);
  assert_sign_sets_to(Infinity::Builder(true), TrinaryBoolean::True);
  assert_sign_sets_to(Random::Builder(), TrinaryBoolean::True);
}

void assert_expression_is_real(const char * expression) {
  Shared::GlobalContext context;
  // isReal can be call only on reduced expressions
  Expression e = parse_expression(expression, &context, false).cloneAndReduce(ExpressionNode::ReductionContext(&context, Cartesian, Radian, MetricUnitFormat, ExpressionNode::ReductionTarget::SystemForApproximation));
  quiz_assert_print_if_failure(e.isReal(&context), expression);
}

void assert_expression_is_not_real(const char * expression) {
  Shared::GlobalContext context;
  // isReal can be call only on reduced expressions
  Expression e = parse_expression(expression, &context, false).cloneAndReduce(ExpressionNode::ReductionContext(&context, Cartesian, Radian, MetricUnitFormat, ExpressionNode::ReductionTarget::SystemForApproximation));
  quiz_assert_print_if_failure(!e.isReal(&context), expression);
}

QUIZ_CASE(poincare_properties_is_real) {
  assert_expression_is_real("atan(4)");
  assert_expression_is_not_real("atan(i)");
  assert_expression_is_real("conj(4)");
  assert_expression_is_not_real("conj(i)");
  assert_expression_is_real("sin(4)");
  assert_expression_is_not_real("sin(i)");
  assert_expression_is_real("quo(2,3+a)");
  assert_expression_is_real("sign(2)");
  assert_expression_is_real("abs(2)");
  assert_expression_is_not_real("abs([[1,2]])");
  assert_expression_is_real("ceil(2)");
  assert_expression_is_not_real("ceil([[1,2]])");
  assert_expression_is_not_real("1+2+3+3×i");
  assert_expression_is_real("1+2+3+root(2,3)");
  assert_expression_is_real("1×23×3×root(2,3)");
  assert_expression_is_not_real("1×23×3×root(2,3)×3×i");
  assert_expression_is_not_real("1×23×3×[[1,2]]");
  assert_expression_is_real("π");
  assert_expression_is_not_real("nonreal");
  assert_expression_is_not_real("undef");
  assert_expression_is_real("2.3");
  assert_expression_is_real("2^3.4");
  assert_expression_is_real("(-2)^(-3)");
  assert_expression_is_not_real("i^3.4");
  assert_expression_is_not_real("2^(3.4i)");
  assert_expression_is_not_real("(-2)^0.4");
}

void assert_reduced_expression_polynomial_degree(const char * expression, int degree, const char * symbolName = "x", Preferences::ComplexFormat complexFormat = Cartesian, Preferences::AngleUnit angleUnit = Radian, Preferences::UnitFormat unitFormat = MetricUnitFormat) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  Expression result = e.cloneAndReduce(ExpressionNode::ReductionContext(&globalContext, complexFormat, angleUnit, unitFormat, SystemForApproximation));

  quiz_assert_print_if_failure(result.polynomialDegree(&globalContext, symbolName) == degree, expression);
}

QUIZ_CASE(poincare_properties_polynomial_degree) {
  assert_reduced_expression_polynomial_degree("x+1", 1);
  assert_reduced_expression_polynomial_degree("cos(2)+1", 0);
  assert_reduced_expression_polynomial_degree("diff(3×x+x,x,2)", 0);
  assert_reduced_expression_polynomial_degree("diff(3×x+x,x,x)", 0);
  assert_reduced_expression_polynomial_degree("diff(3×x+x,x,x)", 0, "a");
  assert_reduced_expression_polynomial_degree("(3×x+2)/3", 1);
  assert_reduced_expression_polynomial_degree("(3×x+2)/x", -1);
  assert_reduced_expression_polynomial_degree("int(2×x,x, 0, 1)", -1);
  assert_reduced_expression_polynomial_degree("int(2×x,x, 0, 1)", 0, "a");
  assert_reduced_expression_polynomial_degree("[[1,2][3,4]]", -1);
  assert_reduced_expression_polynomial_degree("(x^2+2)×(x+1)", 3);
  assert_reduced_expression_polynomial_degree("-(x+1)", 1);
  assert_reduced_expression_polynomial_degree("(x^2+2)^(3)", 6);
  assert_reduced_expression_polynomial_degree("2-x-x^3", 3);
  assert_reduced_expression_polynomial_degree("π×x", 1);
  assert_reduced_expression_polynomial_degree("√(-1)×x", -1, "x", Real);

  // f: y→y^2+πy+1
  assert_reduce("1+π×y+y^2→f(y)");
  assert_reduced_expression_polynomial_degree("f(x)", 2);
  // With y=1
  assert_reduce("1→y");
  assert_reduced_expression_polynomial_degree("f(x)", 2);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("y.exp").destroy();
  // a : undef and f : y→ay+πy+1
  assert_reduce("undef→a");
  assert_reduce("1+π×y+y×a→f(y)");
  assert_reduced_expression_polynomial_degree("f(x)", -1); // a is undefined
  // With a = 1
  assert_reduce("1→a");
  assert_reduced_expression_polynomial_degree("f(x)", 1);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
}

void assert_expression_has_variables(const char * expression, const char * variables[], int trueNumberOfVariables) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  constexpr static int k_maxVariableSize = Poincare::SymbolAbstract::k_maxNameSize;
  char variableBuffer[Expression::k_maxNumberOfVariables][k_maxVariableSize] = {{0}};
  int numberOfVariables = e.getVariables(&globalContext, [](const char * symbol, Poincare::Context * context) { return true; }, (char *)variableBuffer, k_maxVariableSize);
  quiz_assert_print_if_failure(trueNumberOfVariables == numberOfVariables, expression);
  if (numberOfVariables < 0) {
    // Too many variables
    return;
  }
  int index = 0;
  while (index < Expression::k_maxNumberOfVariables && (variableBuffer[index][0] != 0 || variables[index][0] != 0)) {
    quiz_assert_print_if_failure(strcmp(variableBuffer[index], variables[index]) == 0, expression);
    index++;
  }
}

QUIZ_CASE(poincare_properties_get_variables) {
  /* Warning: Theses tests are weird because you need to avoid a lot of
  * reserved identifiers like:
  * - e and i
  * - m, g, h, A which are units and can be parsed without '_' now. */
  const char * variableBuffer1[] = {"x","y",""};
  assert_expression_has_variables("x+y", variableBuffer1, 2);
  const char * variableBuffer2[] = {"x","y","z","w",""};
  assert_expression_has_variables("x+y+z+2×w", variableBuffer2, 4);
  const char * variableBuffer3[] = {"a","x","y","k","B", ""};
  assert_expression_has_variables("a+x^2+2×y+k!×B", variableBuffer3, 5);
  assert_reduce("x→BABA");
  assert_reduce("y→abab");
  const char * variableBuffer4[] = {"BABA","abab", ""};
  assert_expression_has_variables("BABA+abab", variableBuffer4, 2);
  assert_reduce("z→BBBBBB");
  const char * variableBuffer5[] = {"BBBBBB", ""};
  assert_expression_has_variables("BBBBBB", variableBuffer5, 1);
  const char * variableBuffer6[] = {""};
  assert_expression_has_variables("a+b+c+d+f+g+h+j+k+l+m+n+o+p+q+r+s+t+aa+bb+cc+dd+ee+ff+gg+hh+ii+jj+kk+ll+mm+nn+oo", variableBuffer6, -1);
  assert_expression_has_variables("a+b+c+d+f+j+k", variableBuffer6, -1);
  // f: x → 1+πx+x^2+toto
  assert_reduce("1+π×x+x^2+\"toto\"→f(x)");
  const char * variableBuffer7[] = {"\"tata\"","\"toto\"", ""};
  assert_expression_has_variables("f(\"tata\")", variableBuffer7, 2);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("BABA.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("abab.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("BBBBBB.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();

  const char * variableBuffer8[] = {"y", ""};
  assert_expression_has_variables("diff(3x,x,0)y-2", variableBuffer8, 1);
  const char * variableBuffer9[] = {"a", "b", "c", "d", "f", "j"};
  assert_expression_has_variables("a+b+c+d+f+j", variableBuffer9, 6);

  const char * variableBuffer10[] = {"c", "z", "a", "b", ""};
  assert_expression_has_variables("int(c×x×z, x, a, b)", variableBuffer10, 4);
  const char * variableBuffer11[] = {"\"box\"", "y", "z", "a", ""};
  assert_expression_has_variables("\"box\"+y×int(z,x,a,0)", variableBuffer11, 4);

  // f: x → 0
  assert_reduce("0→f(x)");
  assert_reduce("x→va");
  const char * variableBuffer12[] = {"va", ""};
  assert_expression_has_variables("f(va)", variableBuffer12, 1);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();
  // f: x → a, with a = 12
  assert_reduce("12→a");
  assert_reduce("a→f(x)");
  const char * variableBuffer13[] = {"a", "x", ""};
  assert_expression_has_variables("f(x)", variableBuffer13, 2);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
  // f: x → 1, g: x → 2
  assert_reduce("1→f(x)");
  assert_reduce("2→g(x)");
  const char * variableBuffer14[] = {"x", "y", ""};
  assert_expression_has_variables("f(g(x)+y)", variableBuffer14, 2);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("g.func").destroy();

  // x = 1
  assert_reduce("1→x");
  const char * variableBuffer15[] = {"x","y",""};
  assert_expression_has_variables("x+y", variableBuffer15, 2);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("x.exp").destroy();

  // x = a + b
  assert_reduce("1→a");
  assert_reduce("a+b+c→x");
  const char * variableBuffer16[] = {"x","y",""};
  assert_expression_has_variables("x+y", variableBuffer16, 2);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("x.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();

  // f: x → a+g(y+x), g: x → x+b, a = b + c + x
  assert_reduce("b+c+x→a");
  assert_reduce("x+b→g(x)");
  assert_reduce("a+g(x+y)→f(x)");
  const char * variableBuffer17[] = {"a", "x", "y", "b", ""};
  assert_expression_has_variables("f(x)", variableBuffer17, 4);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("g.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("va.exp").destroy();
}

void assert_reduced_expression_has_polynomial_coefficient(const char * expression, const char * symbolName, const char ** coefficients, Preferences::ComplexFormat complexFormat = Cartesian, Preferences::AngleUnit angleUnit = Radian, Preferences::UnitFormat unitFormat = MetricUnitFormat, ExpressionNode::SymbolicComputation symbolicComputation = ReplaceAllDefinedSymbolsWithDefinition) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  e = e.cloneAndReduce(ExpressionNode::ReductionContext(&globalContext, complexFormat, angleUnit, unitFormat, SystemForAnalysis, symbolicComputation));
  Expression coefficientBuffer[Poincare::Expression::k_maxNumberOfPolynomialCoefficients];
  int d = e.getPolynomialReducedCoefficients(symbolName, coefficientBuffer, &globalContext, complexFormat, Radian, unitFormat, symbolicComputation);
  for (int i = 0; i <= d; i++) {
    Expression f = parse_expression(coefficients[i], &globalContext, false);
    coefficientBuffer[i] = coefficientBuffer[i].cloneAndReduce(ExpressionNode::ReductionContext(&globalContext, complexFormat, angleUnit, unitFormat, SystemForAnalysis, symbolicComputation));
    f = f.cloneAndReduce(ExpressionNode::ReductionContext(&globalContext, complexFormat, angleUnit, unitFormat, SystemForAnalysis, symbolicComputation));
    quiz_assert_print_if_failure(coefficientBuffer[i].isIdenticalTo(f), expression);
  }
  quiz_assert_print_if_failure(coefficients[d+1] == 0, expression);
}

QUIZ_CASE(poincare_properties_get_polynomial_coefficients) {
  const char * coefficient0[] = {"2", "1", "1", 0};
  assert_reduced_expression_has_polynomial_coefficient("x^2+x+2", "x", coefficient0);
  const char * coefficient1[] = {"12+(-6)×π", "12", "3", 0}; //3×x^2+12×x-6×π+12
  assert_reduced_expression_has_polynomial_coefficient("3×(x+2)^2-6×π", "x", coefficient1);
  // TODO: decomment when enable 3-degree polynomes
  //const char * coefficient2[] = {"2+32×x", "2", "6", "2", 0}; //2×n^3+6×n^2+2×n+2+32×x
  //assert_reduced_expression_has_polynomial_coefficient("2×(n+1)^3-4n+32×x", "n", coefficient2);
  const char * coefficient3[] = {"1", "-π", "1", 0}; //x^2-π×x+1
  assert_reduced_expression_has_polynomial_coefficient("x^2-π×x+1", "x", coefficient3);

  // f: x→x^2+Px+1
  assert_reduce("1+π×x+x^2→f(x)");
  const char * coefficient4[] = {"1", "π", "1", 0}; //x^2+π×x+1
  assert_reduced_expression_has_polynomial_coefficient("f(x)", "x", coefficient4);
  const char * coefficient5[] = {"0", "i", 0}; //√(-1)x
  assert_reduced_expression_has_polynomial_coefficient("√(-1)x", "x", coefficient5);
  const char * coefficient6[] = {0}; //√(-1)x
  assert_reduced_expression_has_polynomial_coefficient("√(-1)x", "x", coefficient6, Real);

  // 3 -> x
  assert_reduce("3→x");
  const char * coefficient7[] = {"4", 0};
  assert_reduced_expression_has_polynomial_coefficient("x+1", "x", coefficient7 );
  const char * coefficient8[] = {"2", "1", 0};
  assert_reduced_expression_has_polynomial_coefficient("x+2", "x", coefficient8, Real, Radian, MetricUnitFormat, DoNotReplaceAnySymbol);
  assert_reduced_expression_has_polynomial_coefficient("x+2", "x", coefficient8, Real, Radian, MetricUnitFormat, ReplaceDefinedFunctionsWithDefinitions);
  assert_reduced_expression_has_polynomial_coefficient("f(x)", "x", coefficient4, Cartesian, Radian, MetricUnitFormat, ReplaceDefinedFunctionsWithDefinitions);

  // Clear the storage
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("x.exp").destroy();
}

void assert_reduced_expression_unit_is(const char * expression, const char * unit) {
  Shared::GlobalContext globalContext;
  ExpressionNode::ReductionContext redContext(&globalContext, Real, Degree, MetricUnitFormat, SystemForApproximation);
  Expression e = parse_expression(expression, &globalContext, false);
  Expression u1;
  e = e.reduceAndRemoveUnit(redContext, &u1);
  Expression e2 = parse_expression(unit, &globalContext, false);
  Expression u2;
  e2.reduceAndRemoveUnit(redContext, &u2);
  quiz_assert_print_if_failure(u1.isUninitialized() == u2.isUninitialized() && (u1.isUninitialized() || u1.isIdenticalTo(u2)), expression);
}

QUIZ_CASE(poincare_properties_remove_unit) {
  assert_reduced_expression_unit_is("_km", "_m");
  assert_reduced_expression_unit_is("_min/_km", "_m^(-1)×_s");
  assert_reduced_expression_unit_is("_km^3", "_m^3");
  assert_reduced_expression_unit_is("1_m+_km", "_m");
  assert_reduced_expression_unit_is("_L^2×3×_s", "_m^6×_s");
}

void assert_additional_results_compute_to(const char * expression, const char * * results, int length, Preferences::UnitFormat unitFormat = MetricUnitFormat) {
  Shared::GlobalContext globalContext;
  constexpr int maxNumberOfResults = 5;
  assert(length <= maxNumberOfResults);
  Expression additional[maxNumberOfResults];
  ExpressionNode::ReductionContext reductionContext = ExpressionNode::ReductionContext(&globalContext, Cartesian, Degree, unitFormat, User, ReplaceAllSymbolsWithUndefined, DefaultUnitConversion);
  Expression units;
  Expression e = parse_expression(expression, &globalContext, false).reduceAndRemoveUnit(reductionContext, &units);
  double value = e.approximateToScalar<double>(&globalContext, Cartesian, Degree);

  if (!Unit::ShouldDisplayAdditionalOutputs(value, units, unitFormat)) {
    quiz_assert(length == 0);
    return;
  }
  const int numberOfResults = Unit::SetAdditionalExpressions(units, value, additional, maxNumberOfResults, reductionContext, Expression());

  quiz_assert(numberOfResults == length);
  for (int i = 0; i < length; i++) {
    assert_expression_serialize_to(additional[i], results[i], Preferences::PrintFloatMode::Decimal);
  }
}

QUIZ_CASE(poincare_expression_additional_results) {
  // Time
  assert_additional_results_compute_to("3×_s", nullptr, 0);
  const char * array1[1] = {"1×_min+1×_s"};
  assert_additional_results_compute_to("61×_s", array1, 1);
  const char * array2[1] = {"1×_day+10×_h+17×_min+36×_s"};
  assert_additional_results_compute_to("123456×_s", array2, 1);
  const char * array3[1] = {"7×_day"};
  assert_additional_results_compute_to("1×_week", array3, 1);

  // Distance
  const char * array4[1] = {"19×_mi+853×_yd+1×_ft+7×_in"};
  assert_additional_results_compute_to("1234567×_in", array4, 1, Imperial);
  const char * array5[1] = {"1×_yd+7.700787×_in"};
  assert_additional_results_compute_to("1.11×_m", array5, 1, Imperial);
  assert_additional_results_compute_to("1.11×_m", nullptr, 0, MetricUnitFormat);

  // Masses
  const char * array6[1] = {"1×_shtn+240×_lb"};
  assert_additional_results_compute_to("1×_lgtn", array6, 1, Imperial);
  const char * array7[1] = {"2×_lb+3.273962×_oz"};
  assert_additional_results_compute_to("1×_kg", array7, 1, Imperial);
  assert_additional_results_compute_to("1×_kg", nullptr, 0, MetricUnitFormat);

  // Temperatures
  const char * array14[2] = {"-273.15×_°C", "-459.67×_°F"};
  assert_additional_results_compute_to("0×_K", array14, 2, MetricUnitFormat);
  const char * array15[2] = {"-279.67×_°F", "-173.15×_°C"};
  assert_additional_results_compute_to("100×_K", array15, 2, Imperial);
  const char * array16[2] = {"12.02×_°F", "262.05×_K"};
  assert_additional_results_compute_to("-11.1×_°C", array16, 2);
  const char * array17[2] = {"-20×_°C", "253.15×_K"};
  assert_additional_results_compute_to("-4×_°F", array17, 2);

  // Energy
  const char * array8[3] = {"3.6×_MJ", "1×_kW×_h", "2.246943ᴇ13×_TeV"};
  assert_additional_results_compute_to("3.6×_MN_m", array8, 3);

  // Volume
  const char * array9[2] = {"264×_gal+1×_pt+0.7528377×_cup", "1000×_L"};
  assert_additional_results_compute_to("1×_m^3", array9, 2, Imperial);
  const char * array10[2] = {"48×_gal+1×_pt+1.5625×_cup", "182.5426×_L"};
  assert_additional_results_compute_to("12345×_tbsp", array10, 2, Imperial);
  const char * array11[2] = {"182.5426×_L"};
  assert_additional_results_compute_to("12345×_tbsp", array11, 1, MetricUnitFormat);

  // Speed
  const char * array12[1] = {"3.6×_km×_h^\x12-1\x13"};
  assert_additional_results_compute_to("1×_m/_s", array12, 1, MetricUnitFormat);
  const char * array13[2] = {"2.236936×_mi×_h^\x12-1\x13", "3.6×_km×_h^\x12-1\x13"};
  assert_additional_results_compute_to("1×_m/_s", array13, 2, Imperial);
}

void assert_list_length_in_children_is(const char * definition, int targetLength) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(definition, &globalContext, false);
  quiz_assert_print_if_failure(e.lengthOfListChildren() == targetLength, definition);
}

QUIZ_CASE(poincare_expression_children_list_length) {
  assert_list_length_in_children_is("1+1", Expression::k_noList);
  assert_list_length_in_children_is("1+{}", 0);
  assert_list_length_in_children_is("1*{2,3,4}*5*{6,7,8}", 3);
  assert_list_length_in_children_is("{1,-2,3,-4}^2", 4);
  assert_list_length_in_children_is("{1,2}+{3,4,5}", Expression::k_mismatchedLists);
}

void assert_is_continuous_between_values(const char * expression, float x1, float x2, bool isContinuous) {
  Shared::GlobalContext context;
  Expression e = parse_expression(expression, &context, false);
  quiz_assert_print_if_failure(!isContinuous == e.isDiscontinuousBetweenValuesForSymbol("x", x1, x2, &context, Preferences::ComplexFormat::Cartesian, Preferences::AngleUnit::Degree), expression);
}

QUIZ_CASE(poincare_expression_continuous) {
  assert_is_continuous_between_values("x+x^2", 2.43f, 2.45f, true);
  assert_is_continuous_between_values("x+x^2", 2.45f, 2.47f, true);
  assert_is_continuous_between_values("x+floor(x^2)", 2.43f, 2.45f, false);
  assert_is_continuous_between_values("x+floor(x^2)", 2.45f, 2.47f, true);
  assert_is_continuous_between_values("x+floor(x^2)", 2.43f, 2.45f, false);
  assert_is_continuous_between_values("x+floor(x^2)", 2.45f, 2.47f, true);
  assert_is_continuous_between_values("x+ceil(x^2)", 2.43f, 2.45f, false);
  assert_is_continuous_between_values("x+ceil(x^2)", 2.45f, 2.47f, true);
  assert_is_continuous_between_values("x+round(x^2, 0)", 2.34f, 2.36f, false);
  assert_is_continuous_between_values("x+round(x^2, 0)", 2.36f, 2.38f, true);
  assert_is_continuous_between_values("x+random()", 2.43f, 2.45f, false);
  assert_is_continuous_between_values("x+randint(1,10)", 2.43f, 2.45f, false);
}

void assert_update_angle_unit(const char * expression, Preferences::AngleUnit initial, Preferences::AngleUnit updated) {
  Shared::GlobalContext context;
  Expression e = parse_expression(expression, &context, false);
  quiz_assert_print_if_failure(Expression::UpdatedAngleUnitWithExpressionInput(initial, e, &context) == updated, expression);
}

QUIZ_CASE(poincare_expression_update_angle_unit) {
  assert_update_angle_unit("45°+15°", Preferences::AngleUnit::Radian, Preferences::AngleUnit::Degree);
  assert_update_angle_unit("(15°+45rad)+15°", Preferences::AngleUnit::Radian, Preferences::AngleUnit::Radian);
  assert_update_angle_unit("45rad+15°", Preferences::AngleUnit::Degree, Preferences::AngleUnit::Degree);
  assert_update_angle_unit("45rad+15rad", Preferences::AngleUnit::Degree, Preferences::AngleUnit::Radian);
  assert_update_angle_unit("45gon+15gon", Preferences::AngleUnit::Radian, Preferences::AngleUnit::Gradian);
  assert_update_angle_unit("45rad+15°", Preferences::AngleUnit::Gradian, Preferences::AngleUnit::Gradian);
}
