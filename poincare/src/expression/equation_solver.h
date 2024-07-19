#ifndef POINCARE_EXPRESSION_EQUATION_SOLVER_H
#define POINCARE_EXPRESSION_EQUATION_SOLVER_H

#include <poincare/range.h>
#include <poincare/src/memory/tree.h>

#include "projection.h"
#include "symbol.h"

namespace Poincare::Internal {

/* Solver methods are a direct (and incomplete for now) adaptation of methods in
 * apps/solver/system_of_equations.cpp. */

class EquationSolver {
 public:
  enum class Type : uint8_t {
    LinearSystem,
    PolynomialMonovariable,
    GeneralMonovariable,
  };

  enum class Error {
    NoError = 0,
    EquationUndefined = 1,
    EquationNonreal = 2,
    TooManyVariables = 3,
    NonLinearSystem = 4,
    RequireApproximateSolution = 5,
    DisabledInExamMode,  // TODO rebased from poincare
  };

  struct Context {
    Type type;
    int8_t degree;
    // If true, defined userVariables are ignored.
    bool overrideUserVariables = false;
    bool exactResults = true;
    bool hasMoreSolutions = false;
    int numberOfVariables = 0;
    char variables[6][Symbol::k_maxNameSize];
    // Context used for apps/solver compatibility
    int numberOfUserVariables = 0;
    char userVariables[6][Symbol::k_maxNameSize];
  };

  constexpr static int k_maxNumberOfApproximateSolutions = 10;

  // Return list of exact solutions.
  static Tree* ExactSolve(const Tree* equationsSet, Context* context,
                          ProjectionContext projectionContext, Error* error);

  static Range1D<double> AutomaticInterval(const Tree* equationSet,
                                           Context* context);

  static Tree* ApproximateSolve(const Tree* equationsSet, Range1D<double> range,
                                Context* context);

 private:
  // Return list of exact solutions.
  static Tree* PrivateExactSolve(const Tree* equationsSet, Context* context,
                                 ProjectionContext projectionContext,
                                 Error* error);
  static void ProjectAndReduce(Tree* equationsSet,
                               ProjectionContext projectionContext,
                               Error* error);
  // Return list of solutions for linear system.
  static Tree* SolveLinearSystem(const Tree* equationsSet,
                                 uint8_t numberOfVariables, Context* context,
                                 Error* error);
  // Return list of solutions for a polynomial equation.
  static Tree* SolvePolynomial(const Tree* equationsSet,
                               uint8_t numberOfVariables, Context* context,
                               Error* error);

  // Return list of linear coefficients for each variables and final constant.
  static Tree* GetLinearCoefficients(const Tree* equation,
                                     uint8_t numberOfVariables,
                                     Context* context);
  // Prepare a solution before display
  static Error RegisterSolution(Tree* solution, uint8_t variableId,
                                Context* context);
};

}  // namespace Poincare::Internal

#endif
