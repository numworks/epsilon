#include <quiz.h>
#include <apps/shared/global_context.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <cmath>
#include "../equation_store.h"
#include "../../../poincare/test/helper.h"

using namespace Poincare;

namespace Solver {

void addEquationWithText(EquationStore * equationStore, const char * text, Context * context) {
  Ion::Storage::Record::ErrorStatus err = equationStore->addEmptyModel();
  quiz_assert_print_if_failure(err == Ion::Storage::Record::ErrorStatus::None, text);
  (void) err; // Silence warning in release mode
  Ion::Storage::Record record = equationStore->recordAtIndex(equationStore->numberOfModels()-1);
  Shared::ExpiringPointer<Equation> model = equationStore->modelForRecord(record);
  model->setContent(text, context);
}

void assert_equation_system_exact_solve_to(const char * equations[], EquationStore::Error error, EquationStore::Type type, const char * variables[], const char * solutions[], int numberOfSolutions, bool didReplaceFunctionsButNotSymbols = false) {
  Shared::GlobalContext globalContext;
  EquationStore equationStore;
  int index = 0;
  while (equations[index] != 0) {
    addEquationWithText(&equationStore, equations[index++], &globalContext);
  }
  bool replaceFunctionsButNotSymbols = false;
  EquationStore::Error err = equationStore.exactSolve(&globalContext, &replaceFunctionsButNotSymbols);
  quiz_assert_print_if_failure(err == error, equations[0]);
  quiz_assert_print_if_failure(replaceFunctionsButNotSymbols == didReplaceFunctionsButNotSymbols, equations[0]);
  if (err != EquationStore::Error::NoError) {
    equationStore.removeAll();
    return;
  }
  quiz_assert_print_if_failure(equationStore.type() == type, equations[0]);
  quiz_assert_print_if_failure(equationStore.numberOfSolutions() == numberOfSolutions, equations[0]);
  if (numberOfSolutions == INT_MAX) {
    equationStore.removeAll();
    return;
  }
  if (type == EquationStore::Type::LinearSystem) {
    for (int i = 0; i < numberOfSolutions; i++) {
      quiz_assert_print_if_failure(strcmp(equationStore.variableAtIndex(i),variables[i]) == 0, equations[0]);
    }
  } else {
    quiz_assert_print_if_failure(strcmp(equationStore.variableAtIndex(0), variables[0]) == 0, equations[0]);
  }
  constexpr int bufferSize = 200;
  char buffer[bufferSize];
  for (int i = 0; i < numberOfSolutions; i++) {
    equationStore.exactSolutionLayoutAtIndex(i, true).serializeForParsing(buffer, bufferSize);
    quiz_assert_print_if_failure(strcmp(buffer, solutions[i]) == 0, equations[0]);
  }
  equationStore.removeAll();
}

void assert_equation_approximate_solve_to(const char * equations, double xMin, double xMax, const char * variable, double solutions[], int numberOfSolutions, bool hasMoreSolutions) {
  Shared::GlobalContext globalContext;
  EquationStore equationStore;
  addEquationWithText(&equationStore, equations, &globalContext);
  bool replaceFunctionsButNotSymbols = false;
  EquationStore::Error err = equationStore.exactSolve(&globalContext, &replaceFunctionsButNotSymbols);
  quiz_assert(err == EquationStore::Error::RequireApproximateSolution);
  equationStore.setIntervalBound(0, xMin);
  equationStore.setIntervalBound(1, xMax);
  equationStore.approximateSolve(&globalContext, replaceFunctionsButNotSymbols);
  quiz_assert(equationStore.numberOfSolutions() == numberOfSolutions);
  quiz_assert(strcmp(equationStore.variableAtIndex(0), variable)== 0);
  for (int i = 0; i < numberOfSolutions; i++) {
    quiz_assert(std::fabs(equationStore.approximateSolutionAtIndex(i) - solutions[i]) < 1E-5);
  }
  quiz_assert(equationStore.haveMoreApproximationSolutions(&globalContext, replaceFunctionsButNotSymbols) == hasMoreSolutions);
  equationStore.removeAll();
}

QUIZ_CASE(equation_solve) {
  // x+y+z+a+b+c+d = 0
  const char * variables1[] = {""};
  const char * equations0[] = {"x+y+z+a+b+c+d=0", 0};
  assert_equation_system_exact_solve_to(equations0, EquationStore::Error::TooManyVariables, EquationStore::Type::LinearSystem, (const char **)variables1, nullptr, 0);

  // x^2+y = 0
  const char * equations1[] = {"x^2+y=0", 0};
  assert_equation_system_exact_solve_to(equations1, EquationStore::Error::NonLinearSystem, EquationStore::Type::LinearSystem, (const char **)variables1, nullptr, 0);

  // cos(x) = 0
  const char * equations2[] = {"cos(x)=0", 0};
  assert_equation_system_exact_solve_to(equations2, EquationStore::Error::RequireApproximateSolution, EquationStore::Type::LinearSystem, (const char **)variables1, nullptr, 0);

  // 2 = 0
  const char * equations3[] = {"2=0", 0};
  assert_equation_system_exact_solve_to(equations3,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variables1, nullptr, 0);
  // 0 = 0
  const char * equations4[] = {"0=0", 0};
  assert_equation_system_exact_solve_to(equations4,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variables1, nullptr, INT_MAX);

  // x-x+2 = 0
  const char * equations5[] = {"x-x+2=0", 0};
  assert_equation_system_exact_solve_to(equations5,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variables1, nullptr, 0);

  // x-x= 0
  const char * equations6[] = {"x-x=0", 0};
  assert_equation_system_exact_solve_to(equations6,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variables1, nullptr, INT_MAX);

  const char * variablesx[] = {"x", ""};
  // 2x+3=4
  const char * equations7[] = {"2x+3=4", 0};
  const char * solutions7[] = {"\u0012\u00121\u0013/\u00122\u0013\u0013"}; // 1/2
  assert_equation_system_exact_solve_to(equations7,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesx, solutions7, 1);

  // 3x^2-4x+4=2
  const char * equations8[] = {"3×x^2-4x+4=2", 0};
  const char * solutions8[] = {"\u0012\u00122\u0013/\u00123\u0013\u0013-\u0012\u0012√\u00122\u0013\u0013/\u00123\u0013\u0013𝐢","\u0012\u00122\u0013/\u00123\u0013\u0013+\u0012\u0012√\u00122\u0013\u0013/\u00123\u0013\u0013𝐢", "-8"}; // 2/3-(√(2)/3)𝐢,  2/3+(√(2)/3)𝐢
  assert_equation_system_exact_solve_to(equations8, EquationStore::Error::NoError, EquationStore::Type::PolynomialMonovariable, (const char **)variablesx, solutions8, 3);

  // 2×x^2-4×x+4=3
  const char * equations9[] = {"2×x^2-4×x+4=3", 0};
  const char * solutions9[] = {"\u0012\u0012-√\u00122\u0013+2\u0013/\u00122\u0013\u0013","\u0012\u0012√\u00122\u0013+2\u0013/\u00122\u0013\u0013", "8"}; // (-√(2)+2)/2, (√(2)+2)/2, 8
  assert_equation_system_exact_solve_to(equations9, EquationStore::Error::NoError, EquationStore::Type::PolynomialMonovariable, (const char **)variablesx, solutions9, 3);

  // 2×x^2-4×x+2=0
  const char * equations10[] = {"2×x^2-4×x+2=0", 0};
  const char * solutions10[] = {"1", "0"};
  assert_equation_system_exact_solve_to(equations10, EquationStore::Error::NoError, EquationStore::Type::PolynomialMonovariable, (const char **)variablesx, solutions10, 2);

  // x^2+x+1=3×x^2+pi×x-√(5)
  const char * equations11[] = {"x^2+x+1=3×x^2+π×x-√(5)", 0};
  const char * solutions11[] = {"\u0012\u0012√\u0012π^\u00122\u0013-2π+8√\u00125\u0013+9\u0013-π+1\u0013/\u00124\u0013\u0013", "\u0012\u0012-√\u0012π^\u00122\u0013-2π+8√\u00125\u0013+9\u0013-π+1\u0013/\u00124\u0013\u0013", "π^\u00122\u0013-2π+8√\u00125\u0013+9"}; // (√(π^2-2π+8√(5)+9)-π+1)/4, (-√(π^2-2π+8×√(5)+9)-π+1)/4, π^2-2π+8√(5)+9
  assert_equation_system_exact_solve_to(equations11, EquationStore::Error::NoError, EquationStore::Type::PolynomialMonovariable, (const char **)variablesx, solutions11, 3);

  // (x-3)^2
  const char * equations21[] = {"(x-3)^2=0", 0};
  const char * solutions21[] = {"3", "0"};
  assert_equation_system_exact_solve_to(equations21, EquationStore::Error::NoError, EquationStore::Type::PolynomialMonovariable, (const char **)variablesx, solutions21, 2);

  // TODO
  // x^3 - 4x^2 + 6x - 24 = 0
  //const char * equations10[] = {"2×x^2-4×x+4=3", 0};
  //assert_equation_system_exact_solve_to(equations10, EquationStore::Error::NoError, EquationStore::Type::PolynomialMonovariable, {"x", ""}, {"4", "𝐢×√(6)", "-𝐢×√(6)", "-11616"}, 4);

  //x^3+x^2+1=0
  // x^3-3x-2=0

  // Linear System
  const char * equations12[] = {"x+y=0", 0};
  assert_equation_system_exact_solve_to(equations12,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesx, nullptr, INT_MAX);

  const char * variablesxy[] = {"x", "y", ""};

  const char * equations13[] = {"x+y=0", "3x+y=-5", 0};
  const char * solutions13[] = {"-\u0012\u00125\u0013/\u00122\u0013\u0013", "\u0012\u00125\u0013/\u00122\u0013\u0013"}; // -5/2; 5/2
  assert_equation_system_exact_solve_to(equations13,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesxy, solutions13, 2);

  const char * variablesxyz[] = {"x", "y", "z", ""};

  const char * equations14[] = {"x+y=0", "3x+y+z=-5", "4z-π=0", 0};
  const char * solutions14[] = {"\u0012\u0012-π-20\u0013/\u00128\u0013\u0013", "\u0012\u0012π+20\u0013/\u00128\u0013\u0013", "\u0012\u0012π\u0013/\u00124\u0013\u0013"}; // (-π-20)/8, (π+20)/8, π/4
  assert_equation_system_exact_solve_to(equations14,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesxyz, solutions14, 3);

  const char * variablesxyzabc[] = {"x", "y", "z", "a", "b", "c"};
  const char * equations22[] = {"x+y=0", "3x+y+z=-5", "4z-π=0", "a+b+c=0", "a = 3", "c = a+2", 0};
  const char * solutions22[] = {"\u0012\u0012-π-20\u0013/\u00128\u0013\u0013", "\u0012\u0012π+20\u0013/\u00128\u0013\u0013", "\u0012\u0012π\u0013/\u00124\u0013\u0013", "3", "-8", "5"}; // (-π-20)/8, (π+20)/8, π/4, 3, 5, -8
  assert_equation_system_exact_solve_to(equations22,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesxyzabc, solutions22, 6);

  /* This test case needs the user defined variable. Indeed, in the equation
   * store, m_variables is just before m_userVariables, so bad fetching in
   * m_variables might fetch into m_userVariables and create problems. */
  assert_simplify("0→x");
  const char * variablesbDeyzt[] = {"b", "D", "e", "y", "z", "t"};
  const char * equations23[] = {"b=0", "D=0", "e=0", "", "x+y+z+t=0", 0};
  assert_equation_system_exact_solve_to(equations23,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesbDeyzt, nullptr, INT_MAX);
  Ion::Storage::sharedStorage()->recordNamed("x.exp").destroy();

  // Monovariable non-polynomial equation
  double solutions15[] = {-90.0, 90.0};
  assert_equation_approximate_solve_to("cos(x)=0", -100.0, 100.0, "x", solutions15, 2, false);

  double solutions16[] = {-810.0, -630.0, -450.0, -270.0, -90.0, 90.0, 270.0, 450.0, 630.0, 810.0};
  assert_equation_approximate_solve_to("cos(x)=0", -900.0, 1000.0, "x", solutions16, 10, true);

  double solutions17[] = {0};
  assert_equation_approximate_solve_to("√(y)=0", -900.0, 1000.0, "y", solutions17, 1, false);

  // Long variable names
  const char * variablesabcde[] = {"abcde", ""};
  const char * equations18[] = {"2abcde+3=4", 0};
  const char * solutions18[] = {"\u0012\u00121\u0013/\u00122\u0013\u0013"}; // 1/2
  assert_equation_system_exact_solve_to(equations18,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesabcde, solutions18, 1);

  const char * variablesBig1Big2[] = {"Big1", "Big2", ""};
  const char * equations19[] = {"Big1+Big2=0", "3Big1+Big2=-5", 0};
  const char * solutions19[] = {"-\u0012\u00125\u0013/\u00122\u0013\u0013", "\u0012\u00125\u0013/\u00122\u0013\u0013"}; // -5/2, 5/2
  assert_equation_system_exact_solve_to(equations19,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesBig1Big2, solutions19, 2);

  // conj(x)*x+1 = 0
  const char * equations20one = "conj(x)*x+1=0";
  const char * equations20[] = {equations20one, 0};
  assert_equation_system_exact_solve_to(equations20, EquationStore::Error::RequireApproximateSolution, EquationStore::Type::LinearSystem, (const char **)variables1, nullptr, 0);
  assert_equation_approximate_solve_to(equations20one, -100.0, 100.0, "x", nullptr, 0, false);
}

QUIZ_CASE(equation_solve_complex_format) {
  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Real);
  const char * variablesx[] = {"x", ""};
  // x+I = 0 --> x = -𝐢
  const char * equations0[] = {"x+𝐢=0", 0};
  const char * solutions0[] = {"-𝐢"};
  assert_equation_system_exact_solve_to(equations0,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesx, solutions0, 1);

