#pragma once

#include <assert.h>
#include <omg/string.h>
#include <omg/vector.h>
#include <poincare/helpers/polynomial.h>
#include <poincare/helpers/symbol.h>
#include <poincare/preferences.h>
#include <poincare/range.h>
#include <string.h>

/* EquationSolver namespace is split into 2: in [equation_solver.h] and in
 * [equation_solver_pool.h]
 * This ensure Scandium can use it without including `expression.h`. */

namespace Poincare::EquationSolver {

constexpr static size_t k_maxNumberOfVariables = 6;
constexpr static size_t k_maxNumberOfExactSolutions =
    std::max(k_maxNumberOfVariables,
             // +1 for delta
             PolynomialHelpers::k_maxSolvableDegree + 1);

constexpr static size_t k_maxVariableSize = SymbolHelper::k_maxNameSize;

class VariableArray : public OMG::StaticVector<OMG::String<k_maxVariableSize>,
                                               k_maxNumberOfVariables> {
 public:
  void push(const char* variable) {
    assert(m_size < capacity());
    assert(variable && strlen(variable) < k_maxVariableSize);
    m_data[m_size] = std::string_view{variable, strlen(variable) + 1};
    m_size++;
  }

  void fillWithList(const Internal::Tree* list);

 private:
  // Prevent pushing arrays
  using OMG::StaticVector<OMG::String<k_maxVariableSize>,
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

}  // namespace Poincare::EquationSolver
