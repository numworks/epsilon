#if 0
#include <apps/shared/global_context.h>

#include "helper.h"

using namespace Poincare;

QUIZ_DISABLED_CASE(poincare_properties_is_rational_number) {
  quiz_assert(BasedInteger::Builder("2", OMG::Base::Binary)
                  .isAlternativeFormOfRationalNumber());
  quiz_assert(BasedInteger::Builder("2", OMG::Base::Decimal)
                  .isAlternativeFormOfRationalNumber());
  quiz_assert(BasedInteger::Builder("2", OMG::Base::Hexadecimal)
                  .isAlternativeFormOfRationalNumber());
  quiz_assert(Decimal::Builder("2", 3).isAlternativeFormOfRationalNumber());
  quiz_assert(Rational::Builder(2, 3).isAlternativeFormOfRationalNumber());
  quiz_assert(Opposite::Builder(Rational::Builder(2, 3))
                  .isAlternativeFormOfRationalNumber());
  quiz_assert(Division::Builder(BasedInteger::Builder(1), Rational::Builder(2))
                  .isAlternativeFormOfRationalNumber());
  quiz_assert(Division::Builder(Opposite::Builder(BasedInteger::Builder(1)),
                                Rational::Builder(2))
                  .isAlternativeFormOfRationalNumber());
  quiz_assert(!Float<float>::Builder(1.0f).isAlternativeFormOfRationalNumber());
  quiz_assert(!Float<double>::Builder(1.0).isAlternativeFormOfRationalNumber());
  quiz_assert(!Infinity::Builder(true).isAlternativeFormOfRationalNumber());
  quiz_assert(!Undefined::Builder().isAlternativeFormOfRationalNumber());
  quiz_assert(!Symbol::Builder('a').isAlternativeFormOfRationalNumber());
  quiz_assert(
      !Multiplication::Builder(Rational::Builder(1), Rational::Builder(2))
           .isAlternativeFormOfRationalNumber());
  quiz_assert(!Addition::Builder(Rational::Builder(1), Rational::Builder(2))
                   .isAlternativeFormOfRationalNumber());
}

QUIZ_DISABLED_CASE(poincare_properties_is_infinity) {
  Shared::GlobalContext context;
  assert_expression_has_property("3.4+inf", &context, OExpression::IsPlusOrMinusInfinity);
  assert_expression_has_not_property("2.3+1", &context,
                                     OExpression::IsPlusOrMinusInfinity);
  assert_expression_has_not_property("a", &context, OExpression::IsPlusOrMinusInfinity);
  assert_reduce_and_store("42.3+inf→a");
  assert_expression_has_property("a", &context, OExpression::IsPlusOrMinusInfinity);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
}

QUIZ_DISABLED_CASE(poincare_properties_in_parametric) {
  Shared::GlobalContext context;
  assert_expression_has_property("2x+1", &context, OExpression::IsSymbolic);
  assert_expression_has_not_property("diff(x^2,x,3)", &context,
                                     OExpression::IsSymbolic);
  assert_expression_has_property("diff(y^2+x^2,x,3)", &context,
                                 OExpression::IsSymbolic);
  assert_reduce_and_store("1+inf→x");
  assert_expression_has_property("x", &context, OExpression::IsPlusOrMinusInfinity);
  assert_expression_has_not_property("diff(x^2,x,3)", &context,
                                     OExpression::IsPlusOrMinusInfinity);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp").destroy();
}

void assert_reduced_expression_sign(
    const char* expression, OMG::Troolean isPositive,
    Preferences::ComplexFormat complexFormat = Cartesian,
    Preferences::AngleUnit angleUnit = Radian,
    Preferences::UnitFormat unitFormat = MetricUnitFormat) {
  Shared::GlobalContext globalContext;
  OExpression e = parse_expression(expression, &globalContext);
  e = e.cloneAndReduce(ReductionContext(&globalContext, complexFormat,
                                        angleUnit, unitFormat,
                                        SystemForApproximation));
  quiz_assert_print_if_failure(e.isPositive(&globalContext) == isPositive,
                               expression);
}

QUIZ_DISABLED_CASE(poincare_properties_decimal_sign) {
  quiz_assert(Decimal::Builder(-2, 3).isPositive() == OMG::Troolean::False);
  quiz_assert(Decimal::Builder(-2, -3).isPositive() == OMG::Troolean::False);
  quiz_assert(Decimal::Builder(2, -3).isPositive() == OMG::Troolean::True);
  quiz_assert(Decimal::Builder(2, 3).isPositive() == OMG::Troolean::True);
  quiz_assert(Decimal::Builder(0, 1).isPositive() == OMG::Troolean::True);
}

QUIZ_DISABLED_CASE(poincare_properties_based_integer_sign) {
  quiz_assert(BasedInteger::Builder(2, OMG::Base::Binary).isPositive() ==
              OMG::Troolean::True);
  quiz_assert(BasedInteger::Builder(2, OMG::Base::Decimal).isPositive() ==
              OMG::Troolean::True);
  quiz_assert(BasedInteger::Builder(2, OMG::Base::Hexadecimal).isPositive() ==
              OMG::Troolean::True);
}

QUIZ_DISABLED_CASE(poincare_properties_rational_sign) {
  quiz_assert(Rational::Builder(-2).isPositive() == OMG::Troolean::False);
  quiz_assert(Rational::Builder(-2, 3).isPositive() == OMG::Troolean::False);
  quiz_assert(Rational::Builder(2, 3).isPositive() == OMG::Troolean::True);
  quiz_assert(Rational::Builder(0, 3).isPositive() == OMG::Troolean::True);
}