  // x+√(-1) = 0 --> Not defined in R
  const char * equations1[] = {"x+√(-1)=0", 0};
  assert_equation_system_exact_solve_to(equations1,  EquationStore::Error::EquationUnreal, EquationStore::Type::LinearSystem, (const char **)variablesx, nullptr, 0);

  // x^2+x+1=0 --> No solution in R
  const char * equations2[] = {"x^2+x+1=0", 0};
  const char * delta2[] = {"-3"};
  assert_equation_system_exact_solve_to(equations2, EquationStore::Error::NoError, EquationStore::Type::PolynomialMonovariable, (const char **)variablesx, delta2, 1);

  // x^2-√(-1)=0 --> Not defined in R
  const char * equations3[] = {"x^2-√(-1)=0", 0};
  assert_equation_system_exact_solve_to(equations3, EquationStore::Error::EquationUnreal, EquationStore::Type::PolynomialMonovariable, (const char **)variablesx, nullptr, 0);

  // x+√(-1)×√(-1) = 0 --> Not defined in R
  const char * equations4[] = {"x+√(-1)×√(-1)=0", 0};
  assert_equation_system_exact_solve_to(equations4,  EquationStore::Error::EquationUnreal, EquationStore::Type::LinearSystem, (const char **)variablesx, nullptr, 0);

