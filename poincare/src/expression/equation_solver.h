#ifndef POINCARE_EXPRESSION_EQUATION_SOLVER_H
#define POINCARE_EXPRESSION_EQUATION_SOLVER_H

#include <poincare/range.h>
#include <poincare/src/memory/tree.h>

#include "projection.h"
#include "symbol.h"

namespace Poincare::Internal {

/* Solver methods are a direct (and incomplete for now) adaptation of methods in
 * apps/solver/system_of_equations.cpp. */

template <int N>
class VariableArray {
 public:
  VariableArray() : m_numberOfVariables(0) {}
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
  char m_variables[N][Symbol::k_maxNameSize];
};

class EquationSolver {
 public:
  constexpr static int k_maxNumberOfExactSolutions = 6;
  constexpr static int k_maxNumberOfApproximateSolutions = 10;

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
    DisabledInExamMode,  // TODO_PCJ
  };

  struct Context {
    Type type;
    int8_t degree;
    // If true, defined userVariables are ignored.
    bool overrideUserVariables = false;
    bool exactResults = true;
    bool hasMoreSolutions = false;
    VariableArray<k_maxNumberOfExactSolutions> variables;
    VariableArray<k_maxNumberOfExactSolutions> userVariables;
  };

  // Return list of exact solutions.
  static Tree* ExactSolve(const Tree* equationsSet, Context* context,
                          ProjectionContext projectionContext, Error* error);

  static Range1D<double> AutomaticInterval(const Tree* preparedEquation,
                                           Context* context);

  // Return a List of DoubleFloat
  static Tree* ApproximateSolve(const Tree* preparedEquation,
                                Range1D<double> range, Context* context);

 private:
  // Return list of exact solutions.
  static Tree* PrivateExactSolve(const Tree* equationsSet, Context* context,
                                 ProjectionContext projectionContext,
                                 Error* error);
  static void ProjectAndReduce(Tree* equationsSet,
                               ProjectionContext projectionContext,
                               Error* error);
  // Return list of solutions for linear system.
  static Tree* SolveLinearSystem(const Tree* equationsSet,
                                 uint8_t numberOfVariables, Context* context,
                                 Error* error);
  // Return list of solutions for a polynomial equation.
  static Tree* SolvePolynomial(const Tree* equationsSet,
                               uint8_t numberOfVariables, Context* context,
                               Error* error);

  // Return list of linear coefficients for each variables and final constant.
  static Tree* GetLinearCoefficients(const Tree* equation,
                                     uint8_t numberOfVariables,
                                     Context* context);
  // Prepare a solution before display
  static Error EnhanceSolution(Tree* solution, Context* context);

  constexpr static char k_parameterPrefix = 't';
  static uint32_t TagParametersUsedAsVariables(const Context* context);

  // Set the k-th bit in tags if name == "t{k}" and 0th if name is "t"
  static void TagVariableIfParameter(const char* name, uint32_t* tags,
                                     const Context* context);
};

}  // namespace Poincare::Internal

#endif
