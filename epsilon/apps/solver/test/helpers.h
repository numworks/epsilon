#ifndef APPS_SOLVER_TEST_HELPERS_H
#define APPS_SOLVER_TEST_HELPERS_H

#include <poincare/preferences.h>
#include <poincare/test/old/helper.h>

#include <initializer_list>

#include "../equation_store.h"
#include "../system_of_equations.h"

#define bring_in(prefix, value) static const prefix value = prefix::value;

bring_in(Solver::SystemOfEquations::Error, EquationUndefined);
bring_in(Solver::SystemOfEquations::Error, EquationNonReal);
bring_in(Solver::SystemOfEquations::Error, NoError);
bring_in(Solver::SystemOfEquations::Error, NonLinearSystem);
bring_in(Solver::SystemOfEquations::Error, RequireApproximateSolution);
bring_in(Solver::SystemOfEquations::Error, TooManyVariables);
bring_in(Solver::SystemOfEquations::Error, EquationUnhandled);

// Custom assertions

void assert_solves_to(std::initializer_list<const char*> equations,
                      std::initializer_list<const char*> solutions,
                      const Poincare::Context& context,
                      double approxThreshold = 0.);
void assert_solves_numerically_to(const char* equation, double min, double max,
                                  std::initializer_list<double> solutions,
                                  const Poincare::Context& context,
                                  const char* variable = "x");
void assert_solves_to_error(std::initializer_list<const char*> equations,
                            Solver::SystemOfEquations::Error error,
                            const Poincare::Context& context);
void assert_solves_to_infinite_solutions(
    std::initializer_list<const char*> equations,
    std::initializer_list<const char*> solutions,
    const Poincare::Context& context);

// Auto solving range

void assert_solves_with_auto_solving_range(
    const char* equation, std::initializer_list<double> solutions,
    const Poincare::Context& context);
void assert_auto_solving_range_is(const char* equation, double min, double max,
                                  const Poincare::Context& context);

// Shorthands
inline void assert_solves_to_no_solution(const char* equation,
                                         const Poincare::Context& context) {
  /* Note: Doesn't really work with quadratic equations that will always report
   * at least a delta value. */
  assert_solves_to({equation}, {}, context);
}
inline void assert_solves_to_no_solution(
    std::initializer_list<const char*> equations,
    const Poincare::Context& context) {
  assert_solves_to(equations, {}, context);
}
inline void assert_solves_to_error(const char* equation,
                                   Solver::SystemOfEquations::Error error,
                                   const Poincare::Context& context) {
  assert_solves_to_error({equation}, error, context);
}
inline void assert_solves_to_infinite_solutions(
    const char* equation, const Poincare::Context& context) {
  assert_solves_to_infinite_solutions({equation}, {}, context);
}
inline void assert_solves_to(const char* equation, const char* solution,
                             const Poincare::Context& context) {
  assert_solves_to({equation}, {solution}, context);
}
inline void assert_solves_to(const char* equation,
                             std::initializer_list<const char*> solutions,
                             const Poincare::Context& context) {
  assert_solves_to({equation}, solutions, context);
}

// Helpers

void setComplexFormatAndAngleUnit(Poincare::ComplexFormat complexFormat,
                                  Poincare::AngleUnit angleUnit);

#endif
