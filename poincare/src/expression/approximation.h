#ifndef POINCARE_EXPRESSION_APPROXIMATION_H
#define POINCARE_EXPRESSION_APPROXIMATION_H

#include <omg/signaling_nan.h>
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
  struct Context {
    using VariableType = double;
    Context(Random::Context* randomContext = nullptr,
            AngleUnit angleUnit = AngleUnit::Radian,
            ComplexFormat complexFormat = ComplexFormat::Cartesian,
            VariableType abscissa = NAN, int listElement = -1);

    Context(const Context* parentContext, VariableType abscissa);

    VariableType variable(uint8_t index) const {
      return index == 0
                 ? m_localVariable
                 : (m_parentContext ? m_parentContext->variable(index - 1)
                                    : NAN);
    }

    void setLocalValue(VariableType value) { m_localVariable = value; }

    /* TODO: most members of the context will not change and are needlessly
     * copied when building parent context chains. We should have an independant
     * variable chain instead. */
    AngleUnit m_angleUnit;
    ComplexFormat m_complexFormat;

    VariableType m_localVariable;

    // Tells if we are approximating to get the nth-element of a list
    int16_t m_listElement;
    // Tells if we are approximating to get the nth-element of a point
    int16_t m_pointElement;

    Random::Context* m_randomContext;
    const Context* m_parentContext;
  };

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

  /* pointTree must have a point dimension. angleUnit and complexFormat can be
   * left to default on projected trees. */
  template <typename T>
  static Coordinate2D<T> RootTreeToPoint(
      const Tree* pointTree, AngleUnit angleUnit = AngleUnit::Radian,
      ComplexFormat complexFormat = ComplexFormat::Real) {
    return RootToPointOrScalarPrivate<T>(pointTree, true, false, NAN, -1,
                                         angleUnit, complexFormat)
        .toPoint();
  }

  // angleUnit and complexFormat can be left to default on projected trees.
  template <typename T>
  static Tree* RootTreeToTree(
      const Tree* tree, AngleUnit angleUnit = AngleUnit::Radian,
      ComplexFormat complexFormat = ComplexFormat::Cartesian);

  /* Approximations on tree, depends on current s_context */

  // Approximate a tree with any dimension
  template <typename T>
  static Tree* ToTree(const Tree* e, Dimension dim, const Context* ctx);

  // tree must be of scalar dimension
  template <typename T>
  static std::complex<T> ToComplex(const Tree* e, const Context* ctx);

  // tree must be of scalar dimension and real.
  template <typename T>
  static T To(const Tree* e, const Context* ctx) {
    std::complex<T> value = ToComplex<T>(e, ctx);
    // Remove signaling nan
    return value.imag() == 0 && !IsNonReal(value) ? value.real() : NAN;
  }

  /* Approximate expression at KVarX/K = x. tree must be of scalar dimension and
   * real */
  template <typename T>
  static T To(const Tree* e, T x, const Context* ctx);

  // tree must be of boolean dimension.
  template <typename T>
  static bool ToBoolean(const Tree* e, const Context* ctx);

  // Input tree e must have a positive ListLength
  template <typename T>
  static Tree* ToList(const Tree* e, const Context* ctx);

  // tree must be of point dimension.
  template <typename T>
  static Tree* ToPoint(const Tree* e, const Context* ctx);

  // tree must be of matrix dimension.
  template <typename T>
  static Tree* ToMatrix(const Tree* e, const Context* ctx);

  // Replace a Tree with the Tree of its complex approximation
  static bool ApproximateToComplexTree(Tree* e, const Context* ctx);
  TREE_REF_WRAP_1D(ApproximateToComplexTree, const Context*, nullptr);

  /* Helpers */

  // Optimize a projected function for efficient approximations
  static bool PrepareFunctionForApproximation(Tree* e, const char* variable,
                                              ComplexFormat complexFormat);

  static bool PrepareExpressionForApproximation(Tree* e,
                                                ComplexFormat complexFormat);

  // Return false if e cannot be approximated to a defined value.
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
  static int IndexOfActivePiecewiseBranchAt(const Tree* piecewise, T x,
                                            const Context* ctx);

  template <typename T>
  static T FloatBinomial(T n, T k);
  template <typename T>
  static T FloatGCD(T a, T b);
  template <typename T>
  static T FloatLCM(T a, T b);

  /* After approximation, there could be a remaining small imaginary part. If
   * one is sure that the result should be real, the following function extracts
   * the real part. If the imaginary part is too big, a nullptr is returned
   * instead.
   * This function assumes that Approximation has already been applied
   * to e. */
  static Tree* ExtractRealPartIfImaginaryPartNegligible(const Tree* e);

 private:
  template <typename T>
  static std::complex<T> NonReal() {
    return std::complex<T>(OMG::SignalingNan<T>(), static_cast<T>(0));
  }
  template <typename T>
  static bool IsNonReal(std::complex<T> x) {
    if (OMG::IsSignalingNan(x.real())) {
      assert(x.imag() == static_cast<T>(0));
      return true;
    }
    return false;
  }

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

  static bool PrivateApproximateAndReplaceEveryScalar(Tree* e,
                                                      const Context* ctx);

  /* Variables with id >= firstNonApproximableVarId are considered not
   * approximable. */
  static bool CanApproximate(const Tree* e, int firstNonApproximableVarId);

  template <typename T>
  static std::complex<T> ToComplexSwitch(const Tree* e, const Context* ctx);

  template <typename T>
  static Tree* ToBeautifiedComplex(const Tree* e, const Context* ctx);

  template <typename T>
  static std::complex<T> TrigonometricToComplex(TypeBlock type,
                                                std::complex<T> value,
                                                AngleUnit angleUnit);
  template <typename T>
  static std::complex<T> HyperbolicToComplex(TypeBlock type,
                                             std::complex<T> value);
  template <typename T>
  static T ApproximateIntegral(const Tree* integral, const Context* ctx);
  template <typename T>
  static T ApproximateDerivative(const Tree* function, T at, int order,
                                 const Context* ctx);

  template <typename T>
  static T ApproximateRandom(const Tree* random, const Context* ctx);
  template <typename T>
  static T ApproximateRandomHelper(const Tree* randomTree, const Context* ctx);

  template <typename T>
  static std::complex<T> ApproximatePower(const Tree* power,
                                          const Context* ctx);
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
  static const Tree* SelectPiecewiseBranch(const Tree* piecewise,
                                           const Context* ctx);

  template <typename T>
  static std::complex<T> ApproximateTrace(const Tree* matrix,
                                          const Context* ctx);

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
};

}  // namespace Poincare::Internal

#endif
