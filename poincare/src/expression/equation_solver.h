#ifndef POINCARE_EXPRESSION_EQUATION_SOLVER_H
#define POINCARE_EXPRESSION_EQUATION_SOLVER_H

#include <poincare/expression.h>
#include <poincare/range.h>
#include <poincare/src/memory/tree.h>

#include "projection.h"
#include "symbol.h"

namespace Poincare::Internal {

/* Solver methods are a direct (and incomplete for now) adaptation of methods in
 * apps/solver/system_of_equations.cpp. */

class VariableArray {
 public:
  constexpr static int k_maxNumberOfVariables =
      Expression::k_maxNumberOfVariables;
  VariableArray() : m_numberOfVariables(0) {}
  void fillWithList(const Tree* list);
  void clear() { m_numberOfVariables = 0; }
  void append(const char* variable);
  int numberOfVariables() const { return m_numberOfVariables; }
  const char* variable(int index) const {
    assert(0 <= index && index < m_numberOfVariables);
    assert(m_variables[index][0] != '\0');
    return m_variables[index];
  }

 private:
  int m_numberOfVariables;
  char m_variables[k_maxNumberOfVariables][Symbol::k_maxNameSize];
};

class EquationSolver {
 public:
  constexpr static int k_maxNumberOfExactSolutions =
      std::max(VariableArray::k_maxNumberOfVariables,
               Expression::k_maxPolynomialDegree + 1);
  constexpr static int k_maxNumberOfApproximateSolutions = 10;
  constexpr static int k_maxNumberOfSolutions =
      std::max(k_maxNumberOfExactSolutions, k_maxNumberOfApproximateSolutions);

  enum class Type : uint8_t {
    LinearSystem,
    PolynomialMonovariable,
    GeneralMonovariable,
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

  enum class SolutionStatus : uint8_t {
    Complete,     // All solutions have been found in the interval
    Incomplete,   // There are other solutions
    Interrupted,  // The solver has been interrupted
  };

  struct SolutionMetadata {
    Error error = Error::NoError;
    ComplexFormat complexFormat = ComplexFormat::Cartesian;
    Type type = Type::GeneralMonovariable;
    int8_t degree = 0;
    int8_t numberOfUnknowns = 0;
    // If true, definedVariables are ignored.
    bool overrideDefinedVariables = false;
    VariableArray unknownVariables;
    VariableArray definedVariables;
    SolutionStatus solutionStatus = SolutionStatus::Complete;
  };

  struct SolverResult {
    Tree* solutionList = nullptr;
    SolutionMetadata metadata;
  };

  struct ApproximateSolvingRange {
    Range1D<double> range;
    /* When there are more than k_maxNumberOfApproximateSolutions on one side of
     * 0, the zoom is setting the interval to have a maximum of 5 solutions left
     * of 0 and 5 solutions right of zero. This means that sometimes, for a
     * function like `piecewise(1, x<0; cos(x), x >= 0)`, only 5 solutions will
     * be displayed. We still want to notify the user that more solutions exist.
     */
    bool isRangeIncomplete;
  };

  static SolverResult ExactSolve(const Tree* equationsSet,
                                 ProjectionContext projectionContext);

  static ApproximateSolvingRange ComputeApproximateSolvingRange(
      const Tree* equation, ProjectionContext projectionContext);

  static SolverResult ApproximateSolve(const Tree* equation,
                                       Range1D<double> range,
                                       ProjectionContext projectionContext,
                                       bool isRangeIncomplete = false);

 private:
  // Return list of exact solutions.
  static SolverResult PrivateExactSolve(const Tree* equationsSet,
                                        ProjectionContext projectionContext);
  static Error ProjectAndReduce(Tree* equationsSet,
                                ProjectionContext projectionContext);
  // Return list of solutions for linear system.
  static Tree* SolveLinearSystem(const Tree* equationsSet,
                                 uint8_t numberOfVariables,
                                 SolutionMetadata* metadata);
  // Return list of solutions for a polynomial equation.
  static Tree* SolvePolynomial(const Tree* equationsSet,
                               uint8_t numberOfVariables,
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

  static Tree* PrepareEquationForApproximateSolve(
      const Tree* equation, ProjectionContext projectionContext,
      SolutionMetadata* metadata);
};

}  // namespace Poincare::Internal

#endif
