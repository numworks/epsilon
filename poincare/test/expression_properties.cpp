#include <poincare/context.h>
#include <poincare/prepared_function.h>
#include <poincare/src/expression/continuity.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/src/expression/infinity.h>
#include <poincare/src/expression/polynomial.h>
#include <poincare/src/expression/variables.h>
#include <poincare/system_expression.h>
#include <poincare/test/helpers/symbol_store.h>
#include <poincare/user_expression.h>

#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

void assert_is_number(const char* input, bool isNumber = true) {
  UserExpression e = UserExpression::Builder(parse(input));
  quiz_assert_print_if_failure(e.isConstantNumber() == isNumber, input);
}

void assert_reduced_is_number(const char* input, bool isNumber = true) {
  UserExpression e1 = UserExpression::Builder(parse(input));
  bool reductionFailure = false;
  SystemExpression e2 =
      e1.cloneAndReduce(ProjectionContext{}, &reductionFailure);
  quiz_assert(!reductionFailure);
  quiz_assert_print_if_failure(e2.isConstantNumber() == isNumber, input);
}

QUIZ_CASE(pcj_properties_is_number) {
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

void assert_reduced_is_rational(const char* input, bool isRational = true) {
  UserExpression e1 = UserExpression::Builder(parse(input));
  bool reductionFailure = false;
  SystemExpression e2 =
      e1.cloneAndReduce(ProjectionContext{}, &reductionFailure);
  quiz_assert(!reductionFailure);
  quiz_assert_print_if_failure(e2.isRational() == isRational, input);
}

QUIZ_CASE(pcj_properties_is_rational_number) {
  assert_reduced_is_rational("0b20");
  assert_reduced_is_rational("2");
  assert_reduced_is_rational("0x2");
  assert_reduced_is_rational("2.3");
  assert_reduced_is_rational("2/3");
  assert_reduced_is_rational("-2/3");
  assert_reduced_is_rational("(-1)/2");
  assert_reduced_is_rational("1*2");
  assert_reduced_is_rational("1+2");
  assert_reduced_is_rational("1.0f", false);
  assert_reduced_is_rational("inf", false);
  assert_reduced_is_rational("undef", false);
  assert_reduced_is_rational("a", false);
}

void assert_has_random(const char* input, bool hasRandom = true) {
  UserExpression e = UserExpression::Builder(parse(input));
  quiz_assert_print_if_failure(e.hasRandomNumber() == hasRandom, input);
}

QUIZ_CASE(pcj_properties_has_random) {
  assert_has_random("random()");
  assert_has_random("randint(1,2)");
  assert_has_random("cos(random())");
  assert_has_random("random()-random()");
  assert_has_random("a", false);
  assert_has_random("2/3", false);
}

void assert_is_parametered_expression(const char* input,
                                      bool isParametered = true) {
  UserExpression e = UserExpression::Builder(parse(input));
  quiz_assert_print_if_failure(e.tree()->isParametric() == isParametered,
                               input);
}

QUIZ_CASE(pcj_properties_is_parametered_expression) {
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
  UserExpression e = UserExpression::Builder(parse(input));
  quiz_assert_print_if_failure(e.recursivelyMatches(test) == hasProperty,
                               input);
}

template <typename T>
void assert_expression_has_property(const char* input, T test) {
  assert_expression_has_property_or_not<T>(input, test, true);
}

template <typename T>
void assert_expression_has_not_property(const char* input, T test) {
  assert_expression_has_property_or_not<T>(input, test, false);
}

QUIZ_CASE(pcj_properties_has_approximate) {
  assert_expression_has_property("3.4", &Expression::isApproximate);
  assert_expression_has_property("2.3+1", &Expression::isApproximate);
  assert_expression_has_property("0.1f", &Expression::isApproximate);
  assert_expression_has_not_property("a", &Expression::isApproximate);
}

QUIZ_CASE(pcj_properties_has_matrix) {
  assert_expression_has_property("[[1,2][3,4]]",
                                 &Expression::isOfMatrixDimension);
  assert_expression_has_property("dim([[1,2][3,4]])/3",
                                 &Expression::isOfMatrixDimension);
  assert_expression_has_property("[[1,2][3,4]]^(-1)",
                                 &Expression::isOfMatrixDimension);
  assert_expression_has_property("inverse([[1,2][3,4]])",
                                 &Expression::isOfMatrixDimension);
  assert_expression_has_property("3*identity(4)",
                                 &Expression::isOfMatrixDimension);
  assert_expression_has_property("transpose([[1,2][3,4]])",
                                 &Expression::isOfMatrixDimension);
  assert_expression_has_property("ref([[1,2][3,4]])",
                                 &Expression::isOfMatrixDimension);
  assert_expression_has_property("rref([[1,2][3,4]])",
                                 &Expression::isOfMatrixDimension);
  assert_expression_has_property("cross([[1][2][3]],[[3][4][5]])",
                                 &Expression::isOfMatrixDimension);
  assert_expression_has_property("diff([[1,2][3,4]],x,2)",
                                 &Expression::isOfMatrixDimension);
  assert_expression_has_property("sign([[1,2][3,4]])",
                                 &Expression::isOfMatrixDimension);
  assert_expression_has_property("trace([[1,2][3,4]])",
                                 &Expression::isOfMatrixDimension);
  assert_expression_has_property("det([[1,2][3,4]])",
                                 &Expression::isOfMatrixDimension);
  assert_expression_has_not_property("2*3+1", &Expression::isOfMatrixDimension);
}

void assert_projected_is_infinity(const char* input, ProjectionContext* projCtx,
                                  bool isInfinity = true) {
  Tree* e = parse(input, projCtx->m_context);
  Simplification::ToSystem(e, projCtx);
  quiz_assert_print_if_failure(e->hasDescendantSatisfying([](const Tree* e) {
    return Internal::Infinity::IsPlusOrMinusInfinity(e);
  }) == isInfinity,
                               input);
}

QUIZ_CASE(pcj_properties_is_infinity) {
  PoincareTest::SymbolStore symbolStore;
  ProjectionContext projCtx = {
      .m_symbolic = SymbolicComputation::ReplaceAllSymbols,
      .m_context = symbolStore,
  };
  assert_projected_is_infinity("3.4+inf", &projCtx);
  assert_projected_is_infinity("2.3+1", &projCtx, false);
  assert_projected_is_infinity("a", &projCtx, false);

  PoincareTest::store("42.3+inf→a", symbolStore);
  assert_projected_is_infinity("a", &projCtx);

  PoincareTest::store("1+inf→x", symbolStore);
  assert_projected_is_infinity("x", &projCtx);
  assert_projected_is_infinity("diff(x^2,x,3)", &projCtx, false);
}

void assert_expression_has_variables(const char* input,
                                     const Tree* expectedVariables,
                                     ProjectionContext* projCtx) {
  Tree* e = parse(input, projCtx->m_context);
  Simplification::ToSystem(e, projCtx);
  const Tree* variables = Variables::GetUserSymbols(e);
  assert_trees_are_equal(variables, expectedVariables);
}

QUIZ_CASE(pcj_properties_get_variables) {
  /* Warning: Theses tests are weird because you need to avoid a lot of
   * reserved identifiers like:
   * - e and i
   * - m, g, h, l, s, t, A which are units and can be parsed without '_' now. */
  PoincareTest::SymbolStore symbolStore;
  ProjectionContext projCtx = {
      .m_symbolic = SymbolicComputation::ReplaceDefinedFunctions,
      .m_context = symbolStore,
  };

  assert_expression_has_variables("x+y", KList("x"_e, "y"_e), &projCtx);
  assert_expression_has_variables("x+y+z+2w", KList("w"_e, "x"_e, "y"_e, "z"_e),
                                  &projCtx);
  assert_expression_has_variables(
      "a+x^2+2y+k!B", KList("B"_e, "a"_e, "k"_e, "x"_e, "y"_e), &projCtx);

  PoincareTest::store("x→BABA", symbolStore);
  PoincareTest::store("y→abab", symbolStore);
  assert_expression_has_variables("BABA+abab", KList("BABA"_e, "abab"_e),
                                  &projCtx);
  PoincareTest::store("z→BBBBBB", symbolStore);
  assert_expression_has_variables("BBBBBB", KList("BBBBBB"_e), &projCtx);
  assert_expression_has_variables(
      "a+b+c+d+f+j+k+n+o+p+q+r",
      KList("a"_e, "b"_e, "c"_e, "d"_e, "f"_e, "j"_e, "k"_e, "n"_e, "o"_e,
            "p"_e, "q"_e, "r"_e),
      &projCtx);
  PoincareTest::store("1+π×x+x^2+\"toto\"→f(x)", symbolStore);
  assert_expression_has_variables("f(\"tata\")",
                                  KList("\"tata\""_e, "\"toto\""_e), &projCtx);
  symbolStore.reset();

  assert_expression_has_variables("diff(3x,x,0)y-2", KList("y"_e), &projCtx);
  assert_expression_has_variables(
      "a+b+c+d+f+j", KList("a"_e, "b"_e, "c"_e, "d"_e, "f"_e, "j"_e), &projCtx);
  assert_expression_has_variables("\"box\"+y×int(z,x,a,0)",
                                  KList("\"box\""_e, "a"_e, "y"_e, "z"_e),
                                  &projCtx);

  PoincareTest::store("0→f(x)", symbolStore);
  PoincareTest::store("x→va", symbolStore);
  assert_expression_has_variables("f(va)", KList("va"_e), &projCtx);
  symbolStore.reset();

  PoincareTest::store("12→a", symbolStore);
  PoincareTest::store("a→f(x)", symbolStore);
  assert_expression_has_variables("f(x)", KList("a"_e, "x"_e), &projCtx);
  symbolStore.reset();

  PoincareTest::store("1→f(x)", symbolStore);
  PoincareTest::store("2→g(x)", symbolStore);
  assert_expression_has_variables("f(g(x)+y)", KList("x"_e, "y"_e), &projCtx);
  symbolStore.reset();

  PoincareTest::store("1→x", symbolStore);
  assert_expression_has_variables("x+y", KList("x"_e, "y"_e), &projCtx);
  symbolStore.reset();

  PoincareTest::store("1→a", symbolStore);
  PoincareTest::store("a+b+c→x", symbolStore);
  assert_expression_has_variables("x+y", KList("x"_e, "y"_e), &projCtx);
  symbolStore.reset();

  PoincareTest::store("b+c+x→a", symbolStore);
  PoincareTest::store("x+b→g(x)", symbolStore);
  PoincareTest::store("a+g(x+y)→f(x)", symbolStore);
  assert_expression_has_variables("f(x)", KList("a"_e, "b"_e, "x"_e, "y"_e),
                                  &projCtx);
}

void assert_reduced_expression_has_polynomial_coefficient(
    const char* input, const char* symbolName, const Tree* expectedCoefficients,
    const Context& context = Poincare::EmptySymbolContext{},
    ComplexFormat complexFormat = ComplexFormat::Cartesian,
    AngleUnit angleUnit = AngleUnit::Radian,
    Preferences::UnitFormat unitFormat = Preferences::UnitFormat::Metric,
    SymbolicComputation symbolicComputation =
        SymbolicComputation::ReplaceDefinedSymbols) {
  ProjectionContext projContext{.m_complexFormat = complexFormat,
                                .m_angleUnit = angleUnit,
                                .m_symbolic = symbolicComputation,
                                .m_context = context};
  UserExpression e = UserExpression::Builder(parse(input, context));
  bool reductionFailure = false;
  SystemExpression s = e.cloneAndReduce(projContext, &reductionFailure);
  quiz_assert(!reductionFailure);
  Tree* coefficients = PolynomialParser::GetReducedCoefficients(s, symbolName);
  assert_trees_are_equal(coefficients, expectedCoefficients);
}

QUIZ_CASE(pcj_properties_get_polynomial_coefficients) {
  assert_reduced_expression_has_polynomial_coefficient("x^2+x+2", "x",
                                                       KList(2_e, 1_e, 1_e));
  assert_reduced_expression_has_polynomial_coefficient(
      "3×(x+2)^2-6×π", "x", KList(KAdd(12_e, KMult(-6_e, π_e)), 12_e, 3_e));
  assert_reduced_expression_has_polynomial_coefficient(
      "2×(n+1)^3-4n+32×x", "n",
      KList(KAdd(2_e, KMult(32_e, "x"_e)), 2_e, 6_e, 2_e));
  assert_reduced_expression_has_polynomial_coefficient(
      "x^2-π×x+1", "x", KList(1_e, KMult(-1_e, π_e), 1_e));

  PoincareTest::SymbolStore symbolStore;
  // f: x→x^2+Px+1
  PoincareTest::store("1+π×x+x^2→f(x)", symbolStore);
  assert_reduced_expression_has_polynomial_coefficient(
      "f(x)", "x", KList(1_e, π_e, 1_e), symbolStore);
  assert_reduced_expression_has_polynomial_coefficient("√(-1)x", "x",
                                                       KList(0_e, i_e));
  assert_reduced_expression_has_polynomial_coefficient(
      "√(-1)x", "x", KList(KNonReal), Poincare::EmptySymbolContext{},
      ComplexFormat::Real);

  // 3 -> x
  PoincareTest::store("3→x", symbolStore);
  assert_reduced_expression_has_polynomial_coefficient("x+1", "x", KList(4_e),
                                                       symbolStore);
  assert_reduced_expression_has_polynomial_coefficient(
      "x+2", "x", KList(2_e, 1_e), symbolStore, ComplexFormat::Real,
      AngleUnit::Radian, Preferences::UnitFormat::Metric,
      SymbolicComputation::KeepAllSymbols);
  assert_reduced_expression_has_polynomial_coefficient(
      "x+2", "x", KList(2_e, 1_e), symbolStore, ComplexFormat::Real,
      AngleUnit::Radian, Preferences::UnitFormat::Metric,
      SymbolicComputation::ReplaceDefinedFunctions);
  assert_reduced_expression_has_polynomial_coefficient(
      "f(x)", "x", KList(1_e, π_e, 1_e), symbolStore, ComplexFormat::Cartesian,
      AngleUnit::Radian, Preferences::UnitFormat::Metric,
      SymbolicComputation::ReplaceDefinedFunctions);
}

void assert_list_length_in_children_is(const char* definition,
                                       int targetLength) {
  UserExpression e = UserExpression::Builder(parse(definition));
  bool isValid = targetLength != -2;
  quiz_assert_print_if_failure(
      Poincare::Internal::Dimension::DeepCheck(e.tree()) == isValid,
      definition);
  if (isValid) {
    quiz_assert_print_if_failure(
        Poincare::Internal::Dimension::ListLength(e.tree()) == targetLength,
        definition);
  }
}

constexpr int k_mismatchedLists = -2;

QUIZ_CASE(pcj_properties_children_list_length) {
  assert_list_length_in_children_is(
      "1+1", Poincare::Internal::Dimension::k_nonListListLength);
  assert_list_length_in_children_is("1+{}", 0);
  assert_list_length_in_children_is("1*{2,3,4}*5*{6,7,8}", 3);
  assert_list_length_in_children_is("{1,-2,3,-4}^2", 4);
  assert_list_length_in_children_is("{1,2}+{3,4,5}", k_mismatchedLists);
}

void assert_is_list_of_points(const char* input, const Context& context,
                              bool truth = true) {
  UserExpression e = UserExpression::Builder(parse(input, context));
  bool isListOfPoints = e.dimension(context).isListOfPoints();
  quiz_assert_print_if_failure(isListOfPoints == truth, input);
}

QUIZ_CASE(pcj_properties_list_of_points) {
  PoincareTest::SymbolStore symbolStore;

  PoincareTest::store("(1,2)→a", symbolStore);
  PoincareTest::store("3→b", symbolStore);

  assert_is_list_of_points("{(1,2)}", symbolStore);
  assert_is_list_of_points("{(1,-2),(-3.4,5.6)}", symbolStore);
  assert_is_list_of_points("{a,(3,4)}", symbolStore);
  assert_is_list_of_points("{(undef,1),(3,undef),(undef, undef)}", symbolStore);

  assert_is_list_of_points("{1,2,3}", symbolStore, false);
  assert_is_list_of_points("{(1,2),3}", symbolStore, false);
  assert_is_list_of_points("{(1,2),3,(4,5)}", symbolStore, false);
  assert_is_list_of_points("{undef,1}", symbolStore, false);
  assert_is_list_of_points("{b}", symbolStore, false);

  // TODO_PCJ: These used to be lists of points but are not anymore.
  assert_is_list_of_points("{}", symbolStore, false);
  assert_is_list_of_points("{undef}", symbolStore, false);
  assert_is_list_of_points("{x}", symbolStore, false);
  assert_is_list_of_points("{a,undef,(3,4)}", symbolStore, false);
  assert_is_list_of_points("{a,x,(3,4)}", symbolStore, false);
}

void assert_is_continuous_on_interval(const char* input, float x1, float x2,
                                      bool isContinuous) {
  UserExpression e1 = UserExpression::Builder(parse(input));
  bool reductionFailure = false;
  SystemExpression e2 =
      e1.cloneAndReduce(ProjectionContext{}, &reductionFailure);
  quiz_assert(!reductionFailure);
  PreparedFunction e3 = e2.getPreparedFunction("x", true);
  quiz_assert_print_if_failure(
      !isContinuous ==
          Continuity::IsDiscontinuousOnInterval<float>(e3.tree(), x1, x2),
      input);
}

QUIZ_CASE(pcj_properties_is_continuous) {
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
  bool reductionFailure = false;
  SystemExpression reducedExpression =
      UserExpression::Builder(parse(input))
          .cloneAndReduce(ProjectionContext{}, &reductionFailure);
  quiz_assert(!reductionFailure);
  quiz_assert_print_if_failure(
      Variables::HasUserSymbols(reducedExpression.tree()) == isSymbolic, input);
}

QUIZ_CASE(pcj_properties_deep_is_symbolic) {
  assert_reduced_deep_is_symbolic("2/cos(3x+2)");
  assert_reduced_deep_is_symbolic("2/int(5x, x, 3, 4)", false);
  assert_reduced_deep_is_symbolic("2/int(5xy, x, 3, 4)");
  assert_reduced_deep_is_symbolic("2/int(diff(xy, y, 2), x, 3, 4)", false);
  assert_reduced_deep_is_symbolic("2/int(diff(xy^n, y, 2), x, 3, 4)");
  assert_reduced_deep_is_symbolic("2x+1");
  assert_reduced_deep_is_symbolic("diff(x^2,x,3)", false);
  assert_reduced_deep_is_symbolic("diff(y^2+x^2,x,3)", false);
}
