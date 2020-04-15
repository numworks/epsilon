#ifndef APPS_SOLVER_TEST_HELPERS_H
#define APPS_SOLVER_TEST_HELPERS_H

#include <poincare/preferences.h>
#include <initializer_list>
#include "../equation_store.h"
#include <poincare/test/helper.h>

#define bring_in(prefix, value) static const prefix value = prefix::value;

bring_in(Solver::EquationStore::Error, EquationUnreal);
bring_in(Solver::EquationStore::Error, NoError);
bring_in(Solver::EquationStore::Error, NonLinearSystem);
bring_in(Solver::EquationStore::Error, RequireApproximateSolution);
bring_in(Solver::EquationStore::Error, TooManyVariables);

// Custom assertions

void assert_solves_to(std::initializer_list<const char *> equations, std::initializer_list<const char *> solutions);
void assert_solves_numerically_to(const char * equation, double min, double max, std::initializer_list<double> solutions, const char * variable = "x");
void assert_solves_to_error(const char * equation, Solver::EquationStore::Error error);
void assert_solves_to_infinite_solutions(std::initializer_list<const char *> equations);

// Shorthands
inline void assert_solves_to_no_solution(const char * equation) {
  /* Note: Doesn't really work with quadratic equations that will always report
   * at least a delta value. */
  assert_solves_to({equation}, {});
}
inline void assert_solves_to_infinite_solutions(const char * equation) {
  assert_solves_to_infinite_solutions({equation});
}
inline void assert_solves_to(const char * equation, const char * solution) {
  assert_solves_to({equation}, {solution});
}
inline void assert_solves_to(const char * equation, std::initializer_list<const char *> solutions) {
  assert_solves_to({equation}, solutions);
}


// Helpers

void set_complex_format(Poincare::Preferences::ComplexFormat format);
void reset_complex_format();

void set(const char * variable, const char * value);
void unset(const char * variable);

#endif
