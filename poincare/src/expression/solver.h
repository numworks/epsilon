#ifndef POINCARE_EXPRESSION_SOLVER_H
#define POINCARE_EXPRESSION_SOLVER_H

#include <poincare/src/memory/tree.h>

#include "projection.h"
#include "symbol.h"

namespace Poincare::Internal {

/* Solver methods are a direct (and incomplete for now) adaptation of methods in
 * apps/solver/system_of_equations.cpp. */

class Solver {
 public:
  struct Context {
    // If true, defined userVariables are ignored.
    bool overrideUserVariables = false;
    bool exactResults = true;
    // Context used for apps/solver compatibility
    int numberOfUserVariables = 0;
    char userVariables[6][Symbol::k_maxNameSize];
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

  // Return list of exact solutions.
  static Tree* ExactSolve(const Tree* equationsSet, Context* context,
                          ProjectionContext projectionContext, Error* error);

 private:
  // Return list of exact solutions.
  static Tree* PrivateExactSolve(const Tree* equationsSet, Context* context,
                                 ProjectionContext projectionContext,
                                 Error* error);
  static void ProjectAndSimplify(Tree* equationsSet,
                                 ProjectionContext projectionContext,
                                 Error* error);
  // Return list of solutions for linear system.
  static Tree* SolveLinearSystem(const Tree* equationsSet,
                                 uint8_t numberOfVariables, Context context,
                                 Error* error);
  // Return list of solutions for a polynomial equation.
  static Tree* SolvePolynomial(const Tree* equationsSet,
                               uint8_t numberOfVariables, Context context,
                               Error* error) {
    // TODO: Implement
    *error = Error::EquationUndefined;
    return nullptr;
  }
  // Return list of linear coefficients for each variables and final constant.
  static Tree* GetLinearCoefficients(const Tree* equation,
                                     uint8_t numberOfVariables,
                                     Context context);
  // Prepare a solution before display
  static Error RegisterSolution(Tree* solution, uint8_t variableId,
                                Context context);
};

}  // namespace Poincare::Internal

#endif