QUIZ_DISABLED_CASE(poincare_properties_expression_sign) {
  Shared::GlobalContext context;
  quiz_assert(
      ArcCosine::Builder(Rational::Builder(-1, 7)).isPositive(&context) ==
      OMG::Troolean::True);
  quiz_assert(ArcCosine::Builder(Symbol::Builder('a')).isPositive(&context) ==
              OMG::Troolean::Unknown);
  quiz_assert(ArcSine::Builder(Rational::Builder(-1, 7)).isPositive(&context) ==
              OMG::Troolean::False);
  quiz_assert(
      ArcTangent::Builder(Rational::Builder(1, 7)).isPositive(&context) ==
      OMG::Troolean::True);
  quiz_assert(Ceiling::Builder(Rational::Builder(7, 3)).isPositive(&context) ==
              OMG::Troolean::Unknown);
  quiz_assert(Floor::Builder(Rational::Builder(7, 3)).isPositive(&context) ==
              OMG::Troolean::Unknown);
  quiz_assert(Round::Builder(Rational::Builder(7, 3), Rational::Builder(1))
                  .isPositive(&context) == OMG::Troolean::True);
  quiz_assert(
      Conjugate::Builder(ComplexCartesian::Builder(
                             Rational::Builder(2, 3),
                             BasedInteger::Builder(0, OMG::Base::Binary)))
          .isPositive(&context) == OMG::Troolean::True);
  quiz_assert(
      DivisionRemainder::Builder(Decimal::Builder(2.0), Decimal::Builder(3.0))
          .isPositive(&context) == OMG::Troolean::True);
  quiz_assert(
      AbsoluteValue::Builder(Rational::Builder(-14)).isPositive(&context) ==
      OMG::Troolean::True);
  quiz_assert(
      FracPart::Builder(Rational::Builder(-7, 3)).isPositive(&context) ==
      OMG::Troolean::True);
  quiz_assert(GreatCommonDivisor::Builder(
                  {Rational::Builder(-7), Rational::Builder(-7)})
                  .isPositive(&context) == OMG::Troolean::True);
  quiz_assert(LeastCommonMultiple::Builder(
                  {Rational::Builder(-7), Rational::Builder(-7)})
                  .isPositive(&context) == OMG::Troolean::True);
  quiz_assert(Opposite::Builder(Rational::Builder(7)).isPositive(&context) ==
              OMG::Troolean::False);
  quiz_assert(
      Parenthesis::Builder(Rational::Builder(-7)).isPositive(&context) ==
      OMG::Troolean::False);
  quiz_assert(
      PermuteCoefficient::Builder(Rational::Builder(7), Rational::Builder(8))
          .isPositive(&context) == OMG::Troolean::True);
  quiz_assert(RealPart::Builder(Rational::Builder(-7)).isPositive(&context) ==
              OMG::Troolean::False);
  quiz_assert(
      SignFunction::Builder(Rational::Builder(-7)).isPositive(&context) ==
      OMG::Troolean::False);
  quiz_assert(OUnit::Builder(OUnit::k_powerRepresentatives,
                             OUnit::Prefix::EmptyPrefix())
                  .isPositive(&context) == OMG::Troolean::True);
  quiz_assert(
      VectorNorm::Builder(BasedInteger::Builder(1)).isPositive(&context) ==
      OMG::Troolean::True);
  quiz_assert(Division::Builder(Rational::Builder(7, 3), Rational::Builder(-1))
                  .isPositive(&context) == OMG::Troolean::False);
  quiz_assert(
      DivisionQuotient::Builder(Rational::Builder(-7), Rational::Builder(-1))
          .isPositive(&context) == OMG::Troolean::True);
  quiz_assert(
      ArcSine::Builder(ArcTangent::Builder(Opposite::Builder(RealPart::Builder(
                           ArcCosine::Builder(Constant::PiBuilder())))))
          .isPositive(&context) == OMG::Troolean::False);
}

constexpr OMG::Troolean Positive = OMG::Troolean::True;
constexpr OMG::Troolean Negative = OMG::Troolean::False;
constexpr OMG::Troolean Unknown = OMG::Troolean::Unknown;

QUIZ_DISABLED_CASE(poincare_properties_sign) {
  assert_reduced_expression_sign("abs(-cos(2)+i)", Positive);
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
  assert_reduce_and_store("42→a");
  assert_reduced_expression_sign("a", Positive);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
}

void assert_sign_sets_to(
    OExpression e, OMG::Troolean isPositive,
    Preferences::ComplexFormat complexFormat = Cartesian,
    Preferences::AngleUnit angleUnit = Radian,
    Preferences::UnitFormat unitFormat = MetricUnitFormat) {
  Shared::GlobalContext context;
  OMG::Troolean eSign = e.isPositive(&context);
  assert(eSign == OMG::Troolean::True || eSign == OMG::Troolean::False);
  ReductionContext reductionContext(&context, complexFormat, angleUnit,
                                    unitFormat, User);
  ApproximationContext approximationContext(reductionContext);
  double eValue = e.approximateToRealScalar<double>(approximationContext);
  OExpression f =
      e.setSign(isPositive == OMG::Troolean::True, reductionContext);
  quiz_assert(f.isPositive(&context) == isPositive);
  double fValue = f.approximateToRealScalar<double>(approximationContext);
  quiz_assert(fValue == (eSign == isPositive ? eValue : -eValue) ||
              (std::isnan(fValue) == std::isnan(eValue)));
}

QUIZ_DISABLED_CASE(poincare_properties_set_sign_positive) {
  assert_sign_sets_to(Factorial::Builder(Rational::Builder(3)),
                      OMG::Troolean::True);
  assert_sign_sets_to(
      DivisionRemainder::Builder(Rational::Builder(33), Rational::Builder(-5)),
      OMG::Troolean::True);
  assert_sign_sets_to(
      Power::Builder(Rational::Builder(-2), Rational::Builder(5)),
      OMG::Troolean::True);
  assert_sign_sets_to(Float<float>::Builder(-1.234f), OMG::Troolean::True);
  assert_sign_sets_to(Float<double>::Builder(-2.468), OMG::Troolean::True);
  assert_sign_sets_to(Rational::Builder(2, 7), OMG::Troolean::True);
  assert_sign_sets_to(RealPart::Builder(ComplexCartesian::Builder(
                          Rational::Builder(3, 2), Rational::Builder(0))),
                      OMG::Troolean::True);
  assert_sign_sets_to(Constant::PiBuilder(), OMG::Troolean::True);
  assert_sign_sets_to(FracPart::Builder(Rational::Builder(-34, 5)),
                      OMG::Troolean::True);
  assert_sign_sets_to(
      Round::Builder(Rational::Builder(67, 34), Rational::Builder(1)),
      OMG::Troolean::True);
  assert_sign_sets_to(
      DivisionQuotient::Builder(Rational::Builder(-23), Rational::Builder(12)),
      OMG::Troolean::True);
  assert_sign_sets_to(
      OUnit::Builder(
          &OUnit::k_massRepresentatives[OUnit::k_poundRepresentativeIndex],
          &OUnit::k_prefixes[OUnit::k_emptyPrefixIndex]),
      OMG::Troolean::True);
  assert_sign_sets_to(
      Multiplication::Builder(Rational::Builder(-3, 5), Rational::Builder(2),
                              Rational::Builder(-7, 4)),
      OMG::Troolean::True);
  assert_sign_sets_to(ArcSine::Builder(Rational::Builder(-1, 3)),
                      OMG::Troolean::True);
  assert_sign_sets_to(Factor::Builder(Rational::Builder(120)),
                      OMG::Troolean::True);
  assert_sign_sets_to(ArcCosine::Builder(Rational::Builder(1, 4)),
                      OMG::Troolean::True);
  assert_sign_sets_to(AbsoluteValue::Builder(Symbol::Builder("p", 1)),
                      OMG::Troolean::True);
  assert_sign_sets_to(SignFunction::Builder(Constant::PiBuilder()),
                      OMG::Troolean::True);
  assert_sign_sets_to(Infinity::Builder(true), OMG::Troolean::True);
  assert_sign_sets_to(Random::Builder(), OMG::Troolean::True);
}

