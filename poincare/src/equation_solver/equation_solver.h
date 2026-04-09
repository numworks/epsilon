#pragma once

#include <poincare/equation_solver/equation_solver.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal::EquationSolver {
using namespace Poincare::EquationSolver;

struct PreprocessingResult {
  Internal::Tree* reducedEquationList = nullptr;
  Error error = Error::NoError;
  EquationMetadata equationMetadata;
  Internal::Dimension dimension = Internal::Dimension();
};

enum class UnknownSelectionStrategy : uint8_t {
  // Only includes symbols that are not defined by the user
  OnlyUndefinedSymbols,
  // Includes all symbols, even ones defined by the user
  AllSymbols,
  /* Choses whether or not to override defined variables in order to have only
   * one unknown at the end */
  MaxOneSymbol,
};

PreprocessingResult PreprocessEquationList(
    const Tree* equationList,
    const SimplificationParameters& simplificationParameters,
    UnknownSelectionStrategy selectionStrategy);

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
SolverResult ExactSolve(
    const Tree* equationList,
    const SimplificationParameters& simplificationParameters,
    bool overrideDefinedVariables = false);

SolverResult ExactSolvePreprocessed(PreprocessingResult,
                                    const SimplificationParameters&);

/* Will try to exact solve with overrideDefinedVariables = false
 * but will fallback on overrideDefinedVariables = true if it fails */
SolverResult ExactSolveAdaptive(
    const Tree* equationList,
    const SimplificationParameters& simplificationParameters);

/* If the range is (NaN, NaN), it will be automatically computed. */
SolverResult ApproximateSolve(
    const Tree* equationList,
    const SimplificationParameters& simplificationParameters,
    Range1D<double> range, size_t maxNumberOfSolutions);

struct SingularitySolverResult {
  TreeRef exactSolutionList;
  Error error = Error::NoError;
};

/* Try to find exact singularity points of a scalar single-variable function.
 * Returns a list of singularities (empty if none was found)
 * Given a function f(x), singularities are values of x at which f(x) is not
 * properly defined, because its "value" would be +∞ or -∞. This method aims at
 * exact singularities, so it only works on a very restricted subset of
 * functions.
 * Examples of functions that have detectable exact singularities:
 * f(x)=1/(x^2+3x+2) in x=-1 and x=-2
 * f(x)=ln(abs(x^2)) in x=0
 * Parameter format: the "function" tree should be a non-list scalar function.
 * It can be either a UserExpression or a SystemExpression. It should have a
 * single variable.
 * WARNING: The variable must already be projected to Var0!
 * The main use case is to detect integrand singularities inside the integration
 * bounds, to split the integral at the singularity points (because we have
 * techniques to integrate functions with singularities at the integration
 * bounds). */
SingularitySolverResult ExactSolveFindSingularities(
    const Tree* function, const UserParameters& userParameters);

}  // namespace Poincare::Internal::EquationSolver
