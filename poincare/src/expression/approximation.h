#pragma once

#include <omg/signaling_nan.h>
#include <poincare/point_or_scalar.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_ref.h>
#include <poincare/symbol_context.h>

#include <cmath>
#include <complex>

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

// Precision of floating-point calculations
enum class FloatPrecision { Single, Double };
/* For a given function, if its body depends on the float type (for example if a
 * float / double variable is declared), the function has to be templated with
 the float type. However if the float type is not used in the function body, the
 function should not be templated, to avoid a duplicate template instanciation
 and gain binary size. To forward the float precision information to other
 functions, a FloatPrecision parameter is passed as a function argument. */

/* Helper struct to convert a float template type into a FloatPrecision value.
 * Usage: Precision<T>::value.
 * Note that the reverse operation is not possible. If FloatPrecision is passed
 * as a function parameter, its value is not known at compile-time. So we are
 * forced to conditionally call a "float" or "double" version of a template
 * function dependending on the FloatPrecision value. */
template <typename FloatType>
struct Precision {};
template <>
struct Precision<float> {
  constexpr static FloatPrecision value = FloatPrecision::Single;
};
template <>
struct Precision<double> {
  constexpr static FloatPrecision value = FloatPrecision::Double;
};

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
          const Poincare::SymbolContext& symbolContext = k_emptySymbolContext,
          int16_t listElement = -1)
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
  /* Should always be called after calling an approximation function with a
   * temporary context.
   * TODO: separate random context from the rest. */
  void updateRandomContext(Random::Context randomCtx) const {
    m_randomContext = randomCtx;
  }
  // TODO_PCJ : Maybe better separate const and non const ctx ?
  mutable Random::Context m_randomContext;
  // LocalContext is used to store the values of local variables
  Private::LocalContext* m_localContext;
  // SymbolContext is used to store the values of user symbols and functions
  const Poincare::SymbolContext& m_symbolContext;
  // Tells if we are approximating to get the nth-element of a list
  int16_t m_listElement;
  // Tells if we are approximating to get the nth-element of a point
  int16_t m_pointElement;
  AngleUnit m_angleUnit;
  ComplexFormat m_complexFormat;
};

class BooleanOrUndefined {
 public:
  // Boolean of given value
  constexpr BooleanOrUndefined(bool value) : m_value{value} {}
  // Undefined boolean
  constexpr static BooleanOrUndefined Undef() { return BooleanOrUndefined(); }

  bool isUndefined() const { return m_isUndefined; }
  bool value() const {
    assert(!m_isUndefined);
    return m_value;
  }

 private:
  constexpr BooleanOrUndefined() : m_isUndefined{true} {}
  bool m_isUndefined = false;
  /* In case of an undefined, the stored value is false (but it should not be
   * fetched) */
  bool m_value = false;
};

/* Approximation methods (with Parameters) */

// Common approximation pipeline for all return types
template <typename ReturnType>
ReturnType To(const Tree* e, FloatPrecision floatPrecision, Parameters params,
              const Context& context = Context());

// In this version, the float precision is deduced from the ReturnType
template <typename ReturnType>
ReturnType To(const Tree* e, Parameters params,
              const Context& context = Context());

template <typename ReturnType, typename FloatType>
ReturnType To(const Tree* e, FloatType abscissa, Parameters params,
              const Context& context = Context());

/* Helpers */

/* Approximate expression at KVarX/K = x. tree must be of scalar dimension and
 * real. TODO_PCJ: make private */
template <typename T>
T ToLocalContext(const Tree* e, const Context* ctx, T x);

// Optimize a projected function for efficient approximations
void PrepareFunctionForApproximation(Tree* e, const char* variable,
                                     bool variableIsReal);

// Return false if e cannot be approximated to a defined value.
bool CanApproximate(const Tree* e, bool approxLocalVar = false);

// Approximate every scalar subtree that can be approximated.
bool ApproximateAndReplaceEveryScalar(Tree* e, FloatPrecision floatPrecision,
                                      const Context& context = Context());

constexpr int k_noActiveBranch = -1;
constexpr int k_unknownActiveBranch = -2;
/* Returns k_noActiveBranch if every condition is false, it assumes there is no
 * other free variable than VarX, return k_unknownActiveBranch if a condition
 * was undefined. */
template <typename T>
int IndexOfActivePiecewiseBranchAt(const Tree* piecewise, T x);

template <typename T>
T FloatBinomial(T n, T k);
template <typename T>
T FloatGCD(T a, T b);
template <typename T>
T FloatLCM(T a, T b);

Tree* ExtractRealPart(const Tree* e);