void assert_expression_is_real(const char* expression) {
  Shared::GlobalContext context;
  // isReal can be call only on reduced expressions
  OExpression e = parse_expression(expression, &context)
                      .cloneAndReduce(ReductionContext(&context, Cartesian,
                                                       Radian, MetricUnitFormat,
                                                       SystemForApproximation));
  quiz_assert_print_if_failure(e.isReal(&context), expression);
}

void assert_expression_is_not_real(const char* expression) {
  Shared::GlobalContext context;
  // isReal can be call only on reduced expressions
  OExpression e = parse_expression(expression, &context)
                      .cloneAndReduce(ReductionContext(&context, Cartesian,
                                                       Radian, MetricUnitFormat,
                                                       SystemForApproximation));
  quiz_assert_print_if_failure(!e.isReal(&context), expression);
}

QUIZ_DISABLED_CASE(poincare_properties_is_real) {
  // Numbers
  assert_expression_is_real("2.3");
  assert_expression_is_real("random()");
  assert_expression_is_not_real("nonreal");
  assert_expression_is_not_real(Undefined::Name());

  // Real if does not contain matrix or list
  assert_expression_is_real("abs(2)");
  assert_expression_is_real("abs(2i)");
  assert_expression_is_not_real("abs({2,3})");
  assert_expression_is_not_real("abs([[2,3]])");
  assert_expression_is_not_real("abs(sum({0}×k,k,0,0))");
  assert_expression_is_real("binomial(2,3)");
  assert_expression_is_real("binomial(2i,3)");
  assert_expression_is_real("binomial(2,3i)");
  assert_expression_is_not_real("binomial({2,3},4)");
  assert_expression_is_not_real("binomial(2,{3,4})");
  assert_expression_is_not_real("binomial([[2,3]],4)");
  assert_expression_is_not_real("binomial(2,[[3,4]])");
  assert_expression_is_real("ceil(2)");
  assert_expression_is_real("ceil(2i)");
  assert_expression_is_not_real("ceil({2,3})");
  assert_expression_is_not_real("ceil([[2,3]])");
  assert_expression_is_real("arg(2)");
  assert_expression_is_real("arg(2i)");
  assert_expression_is_not_real("arg({2,3})");
  assert_expression_is_not_real("arg([[2,3]])");
  assert_expression_is_real("diff(2x,x,1)");
  assert_expression_is_not_real("diff(2ix,x,1)");
  assert_expression_is_not_real("diff({2,3}x,x,1)");
  assert_expression_is_not_real("diff([[2,3]]x,x,1)");
  assert_expression_is_real("quo(2,3)");
  assert_expression_is_real("quo(2i,3)");
  assert_expression_is_real("quo(2,3i)");
  assert_expression_is_real("quo(2,3+a)");
  assert_expression_is_not_real("quo({2,3},4)");
  assert_expression_is_not_real("quo(2,{3,4})");
  assert_expression_is_not_real("quo([[2,3]],4)");
  assert_expression_is_not_real("quo(2,[[3,4]])");
  assert_expression_is_real("rem(2,3)");
  assert_expression_is_real("rem(2i,3)");
  assert_expression_is_real("rem(2,3i)");
  assert_expression_is_not_real("rem({2,3},4)");
  assert_expression_is_not_real("rem(2,{3,4})");
  assert_expression_is_not_real("rem([[2,3]],4)");
  assert_expression_is_not_real("rem(2,[[3,4]])");
  assert_expression_is_real("(2)!");
  assert_expression_is_real("(2i)!");
  assert_expression_is_not_real("({2,3})!");
  assert_expression_is_not_real("([[2,3]])!");
  assert_expression_is_real("floor(2)");
  assert_expression_is_real("floor(2i)");
  assert_expression_is_not_real("floor({2,3})");
  assert_expression_is_not_real("floor([[2,3]])");
  assert_expression_is_real("frac(2)");
  assert_expression_is_real("frac(2i)");
  assert_expression_is_not_real("frac({2,3})");
  assert_expression_is_not_real("frac([[2,3]])");
  assert_expression_is_real("gcd(2,3)");
  assert_expression_is_not_real("gcd(2i,3)");
  assert_expression_is_not_real("gcd(2,3i)");
  assert_expression_is_not_real("gcd({2,3},4)");
  assert_expression_is_not_real("gcd(2,{3,4})");
  assert_expression_is_not_real("gcd([[2,3]],4)");
  assert_expression_is_not_real("gcd(2,[[3,4]])");
  assert_expression_is_real("im(2)");
  assert_expression_is_real("im(2i)");
  assert_expression_is_not_real("im({2,3})");
  assert_expression_is_not_real("im([[2,3]])");
  assert_expression_is_real("lcm(2,3)");
  assert_expression_is_not_real("lcm(2i,3)");
  assert_expression_is_not_real("lcm(2,3i)");
  assert_expression_is_not_real("lcm({2,3},4)");
  assert_expression_is_not_real("lcm(2,{3,4})");
  assert_expression_is_not_real("lcm([[2,3]],4)");
  assert_expression_is_not_real("lcm(2,[[3,4]])");
  assert_expression_is_real("permute(2,3)");
  assert_expression_is_real("permute(2i,3)");
  assert_expression_is_real("permute(2,3i)");
  assert_expression_is_not_real("permute({2,3},4)");
  assert_expression_is_not_real("permute(2,{3,4})");
  assert_expression_is_not_real("permute([[2,3]],4)");
  assert_expression_is_not_real("permute(2,[[3,4]])");
  assert_expression_is_real("randint(2,3)");
  assert_expression_is_real("randint(2i,3)");
  assert_expression_is_real("randint(2,3i)");
  assert_expression_is_not_real("randint({2,3},4)");
  assert_expression_is_not_real("randint(2,{3,4})");
  assert_expression_is_not_real("randint([[2,3]],4)");
  assert_expression_is_not_real("randint(2,[[3,4]])");
  assert_expression_is_not_real("randint(randintnorep(0,0,0)×i,0)");
  assert_expression_is_real("re(2)");
  assert_expression_is_real("re(2i)");
  assert_expression_is_not_real("re({2,3})");
  assert_expression_is_not_real("re([[2,3]])");
  assert_expression_is_real("round(2)");
  assert_expression_is_real("round(2i)");
  assert_expression_is_not_real("round({2,3})");
  assert_expression_is_not_real("round([[2,3]])");
  assert_expression_is_real("sign(2)");
  assert_expression_is_real("sign(2i)");
  assert_expression_is_not_real("sign({2,3})");
  assert_expression_is_not_real("sign([[2,3]])");
  assert_expression_is_real("2×_mg");
  assert_expression_is_not_real("2i×_mg");
  assert_expression_is_not_real("{2,3}×_mg");
  assert_expression_is_not_real("[[2,3]]×_mg");

  // Real if children are real
  assert_expression_is_not_real("1+2+3+3×i");
  assert_expression_is_real("1+2+3+root(2,3)");
  assert_expression_is_real("1×23×3×root(2,3)");
  assert_expression_is_not_real("1×23×3×root(2,3)×3×i");
  assert_expression_is_not_real("1×23×3×[[1,2]]");
  assert_expression_is_real("atan(4)");
  assert_expression_is_not_real("atan(i)");
  assert_expression_is_real("conj(4)");
  assert_expression_is_not_real("conj(i)");
  assert_expression_is_real("cos(4)");
  assert_expression_is_not_real("cos(i)");
  assert_expression_is_real("sin(4)");
  assert_expression_is_not_real("sin(i)");
  assert_expression_is_real("tan(4)");
  assert_expression_is_not_real("tan(i)");

  // Constant
  assert_expression_is_real("π");
  assert_expression_is_real("e");
  assert_expression_is_not_real("i");

  // Power
  assert_expression_is_real("2^3.4");
  assert_expression_is_real("(-2)^(-3)");
  assert_expression_is_not_real("i^3.4");
  assert_expression_is_not_real("2^(3.4i)");
  assert_expression_is_not_real("(-2)^0.4");
}

