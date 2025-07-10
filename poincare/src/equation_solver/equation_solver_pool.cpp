#include <poincare/equation_solver/equation_solver_pool.h>
#include <poincare/expression.h>
#include <poincare/src/equation_solver/equation_solver_tree.h>

namespace Poincare {

namespace EquationSolver {

// This will destroy the trees of solutions on the stack
static SolverResult ConvertInternalResult(
    Internal::EquationSolver::SolverResult& internalResult) {
  SolverResult solverResult;

  Internal::Tree* exactSolutionList = internalResult.exactSolutionList;
  Internal::Tree* approximateSolutionList =
      internalResult.approximateSolutionList;

  if (approximateSolutionList) {
    // Do approximate first as its tree is always after the exact one
    solverResult.approximateSolutionList =
        UserExpression::Builder(approximateSolutionList);
  }
  if (exactSolutionList) {
    solverResult.exactSolutionList = UserExpression::Builder(exactSolutionList);
  }

  solverResult.error = internalResult.error;
  solverResult.solutionMetadata = internalResult.solutionMetadata;
  solverResult.equationMetadata = internalResult.equationMetadata;
  return solverResult;
}

SolverResult ExactSolveAdaptive(const UserExpression& equationList,
                                Internal::ProjectionContext projectionContext) {
  Internal::EquationSolver::SolverResult result =
      Internal::EquationSolver::ExactSolveAdaptive(equationList.tree(),
                                                   projectionContext);
  return ConvertInternalResult(result);
}

SolverResult ExactSolve(const UserExpression& equationList,
                        Internal::ProjectionContext projectionContext,
                        bool overrideDefinedVariables) {
  Internal::EquationSolver::SolverResult result =
      Internal::EquationSolver::ExactSolve(
          equationList.tree(), projectionContext, overrideDefinedVariables);
  return ConvertInternalResult(result);
}

SolverResult ApproximateSolve(const UserExpression& equationList,
                              Internal::ProjectionContext projectionContext,
                              Range1D<double> range,
                              size_t maxNumberOfSolutions) {
  Internal::EquationSolver::SolverResult result =
      Internal::EquationSolver::ApproximateSolve(
          equationList.tree(), projectionContext, range, maxNumberOfSolutions);
  return ConvertInternalResult(result);
}

}  // namespace EquationSolver

}  // namespace Poincare
