#include <quiz.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <cmath>
#include "../equation_store.h"
#include "../../../poincare/test/helper.h"

using namespace Poincare;

namespace Solver {

void assert_equation_system_exact_solve_to(const char * equations[], EquationStore::Error error, EquationStore::Type type, const char * variables, const char * solutions[], int numberOfSolutions) {
  char buffer[200];
  GlobalContext globalContext;
  EquationStore equationStore;
  int index = 0;
  while (equations[index] != 0) {
    Shared::ExpressionModel * e = equationStore.addEmptyModel();
    strlcpy(buffer, equations[index++], 200);
    translate_in_special_chars(buffer);
    e->setContent(buffer);
  }
  EquationStore::Error err = equationStore.exactSolve(&globalContext);
  assert(err == error);
  if (err != EquationStore::Error::NoError) {
    return;
  }
  assert(equationStore.type() == type);
  assert(equationStore.numberOfSolutions() == numberOfSolutions);
  if (numberOfSolutions == INT_MAX) {
    return;
  }
  if (type == EquationStore::Type::LinearSystem) {
    for (int i = 0; i < numberOfSolutions; i++) {
      assert(equationStore.variableAtIndex(i) == variables[i]);
    }
  } else {
    assert(equationStore.variableAtIndex(0) == variables[0]);
  }
  int n = type == EquationStore::Type::PolynomialMonovariable ? numberOfSolutions+1 : numberOfSolutions; // Check Delta for PolynomialMonovariable
  for (int i = 0; i < n; i++) {
    equationStore.exactSolutionLayoutAtIndex(i, true)->writeTextInBuffer(buffer, 200);
    translate_in_ASCII_chars(buffer);
    assert(strcmp(buffer, solutions[i]) == 0);
  }
}

void assert_equation_approximate_solve_to(const char * equations, double xMin, double xMax, const char variable, double solutions[], int numberOfSolutions, bool hasMoreSolutions) {
  char buffer[200];
  GlobalContext globalContext;
  EquationStore equationStore;
  Shared::ExpressionModel * e = equationStore.addEmptyModel();
  strlcpy(buffer, equations, 200);
  translate_in_special_chars(buffer);
  e->setContent(buffer);
  EquationStore::Error err = equationStore.exactSolve(&globalContext);
  assert(err == EquationStore::Error::RequireApproximateSolution);
  equationStore.setIntervalBound(0, xMin);
  equationStore.setIntervalBound(1, xMax);
  equationStore.approximateSolve(&globalContext);
  assert(equationStore.numberOfSolutions() == numberOfSolutions);
  assert(equationStore.variableAtIndex(0) == variable);
  for (int i = 0; i < numberOfSolutions; i++) {
    assert(std::fabs(equationStore.approximateSolutionAtIndex(i) - solutions[i]) < 1E-5);
  }
  assert(equationStore.haveMoreApproximationSolutions(&globalContext) == hasMoreSolutions);
}

QUIZ_CASE(equation_solve) {
  // x+y+z+a+b+c+d = 0
  const char * equations0[] = {"x+y+z+a+b+c+d=0", 0};
  assert_equation_system_exact_solve_to(equations0, EquationStore::Error::TooManyVariables, EquationStore::Type::LinearSystem, nullptr, nullptr, 0);

  // x^2+y = 0
  const char * equations1[] = {"x^2+y=0", 0};
  assert_equation_system_exact_solve_to(equations1, EquationStore::Error::NonLinearSystem, EquationStore::Type::LinearSystem, nullptr, nullptr, 0);

  // cos(x) = 0
  const char * equations2[] = {"cos(x)=0", 0};
  assert_equation_system_exact_solve_to(equations2, EquationStore::Error::RequireApproximateSolution, EquationStore::Type::LinearSystem, nullptr, nullptr, 0);

  // 2 = 0
  const char * equations3[] = {"2=0", 0};
  assert_equation_system_exact_solve_to(equations3,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, "", nullptr, 0);
  // 0 = 0
  const char * equations4[] = {"0=0", 0};
  assert_equation_system_exact_solve_to(equations4,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, "", nullptr, INT_MAX);

  // x-x+2 = 0
  const char * equations5[] = {"x-x+2=0", 0};
  assert_equation_system_exact_solve_to(equations5,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, "", nullptr, 0);

  // x-x= 0
  const char * equations6[] = {"x-x=0", 0};
  assert_equation_system_exact_solve_to(equations6,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, "", nullptr, INT_MAX);

  // 2x+3=4
  const char * equations7[] = {"2x+3=4", 0};
  const char * solutions7[] = {"(1)/(2)"};
  assert_equation_system_exact_solve_to(equations7,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, "x", solutions7, 1);

  // 3x^2-4x+4=2
  const char * equations8[] = {"3*x^2-4x+4=2", 0};
  const char * solutions8[] = {"(2-R(2)*I)/(3)","(2+R(2)*I)/(3)", "-8"};
  assert_equation_system_exact_solve_to(equations8, EquationStore::Error::NoError, EquationStore::Type::PolynomialMonovariable, "x", solutions8, 2);

  // 2*x^2-4*x+4=3
  const char * equations9[] = {"2*x^2-4*x+4=3", 0};
  const char * solutions9[] = {"(2-R(2))/(2)","(2+R(2))/(2)", "8"};
  assert_equation_system_exact_solve_to(equations9, EquationStore::Error::NoError, EquationStore::Type::PolynomialMonovariable, "x", solutions9, 2);

  // 2*x^2-4*x+2=0
  const char * equations10[] = {"2*x^2-4*x+2=0", 0};
  const char * solutions10[] = {"1", "0"};
  assert_equation_system_exact_solve_to(equations10, EquationStore::Error::NoError, EquationStore::Type::PolynomialMonovariable, "x", solutions10, 1);

  // x^2+x+1=3*x^2+pi*x-R(5)
  const char * equations11[] = {"x^2+x+1=3*x^2+P*x-R(5)", 0};
  const char * solutions11[] = {"(1-P+R(9+8*R(5)-2*P+P^(2)))/(4)", "(1-P-R(9+8*R(5)-2*P+P^(2)))/(4)", "9+8*R(5)-2*P+P^(2)"};
  assert_equation_system_exact_solve_to(equations11, EquationStore::Error::NoError, EquationStore::Type::PolynomialMonovariable, "x", solutions11, 2);

  // TODO
  // x^3 - 4x^2 + 6x - 24 = 0
  //const char * equations10[] = {"2*x^2-4*x+4=3", 0};
  //assert_equation_system_exact_solve_to(equations10, EquationStore::Error::NoError, EquationStore::Type::PolynomialMonovariable, "x", {"4", "I*R(6)", "-I*R(6)", "-11616"}, 3);

  //x^3+x^2+1=0
  // x^3-3x-2=0

  // Linear System
  const char * equations12[] = {"x+y=0", 0};
  assert_equation_system_exact_solve_to(equations12,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, "", nullptr, INT_MAX);

  const char * equations13[] = {"x+y=0", "3x+y=-5", 0};
  const char * solutions13[] = {"-(5)/(2)", "(5)/(2)"};
  assert_equation_system_exact_solve_to(equations13,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, "xy", solutions13, 2);

  const char * equations14[] = {"x+y=0", "3x+y+z=-5", "4z-P=0", 0};
  const char * solutions14[] = {"(-20-P)/(8)", "(20+P)/(8)", "(P)/(4)"};
  assert_equation_system_exact_solve_to(equations14,  EquationStore::Error::NoError, EquationStore::Type::LinearSystem, "xyz", solutions14, 3);

  // Monovariable non-polynomial equation
  double solutions15[] = {-90.0, 90.0};
  assert_equation_approximate_solve_to("cos(x)=0", -100.0, 100.0, 'x', solutions15, 2, false);

  double solutions16[] = {-810.0, -630.0, -450.0, -270.0, -90.0, 90.0, 270.0, 450.0, 630.0, 810.0};
  assert_equation_approximate_solve_to("cos(x)=0", -900.0, 1000.0, 'x', solutions16, 10, true);

  double solutions17[] = {0};
  assert_equation_approximate_solve_to("R(y)=0", -900.0, 1000.0, 'y', solutions17, 1, false);
}

}
