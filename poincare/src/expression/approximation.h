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

namespace Approximation {
namespace Private {
using VariableType = std::complex<double>;
/* LocalContext is used to store the values of local variable of index 0.
 * Parent contexts hold the values of the ones with higher indexes. */
class LocalContext {
 public:
  LocalContext(VariableType abscissa,
               const LocalContext* parentContext = nullptr)
      : m_localVariable(abscissa), m_parentContext(parentContext) {}
  VariableType variable(uint8_t index) const {
    return index == 0
               ? m_localVariable
               : (m_parentContext ? m_parentContext->variable(index - 1) : NAN);
  }
  void setLocalValue(VariableType value) { m_localVariable = value; }
  VariableType m_localVariable;
  const LocalContext* m_parentContext;
};
}  // namespace Private
struct Parameters {
  // A new m_randomContext will be created
  bool isRootAndCanHaveRandom = false;
  /* Must be true if expression has not been projected and may have parametric
   * functions. */
  bool projectLocalVariables = false;
  /* Last two parameters may only be used on projected expressions. */
  // Tree will be prepared for a more accurate approximation
  bool prepare = false;
  /* Tree will be optimized for successive approximations (useful in function
   * graph or solver for example) by approximating every subTree that can be
   * approximated. It also implies prepare parameter is true. */
  bool optimize = false;
};

class Context {
 public:
  Context(AngleUnit angleUnit = AngleUnit::None,
          ComplexFormat complexFormat = ComplexFormat::None,
          Poincare::Context* symbolContext = nullptr, int16_t listElement = -1)
      : m_randomContext(Random::Context(false)),
        m_localContext(nullptr),
        m_symbolContext(symbolContext),
        m_listElement(listElement),
        m_pointElement(-1),
        m_angleUnit(angleUnit),
        m_complexFormat(complexFormat) {}
  Private::VariableType variable(uint8_t index) const {
    assert(m_localContext);
    return m_localContext->variable(index);
  }
  void setLocalValue(Private::VariableType value) {
    assert(m_localContext);
    m_localContext->setLocalValue(value);
  }
  // TODO_PCJ : Maybe better separate const and non const ctx ?
  mutable Random::Context m_randomContext;
  // LocalContext is used to store the values of local variables
  Private::LocalContext* m_localContext;
  // SymbolContext is used to store the values of user symbols and functions
  Poincare::Context* m_symbolContext;
  // Tells if we are approximating to get the nth-element of a list
  int16_t m_listElement;
  // Tells if we are approximating to get the nth-element of a point
  int16_t m_pointElement;
  AngleUnit m_angleUnit;
  ComplexFormat m_complexFormat;
};

/* Approximation methods (with Parameters) */

template <typename T>
Tree* ToTree(const Tree* e, Parameters params, Context context = Context());

/* Approximate to real any scalar, unit or list with a list element. Return
 * NAN otherwise. */
template <typename T>
T To(const Tree* e, Parameters params, Context context = Context());

/* Approximate to real any scalar, unit, list/point with a list/point element,
with given value for VarX */
template <typename T>
T To(const Tree* e, T abscissa, Parameters params, Context context = Context());

template <typename T>
std::complex<T> ToComplex(const Tree* e, Parameters params,
                          Context context = Context());

template <typename T>
PointOrScalar<T> ToPointOrScalar(const Tree* e, Parameters params,
                                 Context context = Context());
// Approximate with given value for VarX
template <typename T>
PointOrScalar<T> ToPointOrScalar(const Tree* e, T abscissa, Parameters params,
                                 Context context = Context());

template <typename T>
Coordinate2D<T> ToPoint(const Tree* e, Parameters params,
                        Context context = Context());

template <typename T>
bool ToBoolean(const Tree* e, Parameters params, Context context = Context());

/* Helpers */

/* Approximate expression at KVarX/K = x. tree must be of scalar dimension and
 * real. TODO_PCJ: make private */
template <typename T>
T ToLocalContext(const Tree* e, const Context* ctx, T x);

// Optimize a projected function for efficient approximations
void PrepareFunctionForApproximation(Tree* e, const char* variable,
                                     ComplexFormat complexFormat);

// Return false if e cannot be approximated to a defined value.
bool CanApproximate(const Tree* e, bool approxLocalVar = false);

// Approximate every scalar subtree that can be approximated.
template <typename T>
bool ApproximateAndReplaceEveryScalar(Tree* e, Context context = Context());

/* Returns -1 if every condition is false, it assumes there is no other free
 * variable than VarX */
template <typename T>
int IndexOfActivePiecewiseBranchAt(const Tree* piecewise, T x);

template <typename T>
T FloatBinomial(T n, T k);
template <typename T>
T FloatGCD(T a, T b);
template <typename T>
T FloatLCM(T a, T b);

/* After approximation, there could be a remaining small imaginary part. If
 * one is sure that the result should be real, the following function extracts
 * the real part. If the imaginary part is too big, a nullptr is returned
 * instead.
 * This function assumes that Approximation has already been applied
 * to e. */
Tree* ExtractRealPartIfImaginaryPartNegligible(const Tree* e);

template <typename T>
bool IsNonReal(std::complex<T> x) {
  assert(!OMG::IsSignalingNan(x.real()) || x.imag() == static_cast<T>(0));
  return OMG::IsSignalingNan(x.real());
}

namespace Private {
// Update the approximation's context. Return a clone of e if necessary.
template <typename T>
Tree* PrepareTreeAndContext(const Tree* e, Parameters params, Context& context);

// Tree can be of any dimension
template <typename T>
Tree* PrivateToTree(const Tree* e, Dimension dim, const Context* ctx);

// Tree must be of scalar dimension or equivalent
template <typename T>
std::complex<T> PrivateToComplex(const Tree* e, const Context* ctx);

// Tree must be of scalar dimension or equivalent, and real.
template <typename T>
T PrivateTo(const Tree* e, const Context* ctx);

// Tree must be of boolean dimension.
template <typename T>
bool PrivateToBoolean(const Tree* e, const Context* ctx);

// Tree must be of point dimension.
template <typename T>
Tree* PrivateToPoint(const Tree* e, const Context* ctx);

// Tree must be of matrix dimension.
template <typename T>
Tree* ToMatrix(const Tree* e, const Context* ctx);

// Tree must be of list dimension.
template <typename T>
Tree* ToList(const Tree* e, const Context* ctx);

template <typename T>
std::complex<T> NonReal() {
  return std::complex<T>(OMG::SignalingNan<T>(), static_cast<T>(0));
}

std::complex<float> HelperUndefDependencies(const Tree* dep,
                                            const Context* ctx);
template <typename T>
std::complex<T> UndefDependencies(const Tree* dep, const Context* ctx);

// Expression must have been projected beforehand
bool PrepareExpressionForApproximation(Tree* e);
bool ShallowPrepareForApproximation(Tree* e, void* ctx);

template <typename T>
bool PrivateApproximateAndReplaceEveryScalar(Tree* e, const Context* ctx);
template <typename T>
bool SkipApproximation(TypeBlock type);
template <typename T>
bool SkipApproximation(TypeBlock type, TypeBlock parentType, int indexInParent,
                       bool previousChildWasApproximated);

/* Variables with id >= firstNonApproximableVarId are considered not
 * approximable. */
bool CanApproximate(const Tree* e, int firstNonApproximableVarId);

template <typename T>
std::complex<T> ToComplexSwitch(const Tree* e, const Context* ctx);

template <typename T>
Tree* ToComplexTree(const Tree* e, const Context* ctx);

// Replace a Tree with the Tree of its complex approximation
inline void ToComplexTreeInplace(Tree* e, const Context* ctx) {
  e->moveTreeOverTree(ToComplexTree<double>(e, ctx));
}

template <typename T>
std::complex<T> TrigonometricToComplex(TypeBlock type, std::complex<T> value,
                                       AngleUnit angleUnit);
template <typename T>
std::complex<T> HyperbolicToComplex(TypeBlock type, std::complex<T> value);
template <typename T>
T ApproximateIntegral(const Tree* integral, const Context* ctx);

template <typename T>
T ApproximateDerivative(const Tree* function, T at, int order,
                        const Context* ctx);

template <typename T>
T ApproximateRandom(const Tree* random, const Context* ctx);
template <typename T>
T ApproximateRandomHelper(const Tree* randomTree, const Context* ctx);

template <typename T>
std::complex<T> ApproximatePower(const Tree* power, const Context* ctx,
                                 ComplexFormat complexFormat);

template <typename T>
std::complex<T> ApproximateRoot(const Tree* root, const Context* ctx);

/* Approximate the conditions of a piecewise and return the tree corresponding
 * to the matching branch */
template <typename T>
const Tree* SelectPiecewiseBranch(const Tree* piecewise, const Context* ctx);

template <typename T>
std::complex<T> ApproximateTrace(const Tree* matrix, const Context* ctx);

template <typename T>
bool IsIntegerRepresentationAccurate(T x);

template <typename T>
T PositiveIntegerApproximation(T c);

template <typename T>
std::complex<T> NeglectRealOrImaginaryPartIfNegligible(
    std::complex<T> result, std::complex<T> input1,
    std::complex<T> input2 = 1.0, bool enableNullResult = true);

template <typename T>
std::complex<T> MakeResultRealIfInputIsReal(std::complex<T> result,
                                            std::complex<T> input);
}  // namespace Private
}  // namespace Approximation

}  // namespace Poincare::Internal

#endif
