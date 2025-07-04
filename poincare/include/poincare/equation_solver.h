#ifndef POINCARE_EQUATION_SOLVER_H
#define POINCARE_EQUATION_SOLVER_H

#include <poincare/expression.h>
#include <poincare/src/expression/equation_solver.h>
#include <poincare/src/expression/projection.h>

// This is the public interface for the EquationSolver.

namespace Poincare {

namespace EquationSolver {

// TODO: These should be in a separate header instead of exposing internal
using Error = Poincare::Internal::EquationSolver::Error;
using SolvingMethod = Poincare::Internal::EquationSolver::SolvingMethod;
using SolutionType = Poincare::Internal::EquationSolver::SolutionType;
using VariableArray = Poincare::Internal::EquationSolver::VariableArray;

using EquationMetadata = Poincare::Internal::EquationSolver::EquationMetadata;
using SolutionMetadata = Poincare::Internal::EquationSolver::SolutionMetadata;

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
                                Internal::ProjectionContext projectionContext);

// See Poincare::Internal::EquationSolver::ExactSolve
SolverResult ExactSolve(const UserExpression& equationList,
                        Internal::ProjectionContext projectionContext,
                        bool overrideDefinedVariables = false);

/* If the range is (NaN, NaN), it will be automatically computed.
 * See Poincare::Internal::EquationSolver::ApproximateSolve */
SolverResult ApproximateSolve(const UserExpression& equationList,
                              Internal::ProjectionContext projectionContext,
                              Range1D<double> range,
                              size_t maxNumberOfSolutions);

}  // namespace EquationSolver
}  // namespace Poincare

#endif
