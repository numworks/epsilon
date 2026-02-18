#include <poincare/equation_solver/equation_solver_pool.h>
#include <poincare/src/equation_solver/equation_solver.h>
#include <poincare/src/expression/projection.h>
#include <poincare/user_expression.h>

namespace Poincare::EquationSolver {

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

SolverResult ExactSolveAdaptive(
    const UserExpression& equationList,
    const SimplificationParameters& simplificationParameters) {
  Internal::EquationSolver::SolverResult result =
      Internal::EquationSolver::ExactSolveAdaptive(equationList.tree(),
                                                   simplificationParameters);
  return ConvertInternalResult(result);
}

SolverResult ExactSolve(
    const UserExpression& equationList,
    const SimplificationParameters& simplificationParameters,
    bool overrideDefinedVariables) {
  Internal::EquationSolver::SolverResult result =
      Internal::EquationSolver::ExactSolve(equationList.tree(),
                                           simplificationParameters,
                                           overrideDefinedVariables);
  return ConvertInternalResult(result);
}

SolverResult ApproximateSolve(
    const UserExpression& equationList,
    const SimplificationParameters& simplificationParameters,
    Range1D<double> range, size_t maxNumberOfSolutions) {
  Internal::EquationSolver::SolverResult result =
      Internal::EquationSolver::ApproximateSolve(equationList.tree(),
                                                 simplificationParameters,
                                                 range, maxNumberOfSolutions);
  return ConvertInternalResult(result);
}

}  // namespace Poincare::EquationSolver