  // root(-8,3)*x+3 = 0 --> 3/2 in R
  const char * equations5[] = {"root(-8,3)*x+3=0", 0};
  const char * solutions5[] = {"\u0012\u00123\u0013/\u00122\u0013\u0013"};
  assert_equation_system_exact_solve_to(equations5,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesx, solutions5, 1);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Cartesian);
  // x+𝐢 = 0 --> x = -𝐢
  assert_equation_system_exact_solve_to(equations0,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesx, solutions0, 1);

  // x+√(-1) = 0 --> x = -𝐢
  assert_equation_system_exact_solve_to(equations1,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesx, solutions0, 1);

  // x^2+x+1=0
  const char * solutions2[] = {"-\u0012\u00121\u0013/\u00122\u0013\u0013-\u0012\u0012√\u00123\u0013\u0013/\u00122\u0013\u0013𝐢","-\u0012\u00121\u0013/\u00122\u0013\u0013+\u0012\u0012√\u00123\u0013\u0013/\u00122\u0013\u0013𝐢", "-3"}; // -1/2-((√(3))/2)𝐢, -1/2+((√(3))/2)𝐢, -3
  assert_equation_system_exact_solve_to(equations2, EquationStore::Error::NoError, EquationStore::Type::PolynomialMonovariable, (const char **)variablesx, solutions2, 3);

  // x^2-√(-1)=0
  const char * solutions3[] = {"-\u0012\u0012√\u00122\u0013\u0013/\u00122\u0013\u0013-\u0012\u0012√\u00122\u0013\u0013/\u00122\u0013\u0013𝐢", "\u0012\u0012√\u00122\u0013\u0013/\u00122\u0013\u0013+\u0012\u0012√\u00122\u0013\u0013/\u00122\u0013\u0013𝐢","4𝐢"}; // -√(2)/2-(√(2)/2)𝐢, √(2)/2+(√(2)/2)𝐢, 4𝐢
  assert_equation_system_exact_solve_to(equations3, EquationStore::Error::NoError, EquationStore::Type::PolynomialMonovariable, (const char **)variablesx, solutions3, 3);

  // x+√(-1)×√(-1) = 0
  const char * solutions4[] = {"1"};
  assert_equation_system_exact_solve_to(equations4,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesx, solutions4, 1);

  const char * solutions5Cartesain[] = {"-\u0012\u00123\u0013/\u00124\u0013\u0013+\u0012\u00123√\u00123\u0013\u0013/\u00124\u0013\u0013𝐢"}; //-3/4+(3√3/4)*𝐢
  assert_equation_system_exact_solve_to(equations5,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesx, solutions5Cartesain, 1);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Polar);
  // x+𝐢 = 0 --> x = e^(-π/2×i)
  const char * solutions0Polar[] = {"ℯ^\u0012-\u0012\u0012π\u0013/\u00122\u0013\u0013𝐢\u0013"}; // ℯ^(-(π/2)𝐢)
  assert_equation_system_exact_solve_to(equations0,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesx, solutions0Polar, 1);

  // x+√(-1) = 0 --> x = e^(-π/2𝐢)
  assert_equation_system_exact_solve_to(equations1,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesx, solutions0Polar, 1);

  // x^2+x+1=0
  const char * solutions2Polar[] = {"ℯ^\u0012-\u0012\u00122π\u0013/\u00123\u0013\u0013𝐢\u0013","ℯ^\u0012\u0012\u00122π\u0013/\u00123\u0013\u0013𝐢\u0013", "3ℯ^\u0012π·𝐢\u0013"}; // ℯ^(-(2π/3)𝐢), ℯ^((2π/3)𝐢), 3ℯ^(π𝐢)
  assert_equation_system_exact_solve_to(equations2, EquationStore::Error::NoError, EquationStore::Type::PolynomialMonovariable, (const char **)variablesx, solutions2Polar, 3);

  // x^2-√(-1)=0
  const char * solutions3Polar[] = {"ℯ^\u0012-\u0012\u00123π\u0013/\u00124\u0013\u0013𝐢\u0013", "ℯ^\u0012\u0012\u0012π\u0013/\u00124\u0013\u0013𝐢\u0013", "4ℯ^\u0012\u0012\u0012π\u0013/\u00122\u0013\u0013𝐢\u0013"}; // ℯ^(-(3×π/4)𝐢)", "ℯ^((π/4)𝐢)", "4ℯ^((π/2)𝐢)
  assert_equation_system_exact_solve_to(equations3, EquationStore::Error::NoError, EquationStore::Type::PolynomialMonovariable, (const char **)variablesx, solutions3Polar, 3);

  const char * solutions5Polar[] = {"\u0012\u00123\u0013/\u00122\u0013\u0013ℯ^\u0012\u0012\u00122π\u0013/\u00123\u0013\u0013𝐢\u0013"}; //3/2ℯ^\u0012\u00122π\u0012/3\u0013𝐢"};
  assert_equation_system_exact_solve_to(equations5,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variablesx, solutions5Polar, 1);

  // Put back the complex format
  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Real);
}

