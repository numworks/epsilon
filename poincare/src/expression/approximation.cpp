#include "approximation.h"

#include <omg/float.h>
#include <omg/unreachable.h>
#include <omg/vector.h>
#include <poincare/sequence_approximation_helper.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/statistics/distributions/distribution_method.h>
#include <poincare/src/statistics/statistics_dataset_column.h>
#include <poincare/statistics/distribution.h>
#include <poincare/statistics/statistics_dataset.h>
#include <poincare/symbol_context.h>
#include <poincare/trigonometry.h>

#include <bit>
#include <cmath>
#include <complex>

#include "arithmetic.h"
#include "dependency.h"
#include "dimension.h"
#include "float_helper.h"
#include "matrix.h"
#include "number.h"
#include "physical_constant.h"
#include "random.h"
#include "simplification.h"
#include "symbol.h"
#include "undefined.h"
#include "units/unit.h"
#include "variables.h"
#include "vector.h"

namespace Poincare::Internal::Approximation {

static_assert(!POINCARE_NO_FLOAT_APPROXIMATION ||
                  !(POINCARE_MATRIX || POINCARE_LIST || POINCARE_POINT),
              "the double-only approximation is only available with scalars "
              "and booleans");

template <typename T>
struct FloatTypeAccessor {
  /* In the general case, the floating-point type can be accessed looking at the
   * value_type type definition of T (for example
   * std::complex<float>::value_type is float) */
  using type = typename T::value_type;
};

/* The float and double types are special cases as they do not contain a
 * value_type nested type. These two template specializations are there to
 * redirect FloatTypeAccessor to the "float" or "double" type itself */
template <>
struct FloatTypeAccessor<float> {
  using type = float;
};

template <>
struct FloatTypeAccessor<double> {
  using type = double;
};

/* Helper to replace a type built on float by a type built on double. */
template <typename T>
struct FloatToDouble {
  using type = T;
};

template <>
struct FloatToDouble<float> {
  using type = double;
};

template <>
struct FloatToDouble<std::complex<float>> {
  using type = std::complex<double>;
};

template <>
struct FloatToDouble<Coordinate2D<float>> {
  using type = Coordinate2D<double>;
};

template <typename ReturnType>
ReturnType To(const Tree* e, const Parameters& parameters) {
  using float_type = typename FloatTypeAccessor<ReturnType>::type;
  return Private::Process<ReturnType>(e, Precision<float_type>::value,
                                      parameters);
}

template <typename ReturnType>
ReturnType To(const Tree* e, FloatPrecision floatPrecision,
              const Parameters& parameters) {
  return Private::Process<ReturnType>(e, floatPrecision, parameters);
}

template <typename ReturnType, typename FloatType>
ReturnType To(const Tree* e, FloatType abscissa, const Parameters& parameters) {
  static_assert(
      std::is_same_v<ReturnType, Tree*> ||
      std::is_same_v<ReturnType, BooleanOrUndefined> ||
      std::is_same_v<FloatType, typename FloatTypeAccessor<ReturnType>::type>);
  return Private::Process<ReturnType>(e, Precision<FloatType>::value,
                                      parameters, abscissa);
}

template <typename ReturnType, typename FloatType>
ReturnType To(const Tree* e, FloatType abscissa, int16_t listElementIndex,
              const Parameters& parameters) {
  static_assert(
      std::is_same_v<ReturnType, Tree*> ||
      std::is_same_v<ReturnType, BooleanOrUndefined> ||
      std::is_same_v<FloatType, typename FloatTypeAccessor<ReturnType>::type>);
  return Private::Process<ReturnType>(e, Precision<FloatType>::value,
                                      parameters, abscissa, listElementIndex);
}

bool Approximate(Tree* e, FloatPrecision floatPrecision,
                 const Parameters& parameters) {
  Tree* approximated = Private::Process<Tree*>(e, floatPrecision, parameters);
  bool hasChanged = !approximated->treeIsIdenticalTo(e);
  e->moveTreeOverTree(approximated);
  return hasChanged;
}

/* Helpers */

bool CanApproximate(const Tree* e, bool approxLocalVar) {
  return Private::CanApproximate(e, 0);
}

template <typename T>
T FloatBinomial(T n, T k) {
  if (k != std::round(k)) {
    return NAN;
  }
  bool nIsInt = n == std::round(n);
  if (k < 0 || (nIsInt && 0 <= n && n < k)) {
    return 0.;
  }
  // Generalized definition allows any n value
  bool generalized = !nIsInt || n < 0;
  // Take advantage of symmetry
  k = (!generalized && k > (n - k)) ? n - k : k;

  T result = 1;
  // NOTE: i being a float could lead to infinite loop due to i+1 == i
  for (double i = 0; i < k; i++) {
    result *= (n - static_cast<T>(i)) / (k - static_cast<T>(i));
    if (std::isinf(result) || std::isnan(result) || result == 0.) {
      return result;
    }
  }
  // If not generalized, the output must be rounded
  return generalized ? result : std::round(result);
}

template <typename T>
static std::complex<T> FloatMultiplication(std::complex<T> c,
                                           std::complex<T> d) {
  // Special case to prevent (inf,0)*(1,0) from returning (inf, nan).
  if (std::isinf(std::abs(c)) || std::isinf(std::abs(d))) {
    constexpr T zero = static_cast<T>(0.0);
    // Handle case of pure imaginary/real multiplications
    if (c.imag() == zero && d.imag() == zero) {
      return {c.real() * d.real(), zero};
    }
    if (c.real() == zero && d.real() == zero) {
      return {-c.imag() * d.imag(), zero};
    }
    if (c.imag() == zero && d.real() == zero) {
      return {zero, c.real() * d.imag()};
    }
    if (c.real() == zero && d.imag() == zero) {
      return {zero, c.imag() * d.real()};
    }
  }
  // Special cases to properly contaminate multiplications with NonReal
  if (IsNonReal(d) || IsNonReal(c)) {
    std::complex<T> other = IsNonReal(d) ? c : d;
    if (IsNonReal(other) ||
        (std::isfinite(other.real()) && std::isfinite(other.imag()))) {
      // NonReal * NonReal = NonReal
      // Finite complex * NonReal = NonReal
      return Private::NonReal<T>();
    }
  }
  // Other cases are left to the standard library, and might return NaN.
  return c * d;
}

template <typename T>
static std::complex<T> FloatDivision(std::complex<T> c, std::complex<T> d) {
  if (d.real() == 0 && d.imag() == 0) {
    return NAN;
  }
  // Special case to prevent (inf,0)/(1,0) from returning (inf, nan).
  if (std::isinf(std::abs(c)) || std::isinf(std::abs(d))) {
    // Handle case when d is pure imaginary/real
    if (d.imag() == 0) {
      return {c.real() / d.real(), c.imag() / d.real()};
    }
    if (d.real() == 0) {
      return {c.imag() / d.imag(), -c.real() / d.imag()};
    }
    // Other cases are left to the standard library, and might return NaN.
  }
  return c / d;
}

/* TODO: users of this function just want to test equality of branch and do not
 * need the index */
template <typename T>
int IndexOfActivePiecewiseBranchAt(const Tree* piecewise, T x) {
  assert(piecewise->isPiecewise());
  Private::Context context;
  context.localEvaluator.pushVariable(x);
  const Tree* branch =
      Private::Approximator(Precision<T>::value, SystemExpressionPolicy{},
                            std::move(context))
          .selectPiecewiseBranch(piecewise);
  /* SelectPiecewiseBranch may return KUndefSystem to signal no active
   * branches */
  return !branch                  ? k_unknownActiveBranch
         : branch == KUndefSystem ? k_noActiveBranch
                                  : piecewise->indexOfChild(branch);
}

Tree* ExtractRealPart(const Tree* e) {
  if (GetComplexProperties(e).isReal()) {
    return e->cloneTree();
  }
  std::complex<double> value =
      Approximation::To<std::complex<double>>(e, Parameters{});
  return SharedTreeStack->pushDoubleFloat(value.real());
}

}  // namespace Poincare::Internal::Approximation

