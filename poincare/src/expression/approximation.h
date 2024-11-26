#ifndef POINCARE_EXPRESSION_APPROXIMATION_H
#define POINCARE_EXPRESSION_APPROXIMATION_H

#include <omg/signaling_nan.h>
#include <poincare/old/context.h>
#include <poincare/point_or_scalar.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_ref.h>

#include <cmath>
#include <complex>

#include "context.h"
#include "random.h"

namespace Poincare::Internal {

struct Dimension;

/* Approximation is implemented on all block types.
 * We could have asserted that we project before approximating (and thus
 * implemented the approximation only on internal types) but this increases the
 * number of operations (for instance, 2 / 3 VS 2 * 3 ^ -1) and decreases the
 * precision.
 * If not projected, additional context is expected (AngleUnit, ...) */

class Approximation final {
  friend struct Matrix;

 private:
  using VariableType = std::complex<double>;
  class LocalContext {
   public:
    LocalContext(VariableType abscissa,
                 const LocalContext* parentContext = nullptr)
        : m_localVariable(abscissa), m_parentContext(parentContext) {}
    VariableType variable(uint8_t index) const {
      return index == 0
                 ? m_localVariable
                 : (m_parentContext ? m_parentContext->variable(index - 1)
                                    : NAN);
    }
    void setLocalValue(VariableType value) { m_localVariable = value; }
    VariableType m_localVariable;
    const LocalContext* m_parentContext;
  };

 public:
  struct Parameter {
    // A new m_randomContext will be created
    bool isRoot = false;
    /* Must be true if expression has not been projected and may have parametric
     * functions. */
    bool projectLocalVariables = false;
    // Tree will be optimized for multiple approximations, only with toTree
    bool optimize = false;
    // Tree will be prepared for a more accurate approximation
    bool prepare = false;
  };

  class Context {
   public:
    Context(AngleUnit angleUnit = AngleUnit::None,
            ComplexFormat complexFormat = ComplexFormat::None,
            Poincare::Context* symbolContext = nullptr,
            int16_t listElement = -1, int16_t pointElement = -1,
            Random::Context randomContext = Random::Context(false),
            LocalContext* localContext = nullptr)
        : m_randomContext(randomContext),
          m_localContext(localContext),
          m_symbolContext(symbolContext),
          m_listElement(listElement),
          m_pointElement(pointElement),
          m_angleUnit(angleUnit),
          m_complexFormat(complexFormat) {}
    VariableType variable(uint8_t index) const {
      assert(m_localContext);
      return m_localContext->variable(index);
    }
    void setLocalValue(VariableType value) {
      assert(m_localContext);
      m_localContext->setLocalValue(value);
    }
    // TODO_PCJ : Maybe better separate const and non const ctx ?
    mutable Random::Context m_randomContext;
    LocalContext* m_localContext;
    Poincare::Context* m_symbolContext;
    // Tells if we are approximating to get the nth-element of a list
    int16_t m_listElement;
    // Tells if we are approximating to get the nth-element of a point
    int16_t m_pointElement;
    AngleUnit m_angleUnit;
    ComplexFormat m_complexFormat;
  };

  /* Approximation methods (with Parameter) */

  template <typename T>
  static Tree* ToTree(const Tree* e, Parameter param,
                      Context context = Context());

  template <typename T>
  static T To(const Tree* e, Parameter param, Context context = Context());

  // Approximate to real with given value for VarX
  template <typename T>
  static T To(const Tree* e, T abscissa, Parameter param,
              Context context = Context());

  template <typename T>
  static std::complex<T> ToComplex(const Tree* e, Parameter param,
                                   Context context = Context());

  template <typename T>
  static PointOrScalar<T> ToPointOrScalar(const Tree* e, Parameter param,
                                          Context context = Context());
  // Approximate with given value for VarX
  template <typename T>
  static PointOrScalar<T> ToPointOrScalar(const Tree* e, T abscissa,
                                          Parameter param,
                                          Context context = Context());

  template <typename T>
  static Coordinate2D<T> ToPoint(const Tree* e, Parameter param,
                                 Context context = Context());

  template <typename T>
  static bool ToBoolean(const Tree* e, Parameter param,
                        Context context = Context());

