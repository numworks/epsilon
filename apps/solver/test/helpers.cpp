#include "helpers.h"

#include <apps/global_preferences.h>
#include <apps/shared/global_context.h>
#include <assert.h>
#include <limits.h>
#include <poincare/store.h>
#include <quiz.h>
#include <string.h>

#include <cmath>

#include "../equation_store.h"
#include "../solver_context.h"

using namespace Solver;
using namespace Poincare;

// Private sub-helpers

template <typename T>
void solve_and_process_error(std::initializer_list<const char *> equations,
                             T &&lambda) {
  Shared::GlobalContext globalContext;
  SolverContext solverContext(&globalContext);
  EquationStore equationStore;
  for (const char *equation : equations) {
    Ion::Storage::Record::ErrorStatus err = equationStore.addEmptyModel();
    quiz_assert_print_if_failure(err == Ion::Storage::Record::ErrorStatus::None,
                                 equation);
    Ion::Storage::Record record =
        equationStore.recordAtIndex(equationStore.numberOfModels() - 1);
    Shared::ExpiringPointer<Equation> model =
        equationStore.modelForRecord(record);
    err = model->setContent(equation, &solverContext);
    quiz_assert_print_if_failure(err == Ion::Storage::Record::ErrorStatus::None,
                                 equation);
  }
  equationStore.tidyDownstreamPoolFrom();
  EquationStore::Error err = equationStore.exactSolve(&solverContext);
  lambda(&equationStore, err);
  equationStore.removeAll();
}

template <typename T>
void solve_and(std::initializer_list<const char *> equations, T &&lambda) {
  solve_and_process_error(
      equations, [lambda](EquationStore *store, EquationStore::Error error) {
        quiz_assert(error == NoError);
        lambda(store);
      });
}

// Helpers

void assert_solves_to_error(std::initializer_list<const char *> equations,
                            EquationStore::Error error) {
  solve_and_process_error(
      equations, [error](EquationStore *store, EquationStore::Error e) {
        quiz_assert(e == error);
      });
}

void assert_solves_to_infinite_solutions(
    std::initializer_list<const char *> equations) {
  solve_and(equations, [](EquationStore *store) {
    quiz_assert(store->type() == EquationStore::Type::LinearSystem &&
                store->hasMoreSolutions());
  });
}

void assert_solves_to(std::initializer_list<const char *> equations,
                      std::initializer_list<const char *> solutions) {
  solve_and(equations, [solutions](EquationStore *store) {
    Shared::GlobalContext globalContext;
    SolverContext solverContext(&globalContext);
    int i = 0;
    for (const char *solution : solutions) {
      // Solutions are specified under the form "foo=bar"
      constexpr int maxSolutionLength = 100;
      char editableSolution[maxSolutionLength];
      strlcpy(editableSolution, solution, maxSolutionLength);

      char *equal = strchr(editableSolution, '=');
      quiz_assert(equal != nullptr);
      *equal = 0;

      const char *expectedVariable = editableSolution;
      if (store->type() != EquationStore::Type::PolynomialMonovariable) {
        /* For some reason the EquationStore returns up to 3 results but always
         * just one variable, so we don't check variable name...
         * TODO: Change this poor behavior. */
        const char *obtainedVariable = store->variable(i);
        quiz_assert(strcmp(obtainedVariable, expectedVariable) == 0);
      }

      /* Now for the ugly part!
       * At the moment, the EquationStore doesn't let us retrieve solutions as
       * Expression. We can only get Layout. It somewhat makes sense for how it
       * is used in the app, but it's a nightmare to test, so changing this
       * behavior is a TODO. */

      const char *expectedValue = equal + 1;

      /* We compare Expressions, by parsing the expected Expression and
       * serializing and parsing the obtained layout. We need to ignore the
       * parentheses during the comparison, because to create an expression from
       * a const char * we need to add parentheses that are not necessary when
       * creating an expression from a layout. */

      Expression expectedExpression =
          Expression::Parse(expectedValue, &solverContext, false);
      quiz_assert(!expectedExpression.isUninitialized());

      Layout obtainedLayout = store->solution(i)->exactLayout();
      constexpr int bufferSize = 200;
      char obtainedLayoutBuffer[bufferSize];
      obtainedLayout.serializeForParsing(obtainedLayoutBuffer, bufferSize);
      Expression obtainedExpression =
          Expression::Parse(obtainedLayoutBuffer, &solverContext, false);
      quiz_assert(expectedExpression.isIdenticalToWithoutParentheses(
          obtainedExpression));

      i++;
    }
    quiz_assert(store->numberOfSolutions() == i);
  });
}

void assert_solves_numerically_to(const char *equation, double min, double max,
                                  std::initializer_list<double> solutions,
                                  const char *variable) {
  solve_and_process_error({equation}, [min, max, solutions, variable](
                                          EquationStore *store,
                                          EquationStore::Error e) {
    Shared::GlobalContext globalContext;
    SolverContext solverContext(&globalContext);
    quiz_assert(e == RequireApproximateSolution);
    store->setApproximateResolutionMinimum(min);
    store->setApproximateResolutionMaximum(max);
    store->approximateSolve(&solverContext);

    quiz_assert(strcmp(store->variable(0), variable) == 0);
    int i = 0;
    for (double solution : solutions) {
      assert_roughly_equal(store->solution(i++)->approximate(), solution, 1E-5);
    }
    quiz_assert(store->numberOfSolutions() == i);
  });
}

void set_complex_format(Preferences::ComplexFormat format) {
  Preferences::sharedPreferences->setComplexFormat(format);
}
void reset_complex_format() {
  Preferences defaultPreferences;
  Preferences::sharedPreferences->setComplexFormat(
      defaultPreferences.complexFormat());
}

void set(const char *variable, const char *value) {
  const char *assign = "→";

  char buffer[32];
  assert(strlen(value) + strlen(assign) + strlen(variable) < sizeof(buffer));

  buffer[0] = 0;
  strlcat(buffer, value, sizeof(buffer));
  strlcat(buffer, assign, sizeof(buffer));
  strlcat(buffer, variable, sizeof(buffer));

  Shared::GlobalContext globalContext;
  SolverContext solverContext(&globalContext);
  Expression e = Expression::ParseAndSimplify(
      buffer, &solverContext, Preferences::sharedPreferences->complexFormat(),
      Preferences::sharedPreferences->angleUnit(),
      GlobalPreferences::sharedGlobalPreferences->unitFormat());
  static_cast<Store &>(e).storeValueForSymbol(&globalContext);
}

void unset(const char *variable) {
  // The variable is either an expression or a function
  Ion::Storage::FileSystem::sharedFileSystem
      ->recordBaseNamedWithExtension(variable, "exp")
      .destroy();
  Ion::Storage::FileSystem::sharedFileSystem
      ->recordBaseNamedWithExtension(variable, "func")
      .destroy();
}
