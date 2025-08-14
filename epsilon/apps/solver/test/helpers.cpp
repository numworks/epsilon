#include "helpers.h"

#include <apps/global_preferences.h>
#include <assert.h>
#include <limits.h>
#include <poincare/helpers/store.h>
#include <poincare/test/float_helper.h>
#include <poincare/test/helper.h>
#include <quiz.h>
#include <string.h>

#include <cmath>

#include "../equation_store.h"

using namespace Solver;
using namespace Poincare;

// Private sub-helpers

template <typename T>
void solve_and_process_error(std::initializer_list<const char*> equations,
                             const Context& context, T&& lambda) {
  EquationStore equationStore;
  SystemOfEquations system(&equationStore);
  for (const char* equation : equations) {
    Ion::Storage::Record::ErrorStatus err = equationStore.addEmptyModel();
    quiz_assert_print_if_failure(err == Ion::Storage::Record::ErrorStatus::None,
                                 equation);
    Ion::Storage::Record record =
        equationStore.recordAtIndex(equationStore.numberOfModels() - 1);
    OMG::ExpiringPointer<Equation> model = equationStore.modelForRecord(record);
    err = model->setContent(Layout::Parse(equation), context);
    quiz_assert_print_if_failure(err == Ion::Storage::Record::ErrorStatus::None,
                                 equation);
  }
  equationStore.tidyDownstreamPoolFrom();
  system.tidy();
  SystemOfEquations::Error err = system.exactSolve(context);
  lambda(&system, err);
  equationStore.removeAll();
}

template <typename T>
void solve_and(std::initializer_list<const char*> equations,
               const Context& context, T&& lambda) {
  solve_and_process_error(
      equations, context,
      [lambda](SystemOfEquations* system, SystemOfEquations::Error error) {
        quiz_assert(error == NoError);
        lambda(system);
      });
}

