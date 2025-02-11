#include "helpers.h"

#include <apps/global_preferences.h>
#include <apps/shared/global_context.h>
#include <assert.h>
#include <limits.h>
#include <poincare/helpers/store.h>
#include <poincare/test/helper.h>
#include <quiz.h>
#include <string.h>

#include <cmath>

#include "../equation_store.h"
#include "../solver_context.h"

using namespace Solver;
using namespace Poincare;

// Private sub-helpers

template <typename T>
void solve_and_process_error(std::initializer_list<const char*> equations,
                             SolverContext* solverContext, T&& lambda) {
  EquationStore equationStore;
  SystemOfEquations system(&equationStore);
  for (const char* equation : equations) {
    Ion::Storage::Record::ErrorStatus err = equationStore.addEmptyModel();
    quiz_assert_print_if_failure(err == Ion::Storage::Record::ErrorStatus::None,
                                 equation);
    Ion::Storage::Record record =
        equationStore.recordAtIndex(equationStore.numberOfModels() - 1);
    Shared::ExpiringPointer<Equation> model =
        equationStore.modelForRecord(record);
    err = model->setContent(Layout::Parse(equation), solverContext);
    quiz_assert_print_if_failure(err == Ion::Storage::Record::ErrorStatus::None,
                                 equation);
  }
  equationStore.tidyDownstreamPoolFrom();
  system.tidy();
  SystemOfEquations::Error err = system.exactSolve(solverContext);
  lambda(&system, err);
  equationStore.removeAll();
}

template <typename T>
void solve_and(std::initializer_list<const char*> equations,
               SolverContext* solverContext, T&& lambda) {
  solve_and_process_error(
      equations, solverContext,
      [lambda](SystemOfEquations* system, SystemOfEquations::Error error) {
        quiz_assert(error == NoError);
        lambda(system);
      });
}

void assert_solves_with_range_to(const char* equation, double min, double max,
                                 std::initializer_list<double> solutions,
                                 Shared::GlobalContext* globalContext,
                                 const char* variable = nullptr) {
  SolverContext solverContext(globalContext);
  solve_and_process_error(
      {equation}, &solverContext,
      [min, max, solutions, variable, &solverContext](
          SystemOfEquations* system, SystemOfEquations::Error e) {
        quiz_assert(e == RequireApproximateSolution);
        assert(std::isnan(min) == std::isnan(max));
        if (std::isnan(min)) {
          system->autoComputeApproximateSolvingRange(&solverContext);
        } else {
          system->setApproximateSolvingRange(Range1D(min, max));
        }
        system->approximateSolve(&solverContext);
        if (variable) {
          quiz_assert(strcmp(system->variable(0), variable) == 0);
        }
        size_t i = 0;
        for (double solution : solutions) {
          assert_roughly_equal(system->solution(i++)->approximate(), solution,
                               1E-5);
        }
        quiz_assert(system->numberOfSolutions() == i);
      });
}

// Helpers

void assert_solves_to_error(std::initializer_list<const char*> equations,
                            SystemOfEquations::Error error,
                            Shared::GlobalContext* globalContext) {
  SolverContext solverContext(globalContext);
  solve_and_process_error(
      equations, &solverContext,
      [error](SystemOfEquations* system, SystemOfEquations::Error e) {
        quiz_assert(e == error);
      });
}

