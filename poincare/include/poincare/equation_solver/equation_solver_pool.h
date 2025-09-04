#pragma once

#include <poincare/user_expression.h>

#include "equation_solver.h"

namespace Poincare {

namespace EquationSolver {

// Similar to Poincare::Internal::EquationSolver::SolverResult
struct SolverResult {
  // List of exact solutions, or nullptr if no exact solution
  UserExpression exactSolutionList;
  // List of approximate solutions, or nullptr if no approximate solution
  UserExpression approximateSolutionList;
  Error error;
  SolutionMetadata solutionMetadata;
  EquationMetadata equationMetadata;
};

/* Will try to exact solve with overrideDefinedVariables = false
 * but will fallback on overrideDefinedVariables = true if it fails.
 * See Poincare::Internal::EquationSolver::ExactSolveAdaptive */
SolverResult ExactSolveAdaptive(const UserExpression& equationList,
                                ProjectionContext projectionContext);

// See Poincare::Internal::EquationSolver::ExactSolve
SolverResult ExactSolve(const UserExpression& equationList,
                        ProjectionContext projectionContext,
                        bool overrideDefinedVariables = false);

/* If the range is (NaN, NaN), it will be automatically computed.
 * See Poincare::Internal::EquationSolver::ApproximateSolve */
SolverResult ApproximateSolve(const UserExpression& equationList,
                              ProjectionContext projectionContext,
                              Range1D<double> range,
                              size_t maxNumberOfSolutions);

}  // namespace EquationSolver
}  // namespace Poincare
