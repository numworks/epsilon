#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include <limits.h>
#include "helper.h"

using namespace Poincare;

void assert_equation_solve_to(const char * expression, int expectedNumberOfSolution, const char ** solutions, const char * delta, Expression::AngleUnit angleUnit = Expression::AngleUnit::Degree) {
  GlobalContext globalContext;
  Expression * e = parse_expression(expression);
  assert(e->type() == Expression::Type::Equal);
  Expression * solutionBuffer[Poincare::Expression::k_maxNumberOfVariables];
  Expression * deltaBuffer[1] = {nullptr};
  int numberOfSolutions = static_cast<Equal *>(e)->solve(solutionBuffer, deltaBuffer, globalContext, angleUnit);
  assert(numberOfSolutions == expectedNumberOfSolution);
  if (numberOfSolutions >= 0 && numberOfSolutions < INT_MAX) {
    for (int i = 0; i < numberOfSolutions; i++) {
      Expression * f = parse_expression(solutions[i]);
      Expression::Simplify(&f, globalContext, angleUnit);
      assert(solutionBuffer[i]->isIdenticalTo(f));
      delete f;
      delete solutionBuffer[i];
    }
    assert(solutions[numberOfSolutions] == 0);
    if (delta) {
      Expression * deltaExpression = parse_expression(delta);
      Expression::Simplify(&deltaExpression, globalContext, angleUnit);
      assert(deltaBuffer[0]->isIdenticalTo(deltaExpression));
      delete deltaBuffer[0];
      delete deltaExpression;
    }
  }
  delete e;
}

QUIZ_CASE(poincare_solve_equations) {
  // x+y+z+a+b+c+d = 0
  assert_equation_solve_to("x+y+z+a+b+c+d=0", -10, nullptr, nullptr);
  // 2 = 0
  assert_equation_solve_to("2=0", -1, nullptr, nullptr);
  // 0 = 0
  assert_equation_solve_to("0=0", INT_MAX, nullptr, nullptr);
  // cos(x) = 0
  // TODO
  // x-x+2 = 0
  assert_equation_solve_to("x-x+2=0", -1, nullptr, nullptr);
  // x-x= 0
  assert_equation_solve_to("x-x=0", INT_MAX, nullptr, nullptr);
  // 2x+3=4
  const char * solutions0[] = {"1/2", 0};
  assert_equation_solve_to("2*x+3=4", 1, solutions0, nullptr);
  // 3x^2-4x+4=2
  const char * solutions1[] = {"1-R(2)/2", "1+R(2)/2", 0};
  const char * delta1 = "8";
  assert_equation_solve_to("2*x^2-4*x+4=3", 2, solutions1, delta1);
  // 3x^2-4x+2=0
  const char * solutions2[] = {"1", 0};
  const char * delta2 = "0";
  assert_equation_solve_to("2*x^2-4*x+2=0", 1, solutions2, delta2);
#if 0
  // x^3 - 4x^2 + 6x - 24 = 0
  const char * solutions3[] = {"4", "I*R(6)", "-I*R(6)", 0};
  const char * delta3 = "-11616";
  assert_equation_solve_to("x^3+x^2+1=0", 3, solutions3, delta3);
  // x^3-3x-2=0
  const char * solutions4[] = {"-1", "2", 0};
  const char * delta4 = "0";
  assert_equation_solve_to("x^3-3x-2=0", 2, solutions4, delta4);
  // TODO: polynome degree 3, 4
#endif
  // TODO: linear system
}
