#ifndef POINCARE_EXPRESSION_APPROXIMATION_H
#define POINCARE_EXPRESSION_APPROXIMATION_H

#include <float.h>
#include <poincare/point_or_scalar.h>
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
  /* Approximations on root tree, independent from current s_context. */

  /* preparedFunction is scalar and must have been prepared with
   * PrepareFunctionForApproximation. */
  template <typename T>
  static T RootPreparedToReal(const Tree* preparedFunction, T abscissa,
                              int listElement = -1) {
    return RootToPointOrScalarPrivate<T>(preparedFunction, false, true,
                                         abscissa, listElement)
        .toScalar();
  }

  /* preparedFunction is scalar and must have been prepared with
   * PrepareFunctionForApproximation. */
  template <typename T>
  static std::complex<T> RootPreparedToComplex(const Tree* preparedFunction,
                                               T abscissa = NAN);

  /* preparedFunction is scalar or point, and must have been prepared with
   * PrepareFunctionForApproximation. */
  template <typename T>
  static PointOrScalar<T> RootPreparedToPointOrScalar(
      const Tree* preparedFunction, T abscissa);

  /* scalarTree must have a scalar dimension. angleUnit and complexFormat can be
   * left to default on projected trees. */
  template <typename T>
  static T RootTreeToReal(const Tree* scalarTree,
                          AngleUnit angleUnit = AngleUnit::Radian,
                          ComplexFormat complexFormat = ComplexFormat::Real) {
    return RootToPointOrScalarPrivate<T>(scalarTree, false, false, NAN, -1,
                                         angleUnit, complexFormat)
        .toScalar();
  }

  // angleUnit and complexFormat can be left to default on projected trees.
  template <typename T>
  static Tree* RootTreeToTree(
      const Tree* tree, AngleUnit angleUnit = AngleUnit::Radian,
      ComplexFormat complexFormat = ComplexFormat::Cartesian);

  /* Approximations on tree, depends on current s_context */

  // Approximate a tree with any dimension
  template <typename T>
  static Tree* ToTree(const Tree* e, Dimension dim);

  // tree must be of scalar dimension
  template <typename T>
  static std::complex<T> ToComplex(const Tree* e);

  // tree must be of scalar dimension and real.
  template <typename T>
  static T To(const Tree* e) {
    std::complex<T> value = ToComplex<T>(e);
    return value.imag() == 0 ? value.real() : NAN;
  }

  /* Approximate expression at KVarX/K = x. tree must be of scalar dimension and
   * real */
  template <typename T>
  static T To(const Tree* e, T x) {
    assert(s_context);
    s_context->setLocalValue(x);
    return To<T>(e);
  }

  // tree must be of boolean dimension.
  template <typename T>
  static bool ToBoolean(const Tree* e);

  /* tree must be of list dimension.
  The returned tree is either a list of scalars or a list of points. */
  template <typename T>
  static Tree* ToList(const Tree* e);

  // tree must be of point dimension.
  template <typename T>
  static Tree* ToPoint(const Tree* e);

  // tree must be of matrix dimension.
  template <typename T>
  static Tree* ToMatrix(const Tree* e);

  // Replace a Tree with the Tree of its complex approximation
  static bool ApproximateToComplexTree(Tree* e);
  TREE_REF_WRAP(ApproximateToComplexTree);

  /* Helpers */

  // Optimize a projected function for efficient approximations
  static bool PrepareFunctionForApproximation(Tree* e, const char* variable,
                                              ComplexFormat complexFormat);

  static bool PrepareExpressionForApproximation(Tree* e,
                                                ComplexFormat complexFormat);

  // Return false if e could not be approximated to a defined value.
  static bool CanApproximate(const Tree* e, bool approxLocalVar = false) {
    return CanApproximate(e, 0);
  }

  // Approximate every scalar subtree that can be approximated.
  static bool ApproximateAndReplaceEveryScalar(
      Tree* e, const ProjectionContext* ctx = nullptr);
  TREE_REF_WRAP_1D(ApproximateAndReplaceEveryScalar, const ProjectionContext*,
                   nullptr);

  /* Returns -1 if every condition is false, it assumes there is no other free
   * variable than VarX */
  template <typename T>
  static int IndexOfActivePiecewiseBranchAt(const Tree* piecewise, T x);

  template <typename T>
  static T FloatBinomial(T n, T k);
  template <typename T>
  static T FloatGCD(T a, T b);
  template <typename T>
  static T FloatLCM(T a, T b);

 private:
  template <typename T>
  static PointOrScalar<T> RootToPointOrScalarPrivate(
      const Tree* e, bool isPoint, bool isPrepared = true, T abscissa = NAN,
      int listElement = -1, AngleUnit angleUnit = AngleUnit::Radian,
      ComplexFormat complexFormat = ComplexFormat::Real);

  // tree must be of given dimension and list length.
  template <typename T>
  static Tree* RootTreeToTreePrivate(const Tree* e, AngleUnit angleUnit,
                                     ComplexFormat complexFormat, Dimension dim,
                                     int listLength);

  static bool ShallowPrepareForApproximation(Tree* e, void* ctx);

  static bool PrivateApproximateAndReplaceEveryScalar(Tree* e);

  /* Variables with id >= firstNonApproximableVarId are considered not
   * approximable. */
  static bool CanApproximate(const Tree* e, int firstNonApproximableVarId);

  template <typename T>
  static std::complex<T> ToComplexSwitch(const Tree* e);

  template <typename T>
  static Tree* ToBeautifiedComplex(const Tree* e);

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
  template <typename T>
  static std::complex<T> ComputeComplexPower(const std::complex<T> c,
                                             const std::complex<T> d,
                                             ComplexFormat complexFormat);
  template <typename T>
  static std::complex<T> ComputeNotPrincipalRealRootOfRationalPow(
      const std::complex<T> c, T p, T q);

  /* Approximate the conditions of a piecewise and return the tree corresponding
   * to the matching branch */
  template <typename T>
  static const Tree* SelectPiecewiseBranch(const Tree* piecewise);

  template <typename T>
  static std::complex<T> ApproximateTrace(const Tree* matrix);

  template <typename T>
  static bool IsIntegerRepresentationAccurate(T x);
  template <typename T>
  static T PositiveIntegerApproximation(T c);
  template <typename T>
  static std::complex<T> NeglectRealOrImaginaryPartIfNegligible(
      std::complex<T> result, std::complex<T> input1,
      std::complex<T> input2 = 1.0, bool enableNullResult = true);
  template <typename T>
  static std::complex<T> MakeResultRealIfInputIsReal(std::complex<T> result,
                                                     std::complex<T> input);

  struct Context {
    using VariableType = double;
    Context(AngleUnit angleUnit = AngleUnit::Radian,
            ComplexFormat complexFormat = ComplexFormat::Cartesian,
            VariableType abscissa = NAN, int listElement = -1);

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
    bool m_encounteredComplex;

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
