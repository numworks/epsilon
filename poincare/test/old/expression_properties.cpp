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

#endif

#include <apps/shared/global_context.h>
#include <ion/storage/file_system.h>
#include <poincare/expression.h>
#include <poincare/src/expression/continuity.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/src/expression/polynomial.h>
#include <poincare/src/expression/variables.h>

#include "../helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

void assert_is_number(const char* input, bool isNumber = true) {
  Shared::GlobalContext context;
  UserExpression e = UserExpression::Builder(parse(input, &context));
  quiz_assert_print_if_failure(e.isConstantNumber() == isNumber, input);
}

void assert_reduced_is_number(const char* input, bool isNumber = true) {
  Shared::GlobalContext context;
  UserExpression e1 = UserExpression::Builder(parse(input, &context));
  ReductionContext reductionContext(&context);
  bool reductionFailure = false;
  SystemExpression e2 = e1.cloneAndReduce(reductionContext, &reductionFailure);
  quiz_assert(!reductionFailure);
  quiz_assert_print_if_failure(e2.isConstantNumber() == isNumber, input);
}

QUIZ_CASE(poincare_properties_is_number) {
  // Always a number from parsing
  assert_is_number("2");
  assert_is_number("0b10");
  assert_is_number("0x2");
  assert_is_number("2.3");
  assert_is_number("π");
  assert_is_number("inf");
  assert_is_number("undef");
  assert_reduced_is_number("2");
  assert_reduced_is_number("0b10");
  assert_reduced_is_number("0x2");
  assert_reduced_is_number("2.3");
  assert_reduced_is_number("π");
  assert_reduced_is_number("inf");
  assert_reduced_is_number("undef");

  // Number after reduction
  assert_is_number("2/3", false);
  assert_is_number("1*2", false);
  assert_is_number("1+2", false);
  assert_reduced_is_number("2/3");
  assert_reduced_is_number("1*2");
  assert_reduced_is_number("1+2");

  // Not a number
  assert_is_number("a", false);
  assert_reduced_is_number("a", false);
  assert_is_number("[[0]]", false);
  assert_reduced_is_number("[[0]]", false);
  assert_is_number("(1,2)", false);
  assert_reduced_is_number("(1,2)", false);
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
void assert_expression_has_property_or_not(const char* input, T test,
                                           bool hasProperty) {
  Shared::GlobalContext context;
  UserExpression e = UserExpression::Builder(parse(input, &context));
  quiz_assert_print_if_failure(
      e.recursivelyMatches(test, &context) == hasProperty, input);
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

void assert_projected_is_infinity(const char* input, ProjectionContext* projCtx,
                                  bool isInfinity = true) {
  Tree* e = parse(input, projCtx->m_context);
  Simplification::ToSystem(e, projCtx);
  SystemExpression expr = SystemExpression::Builder(e);
  quiz_assert_print_if_failure(
      expr.recursivelyMatches(&SystemExpression::isPlusOrMinusInfinity,
                              projCtx->m_context) == isInfinity,
      input);
}

QUIZ_CASE(poincare_properties_is_infinity) {
  Shared::GlobalContext globalContext;
  ProjectionContext projCtx = {
      .m_symbolic = SymbolicComputation::ReplaceDefinedFunctions,
      .m_context = &globalContext,
  };
  assert_projected_is_infinity("3.4+inf", &projCtx);
  assert_projected_is_infinity("2.3+1", &projCtx, false);
  assert_projected_is_infinity("a", &projCtx, false);

  store("42.3+inf→a", &globalContext);
  assert_projected_is_infinity("a", &projCtx);

  store("1+inf→x", &globalContext);
  assert_projected_is_infinity("x", &projCtx);
  assert_projected_is_infinity("diff(x^2,x,3)", &projCtx, false);

  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp").destroy();
}

void assert_expression_has_variables(const char* input,
                                     const Tree* expectedVariables,
                                     ProjectionContext* projCtx) {
  Tree* e = parse(input, projCtx->m_context);
  Simplification::ToSystem(e, projCtx);
  const Tree* variables = Variables::GetUserSymbols(e);
  assert_trees_are_equal(variables, expectedVariables);
}

QUIZ_CASE(poincare_properties_get_variables) {
  /* Warning: Theses tests are weird because you need to avoid a lot of
   * reserved identifiers like:
   * - e and i
   * - m, g, h, l, s, t, A which are units and can be parsed without '_' now. */
  Shared::GlobalContext globalContext;
  ProjectionContext projCtx = {
      .m_symbolic = SymbolicComputation::ReplaceDefinedFunctions,
      .m_context = &globalContext,
  };
  assert(
      Ion::Storage::FileSystem::sharedFileSystem->numberOfRecords() ==
      Ion::Storage::FileSystem::sharedFileSystem->numberOfRecordsWithExtension(
          "sys"));

  assert_expression_has_variables("x+y", KSet("x"_e, "y"_e), &projCtx);
  assert_expression_has_variables("x+y+z+2w", KSet("w"_e, "x"_e, "y"_e, "z"_e),
                                  &projCtx);
  assert_expression_has_variables(
      "a+x^2+2y+k!B", KSet("B"_e, "a"_e, "k"_e, "x"_e, "y"_e), &projCtx);

  store("x→BABA", &globalContext);
  store("y→abab", &globalContext);
  assert_expression_has_variables("BABA+abab", KSet("BABA"_e, "abab"_e),
                                  &projCtx);
  store("z→BBBBBB", &globalContext);
  assert_expression_has_variables("BBBBBB", KSet("BBBBBB"_e), &projCtx);
  assert_expression_has_variables(
      "a+b+c+d+f+j+k+n+o+p+q+r",
      KSet("a"_e, "b"_e, "c"_e, "d"_e, "f"_e, "j"_e, "k"_e, "n"_e, "o"_e, "p"_e,
           "q"_e, "r"_e),
      &projCtx);
  store("1+π×x+x^2+\"toto\"→f(x)", &globalContext);
  assert_expression_has_variables("f(\"tata\")",
                                  KSet("\"tata\""_e, "\"toto\""_e), &projCtx);

  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("BABA.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("abab.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("BBBBBB.exp")
      .destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();

  assert_expression_has_variables("diff(3x,x,0)y-2", KSet("y"_e), &projCtx);
  assert_expression_has_variables(
      "a+b+c+d+f+j", KSet("a"_e, "b"_e, "c"_e, "d"_e, "f"_e, "j"_e), &projCtx);
  assert_expression_has_variables("\"box\"+y×int(z,x,a,0)",
                                  KSet("\"box\""_e, "a"_e, "y"_e, "z"_e),
                                  &projCtx);

  store("0→f(x)", &globalContext);
  store("x→va", &globalContext);
  assert_expression_has_variables("f(va)", KSet("va"_e), &projCtx);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();

  store("12→a", &globalContext);
  store("a→f(x)", &globalContext);
  assert_expression_has_variables("f(x)", KSet("a"_e, "x"_e), &projCtx);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();

  store("1→f(x)", &globalContext);
  store("2→g(x)", &globalContext);
  assert_expression_has_variables("f(g(x)+y)", KSet("x"_e, "y"_e), &projCtx);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("g.func").destroy();

  store("1→x", &globalContext);
  assert_expression_has_variables("x+y", KSet("x"_e, "y"_e), &projCtx);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp").destroy();

  store("1→a", &globalContext);
  store("a+b+c→x", &globalContext);
  assert_expression_has_variables("x+y", KSet("x"_e, "y"_e), &projCtx);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();

  store("b+c+x→a", &globalContext);
  store("x+b→g(x)", &globalContext);
  store("a+g(x+y)→f(x)", &globalContext);
  assert_expression_has_variables("f(x)", KSet("a"_e, "b"_e, "x"_e, "y"_e),
                                  &projCtx);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("g.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("va.exp").destroy();
}

void assert_reduced_expression_has_polynomial_coefficient(
    const char* input, const char* symbolName, const Tree* expectedCoefficients,
    Shared::GlobalContext* globalContext,
    Preferences::ComplexFormat complexFormat =
        Preferences::ComplexFormat::Cartesian,
    Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Radian,
    Preferences::UnitFormat unitFormat = Preferences::UnitFormat::Metric,
    SymbolicComputation symbolicComputation =
        SymbolicComputation::ReplaceDefinedSymbols) {
  ReductionContext redContext =
      ReductionContext(globalContext, complexFormat, angleUnit, unitFormat,
                       ReductionTarget::SystemForAnalysis, symbolicComputation);
  UserExpression e = UserExpression::Builder(parse(input, globalContext));
  bool reductionFailure = false;
  e = e.cloneAndReduce(redContext, &reductionFailure);
  quiz_assert(!reductionFailure);
  Tree* coefficients = PolynomialParser::GetReducedCoefficients(e, symbolName);
  assert_trees_are_equal(coefficients, expectedCoefficients);
}

QUIZ_CASE(poincare_properties_get_polynomial_coefficients) {
  Shared::GlobalContext globalContext;
  assert_reduced_expression_has_polynomial_coefficient(
      "x^2+x+2", "x", KList(2_e, 1_e, 1_e), &globalContext);
  assert_reduced_expression_has_polynomial_coefficient(
      "3×(x+2)^2-6×π", "x", KList(KAdd(12_e, KMult(-6_e, π_e)), 12_e, 3_e),
      &globalContext);
  assert_reduced_expression_has_polynomial_coefficient(
      "2×(n+1)^3-4n+32×x", "n",
      KList(KAdd(2_e, KMult(32_e, "x"_e)), 2_e, 6_e, 2_e), &globalContext);
  assert_reduced_expression_has_polynomial_coefficient(
      "x^2-π×x+1", "x", KList(1_e, KMult(-1_e, π_e), 1_e), &globalContext);

  // f: x→x^2+Px+1
  store("1+π×x+x^2→f(x)", &globalContext);
  assert_reduced_expression_has_polynomial_coefficient(
      "f(x)", "x", KList(1_e, π_e, 1_e), &globalContext);
  assert_reduced_expression_has_polynomial_coefficient(
      "√(-1)x", "x", KList(0_e, i_e), &globalContext);
  assert_reduced_expression_has_polynomial_coefficient(
      "√(-1)x", "x", KList(KNonReal), &globalContext,
      Preferences::ComplexFormat::Real);

  // 3 -> x
  store("3→x", &globalContext);
  assert_reduced_expression_has_polynomial_coefficient("x+1", "x", KList(4_e),
                                                       &globalContext);
  assert_reduced_expression_has_polynomial_coefficient(
      "x+2", "x", KList(2_e, 1_e), &globalContext,
      Preferences::ComplexFormat::Real, Preferences::AngleUnit::Radian,
      Preferences::UnitFormat::Metric, SymbolicComputation::KeepAllSymbols);
  assert_reduced_expression_has_polynomial_coefficient(
      "x+2", "x", KList(2_e, 1_e), &globalContext,
      Preferences::ComplexFormat::Real, Preferences::AngleUnit::Radian,
      Preferences::UnitFormat::Metric,
      SymbolicComputation::ReplaceDefinedFunctions);
  assert_reduced_expression_has_polynomial_coefficient(
      "f(x)", "x", KList(1_e, π_e, 1_e), &globalContext,
      Preferences::ComplexFormat::Cartesian, Preferences::AngleUnit::Radian,
      Preferences::UnitFormat::Metric,
      SymbolicComputation::ReplaceDefinedFunctions);

  // Clear the storage
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp").destroy();
}

void assert_list_length_in_children_is(const char* definition,
                                       int targetLength) {
  Shared::GlobalContext context;
  UserExpression e = UserExpression::Builder(parse(definition, &context));
  bool isValid = targetLength != -2;
  quiz_assert_print_if_failure(
      Poincare::Internal::Dimension::DeepCheck(e.tree(), &context) == isValid,
      definition);
  if (isValid) {
    quiz_assert_print_if_failure(Poincare::Internal::Dimension::ListLength(
                                     e.tree(), &context) == targetLength,
                                 definition);
  }
}

constexpr int k_mismatchedLists = -2;

QUIZ_CASE(poincare_properties_children_list_length) {
  assert_list_length_in_children_is(
      "1+1", Poincare::Internal::Dimension::k_nonListListLength);
  assert_list_length_in_children_is("1+{}", 0);
  assert_list_length_in_children_is("1*{2,3,4}*5*{6,7,8}", 3);
  assert_list_length_in_children_is("{1,-2,3,-4}^2", 4);
  assert_list_length_in_children_is("{1,2}+{3,4,5}", k_mismatchedLists);
}

void assert_is_list_of_points(const char* input, Context* context,
                              bool truth = true) {
  UserExpression e = UserExpression::Builder(parse(input, context));
  bool isListOfPoints = e.dimension(context).isListOfPoints();
  quiz_assert_print_if_failure(isListOfPoints == truth, input);
}

QUIZ_CASE(poincare_properties_list_of_points) {
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

void assert_is_continuous_on_interval(const char* input, float x1, float x2,
                                      bool isContinuous) {
  Shared::GlobalContext context;
  UserExpression e1 = UserExpression::Builder(parse(input, &context));
  ReductionContext reductionContext(&context);
  bool reductionFailure = false;
  SystemExpression e2 = e1.cloneAndReduce(reductionContext, &reductionFailure);
  quiz_assert(!reductionFailure);
  SystemFunction e3 = e2.getSystemFunction("x", true);
  quiz_assert_print_if_failure(
      !isContinuous ==
          Continuity::IsDiscontinuousOnInterval<float>(e3.tree(), x1, x2),
      input);
}

QUIZ_CASE(poincare_properties_is_continuous) {
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

void assert_reduced_deep_is_symbolic(const char* input,
                                     bool isSymbolic = true) {
  Shared::GlobalContext context;
  UserExpression e1 = UserExpression::Builder(parse(input, &context));
  ReductionContext reductionContext(&context);
  bool reductionFailure = false;
  SystemExpression e2 = e1.cloneAndReduce(reductionContext, &reductionFailure);
  quiz_assert(!reductionFailure);
  quiz_assert_print_if_failure(
      e2.deepIsOfType(
          {Type::UserSymbol, Type::UserFunction, Type::UserSequence},
          &context) == isSymbolic,
      input);
}

QUIZ_CASE(poincare_properties_deep_is_symbolic) {
  assert_reduced_deep_is_symbolic("2/cos(3x+2)");
  assert_reduced_deep_is_symbolic("2/int(5x, x, 3, 4)", false);
  assert_reduced_deep_is_symbolic("2/int(5xy, x, 3, 4)");
  assert_reduced_deep_is_symbolic("2/int(diff(xy, y, 2), x, 3, 4)", false);
  assert_reduced_deep_is_symbolic("2/int(diff(xy^n, y, 2), x, 3, 4)");
  assert_reduced_deep_is_symbolic("2x+1");
  assert_reduced_deep_is_symbolic("diff(x^2,x,3)", false);
  assert_reduced_deep_is_symbolic("diff(y^2+x^2,x,3)", false);
}
