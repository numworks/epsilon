#ifndef POINCARE_EXPRESSION_EQUATION_SOLVER_H
#define POINCARE_EXPRESSION_EQUATION_SOLVER_H

#include <poincare/equation_solver/equation_solver_properties.h>

namespace Poincare::Internal {

class EquationSolver {
 public:
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
  static SolverResult ExactSolve(const Tree* equationList,
                                 ProjectionContext projectionContext,
                                 bool overrideDefinedVariables = false);

  /* Will try to exact solve with overrideDefinedVariables = false
   * but will fallback on overrideDefinedVariables = true if it fails */
  static SolverResult ExactSolveAdaptive(const Tree* equationList,
                                         ProjectionContext projectionContext);

  /* If the range is (NaN, NaN), it will be automatically computed. */
  static SolverResult ApproximateSolve(const Tree* equationList,
                                       ProjectionContext projectionContext,
                                       Range1D<double> range,
                                       size_t maxNumberOfSolutions);

 private:
  enum class UnknownSelectionStrategy : uint8_t {
    // Only includes symbols that are not defined by the user
    OnlyUndefinedSymbols,
    // Includes all symbols, even ones defined by the user
    AllSymbols,
    /* Choses whether or not to override defined variables in order to have only
     * one unknown at the end */
    MaxOneSymbol,
  };

  struct PreprocessingResult {
    Tree* reducedEquationList = nullptr;
    Error error = Error::NoError;
    EquationMetadata equationMetadata;
  };

  /* This is used by both ExactSolve and ApproximateSolve.
   * It computes the EquationMetadata, and reduces the equation list. */
  static PreprocessingResult PreprocessEquationList(
      const Tree* equationList, ProjectionContext* projectionContext,
      UnknownSelectionStrategy selectionStrategy);

  // Return list of solutions for linear system.
  static SolverResult SolveLinearSystem(
      const Tree* equationList, const EquationMetadata& equationMetadata,
      Context* context);
  // Return list of solutions for a polynomial equation.
  static SolverResult SolvePolynomial(const Tree* equationList,
                                      const EquationMetadata& equationMetadat);

  // Return list of linear coefficients for each variables and final constant.
  static Tree* GetLinearCoefficients(const Tree* equation,
                                     uint8_t numberOfVariables);

  constexpr static char k_parameterPrefix = 't';
  static uint32_t TagParametersUsedAsVariables(VariableArray variables);

  // Return the userSymbol for the next additional parameter variable.
  static Tree* GetNextParameterSymbol(size_t* parameterIndex,
                                      uint32_t usedParameterIndices,
                                      Poincare::Context* context);
};

}  // namespace Poincare::Internal

#endif
