#ifndef POINCARE_EXPRESSION_APPROXIMATION_H
#define POINCARE_EXPRESSION_APPROXIMATION_H

#include <float.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_ref.h>

#include <cmath>
#include <complex>

#include "context.h"
#include "random.h"

namespace Poincare::Internal {

struct ProjectionContext;
struct Dimension;

/* Approximation is implemented on all block types.
 * We could have asserted that we reduce before approximating (and thus
 * implemented the approximation only on internal types) but this increases the
 * number of operations (for instance, 2 / 3 VS 2 * 3 ^ -1) and decreases the
 * precision. We rather beautify before approximating.
 */

class Approximation final {
 public:
  // Optimize function for efficient approximations
  static void PrepareFunctionForApproximation(Tree* expr, const char* variable,
                                              AngleUnit angleUnit,
                                              ComplexFormat complexFormat);

  template <typename T>
  static T ToReal(const Tree* preparedExpression) {
    return ToReal<T>(preparedExpression, NAN);
  }

  template <typename T>
  static T ToReal(const Tree* preparedFunction, T abscissa);

  // Approximate a tree with any dimension, isolated from any outer context.
  template <typename T>
  static Tree* RootTreeToTree(const Tree* node, AngleUnit angleUnit,
                              ComplexFormat complexFormat);
  // Approximate a tree with any dimension, isolated from any outer context.
  template <typename T>
  static Tree* RootTreeToTree(const Tree* node, AngleUnit angleUnit,
                              ComplexFormat complexFormat, Dimension dim,
                              int listLength);

  // Approximate an entire scalar tree, isolated from any outer context.
  template <typename T>
  static std::complex<T> RootTreeToComplex(const Tree* node,
                                           AngleUnit angleUnit,
                                           ComplexFormat complexFormat);

  // Approximate an entire scalar tree, isolated from any outer context.
  template <typename T>
  static T RootTreeToReal(const Tree* node,
                          AngleUnit angleUnit = AngleUnit::Radian,
                          ComplexFormat complexFormat = ComplexFormat::Real) {
    std::complex<T> value =
        RootTreeToComplex<T>(node, angleUnit, complexFormat);
    return value.imag() == 0 ? value.real() : NAN;
  }

  // Helper to replace a tree by its approximation
  static bool SimplifyComplex(Tree* node);
  EDITION_REF_WRAP(SimplifyComplex);

  // Approximate a tree with any dimension
  template <typename T>
  static Tree* ToTree(const Tree* node, Dimension dim);

  template <typename T>
  static std::complex<T> ToComplex(const Tree* node);

  template <typename T>
  static T To(const Tree* node) {
    std::complex<T> value = ToComplex<T>(node);
    return value.imag() == 0 ? value.real() : NAN;
  }

  // Approximate expression at KVarX/K = x
  template <typename T>
  static T To(const Tree* node, T x) {
    s_context->setLocalValue(x);
    std::complex<T> value = ToComplex<T>(node);
    return value.imag() == 0 ? value.real() : NAN;
  }

  template <typename T>
  static bool ToBoolean(const Tree* node);

  template <typename T>
  static Tree* ToList(const Tree* node);

  template <typename T>
  static Tree* ToPoint(const Tree* node);

  template <typename T>
  static Tree* ToMatrix(const Tree* node);

  /* If collapse is true, approximate parents if all children have approximated.
   * Also raise if result is undefined. */
  static bool ApproximateAndReplaceEveryScalar(
      Tree* tree, const ProjectionContext* ctx = nullptr);
  EDITION_REF_WRAP_1D(ApproximateAndReplaceEveryScalar,
                      const ProjectionContext*, nullptr);

  /* Returns -1 if every condition is false, it assumes there is no other free
   * variable than VarX */
  template <typename T>
  static int IndexOfActivePiecewiseBranchAt(const Tree* piecewise, T x);

 private:
  static bool ShallowPrepareForApproximation(Tree* expr, void* ctx);

  template <typename T>
  using Reductor = T (*)(T, T);
  template <typename T, typename U>
  using Mapper = U (*)(T);
  template <typename T, typename U>
  static U MapAndReduce(const Tree* node, Reductor<U> reductor,
                        Mapper<std::complex<T>, U> mapper = nullptr);
  template <typename T>
  static bool ApproximateAndReplaceEveryScalarT(Tree* tree);

  template <typename T>
  static std::complex<T> TrigonometricToComplex(TypeBlock type,
                                                std::complex<T> value,
                                                AngleUnit angleUnit);
  template <typename T>
  static std::complex<T> HyperbolicToComplex(TypeBlock type,
                                             std::complex<T> value);
  template <typename T>
  static T ApproximateIntegral(const Tree* integral);
  template <typename T>
  static T ApproximateDerivative(const Tree* function, T at, int order);
  template <typename T>
  static std::complex<T> ApproximatePower(const Tree* power,
                                          ComplexFormat complexFormat);

  /* Approximate the conditions of a piecewise and return the tree corresponding
   * to the matching branch */
  template <typename T>
  static const Tree* SelectPiecewiseBranch(const Tree* piecewise);

  struct Context {
    using VariableType = double;
    Context(AngleUnit angleUnit, ComplexFormat complexFormat);

    VariableType variable(uint8_t index) const {
      return m_variables[indexForVariable(index)];
    }
    void shiftVariables() { m_variablesOffset--; }
    void unshiftVariables() { m_variablesOffset++; }

    void setLocalValue(VariableType value) {
      m_variables[indexForVariable(0)] = value;
    }
    // with sum(sum(l,l,1,k),k,1,n) m_variables stores [n, NaN, …, NaN, l, k]
    uint8_t indexForVariable(uint8_t index) const {
      assert(index < m_variablesOffset);
      return (index + m_variablesOffset) % k_maxNumberOfVariables;
    }
    AngleUnit m_angleUnit;
    ComplexFormat m_complexFormat;

    static constexpr int k_maxNumberOfVariables = 16;
    VariableType m_variables[k_maxNumberOfVariables];
    uint8_t m_variablesOffset;

    // Tells if we are approximating to get the nth-element of a list
    int m_listElement;
    // Tells if we are approximating to get the nth-element of a point
    int m_pointElement;
  };

  /* Approximation context is created by entry points RootTreeTo* and passed
   * down to the function hierarchy using a static pointer to avoid carrying an
   * extra argument everywhere. */
  static Context* s_context;
  static Random::Context* s_randomContext;
};

}  // namespace Poincare::Internal

#endif
