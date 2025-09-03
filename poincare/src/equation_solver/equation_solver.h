#pragma once

#include <poincare/equation_solver/equation_solver.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal::EquationSolver {
using namespace Poincare::EquationSolver;

struct SolverResult {
  TreeRef exactSolutionList;
  TreeRef approximateSolutionList;
  Error error = Error::NoError;
  // Stores metadata about the equations and their unknowns.
  EquationMetadata equationMetadata;
  // Stores metadata about the solutions and how they were computed.
  SolutionMetadata solutionMetadata;
};

// Return list of exact solutions.
SolverResult ExactSolve(const Tree* equationList,
                        ProjectionContext projectionContext,
                        bool overrideDefinedVariables = false);

/* Will try to exact solve with overrideDefinedVariables = false
 * but will fallback on overrideDefinedVariables = true if it fails */
SolverResult ExactSolveAdaptive(const Tree* equationList,
                                ProjectionContext projectionContext);

/* If the range is (NaN, NaN), it will be automatically computed. */
SolverResult ApproximateSolve(const Tree* equationList,
                              ProjectionContext projectionContext,
                              Range1D<double> range,
                              size_t maxNumberOfSolutions);

}  // namespace Poincare::Internal::EquationSolver