QUIZ_CASE(equation_and_symbolic_computation) {
  // x+a=0 : non linear system
  const char * equation1[] = {"x+a=0", 0};
  assert_equation_system_exact_solve_to(equation1, EquationStore::Error::NoError, EquationStore::Type::LinearSystem, nullptr, nullptr, INT_MAX);

  // -3->a
  Shared::GlobalContext globalContext;
  Expression::ParseAndSimplify("-3→a", &globalContext, Preferences::ComplexFormat::Polar, Preferences::AngleUnit::Degree);

  // x+a = 0 : x = 3
  const char * variables1[] = {"x", ""};
  const char * solutions1[] = {"3"};
  assert_equation_system_exact_solve_to(equation1, EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variables1, solutions1, 1);

  /* a = 0 : the equation has no solution as the user defined a = -3, so a is
   * not replaced with its context value and the result is a = 0. */
  const char * equation2[] = {"a=0", 0};
  const char * variables2[] = {"a", ""};
  const char * solutions2[] = {"0"};
  assert_equation_system_exact_solve_to(equation2, EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variables2, solutions2, 1, true);

  // 4->b
  Expression::ParseAndSimplify("-4→b", &globalContext, Preferences::ComplexFormat::Polar, Preferences::AngleUnit::Degree);

  /* a + b = 0 : the equation has no solution as the user defined a = -3, and
   * b = -4 so a and b are not replaced with their context values and the result
   * is an infinity of solutions. */
  const char * equation3[] = {"a+b=0", 0};
  const char * variables3[] = {"a", "b", ""};
  assert_equation_system_exact_solve_to(equation3, EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variables3, nullptr, INT_MAX, true);

  // a + b + c = 0 : the equation has the solution c = -7
  const char * equation4[] = {"a+b+c=0", 0};
  const char * variables4[] = {"c", ""};
  const char * solutions4[] = {"7"};
  assert_equation_system_exact_solve_to(equation4, EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variables4, solutions4, 1);

  /* a + c = 0 and a = 3: the system has no solution as the user defined a = -3,
   * so a is not replaced with its context value and the result is a = 3 and
   * c = -3. */
  const char * equation5[] = {"a+c=0", "a=3", 0};
  const char * variables5[] = {"a", "c", ""};
  const char * solutions5[] = {"3", "-3"};
  assert_equation_system_exact_solve_to(equation5, EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variables5, solutions5, 2, true);

  // x+1->f(x)
  Expression::ParseAndSimplify("x+1→f(x)", &globalContext, Preferences::ComplexFormat::Polar, Preferences::AngleUnit::Degree);

  // f(x) = 0 : x = -1
  const char * equation6[] = {"f(x)=0", 0};
  const char * variables6[] = {"x", ""};
  const char * solutions6[] = {"-1",};
  assert_equation_system_exact_solve_to(equation6, EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variables6, solutions6, 1);

  /* f(a) = 0 : the equation has no solution as the user defined a = -3, so a is
   * not replaced with its context value and the result is a = -1. */
  const char * equation7[] = {"f(a)=0", 0};
  const char * variables7[] = {"a", ""};
  const char * solutions7[] = {"-1",};
  assert_equation_system_exact_solve_to(equation7, EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variables7, solutions7, 1, true);

  // a+x+1->g(x)
  Expression::ParseAndSimplify("a+x+2→g(x)", &globalContext, Preferences::ComplexFormat::Polar, Preferences::AngleUnit::Degree);

  // g(x) = 0 : x = 2
  const char * equation8[] = {"g(x)=0", 0};
  const char * variables8[] = {"x", ""};
  const char * solutions8[] = {"1",};
  assert_equation_system_exact_solve_to(equation8, EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variables8, solutions8, 1);

  /* g(a) = 0 : the equation has no solution as the user defined a = -3, so a is
   * not replaced with its context value and the equation becomes a+a+2=0. The
   * solution is a = -1. */
  const char * equation9[] = {"g(a)=0", 0};
  const char * variables9[] = {"a", ""};
  const char * solutions9[] = {"-1",};
  assert_equation_system_exact_solve_to(equation9, EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variables9, solutions9, 1, true);

  /* c = d
   * d = 5
   * h(x) = c + d + 3
   *   /c = -3
   *   \h(x) = 0
   * c and d context values should not be used, and the solution is c = -3, d = 0 */
  Expression::ParseAndSimplify("5→d", &globalContext, Preferences::ComplexFormat::Polar, Preferences::AngleUnit::Degree);
  Expression::ParseAndSimplify("d→c", &globalContext, Preferences::ComplexFormat::Polar, Preferences::AngleUnit::Degree);
  Expression::ParseAndSimplify("c+d+3→h(x)", &globalContext, Preferences::ComplexFormat::Polar, Preferences::AngleUnit::Degree);
  const char * equation10[] = {"h(x)=0", "c = -3", 0};
  const char * variables10[] = {"c", "d", ""};
  const char * solutions10[] = {"-3", "0"};
  assert_equation_system_exact_solve_to(equation10, EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variables10, solutions10, 2, true);

  const char * equation11[] = {"c+d=5", "c-d=1", 0};
  const char * variables11[] = {"c", "d", ""};
  const char * solutions11[] = {"3", "2"};
  assert_equation_system_exact_solve_to(equation11, EquationStore::Error::NoError, EquationStore::Type::LinearSystem, (const char **)variables11, solutions11, 2, true);


  // Clean the storage
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("b.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("c.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("d.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("g.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("h.func").destroy();
}

}