static void compareSolutions(SystemOfEquations* system,
                             std::initializer_list<const char*> solutions,
                             SolverContext* solverContext) {
  /* TODO: this function needs to be reworked so that we can compare Expressions
   * directly, instead of parsing const char * objects and Layouts and comparing
   * afterwards. */

  size_t i = 0;
  for (const char* solution : solutions) {
    // Solutions are specified under the form "foo=bar"
    constexpr int maxSolutionLength = 100;
    char editableSolution[maxSolutionLength];
    strlcpy(editableSolution, solution, maxSolutionLength);

    char* equal = strchr(editableSolution, '=');
    quiz_assert(equal != nullptr);
    *equal = 0;

    const char* expectedVariable = editableSolution;
    if (system->type() != SystemOfEquations::Type::PolynomialMonovariable) {
      /* For some reason the EquationStore returns up to 3 results but always
       * just one variable, so we don't check variable name...
       * TODO: Change this poor behavior. */
      const char* obtainedVariable = system->variable(i);
      quiz_assert(strcmp(obtainedVariable, expectedVariable) == 0);
    }

    /* Now for the ugly part!
     * At the moment, the EquationStore doesn't let us retrieve solutions as
     * Expression. We can only get Layout. It somewhat makes sense for how it
     * is used in the app, but it's a nightmare to test, so changing this
     * behavior is a TODO. */

    const char* expectedValue = equal + 1;

    /* We compare Expressions, by parsing the expected Expression and
     * serializing and parsing the obtained layout. We need to ignore the
     * parentheses during the comparison, because to create an expression from
     * a const char * we need to add parentheses that are not necessary when
     * creating an expression from a layout. */

    bool reductionFailure = false;
    Expression expectedExpression =
        Expression::Parse(expectedValue, solverContext, false)
            .cloneAndReduce(ReductionContext{}, &reductionFailure);
    quiz_assert(!reductionFailure && !expectedExpression.isUninitialized());

    Layout obtainedLayout = system->solution(i)->exactLayout();
    if (obtainedLayout.isUninitialized()) {
      obtainedLayout = system->solution(i)->approximateLayout();
    }
    constexpr int bufferSize = 500;
    char obtainedLayoutBuffer[bufferSize];
    obtainedLayout.serialize(obtainedLayoutBuffer, bufferSize);
    Expression parsedExpression =
        Expression::Parse(obtainedLayoutBuffer, solverContext, false);
    quiz_assert(!parsedExpression.isUninitialized());
    Expression obtainedExpression =
        parsedExpression.cloneAndReduce(ReductionContext{}, &reductionFailure);
    quiz_assert(!reductionFailure && !obtainedExpression.isUninitialized());
#if 0
    quiz_assert(
        !expectedExpression.isUninitialized() &&
        expectedExpression.isIdenticalToWithoutParentheses(obtainedExpression));
#else
    quiz_assert(!expectedExpression.isUninitialized() &&
                expectedExpression.isIdenticalTo(obtainedExpression));
#endif

    i++;
  }
  quiz_assert(system->numberOfSolutions() == i);
}

void assert_solves_to_infinite_solutions(
    std::initializer_list<const char*> equations,
    std::initializer_list<const char*> solutions,
    Shared::GlobalContext* globalContext) {
  SolverContext solverContext(globalContext);
  solve_and(
      equations, &solverContext,
      [solutions, &solverContext](SystemOfEquations* system) {
        quiz_assert(system->type() == SystemOfEquations::Type::LinearSystem &&
                    system->solutionStatus() ==
                        SystemOfEquations::SolutionStatus::Complete);
        compareSolutions(system, solutions, &solverContext);
      });
}

void assert_solves_to(std::initializer_list<const char*> equations,
                      std::initializer_list<const char*> solutions,
                      Shared::GlobalContext* globalContext) {
  SolverContext solverContext(globalContext);
  solve_and(equations, &solverContext,
            [solutions, &solverContext](SystemOfEquations* system) {
              compareSolutions(system, solutions, &solverContext);
            });
}

void assert_solves_numerically_to(const char* equation, double min, double max,
                                  std::initializer_list<double> solutions,
                                  Shared::GlobalContext* globalContext,
                                  const char* variable) {
  assert(!std::isnan(min) && !std::isnan(max));
  return assert_solves_with_range_to(equation, min, max, solutions,
                                     globalContext, variable);
}

void assert_solves_with_auto_solving_range(
    const char* equation, std::initializer_list<double> solutions,
    Shared::GlobalContext* globalContext) {
  return assert_solves_with_range_to(equation, NAN, NAN, solutions,
                                     globalContext);
}

void assert_auto_solving_range_is(const char* equation, double min, double max,
                                  Shared::GlobalContext* globalContext) {
  SolverContext solverContext(globalContext);
  solve_and_process_error(
      {equation}, &solverContext,
      [min, max, &solverContext](SystemOfEquations* system,
                                 SystemOfEquations::Error e) {
        quiz_assert(e == RequireApproximateSolution);
        system->autoComputeApproximateSolvingRange(&solverContext);
        Range1D<double> solvingRange = system->approximateSolvingRange();
        quiz_assert(solvingRange.min() == min && solvingRange.max() == max);
      });
}

void setComplexFormatAndAngleUnit(Preferences::ComplexFormat complexFormat,
                                  Preferences::AngleUnit angleUnit) {
  Preferences::SharedPreferences()->setComplexFormat(complexFormat);
  Preferences::SharedPreferences()->setAngleUnit(angleUnit);
}
