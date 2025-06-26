#ifndef POINCARE_EXPRESSION_EQUATION_SOLVER_H
#define POINCARE_EXPRESSION_EQUATION_SOLVER_H

#include <omg/vector.h>
#include <poincare/expression.h>
#include <poincare/range.h>
#include <poincare/src/memory/tree.h>

#include "projection.h"
#include "symbol.h"

namespace Poincare::Internal {

/* Solver methods are a direct (and incomplete for now) adaptation of methods in
 * apps/solver/system_of_equations.cpp. */

class VariableArray
    : public OMG::StaticVector<char[Symbol::k_maxNameSize],
                               Expression::k_maxNumberOfVariables> {
 public:
  void push(const char* variable);
  void fillWithList(const Tree* list);

 private:
  // Prevent pushing arrays
  using OMG::StaticVector<char[Symbol::k_maxNameSize],
                          Expression::k_maxNumberOfVariables>::push;
};

class EquationSolver {
 public:
  constexpr static int k_maxNumberOfExactSolutions =
      std::max(Expression::k_maxNumberOfVariables,
               Expression::k_maxPolynomialDegree + 1);

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

  struct SolutionMetadata {
    // - General metadata -
    Error error = Error::NoError;
    SolvingMethod solvingMethod = SolvingMethod::GeneralMonovariable;
    SolutionType solutionType = SolutionType::Approximate;

    // - Variables -
    // Variables that are considered unknown in the equations.
    VariableArray unknownVariables;
    // Variables that are defined by the user.
    VariableArray definedVariables;
    // If true, definedVariables are included in unknownVariables.
    bool overrideDefinedVariables = false;

    // - Preferences used for projection -
    ComplexFormat complexFormat = ComplexFormat::Cartesian;

    // - Exact solve only -
    int degree = 0;

    // - Approximate solve only -
    Range1D<double> solvingRange = Range1D<double>();
    bool incompleteSolutions = false;
  };

  struct SolverResult {
    Tree* solutionList = nullptr;
    SolutionMetadata metadata;
  };

  static SolverResult ExactSolve(const Tree* equationList,
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
    SolutionMetadata partialMetadata;
  };

  /* Extracts unknows and userVariables, and reduces the equation set.
   * This is used by both ExactSolve and ApproximateSolve.
   * The following fields of the metadata will be filled:
   * - error (if any)
   * - unknownVariables
   * - definedVariables
   * - overrideDefinedVariables
   * - complexFormat
   */
  static PreprocessingResult PreprocessEquationList(
      const Tree* equationList, ProjectionContext* projectionContext,
      UnknownSelectionStrategy selectionStrategy);

  // Return list of exact solutions.
  static SolverResult PrivateExactSolve(const Tree* equationList,
                                        ProjectionContext projectionContext,
                                        bool overrideDefinedVariables);

  // Return list of solutions for linear system.
  static Tree* SolveLinearSystem(const Tree* equationList,
                                 SolutionMetadata* metadata);
  // Return list of solutions for a polynomial equation.
  static Tree* SolvePolynomial(const Tree* equationList,
                               SolutionMetadata* metadata);

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
