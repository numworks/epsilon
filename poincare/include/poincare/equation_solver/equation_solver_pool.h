#pragma once

#include <poincare/user_expression.h>

#include "equation_solver_properties.h"

namespace Poincare {

namespace EquationSolver {

constexpr static size_t k_maxNumberOfVariables =
    EquationSolverProperties::k_maxNumberOfVariables;
constexpr static size_t k_maxNumberOfExactSolutions =
    EquationSolverProperties::k_maxNumberOfExactSolutions;

using VariableArray = EquationSolverProperties::VariableArray;
using Error = EquationSolverProperties::Error;
using SolvingMethod = EquationSolverProperties::SolvingMethod;
using SolutionType = EquationSolverProperties::SolutionType;

using EquationMetadata = EquationSolverProperties::EquationMetadata;
using SolutionMetadata = EquationSolverProperties::SolutionMetadata;

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