namespace Poincare::Internal::Approximation::Private {

// Utility method to cleanup the tree stack at the end of the Process method
template <typename ReturnType>
ReturnType CleanUpTreeStack(Tree* processed, ReturnType result) {
  assert(processed);
  processed->removeTree();
  return result;
}

Tree* CleanUpTreeStack(Tree* processed, Tree* result) {
  /* If "result" is a Tree*, "processed" (which is the tree just before
   * "result") is removed first (if it is not empty) from the TreeStack, then
   * the "processed" pointer points to "result" data and can be returned.
   * This is done to avoid using a TreeRef for "processed". For other return
   * types, this problem doesn't happen */
  assert(processed);
  assert(processed->nextTree() == result);
  processed->removeTree();
  return processed;
}

bool CheckDimension(const Tree* e,
                    const ExpressionVariantParameters& expressionParameters,
                    bool optimize) {
  const SymbolContext& symbolContext =
      expressionParameters.expressionType() == ExpressionType::User
          ? expressionParameters.get<UserParameters>().context
          : k_emptySymbolContext;
  if (!optimize) {
    return Dimension::DeepCheck(e, symbolContext);
  }
  COSTLY_ASSERT(Dimension::DeepCheck(e, symbolContext));
  return true;
}

FloatPrecision UpdateFloatPrecision(const Tree* e,
                                    FloatPrecision currentFloatPrecision,
                                    bool onlyReplaceNumerical) {
  // Should we do it even without onlyReplaceNumerical?
  if (onlyReplaceNumerical && currentFloatPrecision == FloatPrecision::Double &&
      e->hasDescendantSatisfying(
          [](const Tree* e) { return e->isSingleFloat(); })) {
    // Prevent float to double conversion
    return FloatPrecision::Single;
  }
  return currentFloatPrecision;
}

template <typename ReturnType, typename... ContextArguments>
ReturnType Process(const Tree* e, FloatPrecision floatPrecision,
                   const Parameters& parameters,
                   ContextArguments... contextArguments) {
  if (!CheckDimension(e, parameters.expressionParameters,
                      parameters.policy.optimize)) {
    return UndefinedUnhandledDimension<ReturnType>();
  }
  Tree* processed = PreprocessTree(e, parameters.expressionParameters);

#if POINCARE_NO_FLOAT_APPROXIMATION
  floatPrecision = FloatPrecision::Double;
#else
  floatPrecision = UpdateFloatPrecision(e, floatPrecision,
                                        parameters.policy.onlyReplaceNumerical);
#endif
  Approximator approximator =
      Approximator{floatPrecision, parameters.expressionParameters,
                   Context(contextArguments...)};
  const Tree* targetTree = processed ? processed : e;

#if POINCARE_NO_FLOAT_APPROXIMATION
  /* With this flag, all computations are done using the double
   * templates and result is casted back to ReturnType at the end. */
  using ComputationType = typename FloatToDouble<ReturnType>::type;
#else
  using ComputationType = ReturnType;
#endif

  ComputationType result =
      parameters.policy.onlyReplaceNumerical
          ? approximator.approximateScalars<ComputationType>(targetTree)
          : approximator.specificApproximateTo<ComputationType>(targetTree);
  if (!processed) {
    return result;
  }
  return CleanUpTreeStack(processed, result);
}

Tree* PreprocessTree(const Tree* e,
                     const ExpressionVariantParameters& expressionParameters) {
  // Only clone if necessary
  Tree* clone = nullptr;
  switch (expressionParameters.expressionType()) {
    case ExpressionType::User: {
      assert(Simplification::CanBeUser(e));
      clone = e->cloneTree();
      Variables::ProjectLocalVariablesToId(clone);
      break;
    }
    case ExpressionType::System: {
      assert(Simplification::CanBeSystem(e));
      if (expressionParameters.get<SystemExpressionPolicy>().prepare) {
        if (!clone) {
          clone = e->cloneTree();
        }
        PrepareExpressionForApproximation(clone);
      }
      break;
    }
    case ExpressionType::PreparedFunction: {
      // Nothing to do, expression is already prepared for approximation
      assert(Simplification::CanBePreparedFunction(e));
      break;
    }
    case ExpressionType::Unspecified: {
      // Nothing can be done
      break;
    }
    default:
      OMG_UNREACHABLE
  }
  return clone;
}

template <>
Tree* UndefinedUnhandledDimension() {
  return KUndefUnhandledDimension->cloneTree();
}

template <>
std::complex<float> UndefinedUnhandledDimension() {
  return std::complex<float>{NAN};
}

template <>
std::complex<double> UndefinedUnhandledDimension() {
  return std::complex<double>{NAN};
}

template <>
PointOrRealScalar<float> UndefinedUnhandledDimension() {
  return PointOrRealScalar<float>(NAN, NAN);
}

template <>
PointOrRealScalar<double> UndefinedUnhandledDimension() {
  return PointOrRealScalar<double>(NAN, NAN);
}

template <>
BooleanOrUndefined UndefinedUnhandledDimension() {
  return BooleanOrUndefined::Undef();
}

template <>
float UndefinedUnhandledDimension() {
  return NAN;
}

template <>
double UndefinedUnhandledDimension() {
  return NAN;
}

template <>
Coordinate2D<float> UndefinedUnhandledDimension() {
  return Coordinate2D<float>{NAN, NAN};
}

template <>
Coordinate2D<double> UndefinedUnhandledDimension() {
  return Coordinate2D<double>{NAN, NAN};
}

template <typename T>
std::complex<T> ToUndefinedComplexValue(DefinedStatus status) {
  switch (status) {
    case DefinedStatus::NonReal:
      return NonReal<T>();
    case DefinedStatus::Undefined:
      return std::complex<T>(NAN);
    case DefinedStatus::Defined:
    default:
      OMG_UNREACHABLE
  }
}

Private::Approximator::InternalUserParameters::InternalUserParameters(
    const UserParameters& userParameters)
    : complexFormat{userParameters.complexFormat},
      angleUnit{userParameters.angleUnit},
      symbolContext{userParameters.context} {}

Private::Approximator::InternalUserParameters
Private::Approximator::InternalUserParameters::Make(
    const ExpressionVariantParameters& expressionParameters) {
  switch (expressionParameters.expressionType()) {
    case ExpressionType::User: {
      return InternalUserParameters(expressionParameters.get<UserParameters>());
    }
    case ExpressionType::System:
    case ExpressionType::PreparedFunction:
    case ExpressionType::Unspecified: {
      // ComplexFormat and AngleUnit will be None, symbol context will be Empty
      return InternalUserParameters{};
    }
    default:
      OMG_UNREACHABLE
  }
}

Private::Approximator::Approximator(
    FloatPrecision floatPrecision,
    const ExpressionVariantParameters& expressionParameters, Context&& context)
    : m_floatPrecision(floatPrecision),
      m_parameters{InternalUserParameters::Make(expressionParameters)},
      m_context(std::move(context)) {}

template <>
Tree* Approximator::specificApproximateTo(const Tree* e) {
  COSTLY_ASSERT(Dimension::DeepCheck(e, m_parameters.symbolContext));
  Dimension dim = Dimension::Get(e, m_parameters.symbolContext);
  int listLength = Dimension::ListLength(e, m_parameters.symbolContext);
  if (listLength != Dimension::k_nonListListLength) {
    assert(!dim.isMatrix());
    Tree* result = SharedTreeStack->pushList(listLength);
    assert(m_context.listElement == Context::k_unspecifiedListElement);
    for (int i = 0; i < listLength; i++) {
      m_context.listElement = i;
      privateToTree(e, dim);
    }
    m_context.listElement = Context::k_unspecifiedListElement;
    return result;
  }
  return privateToTree(e, dim);
}

template <>
std::complex<float> Approximator::specificApproximateTo(const Tree* e) {
  return toComplex<float>(e);
};

template <>
std::complex<double> Approximator::specificApproximateTo(const Tree* e) {
  return toComplex<double>(e);
};

template <>
PointOrRealScalar<float> Approximator::specificApproximateTo(const Tree* e) {
  assert(m_floatPrecision == FloatPrecision::Single);
  return toPointOrRealScalar<float>(e);
}

template <>
PointOrRealScalar<double> Approximator::specificApproximateTo(const Tree* e) {
  assert(m_floatPrecision == FloatPrecision::Double);
  return toPointOrRealScalar<double>(e);
}

template <>
BooleanOrUndefined Approximator::specificApproximateTo(const Tree* e) {
  COSTLY_ASSERT(Dimension::DeepCheck(e, m_parameters.symbolContext));
  return privateToBoolean(e);
};

template <>
float Approximator::specificApproximateTo(const Tree* e) {
  assert(m_floatPrecision == FloatPrecision::Single);
  return toFloat<float>(e);
};

template <>
double Approximator::specificApproximateTo(const Tree* e) {
  assert(m_floatPrecision == FloatPrecision::Double);
  return toFloat<double>(e);
};

template <>
Coordinate2D<float> Approximator::specificApproximateTo(const Tree* e) {
  assert(m_floatPrecision == FloatPrecision::Single);
  return privateToCoordinates<float>(e);
}

template <>
Coordinate2D<double> Approximator::specificApproximateTo(const Tree* e) {
  assert(m_floatPrecision == FloatPrecision::Double);
  return privateToCoordinates<double>(e);
}

template <typename FloatType>
std::complex<FloatType> Approximator::toComplex(const Tree* e) {
  assert(Precision<FloatType>::value == m_floatPrecision);
  COSTLY_ASSERT(Dimension::DeepCheck(e, m_parameters.symbolContext));
  assert(Dimension::IsNonListScalar(e, m_parameters.symbolContext));
  return privateToComplex<FloatType>(e);
}

template <typename FloatType>
PointOrRealScalar<FloatType> Approximator::toPointOrRealScalar(const Tree* e) {
  assert(m_floatPrecision == Precision<FloatType>::value);
  COSTLY_ASSERT(Dimension::DeepCheck(e, m_parameters.symbolContext));
  Dimension dim = Dimension::Get(e, m_parameters.symbolContext);
  assert(dim.isScalar() || dim.isPoint() || dim.isUnit());
  return dim.isPoint() ? PointOrRealScalar<FloatType>(
                             specificApproximateTo<Coordinate2D<FloatType>>(e))
                       : PointOrRealScalar<FloatType>(
                             specificApproximateTo<FloatType>(e));
}

template <typename FloatType>
FloatType Approximator::toFloat(const Tree* e) {
  COSTLY_ASSERT(Dimension::DeepCheck(e, m_parameters.symbolContext));
  // Units are tolerated in scalar approximation (replaced with SI ratios).
#if ASSERTIONS && !OPTIMIZE_ASSERTIONS
  Dimension dim = Dimension::Get(e, m_parameters.symbolContext);
  assert(dim.isScalar() || dim.isUnit());
#endif
  COSTLY_ASSERT(m_context.listElement != Context::k_unspecifiedListElement ||
                !Dimension::IsList(e, m_parameters.symbolContext));
  return privateToFloat<FloatType>(e);
}

template <typename T>
Coordinate2D<T> Approximator::privateToCoordinates(const Tree* e) {
  COSTLY_ASSERT(Dimension::DeepCheck(e, m_parameters.symbolContext) &&
                Dimension::Get(e, m_parameters.symbolContext).isPoint());
  T xScalar, yScalar;
  assert(m_context.pointElement == Context::k_unspecifiedPointElement);
  m_context.pointElement = 0;
  xScalar = privateToFloat<T>(e);
  m_context.pointElement = 1;
  yScalar = privateToFloat<T>(e);
  m_context.pointElement = Context::k_unspecifiedPointElement;
  return Coordinate2D<T>(xScalar, yScalar);
}

Tree* Approximator::toList(const Tree* e) {
  assert(m_context.listElement == Context::k_unspecifiedListElement);
  Dimension dimension = Dimension::Get(e, m_parameters.symbolContext);
  int length = Dimension::ListLength(e, m_parameters.symbolContext);
  assert(length != Dimension::k_nonListListLength);
  Tree* list = SharedTreeStack->pushList(length);
  for (int i = 0; i < length; i++) {
    m_context.listElement = i;
    privateToTree(e, dimension);
  }
  m_context.listElement = Context::k_unspecifiedListElement;
  return list;
}

Tree* Approximator::privateToPoint(const Tree* e) {
  assert(m_context.pointElement == Context::k_unspecifiedPointElement);
  Tree* point = SharedTreeStack->pushPoint();
  m_context.pointElement = 0;
  m_floatPrecision == FloatPrecision::Single ? toComplexTree<SmallerFloat>(e)
                                             : toComplexTree<double>(e);
  m_context.pointElement = 1;
  m_floatPrecision == FloatPrecision::Single ? toComplexTree<SmallerFloat>(e)
                                             : toComplexTree<double>(e);
  m_context.pointElement = Context::k_unspecifiedPointElement;
  return point;
}

template <typename T>
T Approximator::toLocalContext(const Tree* e, T x) {
  if (m_context.localEvaluator.isFull()) {
    return UndefinedUnhandledDimension<T>();
  }
  m_context.localEvaluator.pushVariable(x);
  T result = privateToFloat<T>(e);
  m_context.localEvaluator.leaveCurrentScope();
  return result;
}

template <typename T>
std::complex<T> Approximator::privateToComplex(const Tree* e) {
#ifdef POINCARE_MAX_TREE_SIZE_FOR_APPROXIMATION
  if (e->maxDepth() > POINCARE_MAX_TREE_SIZE_FOR_APPROXIMATION) {
    // Prevent recursive approximation algorithms from overflowing the stack
    return NAN;
  }
#endif
#if POINCARE_NO_FLOAT_APPROXIMATION
  /* TODO: this could be a static_assert and the static_cast can be removed once
   * all template instantiation with float (for example toComplex<float>) are
   * removed from projects which use POINCARE_NO_FLOAT_APPROXIMATION */
  assert(Precision<T>::value == FloatPrecision::Double);
  return static_cast<std::complex<T>>(privateToComplexRecursive<double>(e));
#else
  return privateToComplexRecursive<T>(e);
#endif
}

template <typename T>
T Approximator::privateToFloat(const Tree* e) {
#if ASSERTIONS && !OPTIMIZE_ASSERTIONS
  Dimension dim = Dimension::Get(e, m_parameters.symbolContext);
  assert(dim.isScalar() || dim.isPoint() || dim.isUnit());
  // listElement is needed to approximate lists to scalars
  assert(m_context.listElement != Context::k_unspecifiedListElement ||
         !Dimension::IsList(e, m_parameters.symbolContext));
  // pointElement is needed to approximate points to scalars
  assert(m_context.pointElement != Context::k_unspecifiedPointElement ||
         !dim.isPoint());
#endif
  std::complex<T> value = privateToComplex<T>(e);
  // Remove signaling nan
  return value.imag() == 0 && !IsNonReal(value) ? value.real() : NAN;
}

Tree* Approximator::privateToTree(const Tree* e, Dimension dim) {
  /* TODO_PCJ: not all approximation methods come here, but this should always
   * be handled when approximating. */
  if (Projection::HasForbiddenDescendants(e)) {
    return KForbidden->cloneTree();
  }

  if (dim.isBoolean()) {
#if POINCARE_BOOLEAN
    BooleanOrUndefined boolean = privateToBoolean(e);
    if (boolean.isUndefined()) {
      return KUndefBoolean->cloneTree();
    }
    return (boolean.value() ? KTrue : KFalse)->cloneTree();
#else
    OMG_UNREACHABLE;
#endif
  }
  if (dim.isUnit()) {
    // Preserve units and only replace scalar values.
    Tree* result = e->cloneTree();
    approximateAndReplaceEveryScalar(result);
    return result;
  }
  if (dim.isScalar() ||
      (dim.isPoint() &&
       (m_context.pointElement != Context::k_unspecifiedPointElement))) {
    return m_floatPrecision == FloatPrecision::Single
               ? toComplexTree<SmallerFloat>(e)
               : toComplexTree<double>(e);
  }
  assert(dim.isPoint() || dim.isMatrix());
  Tree* result = dim.isPoint() ? privateToPoint(e) : toMatrix(e);
  Undefined::ShallowBubbleUpUndef(result);
  return result;
}

template <typename T>
Tree* Approximator::toComplexTree(const Tree* e) {
  std::complex<T> value = privateToComplex<T>(e);
  T re = value.real(), im = value.imag();
  if (std::isnan(re) || std::isnan(im)) {
    return IsNonReal(value) ? KNonReal->cloneTree() : KUndef->cloneTree();
  }
  if (im == 0.0) {
    return SharedTreeStack->pushFloat(re);
  }
  Tree* result = Tree::FromBlocks(SharedTreeStack->lastBlock());
  // Real part
  if (re != 0.0) {
    SharedTreeStack->pushAdd(2);
    SharedTreeStack->pushFloat(re);
  }
  // Complex part.
  // Push im even if it is 1.0 to ensure the expression is not used as exact.
  SharedTreeStack->pushMult(2);
  SharedTreeStack->pushFloat(im);
  SharedTreeStack->pushComplexI();
  return result;
}

template <typename T>
static bool MergeChildrenOfMultOrAdd(Tree* e) {
  assert(e->isMult() || e->isAdd());
  T merge = (e->isMult() ? 1.0 : 0.0);
  int lastFloatIndex = -1;
  int n = e->numberOfChildren();
  int i = 0;
  Tree* child = e->nextNode();
  while (i < n) {
    if (child->isFloat()) {
      T childValue = child->isSingleFloat() ? FloatHelper::FloatTo(child)
                                            : FloatHelper::DoubleTo(child);
      merge = (e->isMult() ? merge * childValue : merge + childValue);
      lastFloatIndex = i;
      child->removeTree();
      n--;
      NAry::SetNumberOfChildren(e, n);
    } else if (child->type() == e->type()) {
      // Merge Mult/Add that may have appeared
      n += child->numberOfChildren() - 1;
      child->removeNode();
      NAry::SetNumberOfChildren(e, n);
    } else {
      child = child->nextTree();
      i++;
    }
  }
  if (lastFloatIndex != -1) {
    /* To preserve order, push the merged children at an index where there was a
     * float */
    NAry::AddChildAtIndex(e, SharedTreeStack->pushFloat(merge), lastFloatIndex);
    NAry::SquashIfPossible(e);
    return true;
  }
  return false;
}

bool SkipApproximation(TypeBlock type, FloatPrecision floatPrecision) {
  if (type.isComplexI() || type.isSingleFloat()) {
    return true;
  }
  if (type.isDoubleFloat()) {
    /* If the precision is Single, double floats in the expressions are degraded
     * to single floats to avoid having mixed levels of precision. */
    return (floatPrecision == FloatPrecision::Double);
  }
  return false;
}

bool SkipApproximation(TypeBlock type, FloatPrecision floatPrecision,
                       TypeBlock parentType, int indexInParent,
                       bool previousChildWasApproximated) {
  if (SkipApproximation(type, floatPrecision)) {
    return true;
  }
  switch (parentType) {
    case Type::ATrig:
    case Type::Trig:
      // Do not approximate second term in case tree isn't replaced.
      return indexInParent == 1;
    case Type::Pow:
    case Type::PowReal:
      // Only approximate power's index if the entire tree can be approximated.
      return indexInParent == 1 && !previousChildWasApproximated;
    case Type::ListSlice:
    case Type::ListElement:
      assert(indexInParent < 1 || previousChildWasApproximated);
      return indexInParent >= 1 && !previousChildWasApproximated;
    case Type::ListSequence:
      return indexInParent == Parametric::k_valueIndex;
    case Type::RandIntNoRep:
      /* Never approximate last child of RandIntNoRep since it won't be
       * approximated, and an integer child is expected in dimension check. */
      return indexInParent == 2;
    case Type::Identity:
      return true;
    default:
      return false;
  }
}

template <typename ReturnType>
ReturnType Approximator::approximateScalars(const Tree*) {
  // This method only makes sense for a Tree* return type
  assert(false);
  OMG_UNREACHABLE;
}

template <>
Tree* Approximator::approximateScalars(const Tree* e) {
  Tree* eClone = e->cloneTree();
  approximateAndReplaceEveryScalar(eClone);
  return eClone;
}

void Approximator::approximateAndReplaceEveryScalar(Tree* e, bool isRoot) {
  /* If e is not the root, SkipApproximation has already been checked before
   * entering the function */
  if (isRoot && SkipApproximation(e->type(), m_floatPrecision)) {
    return;
  }
  if (Approximation::CanApproximate(e) &&
      Dimension::IsNonListScalar(e, m_parameters.symbolContext)) {
    e->moveTreeOverTree(privateToTree(e, Dimension(DimensionType::Scalar)));
    return;
  }
  bool previousChildWasApproximated = false;
  for (IndexedChild<Tree*> child : e->indexedChildren()) {
    if (!SkipApproximation(child->type(), m_floatPrecision, e->type(),
                           child.index, previousChildWasApproximated)) {
      approximateAndReplaceEveryScalar(child, false);
    }
    previousChildWasApproximated = child->isFloat();
  }
  Undefined::ShallowBubbleUpUndef(e);
  // Merge addition and multiplication's children while preserving order
  if (e->isAdd() || e->isMult()) {
    m_floatPrecision == FloatPrecision::Single
        ? MergeChildrenOfMultOrAdd<SmallerFloat>(e)
        : MergeChildrenOfMultOrAdd<double>(e);
  }
}

template <typename T>
std::complex<T> Approximator::privateToComplexRecursive(const Tree* e) {
#if POINCARE_NO_FLOAT_APPROXIMATION
  /* TODO: this could be a static_assert once all template instantiation with
   * float (for example toComplex<float>) are removed from projects which use
   * POINCARE_NO_FLOAT_APPROXIMATION */
  assert(Precision<T>::value == FloatPrecision::Double);
#endif
  std::complex<T> value = toComplexSwitch<T>(e);
  if (m_parameters.complexFormat == ComplexFormat::Real && value.imag() != 0 &&
      !(Undefined::IsUndefined(value)) && !e->isComplexI()) {
    /* Some operations in reduction can introduce i, but when complex format is
     * real and the factor of i approximates to 0, we don't want to return
     * nonreal. We thus decided not to return nonreal when we approximate i.
     * TODO: tolerate any complex encountered in approximation intermediary
     * steps? */
    return NonReal<T>();
  }
  // We used to flush negative zeros here but it was not worth
  return value;
}

template <typename T>
std::complex<T> Approximator::basicToComplex(const Tree* e) {
  switch (e->type()) {
    case Type::Parentheses:
      return privateToComplexRecursive<T>(e->child(0));
    case Type::ComplexI:
      return std::complex<T>(0, 1);
    case Type::Add: {
      std::complex<T> result = 0;
      for (const Tree* child : e->children()) {
        result += privateToComplexRecursive<T>(child);
      }
      return result;
    }
    case Type::Mult: {
      std::complex<T> result = 1;
      for (const Tree* child : e->children()) {
        result =
            FloatMultiplication<T>(result, privateToComplexRecursive<T>(child));
      }
      return result;
    }
    case Type::Div:
      return FloatDivision<T>(privateToComplexRecursive<T>(e->child(0)),
                              privateToComplexRecursive<T>(e->child(1)));
    case Type::Sub:
      return privateToComplexRecursive<T>(e->child(0)) -
             privateToComplexRecursive<T>(e->child(1));
    case Type::PowReal:
    case Type::Pow: {
      return approximatePower<T>(
          e, e->isPowReal() ? ComplexFormat::Real : m_parameters.complexFormat);
    }
    case Type::GCD:
    case Type::LCM: {
      const Tree* child = e->child(0);
      T result = PositiveIntegerApproximation(privateToFloat<T>(child));
      if (std::isnan(result)) {
        return NAN;
      }
      for (int n = e->numberOfChildren() - 1; n > 0; n--) {
        child = child->nextTree();
        T current = PositiveIntegerApproximation(privateToFloat<T>(child));
        if (std::isnan(current)) {
          return NAN;
        }
        bool overflow = false;
        result = e->isGCD() ? Arithmetic::GCD(result, current)
                            : Arithmetic::LCM(result, current, &overflow);
        if (overflow || !IsIntegerRepresentationAccurate(result)) {
          return NAN;
        }
      }
      return result;
    }
    case Type::Sqrt: {
      std::complex<T> c = privateToComplexRecursive<T>(e->child(0));
      if (c == std::complex<T>(0)) {
        return 0;
      }
      /* With c real <0 and |c| big enough, the approximation errors of
       * std::sqrt become so big that [NeglectRealOrImaginaryPartIfNegligible]
       * fails to remove them, we use the std::sqrt of real for this case */
      if (c.imag() == 0) {
        /* √c with "c" real is:
         * 1. √c if c>0
         * 2. √(-c)*i if c<0 */
        return c.real() > 0 ? std::sqrt(c.real())
                            : std::complex<T>(0, std::sqrt(-c.real()));
      }
      return NeglectRealOrImaginaryPartIfNegligible(std::sqrt(c), c);
    }
    case Type::Root: {
      return approximateRoot<T>(e);
    }
    case Type::Exp:
      return std::exp(privateToComplexRecursive<T>(e->child(0)));
    case Type::Log:
    case Type::Ln: {
      return ComplexLogarithm<T>(privateToComplex<T>(e->child(0)), e->isLog());
    }
    case Type::LogBase: {
      std::complex<T> a = privateToComplexRecursive<T>(e->child(0));
      std::complex<T> b = privateToComplexRecursive<T>(e->child(1));
      return FloatDivision(ComplexLogarithm<T>(a, false),
                           ComplexLogarithm<T>(b, false));
    }
    case Type::Abs:
      return std::abs(privateToComplexRecursive<T>(e->child(0)));
    case Type::Arg:
      return std::arg(privateToComplexRecursive<T>(e->child(0)));
    case Type::Inf:
      return INFINITY;
    case Type::Conj:
      return std::conj(privateToComplexRecursive<T>(e->child(0)));
    case Type::Opposite:
      return FloatMultiplication<T>(-1,
                                    privateToComplexRecursive<T>(e->child(0)));
    case Type::Re: {
      /* TODO_PCJ: Complex NAN should be used in most of the code. Make sure a
       * NAN result cannot be lost. */
      // TODO_PCJ: We used to ignore NAN imag part and return just c.real()
      // TODO: undef are not bubbled-up?
      std::complex<T> c = privateToComplexRecursive<T>(e->child(0));
      return std::isnan(c.imag()) ? NAN : c.real();
    }
    case Type::Im: {
      // TODO: why not use std::im(c)?
      // TODO: undef are not bubbled-up?
      std::complex<T> c = privateToComplexRecursive<T>(e->child(0));
      return std::isnan(c.real()) ? NAN : c.imag();
    }
    default:
      OMG_UNREACHABLE;
  }
}

template <typename T>
std::complex<T> Approximator::allTrigToComplex(const Tree* e) {
  switch (e->type()) {
    case Type::Cos:
    case Type::Sin:
    case Type::Tan:
    case Type::Sec:
    case Type::Csc:
    case Type::Cot:
    case Type::ACos:
    case Type::ASin:
    case Type::ATan:
    case Type::ASec:
    case Type::ACsc:
    case Type::ACot:
      return TrigonometricToComplex(e->type(),
                                    privateToComplexRecursive<T>(e->child(0)),
                                    m_parameters.angleUnit);
    case Type::SinH:
    case Type::CosH:
    case Type::TanH:
    case Type::ArSinH:
    case Type::ArCosH:
    case Type::ArTanH:
      return HyperbolicToComplex(e->type(),
                                 privateToComplexRecursive<T>(e->child(0)));
    case Type::Trig:
    case Type::ATrig: {
      std::complex<T> a = privateToComplexRecursive<T>(e->child(0));
      std::complex<T> b = privateToComplexRecursive<T>(e->child(1));
      assert(b == static_cast<T>(0.0) || b == static_cast<T>(1.0));
      bool isCos = b == static_cast<T>(0.0);
      if (e->isTrig()) {
        return TrigonometricToComplex(isCos ? Type::Cos : Type::Sin, a,
                                      AngleUnit::Radian);
      }
      return TrigonometricToComplex(isCos ? Type::ACos : Type::ASin, a,
                                    AngleUnit::Radian);
    }
    case Type::ATanRad:
      return TrigonometricToComplex(Type::ATan,
                                    privateToComplexRecursive<T>(e->child(0)),
                                    AngleUnit::Radian);
    default:
      OMG_UNREACHABLE;
  }
}

template <typename T>
std::complex<T> Approximator::userSymbolToComplex(const Tree* e) {
  switch (e->type()) {
    case Type::Var: {
      // Local variable
      // TODO: Use template with LocalEvaluator
      std::complex<double> z =
          m_context.localEvaluator.variable(Variables::Id(e));
      return std::complex<T>(z.real(), z.imag());
    }
    case Type::UserFunction:
    case Type::UserVariable: {
      // TODO: Factorize with privateToBoolean and toMatrix
      const Tree* definition =
          m_parameters.symbolContext.expressionForSymbol(e);
      if (!definition) {
        return NAN;
      }
      if (e->isUserVariable()) {
        return privateToComplex<T>(definition);
      }
      /* LocalEvaluator only handles complex scalar, but non scalar children of
       * UserFunction are forbidden in dimension check anyway. */
      std::complex<T> x = privateToComplexRecursive<T>(e->child(0));
      if (std::isnan(x.real()) || std::isnan(x.imag())) {
        return NAN;
      }
      // Only approximate child once and use LocalEvaluator
      if (m_context.localEvaluator.isFull()) {
        return UndefinedUnhandledDimension<std::complex<T>>();
      }
      Tree* definitionClone = definition->cloneTree();
      Variables::ReplaceUserVariable(definitionClone, KUnknownSymbol, 0,
                                     ComplexProperties::Unknown());
      m_context.localEvaluator.pushVariable(x);
      std::complex<T> result = privateToComplex<T>(definitionClone);
      m_context.localEvaluator.leaveCurrentScope();
      definitionClone->removeTree();
      return result;
    }
    case Type::UserSequence: {
      T rank = privateToFloat<T>(e->child(0));
      if (std::isnan(rank) || std::floor(rank) != rank) {
        return NAN;
      }
      return static_cast<T>(
          SequenceApproximationHelper::ApproximateSequenceAtRank(
              Symbol::GetName(e), static_cast<int>(rank)));
    }
    default:
      OMG_UNREACHABLE;
  }
}

// Extract the lower and upper bounds of a Parametric Sum or Product
template <typename T>
Approximator::ParametricBoundOutput Approximator::getParametricBoundOutput(
    const Tree* e) {
  const Tree* lowerBoundChild = e->child(Parametric::k_lowerBoundIndex);
  std::complex<T> low = privateToComplex<T>(lowerBoundChild);
  if (low.imag() != 0 || (int)low.real() != low.real()) {
    return {0, 0, false};
  }
  assert(!Undefined::IsUndefined(low));
  const Tree* upperBoundChild = lowerBoundChild->nextTree();
  std::complex<T> up = privateToComplex<T>(upperBoundChild);
  if (up.imag() != 0 || (int)up.real() != up.real()) {
    return {0, 0, false};
  }
  assert(!Undefined::IsUndefined(up));
  return {static_cast<int>(low.real()), static_cast<int>(up.real()), true};
}

template <typename T>
std::complex<T> Approximator::analysisToComplex(const Tree* e) {
  switch (e->type()) {
    case Type::Sum:
    case Type::Product: {
      ParametricBoundOutput bounds = getParametricBoundOutput<T>(e);
      if (!bounds.isDefined) {
        return NAN;
      }
      // Cloning here to avoid modifying function argument `e`
      static_assert(Parametric::k_sumArgumentIndex ==
                    Parametric::k_prodArgumentIndex);
      if (m_context.localEvaluator.isFull()) {
        return UndefinedUnhandledDimension<std::complex<T>>();
      }
      Tree* child = e->child(Parametric::k_sumArgumentIndex)->cloneTree();
      /* We approximate scalars here to avoid approximating complex
       * constants on every round of the sum/product computation */
      approximateAndReplaceEveryScalar(child);
      m_context.localEvaluator.pushVariable(NAN);
      std::complex<T> result = e->isSum() ? 0 : 1;
      for (int k = bounds.low; k <= bounds.up; k++) {
        m_context.localEvaluator.setLocalValue(k);
        // Reset random context
        m_context.randomContext = Random::Context(true);
        std::complex<T> value = privateToComplexRecursive<T>(child);
        if (e->isSum()) {
          result += value;
        } else {
          result *= value;
        }
        if (Undefined::IsUndefined(result)) {
          break;
        }
      }
      m_context.localEvaluator.leaveCurrentScope();
      child->removeTree();
      return result;
    }
    case Type::Diff: {
      constexpr static int k_maxOrderForApproximation = 4;
      T orderReal = privateToFloat<T>(e->child(2));
      if (orderReal != std::floor(orderReal)) {
        return NAN;
      }
      int order = orderReal;
      const Tree* derivand = e->child(3);
      if (order < 0) {
        return NAN;
      }
      if (order > k_maxOrderForApproximation) {
        /* FIXME:
         * Since approximation of higher order derivative is exponentially
         * complex, we set a threshold above which we won't compute the
         * derivative.
         *
         * The method we use for now for the higher order derivatives is to
         * recursively approximate the derivatives of lower levels.
         * It's as if we approximated diff(diff(diff(diff(..(diff(f(x)))))))
         * But this is method is way too expensive in time and memory.
         *
         * Other methods exists for approximating higher order derivative.
         * This should be investigated
         * */
        return NAN;
      }
      std::complex<T> at = privateToComplexRecursive<T>(e->child(1));
      if (std::isnan(at.real()) || at.imag() != 0) {
        return NAN;
      }
      T result = approximateDerivative(derivand, at.real(), order);
      return result;
    }
    case Type::Integral:
      // TODO: assert(false) if we enforce preparation before approximation
    case Type::IntegralWithAlternatives:
      return approximateIntegral<T>(e);
    default:
      OMG_UNREACHABLE;
  }
}

template <typename T>
std::complex<T> Approximator::matrixToComplex(const Tree* e) {
  switch (e->type()) {
    case Type::Norm:
    case Type::Det: {
      Tree* m = toMatrix(e->child(0));
      Tree* value = nullptr;
      if (e->isDet()) {
        if (!Matrix::RowCanonize(m, true, &value, this)) {
          value = KUndefUnhandled->cloneTree();
        }
      } else {
        assert(e->isNorm());
        value = Vector::Norm(m);
      }
      std::complex<T> v = privateToComplexRecursive<T>(value);
      value->removeTree();
      m->removeTree();
      return v;
    }
    case Type::Trace:
      return approximateTrace<T>(e->child(0));
    case Type::Dot: {
      // TODO use complex conjugate ?
      Tree* u = toMatrix(e->child(0));
      Tree* v = toMatrix(e->child(1));
      Tree* r = Vector::Dot(u, v);
      std::complex<T> result = privateToComplexRecursive<T>(r);
      r->removeTree();
      v->removeTree();
      u->removeTree();
      return result;
    }
    case Type::Point:
      assert(m_context.pointElement != Context::k_unspecifiedPointElement);
      return privateToComplexRecursive<T>(e->child(m_context.pointElement));
    default:
      OMG_UNREACHABLE;
  }
}

template <typename T>
std::complex<T> Approximator::listToComplex(const Tree* e) {
  switch (e->type()) {
    case Type::List: {
      assert(m_context.listElement != Context::k_unspecifiedListElement);
      std::complex<T> result =
          privateToComplexRecursive<T>(e->child(m_context.listElement));
      return result;
    }
    case Type::ListSequence: {
      assert(m_context.listElement != Context::k_unspecifiedListElement);
      if (m_context.localEvaluator.isFull()) {
        return UndefinedUnhandledDimension<std::complex<T>>();
      }
      // epsilon sequences starts at one
      m_context.localEvaluator.pushVariable(m_context.listElement + 1);
      // Reset random context
      m_context.randomContext = Random::Context();
      std::complex<T> result =
          privateToComplexRecursive<T>(e->child(Parametric::FunctionIndex(e)));
      m_context.localEvaluator.leaveCurrentScope();
      return result;
    }
    case Type::Dim: {
      int n = Dimension::ListLength(e->child(0), m_parameters.symbolContext);
      return n >= 0 ? n : NAN;
    }
    case Type::ListElement: {
      const Tree* values = e->child(0);
      const Tree* index = e->child(1);
      T indexValue = privateToFloat<T>(index);
      // Dimension check should only allow finite index
      assert(std::isfinite(indexValue));
      assert(indexValue == std::floor(indexValue));
      if (indexValue < 1 ||
          indexValue >
              Dimension::ListLength(values, m_parameters.symbolContext)) {
        return NAN;
      }
      assert(indexValue >= 1 && indexValue <= INT16_MAX);
      assert(m_context.listElement == Context::k_unspecifiedListElement);
      m_context.listElement = static_cast<int16_t>(indexValue) - 1;
      std::complex<T> result = privateToComplexRecursive<T>(values);
      m_context.listElement = Context::k_unspecifiedListElement;
      return result;
    }
    case Type::ListSlice: {
      assert(m_context.listElement != Context::k_unspecifiedListElement);
      const Tree* values = e->child(0);
      const Tree* startIndex = e->child(1);
      assert(Integer::Is<uint8_t>(startIndex));
      assert(Integer::Is<uint8_t>(e->child(2)));
      int start = std::max(Integer::Handler(startIndex).to<uint8_t>() - 1, 0);
      assert(start >= 0);
      int16_t currentListElement = m_context.listElement;
      m_context.listElement += start;
      std::complex<T> result = privateToComplexRecursive<T>(values);
      m_context.listElement = currentListElement;
      return result;
    }
    case Type::ListSum:
    case Type::ListProduct: {
      const Tree* values = e->child(0);
      int length = Dimension::ListLength(values, m_parameters.symbolContext);
      std::complex<T> result = e->isListSum() ? 0 : 1;
      assert(m_context.listElement == Context::k_unspecifiedListElement);
      for (int i = 0; i < length; i++) {
        m_context.listElement = i;
        std::complex<T> v = privateToComplexRecursive<T>(values);
        result = e->isListSum() ? result + v : result * v;
      }
      m_context.listElement = Context::k_unspecifiedListElement;
      return result;
    }
    case Type::Min:
    case Type::Max: {
      const Tree* values = e->child(0);
      int length = Dimension::ListLength(values, m_parameters.symbolContext);
      assert(length > 0);
      T result = 0.0;
      assert(m_context.listElement == Context::k_unspecifiedListElement);
      for (int i = 0; i < length; i++) {
        m_context.listElement = i;
        std::complex<T> v = privateToComplexRecursive<T>(values);
        if (v.imag() != 0 || std::isnan(v.real())) {
          m_context.listElement = Context::k_unspecifiedListElement;
          return NAN;
        }
        if (i == 0 ||
            (e->isMin() ? (v.real() < result) : (v.real() > result))) {
          result = v.real();
        }
      }
      m_context.listElement = Context::k_unspecifiedListElement;
      return result;
    }
    case Type::Mean:
    case Type::StdDev:
    case Type::SampleStdDev:
    case Type::Variance: {
      const Tree* values = e->child(0);
      const Tree* coefficients = e->child(1);
      int length = Dimension::ListLength(values, m_parameters.symbolContext);
      std::complex<T> sum = 0;
      std::complex<T> sumOfSquares = 0;
      T coefficientsSum = 0;
      assert(m_context.listElement == Context::k_unspecifiedListElement);
      for (int i = 0; i < length; i++) {
        m_context.listElement = i;
        std::complex<T> v = privateToComplexRecursive<T>(values);
        std::complex<T> c = privateToComplexRecursive<T>(coefficients);
        if (c.imag() != 0 || c.real() < 0) {
          m_context.listElement = Context::k_unspecifiedListElement;
          return NAN;
        }
        sum += c.real() * v;
        // TODO v * conj(v) ?
        sumOfSquares += c.real() * v * v;
        coefficientsSum += c.real();
      }
      m_context.listElement = Context::k_unspecifiedListElement;
      if (coefficientsSum == 0) {
        return NAN;
      }
      sum /= coefficientsSum;
      if (e->isMean()) {
        return sum;
      }
      sumOfSquares /= coefficientsSum;
      std::complex<T> var = sumOfSquares - sum * sum;
      if (e->isVariance()) {
        return var;
      }
      std::complex<T> stdDev = std::pow(var, std::complex<T>(0.5));
      if (e->isStdDev()) {
        return stdDev;
      }
      T sampleStdDevCoef =
          std::pow(1 + 1 / (coefficientsSum - 1), static_cast<T>(0.5));
      return stdDev * sampleStdDevCoef;
    }
    case Type::ListSort: {
      /* TODO we are computing all elements and sorting the list for all
       * elements, this is awful */
      assert(m_context.listElement != Context::k_unspecifiedListElement);
      // Temporarily restore whole list in Context for calling ToList
      int16_t currentListElement = m_context.listElement;
      m_context.listElement = Context::k_unspecifiedListElement;
      /* Point element (if any) must be temporarily unspecified too so that
       * sorting applies to the entire point */
      int16_t currentPointElement = m_context.pointElement;
      m_context.pointElement = Context::k_unspecifiedPointElement;
      Tree* list = toList(e->child(0));
      m_context.listElement = currentListElement;
      m_context.pointElement = currentPointElement;
      // TODO: Remove this clone
      Tree* sortedList = nullptr;
      ExceptionTry {
        sortedList = list->cloneTree();
        NAry::SortList(sortedList, Order::OrderType::RealLine);
      }
      ExceptionCatch(exc) {
        if (exc == ExceptionType::SortFail) {
          /* The approximation returns {undef,…,undef} instead of undef but
           * the list should have been sorted during the reduction in most
           * cases. */
          sortedList =
              Undefined::CreateDimensionedUndef(KUndefUnhandled, e->child(0));
        } else {
          TreeStackCheckpoint::Raise(exc);
        }
      }
      assert(sortedList);
      list->moveTreeOverTree(sortedList);
      std::complex<T> result = privateToComplexRecursive<T>(list);
      list->removeTree();
      return result;
    }
    case Type::Median: {
      Tree* list = toList(e->child(0));
      TreeDatasetColumn<T> values(list);
      T median;
      if (Dimension::IsList(e->child(1), m_parameters.symbolContext)) {
        Tree* weightsList = toList(e->child(1));
        TreeDatasetColumn<T> weights(weightsList);
        median = StatisticsDataset<T>(&values, &weights).median();
        weightsList->removeTree();
      } else {
        median = StatisticsDataset<T>(&values).median();
      }
      list->removeTree();
      return median;
    }
    default:
      OMG_UNREACHABLE;
  }
}

template <typename T>
std::complex<T> Approximator::toComplexSwitch(const Tree* e) {
  /* TODO: the second part of this function and several ifs in different cases
   * act differently / more precisely on reals. We should have a dedicated,
   * faster, simpler and more precise real approximation to be used in every
   * cases where we know for sure there are no complexes. */
  assert(e->isExpression());
  if (e->isUndefined()) {
    // TODO: Find a way to pass exact undef type up to toComplexTree.
    return e->isNonReal() ? NonReal<T>() : NAN;
  }
  if (e->isNumber()) {
    return Number::To<T>(e);
  }

  if (e->isRandomized()) {
    return approximateRandom<T>(e);
  }
  /* This method was split into many smaller method to reduce its stack frame.
   * When a method is called, it takes up a certain amount of space on the
   * stack. This amount is particularly relevant here, in a recursive function.
   * Before the split, this method had a stack frame of ~4200 bytes in release,
   * and ~8500 bytes in debug (the impact of the -Os flag). Our stack size on
   * device is 32K, so we could only make a couple of calls before overflowing
   * in the previous section (the buffer zone .heap). This led to a stack
   * overflow crash when approximating deep trees, which could happen easily
   * when entering a formula with Ans and repeatedly pressing EXE
   * (i.e.: √(6Ans+19) EXE EXE EXE ...)
   *
   * Splitting this method in many smaller one ensures the compiler
   * manages to better optimize the stack frame, and drastically increases the
   * amount of recursive calls doable before reaching the stack limit. */
  switch (e->type()) {
    case Type::Parentheses:
    case Type::ComplexI:
    case Type::Add:
    case Type::Mult:
    case Type::Div:
    case Type::Sub:
    case Type::Pow:
    case Type::PowReal:
    case Type::GCD:
    case Type::LCM:
    case Type::Sqrt:
    case Type::Root:
    case Type::Exp:
    case Type::Log:
    case Type::Ln:
    case Type::LogBase:
    case Type::Abs:
    case Type::Arg:
    case Type::Inf:
    case Type::Conj:
    case Type::Opposite:
    case Type::Re:
    case Type::Im:
      return basicToComplex<T>(e);
    case Type::Cos:
    case Type::Sin:
    case Type::Tan:
    case Type::Sec:
    case Type::Csc:
    case Type::Cot:
    case Type::ACos:
    case Type::ASin:
    case Type::ATan:
    case Type::ASec:
    case Type::ACsc:
    case Type::ACot:
    case Type::SinH:
    case Type::CosH:
    case Type::TanH:
    case Type::ArSinH:
    case Type::ArCosH:
    case Type::ArTanH:
    case Type::Trig:
    case Type::ATrig:
    case Type::ATanRad:
      return allTrigToComplex<T>(e);
    case Type::Var:
    case Type::UserFunction:
    case Type::UserVariable:
    case Type::UserSequence:
      return userSymbolToComplex<T>(e);
    case Type::Sum:
    case Type::Product:
    case Type::Diff:
    case Type::Integral:
    case Type::IntegralWithAlternatives:
      // TODO: Explicit parametrics if they contain random nodes?
      return analysisToComplex<T>(e);
    case Type::Norm:
    case Type::Det:
    case Type::Trace:
    case Type::Dot:
    case Type::Point:
      return matrixToComplex<T>(e);
    case Type::List:
    case Type::ListSequence:
    case Type::Dim:
    case Type::ListElement:
    case Type::ListSlice:
    case Type::ListSum:
    case Type::ListProduct:
    case Type::Min:
    case Type::Max:
    case Type::Mean:
    case Type::StdDev:
    case Type::SampleStdDev:
    case Type::Variance:
    case Type::ListSort:
    case Type::Median:
      return listToComplex<T>(e);
    case Type::AngleUnitContext:
    case Type::Piecewise:
    case Type::Distribution:
    case Type::Dep:
    case Type::NonNull:
    case Type::Real:
    case Type::RealPos:
    case Type::RealInteger:
    case Type::Unit:
    case Type::PhysicalConstant:
    case Type::LnUser:
    case Type::Store:
      return miscToComplex<T>(e);
    default:;
  }
  // The remaining operators are defined only on reals
  // TODO: handle potential types with more than 2 children
  return toComplexSwitchOnlyReal<T>(e);
}

template <typename T>
DefinedStatus Approximator::areDependenciesDefined(const Tree* dep) {
  // Dependency children may have different dimensions.
  DefinedStatus status = DefinedStatus::Defined;
  for (const Tree* child : Dependency::Dependencies(dep)->children()) {
    if (Dimension::IsList(child, m_parameters.symbolContext)) {
      /* Ignore lists in dependencies. This is a temporary solution until we
       * decide how to deal with them properly.
       * TODO decide if they should:
       * - not be here (might be possible once we bubble up lists and randoms
       *   correctly and if we forbid lists in functions)
       * - return NAN if any element of the list is undefined
       * - always return NAN
       * - never return NAN (current solution) */
      continue;
    }
    Dimension dim = Dimension::Get(child, m_parameters.symbolContext);
    if (dim.isScalar()) {
      // Optimize most cases
      std::complex<T> c = privateToComplex<T>(child);
      // Only update to nonreal if there is no undef to respect priority
      if (IsNonReal(c) && status == DefinedStatus::Defined) {
        status = DefinedStatus::NonReal;
      } else if (std::isnan(c.real()) || std::isnan(c.imag())) {
        status = DefinedStatus::Undefined;
      }
    } else {
      Tree* a = privateToTree(
          child, Dimension::Get(child, m_parameters.symbolContext));
      if (a->isUndefined()) {
        status = DefinedStatus::Undefined;
      }
      a->removeTree();
    }
  }
  return status;
}

template <typename T>
std::complex<T> Approximator::miscToComplex(const Tree* e) {
  switch (e->type()) {
    case Type::AngleUnitContext: {
      AngleUnit currentAngleUnit = m_parameters.angleUnit;
      m_parameters.angleUnit = static_cast<AngleUnit>(e->nodeValue(0));
      std::complex<T> result = privateToComplexRecursive<T>(e->child(0));
      m_parameters.angleUnit = currentAngleUnit;
      return result;
    }
    case Type::Piecewise: {
      const Tree* branch = selectPiecewiseBranch(e);
      /* SelectPiecewiseBranch may return KUndefSystem to signal no active
       * branches */
      return !branch || branch->isUndefSystem()
                 ? NAN
                 : privateToComplexRecursive<T>(branch);
    }
    case Type::Distribution: {
      const Tree* child = e->child(0);
      DistributionMethod::Abscissae<T> abscissae;
      DistributionMethod::Type method = DistributionMethod::GetType(e);
      for (int i = 0; i < DistributionMethod::NumberOfParameters(method); i++) {
        std::complex<T> c = privateToComplexRecursive<T>(child);
        if (c.imag() != 0) {
          return NAN;
        }
        abscissae[i] = c.real();
        child = child->nextTree();
      }
      Distribution::ParametersArray<T> parameters;
      Distribution::Type distribType = Distribution::GetType(e);
      for (int i = 0; i < Distribution::NumberOfParameters(distribType); i++) {
        std::complex<T> c = privateToComplexRecursive<T>(child);
        if (c.imag() != 0) {
          return NAN;
        }
        parameters[i] = c.real();
        child = child->nextTree();
      }
      return DistributionMethod::EvaluateAtAbscissa<T>(method, abscissae,
                                                       distribType, parameters);
    }
    case Type::Dep: {
      DefinedStatus status = areDependenciesDefined<T>(e);
      return (status == DefinedStatus::Defined)
                 ? privateToComplexRecursive<T>(Dependency::Main(e))
                 : ToUndefinedComplexValue<T>(status);
    }
    case Type::NonNull: {
      std::complex<T> x = privateToComplexRecursive<T>(e->child(0));
      return x == std::complex<T>(0.0) ? NAN : x;
    }
    case Type::Real: {
      std::complex<T> x = privateToComplexRecursive<T>(e->child(0));
      return x.imag() != 0.0 ? NAN : x;
    }
    case Type::RealPos: {
      std::complex<T> x = privateToComplexRecursive<T>(e->child(0));
      return x.real() < 0.0 || x.imag() != 0.0 ? NonReal<T>() : x;
    }
    case Type::RealInteger: {
      std::complex<T> x = privateToComplexRecursive<T>(e->child(0));
      return x.imag() != 0.0 ? NAN : std::floor(x.real()) == x ? x : NAN;
    }
    /* Handle units as their scalar value in basic SI so prefix and
     * representative homogeneity isn't necessary. Dimension is expected to be
     * handled at this point. */
    case Type::Unit: {
      T approxSI = Units::Unit::GetValue(e);
      /* For angle units, convert to the angle value passed in the user
       * parameters. Do not convert if angle unit is None. */
      return (Units::IsPureAngleUnit(e) &&
              m_parameters.angleUnit != AngleUnit::None)
                 ? Trigonometry::ConvertRadianToAngleUnit<T>(
                       approxSI, m_parameters.angleUnit)
                 : approxSI;
    }
    case Type::PhysicalConstant:
      return PhysicalConstant::GetProperties(e).m_value;
    case Type::LnUser: {
      std::complex<T> childApproximation = privateToComplex<T>(e->child(0));
      if (m_parameters.complexFormat == ComplexFormat::Real &&
          (childApproximation.real() < 0 || childApproximation.imag() != 0)) {
        return NonReal<T>();
      }
      if (childApproximation == std::complex<T>(0.0)) {
        return NAN;
      }
      return std::log(childApproximation);
    }
    case Type::Store:
      return privateToComplexRecursive<T>(e->child(0));
    default:
      OMG_UNREACHABLE;
  }
}

template <typename T>
std::complex<T> Approximator::toComplexSwitchOnlyReal(const Tree* e) {
  assert(e->numberOfChildren() <= 2);
  OMG::StaticVector<T, 2> children;
  for (IndexedChild<const Tree*> childNode : e->indexedChildren()) {
    /* Warning: All children are computed in advance here. Calling another
     * approximation on [e] or its children may exponentially decrease
     * performance in some instances. */
    std::complex<T> app = privateToComplexRecursive<T>(childNode);
    if (app.imag() != 0 || std::isnan(app.real())) {
      return NAN;
    }
    assert(!std::isnan(app.imag()));
    children.push(app.real());
  }
  switch (e->type()) {
    case Type::Decimal:
      return children[0] * std::pow(static_cast<T>(10.0), -children[1]);
    case Type::Sign:
    case Type::SignUser: {
      return std::fabs(children[0]) <= OMG::Float::Epsilon<T>() ? 0
             : children[0] < 0                                  ? -1
                                                                : 1;
    }
    case Type::Floor:
    case Type::Ceil: {
      /* Assume low deviation from natural numbers are errors */
      T delta =
          std::fabs((std::round(children[0]) - children[0]) / children[0]);
      if (delta <= OMG::Float::Epsilon<T>()) {
        return std::round(children[0]);
      }
      return e->isFloor() ? std::floor(children[0]) : std::ceil(children[0]);
    }
    case Type::Frac: {
      return children[0] - std::floor(children[0]);
    }
    case Type::Round: {
      assert(!std::isnan(children[1]));
      if (children[1] != std::round(children[1])) {
        return NAN;
      }
      T err = std::pow(10, std::round(children[1]));
      return std::round(children[0] * err) / err;
    }
    case Type::EuclideanDivision:
    case Type::EuclideanDivisionResult:
    case Type::Quo:
    case Type::Rem: {
      T a = children[0];
      T b = children[1];
      assert(!std::isnan(a) && !std::isnan(b));
      if (a != (int)a || b != (int)b) {
        return NAN;
      }
      // TODO: is this really better than std::remainder ?
      T quotient = b >= 0 ? std::floor(a / b) : -std::floor(a / (-b));
      return e->isRem() ? std::round(a - b * quotient) : quotient;
    }

    case Type::Fact: {
      T n = children[0];
      if (n != std::round(n) || n < 0) {
        return NAN;
      }
      T result = 1;
      for (int i = 1; i <= (int)n; i++) {
        result *= static_cast<T>(i);
        if (std::isinf(result)) {
          return result;
        }
      }
      return std::round(result);
    }
    case Type::Binomial: {
      T n = children[0];
      T k = children[1];
      return FloatBinomial<T>(n, k);
    }
    case Type::Permute: {
      T n = children[0];
      T k = children[1];
      if (n != std::round(n) || k != std::round(k) || n < 0.0f || k < 0.0f) {
        return NAN;
      }
      if (k > n) {
        return 0.0;
      }
      T result = 1;
      for (int i = (int)n - (int)k + 1; i <= (int)n; i++) {
        result *= i;
        if (std::isinf(result) || std::isnan(result)) {
          return result;
        }
      }
      return std::round(result);
    }
    case Type::MixedFraction: {
      T integerPart = children[0];
      T fractionPart = children[1];
      if (fractionPart < 0.0 || integerPart != std::fabs(integerPart)) {
        // TODO how can this happen ?
        return NAN;
      }
      return children[0] + children[1];
    }
    case Type::Factor:
      // Useful for the beautification only
      return children[0];
    case Type::PercentSimple:
      return children[0] / 100.0;
    case Type::PercentAddition:
      return children[0] * (1.0 + children[1] / 100.0);
    default:
      assert(false);
      return NAN;
  }
}

template <typename T>
BooleanOrUndefined Approximator::inequalityToBoolean(const Tree* e) {
  T a = privateToFloat<T>(e->child(0));
  T b = privateToFloat<T>(e->child(1));
  if (std::isnan(a) || std::isnan(b)) {
    return BooleanOrUndefined::Undef();
  }
  if (e->isInferior()) {
    return a < b;
  }
  if (e->isInferiorEqual()) {
    return a <= b;
  }
  if (e->isSuperior()) {
    return a > b;
  }
  assert(e->isSuperiorEqual());
  return a >= b;
}

template <typename T>
BooleanOrUndefined Approximator::comparisonToBoolean(const Tree* e) {
  std::complex<T> a = privateToComplex<T>(e->child(0));
  std::complex<T> b = privateToComplex<T>(e->child(1));
  if (std::isnan(a.real()) || std::isnan(a.imag()) || std::isnan(b.real()) ||
      std::isnan(b.imag())) {
    return BooleanOrUndefined::Undef();
  }
  return e->isEqual() == (a == b);
}

BooleanOrUndefined Approximator::privateToBoolean(const Tree* e) {
#if POINCARE_NO_FLOAT_APPROXIMATION
  assert(m_floatPrecision == FloatPrecision::Double);
#endif
  if (e->isTrue()) {
    return true;
  }
  if (e->isFalse()) {
    return false;
  }
  if (e->isInequality()) {
    switch (m_floatPrecision) {
      case FloatPrecision::Single:
        return inequalityToBoolean<SmallerFloat>(e);
      case FloatPrecision::Double:
        return inequalityToBoolean<double>(e);
    }
  }
  if (e->isComparison()) {
    assert(e->isEqual() || e->isNotEqual());
    switch (m_floatPrecision) {
      case FloatPrecision::Single:
        return comparisonToBoolean<SmallerFloat>(e);
      case FloatPrecision::Double:
        return comparisonToBoolean<double>(e);
    }
  }
  if (e->isPiecewise()) {
    const Tree* branch = selectPiecewiseBranch(e);
    if (!branch || branch->isUndefSystem()) {
      /* SelectPiecewiseBranch may return KUndefSystem to signal no active
       * branches */
      return BooleanOrUndefined::Undef();
    }
    return privateToBoolean(branch);
  }
  if (e->isList()) {
    assert(m_context.listElement != Context::k_unspecifiedListElement);
    return privateToBoolean(e->child(m_context.listElement));
  }
  if (e->isParentheses()) {
    return privateToBoolean(e->child(0));
  }
  if (e->isListSort()) {
    /* TODO we are computing all elements and sorting the list for all
     * elements, this is awful */
    // Temporarily restore whole list in Context for calling toList
    int16_t currentListElement = m_context.listElement;
    m_context.listElement = Context::k_unspecifiedListElement;
    assert(m_context.pointElement == Context::k_unspecifiedListElement);
    Tree* list = toList(e->child(0));
    m_context.listElement = currentListElement;
    NAry::SortList(list);
    BooleanOrUndefined result = privateToBoolean(list);
    list->removeTree();
    return result;
  }
  if (e->isListSequence()) {
    assert(m_context.listElement != Context::k_unspecifiedListElement);
    if (m_context.localEvaluator.isFull()) {
      return UndefinedUnhandledDimension<BooleanOrUndefined>();
    }
    // epsilon sequences starts at one
    m_context.localEvaluator.pushVariable(m_context.listElement + 1);
    // Reset random context
    m_context.randomContext = Random::Context();
    BooleanOrUndefined result =
        privateToBoolean(e->child(Parametric::FunctionIndex(e)));
    m_context.localEvaluator.leaveCurrentScope();
    return result;
  }
  if (Undefined::IsUndefSpecial(e, Undefined::SpecialType::Boolean)) {
    return BooleanOrUndefined::Undef();
  }
  if (e->isDep()) {
    DefinedStatus areDepsDefined = (m_floatPrecision == FloatPrecision::Single)
                                       ? areDependenciesDefined<SmallerFloat>(e)
                                       : areDependenciesDefined<double>(e);
    if (areDepsDefined != DefinedStatus::Defined) {
      return BooleanOrUndefined::Undef();
    }
    return privateToBoolean(Dependency::Main(e));
  }
  if (e->isUserFunction() || e->isUserVariable()) {
    // TODO: Factorize with toMatrix and UserSymbolToComplex
    // Global variable
    const Tree* definition = m_parameters.symbolContext.expressionForSymbol(e);
    // Function of symbol cannot have a boolean dimension without a definition.
    assert(definition);
    if (e->isUserVariable()) {
      return privateToBoolean(definition);
    }
    /* LocalEvaluator only handles complex scalar, but non scalar children of
     * UserFunction are forbidden in dimension check anyway. */

    // Local context uses double precision
    std::complex<double> x = privateToComplex<double>(e->child(0));
    if (std::isnan(x.real()) || std::isnan(x.imag())) {
      return BooleanOrUndefined::Undef();
    }
    // Only approximate child once and use LocalEvaluator
    if (m_context.localEvaluator.isFull()) {
      return UndefinedUnhandledDimension<BooleanOrUndefined>();
    }
    Tree* definitionClone = definition->cloneTree();
    Variables::ReplaceUserVariable(definitionClone, KUnknownSymbol, 0,
                                   ComplexProperties::Unknown());
    m_context.localEvaluator.pushVariable(x);
    BooleanOrUndefined result = privateToBoolean(definitionClone);
    m_context.localEvaluator.leaveCurrentScope();
    definitionClone->removeTree();
    return result;
  }
  assert(e->isLogicalOperator());
  BooleanOrUndefined a = privateToBoolean(e->child(0));
  if (a.isUndefined()) {
    return a;
  }
  if (e->isLogicalNot()) {
    return !(a.value());
  }
  BooleanOrUndefined b = privateToBoolean(e->child(1));
  if (b.isUndefined()) {
    return b;
  }
  switch (e->type()) {
    case Type::LogicalAnd:
      return a.value() && b.value();
    case Type::LogicalNand:
      return !(a.value() && b.value());
    case Type::LogicalOr:
      return a.value() || b.value();
    case Type::LogicalNor:
      return !(a.value() || b.value());
    case Type::LogicalXor:
      return a.value() ^ b.value();
    default:
      OMG_UNREACHABLE;
  }
}

template <typename T>
Tree* Approximator::evaluateMatrixPower(const Tree* e) {
  const Tree* base = e->child(0);
  const Tree* index = base->nextTree();
  T value = privateToFloat<T>(index);
  if (std::isnan(value) || value != std::round(value)) {
    return Undefined::CreateDimensionedUndef(KUndef, e);
  }
  Tree* result = toMatrix(base);
  result->moveTreeOverTree(Matrix::Power(result, value, this));
  return result;
}

Tree* Approximator::toMatrix(const Tree* e) {
  /* TODO: Normal matrix nodes and operations with approximated children are
   * used to carry matrix approximation. A dedicated node that knows its
   * children have a fixed size would be more efficient. */
  if (e->isMatrix()) {
    Tree* m = e->cloneNode();
    for (const Tree* child : e->children()) {
      m_floatPrecision == FloatPrecision::Single
          ? toComplexTree<SmallerFloat>(child)
          : toComplexTree<double>(child);
    }
    return m;
  }
  switch (e->type()) {
#if POINCARE_MATRIX
    case Type::Parentheses:
      return toMatrix(e->child(0));
    case Type::Add: {
      const Tree* child = e->child(0);
      int n = e->numberOfChildren() - 1;
      Tree* result = toMatrix(child);
      while (n--) {
        child = child->nextTree();
        Tree* approximatedChild = toMatrix(child);
        Matrix::Addition(result, approximatedChild, this);
        approximatedChild->removeTree();
        result->removeTree();
      }
      return result;
    }
    case Type::Sub: {
      Tree* a = toMatrix(e->child(0));
      Tree* b = toMatrix(e->child(1));
      b->moveTreeOverTree(Matrix::ScalarMultiplication(-1_e, b, this));
      Matrix::Addition(a, b, this);
      a->removeTree();
      a->removeTree();
      return a;
    }
    case Type::Opposite: {
      Tree* a = toMatrix(e->child(0));
      return a->moveTreeOverTree(Matrix::ScalarMultiplication(-1_e, a, this));
    }
    case Type::Mult: {
      bool resultIsMatrix = false;
      Tree* result = nullptr;
      for (const Tree* child : e->children()) {
        bool childIsMatrix =
            Dimension::Get(child, m_parameters.symbolContext).isMatrix();
        Tree* approx = childIsMatrix
                           ? toMatrix(child)
                           : (m_floatPrecision == FloatPrecision::Single
                                  ? toComplexTree<SmallerFloat>(child)
                                  : toComplexTree<double>(child));
        if (result == nullptr) {
          resultIsMatrix = childIsMatrix;
          result = approx;
          continue;
        }
        /* The multiplication can have both scalar and matrix children. Handle
         * all cases depending on the dimensions of the current result and the
         * next child. */
        if (resultIsMatrix && childIsMatrix) {
          Matrix::Multiplication(result, approx, this);
        } else if (resultIsMatrix) {
          Matrix::ScalarMultiplication(approx, result, this);
        } else if (childIsMatrix) {
          Matrix::ScalarMultiplication(result, approx, this);
        } else {
          Tree* mult = SharedTreeStack->pushMult(2);
          result->cloneTree();
          approx->cloneTree();
          Approximator::toComplexTreeInplace(mult);
        }
        resultIsMatrix |= childIsMatrix;
        approx->removeTree();
        result->removeTree();
      }
      return result;
    }
    case Type::Div: {
      Tree* a = toMatrix(e->child(0));
      Tree* s = KDiv->cloneNode();
      (1_e)->cloneTree();
      e->child(1)->cloneTree();
      m_floatPrecision == FloatPrecision::Single ? toComplexTree<SmallerFloat>(s)
                                                 : toComplexTree<double>(s);
      s->removeTree();
      s->moveTreeOverTree(Matrix::ScalarMultiplication(s, a, this));
      a->removeTree();
      return a;
    }
    case Type::PowReal:
    case Type::Pow: {
      return m_floatPrecision == FloatPrecision::Single
                 ? evaluateMatrixPower<SmallerFloat>(e)
                 : evaluateMatrixPower<double>(e);
    }
    case Type::Inverse:
    case Type::Transpose: {
      Tree* result = toMatrix(e->child(0));
      result->moveTreeOverTree(e->isInverse() ? Matrix::Inverse(result, this)
                                              : Matrix::Transpose(result));
      return result;
    }
    case Type::Identity:
      return Matrix::Identity(e->child(0));
    case Type::Ref:
    case Type::Rref: {
      Tree* result = toMatrix(e->child(0));
      if (!Matrix::RowCanonize(result, e->isRref(), nullptr, this)) {
        Undefined::ReplaceTreePreservingDimension(result, KUndef);
      }
      return result;
    }
    case Type::Dim: {
      Dimension dim = Dimension::Get(e->child(0), m_parameters.symbolContext);
      assert(dim.isMatrix());
      Tree* result = SharedTreeStack->pushMatrix(1, 2);
      switch (m_floatPrecision) {
        case FloatPrecision::Single: {
          SharedTreeStack->pushFloat(SmallerFloat(dim.matrix.rows));
          SharedTreeStack->pushFloat(SmallerFloat(dim.matrix.cols));
          break;
        }
        case FloatPrecision::Double: {
          SharedTreeStack->pushDoubleFloat(double(dim.matrix.rows));
          SharedTreeStack->pushDoubleFloat(double(dim.matrix.cols));
          break;
        }
        default:
          OMG_UNREACHABLE
      }
      return result;
    }
    case Type::Cross: {
      Tree* u = toMatrix(e->child(0));
      Tree* v = toMatrix(e->child(1));
      Vector::Cross(u, v);
      u->removeTree();
      u->removeTree();
      return u;
    }
    case Type::Piecewise: {
      const Tree* branch = selectPiecewiseBranch(e);
      if (!branch || branch->isUndefSystem()) {
        /* SelectPiecewiseBranch may return KUndefSystem to signal no active
         * branches */
        return Undefined::CreateDimensionedUndef(KUndef, e);
      }
      return toMatrix(branch);
    }
    case Type::Dep: {
      DefinedStatus dependencyDefinedStatus =
          m_floatPrecision == FloatPrecision::Single
              ? areDependenciesDefined<SmallerFloat>(e)
              : areDependenciesDefined<double>(e);
      switch (dependencyDefinedStatus) {
        case DefinedStatus::Defined:
          return toMatrix(Dependency::Main(e));
        case DefinedStatus::NonReal:
          return Undefined::CreateDimensionedUndef(KNonReal, e);
        case DefinedStatus::Undefined:
          return Undefined::CreateDimensionedUndef(KUndef, e);
        default:
          OMG_UNREACHABLE
      }
    }
    case Type::UserFunction:
    case Type::UserVariable: {
      // TODO: Factorize with privateToBoolean and UserSymbolToComplex
      // Global variable
      // Function of symbol cannot have a Matrix dimension without a context.
      const Tree* definition =
          m_parameters.symbolContext.expressionForSymbol(e);
      // Function of symbol cannot have a Matrix dimension without a definition.
      assert(definition);
      if (e->isUserVariable()) {
        return toMatrix(definition);
      }
      /* LocalEvaluator only handles complex scalar, but non scalar children of
       * UserFunction are forbidden in dimension check anyway. */
      // LocalEvaluator uses a complex double abscissa
      VariableType x = privateToComplex<double>(e->child(0));
      if (std::isnan(x.real()) || std::isnan(x.imag())) {
        return Undefined::CreateDimensionedUndef(KUndef, e);
      }
      // Only approximate child once and use LocalEvaluator
      if (m_context.localEvaluator.isFull()) {
        return UndefinedUnhandledDimension<Tree*>();
      }
      Tree* definitionClone = definition->cloneTree();
      Variables::ReplaceUserVariable(definitionClone, KUnknownSymbol, 0,
                                     ComplexProperties::Unknown());
      m_context.localEvaluator.pushVariable(x);
      TreeRef result = toMatrix(definitionClone);
      m_context.localEvaluator.leaveCurrentScope();
      definitionClone->removeTree();
      return result;
    }
#if POINCARE_SUM_AND_PRODUCT
    case Type::Sum:
    case Type::Product: {
      ParametricBoundOutput bounds = m_floatPrecision == FloatPrecision::Single
                                         ? getParametricBoundOutput<SmallerFloat>(e)
                                         : getParametricBoundOutput<double>(e);
      if (!bounds.isDefined) {
        return Undefined::CreateDimensionedUndef(KUndef, e);
      }
      // TODO: Factorize with same code in AnalysisToComplex without slowing it
      // Cloning here to avoid modifying function argument `e`
      static_assert(Parametric::k_sumArgumentIndex ==
                    Parametric::k_prodArgumentIndex);
      if (m_context.localEvaluator.isFull()) {
        return UndefinedUnhandledDimension<Tree*>();
      }
      Tree* child = e->child(Parametric::k_sumArgumentIndex)->cloneTree();
      /* We approximate scalars here to avoid approximating complex constants on
       * every round of the sum/product computation */
      approximateAndReplaceEveryScalar(child);
      m_context.localEvaluator.pushVariable(NAN);
      Dimension dim = Dimension::Get(child, m_parameters.symbolContext);
      assert(dim.isMatrix());
      assert(e->isSum() || dim.isSquareMatrix());
      TreeRef result =
          e->isSum() ? Matrix::Zero(dim.matrix) : Matrix::Identity(dim.matrix);
      for (int k = bounds.low; k <= bounds.up; k++) {
        m_context.localEvaluator.setLocalValue(k);
        // Reset random context
        m_context.randomContext = Random::Context(true);
        TreeRef value = toMatrix(child);
        if (e->isSum()) {
          result->moveTreeOverTree(Matrix::Addition(result, value, this));
        } else {
          result->moveTreeOverTree(Matrix::Multiplication(result, value, this));
        }
        // Unlike AnalysisToComplex, skip early undef escape case for simplicity
        value->removeTree();
      }
      m_context.localEvaluator.leaveCurrentScope();
      child->removeTree();
      return result;
    }
#endif
#endif
    default:
      OMG_UNREACHABLE;
  }
  return Undefined::CreateDimensionedUndef(KUndef, e);
}

const Tree* Approximator::selectPiecewiseBranch(const Tree* piecewise) {
  assert(piecewise->isPiecewise());
  int n = piecewise->numberOfChildren();
  int i = 0;
  const Tree* child = piecewise->child(0);
  while (i < n) {
    const Tree* condition = child->nextTree();
    i++;
    if (i == n) {
      return child;
    }
    if (condition->hasDescendantSatisfying(
            [](const Tree* e) { return e->isRandomized(); })) {
      // Skip branch selection on random conditions.
      return nullptr;
    }
    const BooleanOrUndefined conditionEvaluation = privateToBoolean(condition);
    if (conditionEvaluation.isUndefined()) {
      // TODO: return a dimensioned Undef
      return nullptr;
    }
    if (conditionEvaluation.value()) {
      return child;
    }
    child = condition->nextTree();
    i++;
  }
  // No clause matched
  // TODO: return a dimensioned Undef
  return KUndefSystem;
}

bool CanApproximate(const Tree* e, int firstNonApproximableVarId) {
  if (e->isRandomized() || e->isUserVariable() || e->isUserFunction() ||
      (e->isVar() && Variables::Id(e) >= firstNonApproximableVarId) ||
      e->isDepList()) {
    return false;
  }
  int childIndex = 0;
  for (const Tree* child : e->children()) {
    bool enterScope = false;
    if (e->isParametric()) {
      if (childIndex == Parametric::k_variableIndex) {
        // Parametric's variable is only used when layouting, ignore it
        childIndex++;
        continue;
      }
      if (Parametric::IsFunctionIndex(childIndex, e)) {
        enterScope = true;
      }
    }
    if (!CanApproximate(child, firstNonApproximableVarId + enterScope)) {
      return false;
    }
    childIndex++;
  }
  return true;
}

}  // namespace Poincare::Internal::Approximation::Private