  /* Helpers */

  /* Approximate expression at KVarX/K = x. tree must be of scalar dimension and
   * real. TODO_PCJ: make private */
  template <typename T>
  static T ToLocalContext(const Tree* e, const Context* ctx, T x);

  // Optimize a projected function for efficient approximations
  static bool PrepareFunctionForApproximation(Tree* e, const char* variable,
                                              ComplexFormat complexFormat);

  // Return false if e cannot be approximated to a defined value.
  static bool CanApproximate(const Tree* e, bool approxLocalVar = false) {
    return CanApproximate(e, 0);
  }

  // Approximate every scalar subtree that can be approximated.
  static bool ApproximateAndReplaceEveryScalar(Tree* e,
                                               Context context = Context());

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

  /* After approximation, there could be a remaining small imaginary part. If
   * one is sure that the result should be real, the following function extracts
   * the real part. If the imaginary part is too big, a nullptr is returned
   * instead.
   * This function assumes that Approximation has already been applied
   * to e. */
  static Tree* ExtractRealPartIfImaginaryPartNegligible(const Tree* e);

  template <typename T>
  static bool IsNonReal(std::complex<T> x) {
    assert(!OMG::IsSignalingNan(x.real()) || x.imag() == static_cast<T>(0));
    return OMG::IsSignalingNan(x.real());
  }

 private:
  // Update the approximation's context. Return a clone of e if necessary.
  static Tree* PrepareContext(const Tree* e, Parameter param, Context* context);

  /* Approximation methods (without Parameter) */

  // Tree can be of any dimension
  template <typename T>
  static Tree* ToTree(const Tree* e, Dimension dim, const Context* ctx);

  // Tree must be of scalar dimension
  template <typename T>
  static std::complex<T> ToComplex(const Tree* e, const Context* ctx);

  // Tree must be of scalar dimension and real.
  template <typename T>
  static T To(const Tree* e, const Context* ctx);

  // Tree must be of boolean dimension.
  template <typename T>
  static bool ToBoolean(const Tree* e, const Context* ctx);

  // Tree must have a positive ListLength
  template <typename T>
  static Tree* ToList(const Tree* e, const Context* ctx);

  // Tree must be of point dimension.
  template <typename T>
  static Tree* ToPoint(const Tree* e, const Context* ctx);

  // Tree must be of matrix dimension.
  template <typename T>
  static Tree* ToMatrix(const Tree* e, const Context* ctx);

  // Replace a Tree with the Tree of its complex approximation
  static bool ApproximateToComplexTree(Tree* e, const Context* ctx);

  template <typename T>
  static std::complex<T> NonReal() {
    return std::complex<T>(OMG::SignalingNan<T>(), static_cast<T>(0));
  }

  static std::complex<float> HelperUndefDependencies(const Tree* dep,
                                                     const Context* ctx);
  template <typename T>
  static std::complex<T> UndefDependencies(const Tree* dep, const Context* ctx);

  static bool PrepareExpressionForApproximation(Tree* e);
  static bool ShallowPrepareForApproximation(Tree* e, void* ctx);

  static bool PrivateApproximateAndReplaceEveryScalar(Tree* e,
                                                      const Context* ctx);

  /* Variables with id >= firstNonApproximableVarId are considered not
   * approximable. */
  static bool CanApproximate(const Tree* e, int firstNonApproximableVarId);

  template <typename T>
  static std::complex<T> ToComplexSwitch(const Tree* e, const Context* ctx);

  template <typename T>
  static Tree* ToComplexTree(const Tree* e, const Context* ctx);

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
  static T GrowthRateAroundAbscissa(T x, T h, int order, const Tree* child,
                                    const Context* ctx);
  template <typename T>
  static T RiddersApproximation(int order, const Tree* child, T x, T h,
                                T* error, const Context* ctx);

  template <typename T>
  static T ApproximateRandom(const Tree* random, const Context* ctx);
  template <typename T>
  static T ApproximateRandomHelper(const Tree* randomTree, const Context* ctx);

  template <typename T>
  static std::complex<T> ApproximatePower(const Tree* power, const Context* ctx,
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
