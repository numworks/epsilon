#ifndef POINCARE_EXPRESSION_EQUATION_SOLVER_H
#define POINCARE_EXPRESSION_EQUATION_SOLVER_H

#include <omg/vector.h>
#include <poincare/range.h>
#include <poincare/src/memory/tree.h>

#include "polynomial.h"
#include "projection.h"
#include "symbol.h"

namespace Poincare::Internal {

/* Solver methods are a direct (and incomplete for now) adaptation of methods in
 * apps/solver/system_of_equations.cpp. */

class EquationSolver {
 public:
  constexpr static int k_maxNumberOfVariables = 6;
  constexpr static int k_maxNumberOfExactSolutions =
      std::max(k_maxNumberOfVariables,
               // +1 for delta
               Polynomial::k_maxPolynomialDegree + 1);

  class VariableArray : public OMG::StaticVector<char[Symbol::k_maxNameSize],
                                                 k_maxNumberOfVariables> {
   public:
    void push(const char* variable);
    void fillWithList(const Tree* list);

   private:
    // Prevent pushing arrays
    using OMG::StaticVector<char[Symbol::k_maxNameSize],
                            k_maxNumberOfVariables>::push;
  };

  enum class Error {
    NoError = 0,
    EquationUndefined = 1,
    EquationNonReal = 2,
    TooManyVariables = 3,
    NonLinearSystem = 4,
    RequireApproximateSolution = 5,
    EquationUnhandled = 6,
    DisabledInExamMode,  // TODO_PCJ
  };

  enum class SolutionType : uint8_t {
    Approximate,
    Exact,
    Formal,
  };

  enum class SolvingMethod : uint8_t {
    GeneralMonovariable,
    LinearSystem,
    PolynomialMonovariable,
  };

  struct EquationMetadata {
    // Variables that are considered unknown in the equations.
    VariableArray unknownVariables;
    // Variables that are defined by the user and present in the equations.
    VariableArray definedVariables;
    // If true, definedVariables are included in unknownVariables.
    bool overrideDefinedVariables = false;
    // Complex format used for projection
    ComplexFormat complexFormat = ComplexFormat::Cartesian;
  };

  struct SolutionMetadata {
    SolvingMethod solvingMethod = SolvingMethod::GeneralMonovariable;
    SolutionType solutionType = SolutionType::Approximate;
    // - Exact solve only -
    int degree = 0;
    // - Approximate solve only -
    Range1D<double> solvingRange = Range1D<double>();
    bool incompleteSolutions = false;
  };

  struct SolverResult {
    Tree* solutionList = nullptr;
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
   * It computes the EquationMetadata, and reduces the equation set. */
  static PreprocessingResult PreprocessEquationList(
      const Tree* equationList, ProjectionContext* projectionContext,
      UnknownSelectionStrategy selectionStrategy);

  // Return list of solutions for linear system.
  static SolverResult SolveLinearSystem(
      const Tree* equationList, const EquationMetadata& equationMetadata);
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