namespace Poincare::Internal::Approximation {

template float To(const Tree*, const Parameters&);
template double To(const Tree*, const Parameters&);
template float To(const Tree*, float, const Parameters&);
template double To(const Tree*, double, const Parameters&);
template float To(const Tree*, float, int16_t, const Parameters&);
template double To(const Tree*, double, int16_t, const Parameters&);
template Tree* To(const Tree*, FloatPrecision, const Parameters&);
template std::complex<float> To(const Tree*, const Parameters&);
template std::complex<double> To(const Tree*, const Parameters&);
template Coordinate2D<float> To(const Tree*, const Parameters&);
template Coordinate2D<double> To(const Tree*, const Parameters&);
template PointOrRealScalar<float> To(const Tree*, float, const Parameters&);
template PointOrRealScalar<double> To(const Tree*, double, const Parameters&);
template BooleanOrUndefined To(const Tree*, FloatPrecision, const Parameters&);

template float FloatBinomial(float, float);
template double FloatBinomial(double, double);

#if !POINCARE_NO_FLOAT_APPROXIMATION
template float Private::Approximator::privateToFloat(const Tree*);
template float Private::Approximator::toLocalContext(const Tree*, float);
template std::complex<float> Private::Approximator::privateToComplexRecursive(
    const Tree* e);
template Tree* Private::Approximator::toComplexTree<float>(const Tree*);
#endif
template int IndexOfActivePiecewiseBranchAt(const Tree*, float);

template double Private::Approximator::privateToFloat(const Tree*);
template double Private::Approximator::toLocalContext(const Tree*, double);
template std::complex<double> Private::Approximator::privateToComplexRecursive(
    const Tree* e);
template Tree* Private::Approximator::toComplexTree<double>(const Tree*);
template int IndexOfActivePiecewiseBranchAt(const Tree*, double);

}  // namespace Poincare::Internal::Approximation