void assert_solves_with_range_to(const char* equation, double min, double max,
                                 std::initializer_list<double> solutions,
                                 const Context& context,
                                 const char* variable = nullptr) {
  solve_and_process_error(
      {equation}, context,
      [min, max, solutions, variable, &context](SystemOfEquations* system,
                                                SystemOfEquations::Error e) {
        quiz_assert(e == RequireApproximateSolution);
        assert(std::isnan(min) == std::isnan(max));
        if (std::isnan(min)) {
          system->useAutoSolvingRange(true);
        } else {
          system->setApproximateSolvingRange(Range1D(min, max));
        }
        system->approximateSolve(context);
        if (variable) {
          quiz_assert(strcmp(system->unknownVariable(0), variable) == 0);
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
                            const Context& context) {
  solve_and_process_error(
      equations, context,
      [error](SystemOfEquations* system, SystemOfEquations::Error e) {
        quiz_assert(e == error);
      });
}

static void compareSolutions(SystemOfEquations* system,
                             std::initializer_list<const char*> solutions,
                             const Context& context,
                             double approximationThreshold = 0.) {
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
    if (system->solvingMethod() !=
        SystemOfEquations::SolvingMethod::PolynomialMonovariable) {
      /* For some reason the EquationStore returns up to 3 results but always
       * just one variable, so we don't check variable name...
       * TODO: Change this poor behavior. */
      const char* obtainedVariable = system->unknownVariable(i);
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
    Internal::ProjectionContext projCtx{
        .m_complexFormat = Internal::ComplexFormat::Cartesian,
        .m_symbolic = Internal::SymbolicComputation::ReplaceDefinedSymbols,
        .m_context = context,
        .m_advanceReduce = false};
    SystemExpression expectedExpression =
        UserExpression::Parse(expectedValue, context,
                              {.forceUnitUnderscore = true})
            .cloneAndReduce(projCtx, &reductionFailure);
    quiz_assert(!reductionFailure && !expectedExpression.isUninitialized());

    Layout obtainedLayout = system->solution(i)->exactLayout();
    if (obtainedLayout.isUninitialized()) {
      obtainedLayout = system->solution(i)->approximateLayout();
    }
    constexpr int bufferSize = 500;
    char obtainedLayoutBuffer[bufferSize];
    obtainedLayout.serialize(obtainedLayoutBuffer);
    UserExpression parsedExpression = UserExpression::Parse(
        obtainedLayoutBuffer, context, {.forceUnitUnderscore = true});
    quiz_assert(!parsedExpression.isUninitialized());
    /* TODO: no need to recreate a projectionContext when const and non const
     * members of ProjectionContext are split (parameters vs metadata) */
    Internal::ProjectionContext projCtx2 = {
        .m_complexFormat = Internal::ComplexFormat::Cartesian,
        .m_symbolic = Internal::SymbolicComputation::ReplaceDefinedSymbols,
        .m_context = context,
        .m_advanceReduce = false};
    SystemExpression obtainedExpression =
        parsedExpression.cloneAndReduce(projCtx2, &reductionFailure);
    quiz_assert(!reductionFailure && !obtainedExpression.isUninitialized());
#if 0
    quiz_assert(
        !expectedExpression.isUninitialized() &&
        expectedExpression.isIdenticalToWithoutParentheses(obtainedExpression));
#else
    bool result = (!expectedExpression.isUninitialized() &&
                   expectedExpression.isIdenticalTo(obtainedExpression));
    if (!result && approximationThreshold != 0.) {
      double expectedApproxed =
          expectedExpression.approximateSystemToRealScalar<double>();
      double obtainedApproxed =
          obtainedExpression.approximateSystemToRealScalar<double>();
      result = roughly_equal(obtainedApproxed, expectedApproxed,
                             approximationThreshold);
#if POINCARE_TREE_LOG
      if (!result) {
        std::cout << "\tWrong Solution approxed: " << obtainedApproxed
                  << "\n\tInstead of approxed: " << expectedApproxed
                  << std::endl;
      }
#endif
    }
#if POINCARE_TREE_LOG
    else if (!result) {
      std::cout << "\tWrong Solution: ";
      obtainedExpression.tree()->logSerialize();
      std::cout << "\tInstead of: ";
      expectedExpression.tree()->logSerialize();
    }
#endif
    quiz_assert(result);
#endif

    i++;
  }
#if POINCARE_TREE_LOG
  if (system->numberOfSolutions() != i) {
    std::cout << "\tWrong number of solution: expected " << i << " got "
              << system->numberOfSolutions() << std::endl;
  }
#endif
  quiz_assert(system->numberOfSolutions() == i);
}

void assert_solves_to_infinite_solutions(
    std::initializer_list<const char*> equations,
    std::initializer_list<const char*> solutions, const Context& context) {
  solve_and(equations, context,
            [solutions, &context](SystemOfEquations* system) {
              quiz_assert(system->solvingMethod() ==
                              SystemOfEquations::SolvingMethod::LinearSystem &&
                          system->solutionType() ==
                              SystemOfEquations::SolutionType::Formal);
              compareSolutions(system, solutions, context);
            });
}

void assert_solves_to(std::initializer_list<const char*> equations,
                      std::initializer_list<const char*> solutions,
                      const Context& context, double appproxThreshold) {
  solve_and(equations, context,
            [solutions, &context, appproxThreshold](SystemOfEquations* system) {
              compareSolutions(system, solutions, context, appproxThreshold);
            });
}

void assert_solves_numerically_to(const char* equation, double min, double max,
                                  std::initializer_list<double> solutions,
                                  const Context& context,
                                  const char* variable) {
  assert(!std::isnan(min) && !std::isnan(max));
  return assert_solves_with_range_to(equation, min, max, solutions, context,
                                     variable);
}

void assert_solves_with_auto_solving_range(
    const char* equation, std::initializer_list<double> solutions,
    const Context& context) {
  return assert_solves_with_range_to(equation, NAN, NAN, solutions, context);
}

void assert_auto_solving_range_is(const char* equation, double min, double max,
                                  const Context& context) {
  solve_and_process_error(
      {equation}, context,
      [min, max, &context](SystemOfEquations* system,
                           SystemOfEquations::Error e) {
        quiz_assert(e == RequireApproximateSolution);
        system->useAutoSolvingRange(true);
        system->approximateSolve(context);
        Range1D<double> solvingRange = system->approximateSolvingRange();
        quiz_assert(solvingRange.min() == min && solvingRange.max() == max);
      });
}

void setComplexFormatAndAngleUnit(Preferences::ComplexFormat complexFormat,
                                  Preferences::AngleUnit angleUnit) {
  GlobalPreferences::SharedGlobalPreferences()->setComplexFormat(complexFormat);
  GlobalPreferences::SharedGlobalPreferences()->setAngleUnit(angleUnit);
}