void assert_expression_has_variables(const char* expression,
                                     const char* variables[],
                                     int trueNumberOfVariables) {
  Shared::GlobalContext globalContext;
  OExpression e = parse_expression(expression, &globalContext);
  constexpr static int k_maxVariableSize =
      Poincare::SymbolHelper::k_maxNameSize;
  char variableBuffer[OExpression::k_maxNumberOfVariables][k_maxVariableSize] =
      {{0}};
  int numberOfVariables = e.getVariables(
      &globalContext,
      [](const char* symbol, Poincare::Context* context) { return true; },
      (char*)variableBuffer, k_maxVariableSize);
  quiz_assert_print_if_failure(trueNumberOfVariables == numberOfVariables,
                               expression);
  if (numberOfVariables < 0) {
    // Too many variables
    return;
  }
  int index = 0;
  while (index < OExpression::k_maxNumberOfVariables &&
         (variableBuffer[index][0] != 0 || variables[index][0] != 0)) {
    quiz_assert_print_if_failure(
        strcmp(variableBuffer[index], variables[index]) == 0, expression);
    index++;
  }
}

QUIZ_DISABLED_CASE(poincare_properties_get_variables) {
  /* Warning: Theses tests are weird because you need to avoid a lot of
   * reserved identifiers like:
   * - e and i
   * - m, g, h, A which are units and can be parsed without '_' now. */
  const char* variableBuffer1[] = {"x", "y", ""};
  assert_expression_has_variables("x+y", variableBuffer1, 2);
  const char* variableBuffer2[] = {"x", "y", "z", "w", ""};
  assert_expression_has_variables("x+y+z+2×w", variableBuffer2, 4);
  const char* variableBuffer3[] = {"a", "x", "y", "k", "B", ""};
  assert_expression_has_variables("a+x^2+2×y+k!×B", variableBuffer3, 5);
  assert_reduce_and_store("x→BABA");
  assert_reduce_and_store("y→abab");
  const char* variableBuffer4[] = {"BABA", "abab", ""};
  assert_expression_has_variables("BABA+abab", variableBuffer4, 2);
  assert_reduce_and_store("z→BBBBBB");
  const char* variableBuffer5[] = {"BBBBBB", ""};
  assert_expression_has_variables("BBBBBB", variableBuffer5, 1);
  const char* variableBuffer6[] = {""};
  assert_expression_has_variables(
      "a+b+c+d+f+g+h+j+k+l+m+n+o+p+q+r+s+t+aa+bb+cc+dd+ee+ff+gg+hh+ii+jj+kk+ll+"
      "mm+nn+oo",
      variableBuffer6, -1);
  assert_expression_has_variables("a+b+c+d+f+j+k", variableBuffer6, -1);
  // f: x → 1+πx+x^2+toto
  assert_reduce_and_store("1+π×x+x^2+\"toto\"→f(x)");
  const char* variableBuffer7[] = {"\"tata\"", "\"toto\"", ""};
  assert_expression_has_variables("f(\"tata\")", variableBuffer7, 2);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("BABA.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("abab.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("BBBBBB.exp")
      .destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();

  const char* variableBuffer8[] = {"y", ""};
  assert_expression_has_variables("diff(3x,x,0)y-2", variableBuffer8, 1);
  const char* variableBuffer9[] = {"a", "b", "c", "d", "f", "j"};
  assert_expression_has_variables("a+b+c+d+f+j", variableBuffer9, 6);

  const char* variableBuffer10[] = {"c", "z", "a", "b", ""};
  assert_expression_has_variables("int(c×x×z, x, a, b)", variableBuffer10, 4);
  const char* variableBuffer11[] = {"\"box\"", "y", "z", "a", ""};
  assert_expression_has_variables("\"box\"+y×int(z,x,a,0)", variableBuffer11,
                                  4);

  // f: x → 0
  assert_reduce_and_store("0→f(x)");
  assert_reduce_and_store("x→va");
  const char* variableBuffer12[] = {"va", ""};
  assert_expression_has_variables("f(va)", variableBuffer12, 1);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
  // f: x → a, with a = 12
  assert_reduce_and_store("12→a");
  assert_reduce_and_store("a→f(x)");
  const char* variableBuffer13[] = {"a", "x", ""};
  assert_expression_has_variables("f(x)", variableBuffer13, 2);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
  // f: x → 1, g: x → 2
  assert_reduce_and_store("1→f(x)");
  assert_reduce_and_store("2→g(x)");
  const char* variableBuffer14[] = {"x", "y", ""};
  assert_expression_has_variables("f(g(x)+y)", variableBuffer14, 2);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("g.func").destroy();

  // x = 1
  assert_reduce_and_store("1→x");
  const char* variableBuffer15[] = {"x", "y", ""};
  assert_expression_has_variables("x+y", variableBuffer15, 2);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp").destroy();

  // x = a + b
  assert_reduce_and_store("1→a");
  assert_reduce_and_store("a+b+c→x");
  const char* variableBuffer16[] = {"x", "y", ""};
  assert_expression_has_variables("x+y", variableBuffer16, 2);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();

  // f: x → a+g(y+x), g: x → x+b, a = b + c + x
  assert_reduce_and_store("b+c+x→a");
  assert_reduce_and_store("x+b→g(x)");
  assert_reduce_and_store("a+g(x+y)→f(x)");
  const char* variableBuffer17[] = {"a", "x", "y", "b", ""};
  assert_expression_has_variables("f(x)", variableBuffer17, 4);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("g.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("va.exp").destroy();
}

void assert_reduced_expression_has_polynomial_coefficient(
    const char* expression, const char* symbolName, const char** coefficients,
    Preferences::ComplexFormat complexFormat = Cartesian,
    Preferences::AngleUnit angleUnit = Radian,
    Preferences::UnitFormat unitFormat = MetricUnitFormat,
    SymbolicComputation symbolicComputation =
        ReplaceDefinedSymbols) {
  Shared::GlobalContext globalContext;
  OExpression e = parse_expression(expression, &globalContext);
  e = e.cloneAndReduce(
      ReductionContext(&globalContext, complexFormat, angleUnit, unitFormat,
                       SystemForAnalysis, symbolicComputation));
  OExpression coefficientBuffer
      [Poincare::OExpression::k_maxNumberOfPolynomialCoefficients];
  int d = e.getPolynomialReducedCoefficients(
      symbolName, coefficientBuffer, &globalContext, complexFormat, Radian,
      unitFormat, symbolicComputation);
  for (int i = 0; i <= d; i++) {
    OExpression f = parse_expression(coefficients[i], &globalContext);
    coefficientBuffer[i] = coefficientBuffer[i].cloneAndReduce(
        ReductionContext(&globalContext, complexFormat, angleUnit, unitFormat,
                         SystemForAnalysis, symbolicComputation));
    f = f.cloneAndReduce(
        ReductionContext(&globalContext, complexFormat, angleUnit, unitFormat,
                         SystemForAnalysis, symbolicComputation));
    quiz_assert_print_if_failure(coefficientBuffer[i].isIdenticalTo(f),
                                 expression);
  }
  quiz_assert_print_if_failure(coefficients[d + 1] == 0, expression);
}

QUIZ_DISABLED_CASE(poincare_properties_get_polynomial_coefficients) {
  const char* coefficient0[] = {"2", "1", "1", 0};
  assert_reduced_expression_has_polynomial_coefficient("x^2+x+2", "x",
                                                       coefficient0);
  const char* coefficient1[] = {"12+(-6)×π", "12", "3", 0};
  assert_reduced_expression_has_polynomial_coefficient("3×(x+2)^2-6×π", "x",
                                                       coefficient1);
  const char* coefficient2[] = {"2+32×x", "2", "6", "2", 0};
  assert_reduced_expression_has_polynomial_coefficient("2×(n+1)^3-4n+32×x", "n",
                                                       coefficient2);
  const char* coefficient3[] = {"1", "-π", "1", 0};
  assert_reduced_expression_has_polynomial_coefficient("x^2-π×x+1", "x",
                                                       coefficient3);

  // f: x→x^2+Px+1
  assert_reduce_and_store("1+π×x+x^2→f(x)");
  const char* coefficient4[] = {"1", "π", "1", 0};
  assert_reduced_expression_has_polynomial_coefficient("f(x)", "x",
                                                       coefficient4);
  const char* coefficient5[] = {"0", "i", 0};
  assert_reduced_expression_has_polynomial_coefficient("√(-1)x", "x",
                                                       coefficient5);
  const char* coefficient6[] = {NonReal::Name(), 0};
  assert_reduced_expression_has_polynomial_coefficient("√(-1)x", "x",
                                                       coefficient6, Real);

  // 3 -> x
  assert_reduce_and_store("3→x");
  const char* coefficient7[] = {"4", 0};
  assert_reduced_expression_has_polynomial_coefficient("x+1", "x",
                                                       coefficient7);
  const char* coefficient8[] = {"2", "1", 0};
  assert_reduced_expression_has_polynomial_coefficient(
      "x+2", "x", coefficient8, Real, Radian, MetricUnitFormat,
      KeepAllSymbols);
  assert_reduced_expression_has_polynomial_coefficient(
      "x+2", "x", coefficient8, Real, Radian, MetricUnitFormat,
      ReplaceDefinedFunctions);
  assert_reduced_expression_has_polynomial_coefficient(
      "f(x)", "x", coefficient4, Cartesian, Radian, MetricUnitFormat,
      ReplaceDefinedFunctions);

  // Clear the storage
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp").destroy();
}

void assert_reduced_expression_unit_is(const char* expression,
                                       const char* unit) {
  Shared::GlobalContext globalContext;
  ReductionContext redContext(&globalContext, Real, Degree, MetricUnitFormat,
                              SystemForApproximation);
  OExpression e = parse_expression(expression, &globalContext);
  OExpression u1;
  e = e.cloneAndReduceAndRemoveUnit(redContext, &u1);
  OExpression e2 = parse_expression(unit, &globalContext);
  OExpression u2;
  e2.cloneAndReduceAndRemoveUnit(redContext, &u2);
  quiz_assert_print_if_failure(
      u1.isUninitialized() == u2.isUninitialized() &&
          (u1.isUninitialized() || u1.isIdenticalTo(u2)),
      expression);
}

QUIZ_DISABLED_CASE(poincare_properties_remove_unit) {
  assert_reduced_expression_unit_is("_km", "_m");
  assert_reduced_expression_unit_is("_min/_km", "_m^(-1)×_s");
  assert_reduced_expression_unit_is("_km^3", "_m^3");
  assert_reduced_expression_unit_is("1_m+_km", "_m");
  assert_reduced_expression_unit_is("_L^2×3×_s", "_m^6×_s");
}

void assert_additional_results_compute_to(
    const char* expression, const char** results, int length,
    Preferences::UnitFormat unitFormat = MetricUnitFormat) {
  Shared::GlobalContext globalContext;
  constexpr int maxNumberOfResults = 5;
  assert(length <= maxNumberOfResults);
  OExpression additional[maxNumberOfResults];
  ReductionContext reductionContext(
      &globalContext, Cartesian, Degree, unitFormat, User,
      ReplaceAllSymbolsWithUndefined, DefaultUnitConversion);
  ApproximationContext approximationContext(reductionContext);
  OExpression units;
  OExpression e = parse_expression(expression, &globalContext)
                      .cloneAndReduceAndRemoveUnit(reductionContext, &units);
  double value = e.approximateToRealScalar<double>(approximationContext);

  if (!OUnit::ShouldDisplayAdditionalOutputs(value, units, unitFormat)) {
    quiz_assert(length == 0);
    return;
  }
  const int numberOfResults = OUnit::SetAdditionalExpressions(
      units, value, additional, maxNumberOfResults, reductionContext,
      OExpression());

  quiz_assert(numberOfResults == length);
  for (int i = 0; i < length; i++) {
    assert_expression_serializes_to(additional[i], results[i], DecimalMode);
  }
}

QUIZ_DISABLED_CASE(poincare_expression_additional_results) {
  // Time
  assert_additional_results_compute_to("3×_s", nullptr, 0);
  {
    const char* array[1] = {"1×_min+1×_s"};
    assert_additional_results_compute_to("61×_s", array, 1);
  }
  {
    const char* array[1] = {"1×_day+10×_h+17×_min+36×_s"};
    assert_additional_results_compute_to("123456×_s", array, 1);
  }
  {
    const char* array[1] = {"7×_day"};
    assert_additional_results_compute_to("1×_week", array, 1);
  }

  // Distance
  {
    const char* array[1] = {"19×_mi+853×_yd+1×_ft+7×_in"};
    assert_additional_results_compute_to("1234567×_in", array, 1, Imperial);
  }
  {
    const char* array[1] = {"1×_yd+7.700787×_in"};
    assert_additional_results_compute_to("1.11×_m", array, 1, Imperial);
  }
  assert_additional_results_compute_to("1.11×_m", nullptr, 0, MetricUnitFormat);

  // Masses
  {
    const char* array[1] = {"1×_shtn+240×_lb"};
    assert_additional_results_compute_to("1×_lgtn", array, 1, Imperial);
  }
  {
    const char* array[1] = {"2×_lb+3.273962×_oz"};
    assert_additional_results_compute_to("1×_kg", array, 1, Imperial);
  }
  assert_additional_results_compute_to("1×_kg", nullptr, 0, MetricUnitFormat);

  // Temperatures
  {
    const char* array[2] = {"-273.15×_°C", "-459.67×_°F"};
    assert_additional_results_compute_to("0×_K", array, 2, MetricUnitFormat);
  }
  {
    const char* array[2] = {"-279.67×_°F", "-173.15×_°C"};
    assert_additional_results_compute_to("100×_K", array, 2, Imperial);
  }
  {
    const char* array[2] = {"12.02×_°F", "262.05×_K"};
    assert_additional_results_compute_to("-11.1×_°C", array, 2);
  }
  {
    const char* array[2] = {"-20×_°C", "253.15×_K"};
    assert_additional_results_compute_to("-4×_°F", array, 2);
  }

  // Energy
  {
    const char* array[3] = {"3.6×_MJ", "1×_kW×_h", "2.246943ᴇ13×_TeV"};
    assert_additional_results_compute_to("3.6×_MN_m", array, 3);
  }

  // Volume
  {
    const char* array[2] = {"264×_gal+1×_pt+0.7528377×_cup", "1000×_L"};
    assert_additional_results_compute_to("1×_m^3", array, 2, Imperial);
  }
  {
    const char* array[2] = {"48×_gal+1×_pt+1.5625×_cup", "182.5426×_L"};
    assert_additional_results_compute_to("12345×_tbsp", array, 2, Imperial);
  }
  {
    const char* array[2] = {"182.5426×_L"};
    assert_additional_results_compute_to("12345×_tbsp", array, 1,
                                         MetricUnitFormat);
  }

  // Speed
  {
    const char* array[1] = {"3.6×_km×_h^\x12-1\x13"};
    assert_additional_results_compute_to("1×_m/_s", array, 1, MetricUnitFormat);
  }
  {
    const char* array[2] = {"2.236936×_mi×_h^\x12-1\x13",
                            "3.6×_km×_h^\x12-1\x13"};
    assert_additional_results_compute_to("1×_m/_s", array, 2, Imperial);
  }

  // No additional result
  assert_additional_results_compute_to("rad×s^(1/2)", nullptr, 0);
}

void assert_list_length_in_children_is(const char* definition,
                                       int targetLength) {
  Shared::GlobalContext globalContext;
  OExpression e = parse_expression(definition, &globalContext);
  quiz_assert_print_if_failure(e.lengthOfListChildren() == targetLength,
                               definition);
}

QUIZ_DISABLED_CASE(poincare_expression_children_list_length) {
  assert_list_length_in_children_is("1+1", OExpression::k_noList);
  assert_list_length_in_children_is("1+{}", 0);
  assert_list_length_in_children_is("1*{2,3,4}*5*{6,7,8}", 3);
  assert_list_length_in_children_is("{1,-2,3,-4}^2", 4);
  assert_list_length_in_children_is("{1,2}+{3,4,5}",
                                    OExpression::k_mismatchedLists);
}

#endif

#include <apps/shared/global_context.h>
#include <ion/storage/file_system.h>
#include <poincare/expression.h>
#include <poincare/src/expression/continuity.h>

#include "../helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

void assert_is_number(const char* input, Context* context,
                      bool isNumber = true) {
  UserExpression e = UserExpression::Builder(parse(input, context));
  quiz_assert_print_if_failure(e.isConstantNumber() == isNumber, input);
}

void assert_reduced_is_number(const char* input, Context* context,
                              bool isNumber = true) {
  UserExpression e1 = UserExpression::Builder(parse(input, context));
  ReductionContext reductionContext(context);
  bool reductionFailure = false;
  SystemExpression e2 = e1.cloneAndReduce(reductionContext, &reductionFailure);
  quiz_assert(!reductionFailure);
  quiz_assert_print_if_failure(e2.isConstantNumber() == isNumber, input);
}

QUIZ_CASE(poincare_properties_is_number) {
  Shared::GlobalContext globalContext;

  // Always a number from parsing
  assert_is_number("2", &globalContext);
  assert_is_number("0b10", &globalContext);
  assert_is_number("0x2", &globalContext);
  assert_is_number("2.3", &globalContext);
  assert_is_number("π", &globalContext);
  assert_is_number("inf", &globalContext);
  assert_is_number("undef", &globalContext);
  assert_reduced_is_number("2", &globalContext);
  assert_reduced_is_number("0b10", &globalContext);
  assert_reduced_is_number("0x2", &globalContext);
  assert_reduced_is_number("2.3", &globalContext);
  assert_reduced_is_number("π", &globalContext);
  assert_reduced_is_number("inf", &globalContext);
  assert_reduced_is_number("undef", &globalContext);

  // Number after reduction
  assert_is_number("2/3", &globalContext, false);
  assert_is_number("1*2", &globalContext, false);
  assert_is_number("1+2", &globalContext, false);
  assert_reduced_is_number("2/3", &globalContext);
  assert_reduced_is_number("1*2", &globalContext);
  assert_reduced_is_number("1+2", &globalContext);

  // Not a number
  assert_is_number("a", &globalContext, false);
  assert_reduced_is_number("a", &globalContext, false);
  assert_is_number("[[0]]", &globalContext, false);
  assert_reduced_is_number("[[0]]", &globalContext, false);
  assert_is_number("(1,2)", &globalContext, false);
  assert_reduced_is_number("(1,2)", &globalContext, false);
}

void assert_has_random(const char* input, bool hasRandom = true) {
  Shared::GlobalContext context;
  UserExpression e = UserExpression::Builder(parse(input, &context));
  quiz_assert_print_if_failure(e.hasRandomNumber() == hasRandom, input);
}

QUIZ_CASE(poincare_properties_has_random) {
  assert_has_random("random()");
  assert_has_random("randint(1,2)");
  assert_has_random("cos(random())");
  assert_has_random("random()-random()");
  assert_has_random("a", false);
  assert_has_random("2/3", false);
}

void assert_is_parametered_expression(const char* input,
                                      bool isParametered = true) {
  Shared::GlobalContext context;
  UserExpression e = UserExpression::Builder(parse(input, &context));
  quiz_assert_print_if_failure(e.isParametric() == isParametered, input);
}

QUIZ_CASE(poincare_properties_is_parametered_expression) {
  assert_is_parametered_expression("diff(2x,x,2)");
  assert_is_parametered_expression("diff(1,x,2,3)");
  assert_is_parametered_expression("int(x,x,2,4)");
  assert_is_parametered_expression("sum(n+1,n,0,10)");
  assert_is_parametered_expression("product(2,n,2,2)");
  assert_is_parametered_expression("sequence(x,x,10)");
  assert_is_parametered_expression("a", false);
  assert_is_parametered_expression("2/3", false);
}

template <typename T>
void assert_expression_has_property_or_not(const char* expression, T test,
                                           bool hasProperty) {
  Shared::GlobalContext context;
  UserExpression e = UserExpression::Builder(parse(expression, &context));
  quiz_assert_print_if_failure(
      e.recursivelyMatches(test, &context) == hasProperty, expression);
}

template <typename T>
void assert_expression_has_property(const char* input, T test) {
  assert_expression_has_property_or_not<T>(input, test, true);
}

template <typename T>
void assert_expression_has_not_property(const char* input, T test) {
  assert_expression_has_property_or_not<T>(input, test, false);
}

QUIZ_CASE(poincare_properties_has_approximate) {
  assert_expression_has_property("3.4", &NewExpression::isApproximate);
  assert_expression_has_property("2.3+1", &NewExpression::isApproximate);
  assert_expression_has_property("0.1f", &NewExpression::isApproximate);
  assert_expression_has_not_property("a", &NewExpression::isApproximate);
}

QUIZ_CASE(poincare_properties_has_matrix) {
  assert_expression_has_property("[[1,2][3,4]]",
                                 &NewExpression::isOfMatrixDimension);
  assert_expression_has_property("dim([[1,2][3,4]])/3",
                                 &NewExpression::isOfMatrixDimension);
  assert_expression_has_property("[[1,2][3,4]]^(-1)",
                                 &NewExpression::isOfMatrixDimension);
  assert_expression_has_property("inverse([[1,2][3,4]])",
                                 &NewExpression::isOfMatrixDimension);
  assert_expression_has_property("3*identity(4)",
                                 &NewExpression::isOfMatrixDimension);
  assert_expression_has_property("transpose([[1,2][3,4]])",
                                 &NewExpression::isOfMatrixDimension);
  assert_expression_has_property("ref([[1,2][3,4]])",
                                 &NewExpression::isOfMatrixDimension);
  assert_expression_has_property("rref([[1,2][3,4]])",
                                 &NewExpression::isOfMatrixDimension);
  assert_expression_has_property("cross([[1][2][3]],[[3][4][5]])",
                                 &NewExpression::isOfMatrixDimension);
  assert_expression_has_property("diff([[1,2][3,4]],x,2)",
                                 &NewExpression::isOfMatrixDimension);
  assert_expression_has_property("sign([[1,2][3,4]])",
                                 &NewExpression::isOfMatrixDimension);
  assert_expression_has_property("trace([[1,2][3,4]])",
                                 &NewExpression::isOfMatrixDimension);
  assert_expression_has_property("det([[1,2][3,4]])",
                                 &NewExpression::isOfMatrixDimension);
  assert_expression_has_not_property("2*3+1",
                                     &NewExpression::isOfMatrixDimension);
}

void assert_is_list_of_points(const char* definition, Context* context,
                              bool truth = true) {
  UserExpression e = UserExpression::Builder(parse(definition, context));
  bool isListOfPoints = e.dimension(context).isListOfPoints();
  quiz_assert_print_if_failure(isListOfPoints == truth, definition);
}

QUIZ_CASE(poincare_expression_list_of_points) {
  Shared::GlobalContext globalContext;
  assert(
      Ion::Storage::FileSystem::sharedFileSystem->numberOfRecords() ==
      Ion::Storage::FileSystem::sharedFileSystem->numberOfRecordsWithExtension(
          "sys"));

  store("(1,2)→a", &globalContext);
  store("3→b", &globalContext);

  assert_is_list_of_points("{(1,2)}", &globalContext);
  assert_is_list_of_points("{(1,-2),(-3.4,5.6)}", &globalContext);
  assert_is_list_of_points("{a,(3,4)}", &globalContext);
  assert_is_list_of_points("{(undef,1),(3,undef),(undef, undef)}",
                           &globalContext);

  assert_is_list_of_points("{1,2,3}", &globalContext, false);
  assert_is_list_of_points("{(1,2),3}", &globalContext, false);
  assert_is_list_of_points("{(1,2),3,(4,5)}", &globalContext, false);
  assert_is_list_of_points("{undef,1}", &globalContext, false);
  assert_is_list_of_points("{b}", &globalContext, false);

  // TODO_PCJ: These used to be lists of points but are not anymore.
  assert_is_list_of_points("{}", &globalContext, false);
  assert_is_list_of_points("{undef}", &globalContext, false);
  assert_is_list_of_points("{x}", &globalContext, false);
  assert_is_list_of_points("{a,undef,(3,4)}", &globalContext, false);
  assert_is_list_of_points("{a,x,(3,4)}", &globalContext, false);
}

void assert_is_continuous_on_interval(const char* expression, float x1,
                                      float x2, bool isContinuous) {
  Shared::GlobalContext context;
  UserExpression e1 = UserExpression::Builder(parse(expression, &context));
  ReductionContext reductionContext(&context);
  bool reductionFailure = false;
  SystemExpression e2 = e1.cloneAndReduce(reductionContext, &reductionFailure);
  quiz_assert(!reductionFailure);
  SystemFunction e3 = e2.getSystemFunction("x", true);
  quiz_assert_print_if_failure(
      !isContinuous ==
          Continuity::IsDiscontinuousOnInterval<float>(e3.tree(), x1, x2),
      expression);
}

QUIZ_CASE(poincare_expression_continuous) {
  assert_is_continuous_on_interval("x+x^2", 2.43f, 2.45f, true);
  assert_is_continuous_on_interval("x+x^2", 2.45f, 2.47f, true);
  assert_is_continuous_on_interval("x+floor(x^2)", 2.43f, 2.45f, false);
  assert_is_continuous_on_interval("x+floor(x^2)", 2.45f, 2.47f, true);
  assert_is_continuous_on_interval("x+floor(x^2)", 2.43f, 2.45f, false);
  assert_is_continuous_on_interval("x+floor(x^2)", 2.45f, 2.47f, true);
  assert_is_continuous_on_interval("x+ceil(x^2)", 2.43f, 2.45f, false);
  assert_is_continuous_on_interval("x+ceil(x^2)", 2.45f, 2.47f, true);
  assert_is_continuous_on_interval("x+round(x^2, 0)", 2.34f, 2.36f, false);
  assert_is_continuous_on_interval("x+round(x^2, 0)", 2.36f, 2.38f, true);
  assert_is_continuous_on_interval("x+random()", 2.43f, 2.45f, false);
  assert_is_continuous_on_interval("x+randint(1,10)", 2.43f, 2.45f, false);
  assert_is_continuous_on_interval("piecewise(-1,x<0,1)", -1.0f, 1.0f, false);
  assert_is_continuous_on_interval("piecewise(-1,random()-0.5<0,1)", -1.0f,
                                   1.0f, false);
}

#if 0

void assert_deep_is_symbolic(const char* expression, bool isSymbolic) {
  Shared::GlobalContext context;
  OExpression e = parse_expression(expression, &context);
  e = e.cloneAndReduce(
      ReductionContext::DefaultReductionContextForAnalysis(&context));
  quiz_assert_print_if_failure(
      e.deepIsSymbolic(&context, KeepAllSymbols) == isSymbolic,
      expression);
}

QUIZ_DISABLED_CASE(poincare_expression_deep_is_symbolic) {
  assert_deep_is_symbolic("2/cos(3x+2)", true);
  assert_deep_is_symbolic("2/int(5x, x, 3, 4)", false);
  assert_deep_is_symbolic("2/int(5xy, x, 3, 4)", true);
  assert_deep_is_symbolic("2/int(diff(xy, y, 2), x, 3, 4)", false);
  assert_deep_is_symbolic("2/int(diff(xy^n, y, 2), x, 3, 4)", true);
}

#endif