template <typename T>
bool IsNonReal(std::complex<T> x) {
  assert(!OMG::IsSignalingNan(x.real()) || x.imag() == static_cast<T>(0));
  return OMG::IsSignalingNan(x.real());
}

namespace Private {
// Update the approximation's context. Return a clone of e if necessary.
/* Note that the Context is const in this method although the random context
 * will be updated. This is taking advantage of the mutable m_randomContext.
 * This allows to call PrepareTree in methods where the Context is
 * const, without the need to create a copy of the whole Context just to update
 * the random context. */
Tree* PrepareTree(const Tree* e, FloatPrecision floatPrecision,
                  Parameters params, const Context& context);

/* Specific processing for the ReturnType. Must be overloaded for each specific
 * ReturnType */
template <typename ReturnType>
ReturnType SpecificTo(const Tree* e, FloatPrecision floatPrecision,
                      Parameters params, const Context& context);

template <typename FloatType>
std::complex<FloatType> ToComplex(const Tree* e, FloatPrecision floatPrecision,
                                  Parameters params, const Context& context);

/* Undefined value representing an unhandled dimension for the T type (Tree,
 * float / double, boolean etc.)
 * TODO: this template method and its specializations can disappear once we find
 * a way to assert the dimension check in To instead of returning an
 * undefined value. */
template <typename T>
T UndefinedUnhandledDimension();

// Tree can be of any dimension
Tree* PrivateToTree(const Tree* e, FloatPrecision floatPrecision, Dimension dim,
                    const Context* ctx);

// Tree must be of scalar dimension or equivalent. Escape if tree is too deep.
template <typename T>
std::complex<T> PrivateToComplex(const Tree* e, const Context* ctx);

// Tree must be of scalar dimension or equivalent
template <typename T>
std::complex<T> PrivateToComplexRecursive(const Tree* e, const Context* ctx);

// Tree must be of scalar dimension or equivalent, and real.
template <typename T>
T PrivateTo(const Tree* e, const Context* ctx);

// Tree must be of boolean dimension.
BooleanOrUndefined PrivateToBoolean(const Tree* e,
                                    FloatPrecision floatPrecision,
                                    const Context* ctx);

// Tree must be of point dimension.
template <typename T>
Coordinate2D<T> PrivateToCoordinates(const Tree* e, const Context& context);

// Tree must be of point dimension.
Tree* PrivateToPoint(const Tree* e, FloatPrecision floatPrecision,
                     const Context* ctx);

// Tree must be of matrix dimension.
Tree* ToMatrix(const Tree* e, FloatPrecision floatPrecision,
               const Context* ctx);

// Tree must be of list dimension.
Tree* ToList(const Tree* e, FloatPrecision floatPrecision, const Context* ctx);

template <typename T>
std::complex<T> NonReal() {
  return std::complex<T>(OMG::SignalingNan<T>(), static_cast<T>(0));
}

enum class DefinedStatus { Defined, NonReal, Undefined };

/* Returns a global DefinedStatus for all the dependencies. The DefinedStatus
 * is prioritized: Defined if all dependencies are well defined, NonReal if
 * one or more dependencies are NonReal but none is Undefined, Undefined
 * otherwise. */
template <typename T>
DefinedStatus AreDependenciesDefined(const Tree* dep, const Context* ctx);

/* Convert a DefinedStatus to an std::complex<T> value: NonReal<T>{} for NonReal
 * and std::complex<T>(NAN) for Undef. ToUndefinedComplexValue is not callable
 * on DefinedStatus::Defined. */
template <typename T>
std::complex<T> ToUndefinedComplexValue(DefinedStatus status);

// Expression must have been projected beforehand
bool PrepareExpressionForApproximation(Tree* e);
bool ShallowPrepareForApproximation(Tree* e, void* ctx);

bool PrivateApproximateAndReplaceEveryScalar(Tree* e,
                                             FloatPrecision floatPrecision,
                                             const Context* ctx);

bool SkipApproximation(TypeBlock type, FloatPrecision floatPrecision);
bool SkipApproximation(TypeBlock type, FloatPrecision floatPrecision,
                       TypeBlock parentType, int indexInParent,
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
std::complex<T> ApproximatePower(const Tree* power, const Context* ctx,
                                 ComplexFormat complexFormat);

template <typename T>
std::complex<T> ApproximateRoot(const Tree* root, const Context* ctx);

template <typename T>
std::complex<T> ComplexLogarithm(std::complex<T> c, bool isLog10);

/* Approximate the conditions of a piecewise and return the tree corresponding
 * to the matching branch. Return nullptr if a condition was undefined. */
const Tree* SelectPiecewiseBranch(const Tree* piecewise,
                                  FloatPrecision floatPrecision,
                                  const Context* ctx);

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
