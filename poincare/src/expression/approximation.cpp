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

using namespace Private;

static_assert(!POINCARE_NO_FLOAT_APPROXIMATION ||
                  !(POINCARE_MATRIX || POINCARE_LIST || POINCARE_POINT),
              "the double-only approximation is only available with scalars "
              "and booleans");

template <typename ReturnType>
ReturnType PostProcessPreparedAndResult(Tree* prepared, ReturnType result) {
  assert(prepared);
  prepared->removeTree();
  return result;
}

Tree* PostProcessPreparedAndResult(Tree* prepared, Tree* result) {
  /* If "result" is a Tree*, "prepared" (which is the tree just before "result")
   * is removed first (if it is not empty) from the TreeStack, then the
   * "prepared" pointer points to "result" data and can be returned. This is
   * done to avoid using a TreeRef for "prepared".
   * For other return types, this problem doesn't happen */
  assert(prepared);
  assert(prepared->nextTree() == result);
  prepared->removeTree();
  return prepared;
}

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

template <typename ReturnType, typename FloatType>
ReturnType To(const Tree* e, FloatType abscissa, Parameters params,
              const Context& context) {
  static_assert(
      std::is_same_v<ReturnType, Tree*> ||
      std::is_same_v<ReturnType, BooleanOrUndefined> ||
      std::is_same_v<FloatType, typename FloatTypeAccessor<ReturnType>::type>);
  // TODO: rework the LocalContext creation workflow
  Context contextCopy = context;
  LocalContext localContext(abscissa, contextCopy.m_localContext);
  contextCopy.m_localContext = &localContext;
  return To<ReturnType>(e, Precision<FloatType>::value, params, contextCopy);
}

template <typename ReturnType>
ReturnType To(const Tree* e, Parameters params, const Context& context) {
  using float_type = typename FloatTypeAccessor<ReturnType>::type;
  return To<ReturnType>(e, Precision<float_type>::value, params, context);
}

template <typename ReturnType>
ReturnType To(const Tree* e, FloatPrecision floatPrecision, Parameters params,
              const Context& context) {
  // TODO: we would prefer to have an assertion here
  if (!Dimension::DeepCheck(e, context.m_symbolContext)) {
    return UndefinedUnhandledDimension<ReturnType>();
  }
  Tree* prepared = PrepareTree(e, floatPrecision, params, context);
  ReturnType result = SpecificTo<ReturnType>(prepared ? prepared : e,
                                             floatPrecision, params, context);
  if (!prepared) {
    return result;
  }
  return PostProcessPreparedAndResult(prepared, result);
}

bool Approximate(Tree* e, FloatPrecision floatPrecision, Parameters params,
                 const Context& context) {
  Tree* approximated = To<Tree*>(e, floatPrecision, params, context);
  bool hasChanged = !approximated->treeIsIdenticalTo(e);
  e->moveTreeOverTree(approximated);
  return hasChanged;
}

template <>
Tree* Private::UndefinedUnhandledDimension() {
  return KUndefUnhandledDimension->cloneTree();
}

template <>
Tree* Private::SpecificTo(const Tree* e, FloatPrecision floatPrecision,
                          Parameters params, const Context& context) {
  if (params.onlyReplaceNumerical) {
    Tree* result = e->cloneTree();
    ApproximateAndReplaceEveryScalar(result, floatPrecision, context);
    return result;
  }
  assert(Dimension::DeepCheck(e, context.m_symbolContext));
  Dimension dim = Dimension::Get(e, context.m_symbolContext);
  int listLength = Dimension::ListLength(e, context.m_symbolContext);
  if (listLength != Dimension::k_nonListListLength) {
    assert(!dim.isMatrix());
    Tree* result = SharedTreeStack->pushList(listLength);
    Context listEvaluationContext = context;
    for (int i = 0; i < listLength; i++) {
      listEvaluationContext.m_listElement = i;
      PrivateToTree(e, floatPrecision, dim, &listEvaluationContext);
    }
    return result;
  }
  return PrivateToTree(e, floatPrecision, dim, &context);
}

template <>
std::complex<float> Private::UndefinedUnhandledDimension() {
  return std::complex<float>{NAN};
}

template <>
std::complex<double> Private::UndefinedUnhandledDimension() {
  return std::complex<double>{NAN};
}

template <>
std::complex<float> Private::SpecificTo(const Tree* e,
                                        FloatPrecision floatPrecision,
                                        Parameters params,
                                        const Context& context) {
  assert(floatPrecision == FloatPrecision::Single);
  return ToComplex<float>(e, floatPrecision, params, context);
};

template <>
std::complex<double> Private::SpecificTo(const Tree* e,
                                         FloatPrecision floatPrecision,
                                         Parameters params,
                                         const Context& context) {
  assert(floatPrecision == FloatPrecision::Double);
  return ToComplex<double>(e, floatPrecision, params, context);
};

template <typename FloatType>
std::complex<FloatType> Private::ToComplex(const Tree* e,
                                           FloatPrecision floatPrecision,
                                           Parameters params,
                                           const Context& context) {
  assert(Precision<FloatType>::value == floatPrecision);
  assert(!params.onlyReplaceNumerical);
  assert(Dimension::DeepCheck(e, context.m_symbolContext));
  assert(Dimension::IsNonListScalar(e, context.m_symbolContext));
  return PrivateToComplex<FloatType>(e, &context);
}

template <>
PointOrRealScalar<float> Private::UndefinedUnhandledDimension() {
  return PointOrRealScalar<float>(NAN, NAN);
}

template <>
PointOrRealScalar<double> Private::UndefinedUnhandledDimension() {
  return PointOrRealScalar<double>(NAN, NAN);
}

template <typename FloatType>
PointOrRealScalar<FloatType> Private::ToPointOrRealScalar(
    const Tree* e, FloatPrecision floatPrecision, Parameters params,
    const Context& context) {
  assert(floatPrecision == Precision<FloatType>::value);
  assert(!params.onlyReplaceNumerical);
  assert(Dimension::DeepCheck(e, context.m_symbolContext));
  Dimension dim = Dimension::Get(e, context.m_symbolContext);
  assert(dim.isScalar() || dim.isPoint() || dim.isUnit());
  return dim.isPoint()
             ? PointOrRealScalar<FloatType>(To<Coordinate2D<FloatType>>(
                   e, floatPrecision, params, context))
             : PointOrRealScalar<FloatType>(
                   To<FloatType>(e, floatPrecision, params, context));
}

template <>
PointOrRealScalar<float> Private::SpecificTo(const Tree* e,
                                             FloatPrecision floatPrecision,
                                             Parameters params,
                                             const Context& context) {
  assert(floatPrecision == FloatPrecision::Single);
  return ToPointOrRealScalar<float>(e, floatPrecision, params, context);
}

template <>
PointOrRealScalar<double> Private::SpecificTo(const Tree* e,
                                              FloatPrecision floatPrecision,
                                              Parameters params,
                                              const Context& context) {
  assert(floatPrecision == FloatPrecision::Double);
  return ToPointOrRealScalar<double>(e, floatPrecision, params, context);
}

template <>
BooleanOrUndefined Private::UndefinedUnhandledDimension() {
  return BooleanOrUndefined::Undef();
}

template <>
BooleanOrUndefined Private::SpecificTo(const Tree* e,
                                       FloatPrecision floatPrecision,
                                       Parameters params,
                                       const Context& context) {
  assert(!params.onlyReplaceNumerical);
  assert(Dimension::DeepCheck(e, context.m_symbolContext));
  return PrivateToBoolean(e, floatPrecision, &context);
};

template <>
float Private::UndefinedUnhandledDimension() {
  return NAN;
}

template <>
double Private::UndefinedUnhandledDimension() {
  return NAN;
}

template <typename FloatType>
FloatType Private::ToFloat(const Tree* e, Parameters params,
                           const Context& context) {
  assert(Dimension::DeepCheck(e, context.m_symbolContext));
  // Units are tolerated in scalar approximation (replaced with SI ratios).
#if ASSERTIONS
  Dimension dim = Dimension::Get(e, context.m_symbolContext);
  assert(dim.isScalar() || dim.isUnit());
#endif
  assert(context.m_listElement != -1 ||
         !Dimension::IsList(e, context.m_symbolContext));
  assert(!params.onlyReplaceNumerical);
  if (context.m_localContext && (std::isnan(context.variable(0).real()) ||
                                 std::isnan(context.variable(0).imag()))) {
    return FloatType{NAN};
  }
  return PrivateTo<FloatType>(e, &context);
}

template <>
float Private::SpecificTo(const Tree* e, FloatPrecision floatPrecision,
                          Parameters params, const Context& context) {
  assert(floatPrecision == FloatPrecision::Single);
  return ToFloat<float>(e, params, context);
};

template <>
double Private::SpecificTo(const Tree* e, FloatPrecision floatPrecision,
                           Parameters params, const Context& context) {
  assert(floatPrecision == FloatPrecision::Double);
  return ToFloat<double>(e, params, context);
};

template <>
Coordinate2D<float> Private::UndefinedUnhandledDimension() {
  return Coordinate2D<float>{NAN, NAN};
}

template <>
Coordinate2D<double> Private::UndefinedUnhandledDimension() {
  return Coordinate2D<double>{NAN, NAN};
}

template <>
Coordinate2D<float> Private::SpecificTo(const Tree* e,
                                        FloatPrecision floatPrecision,
                                        Parameters params,
                                        const Context& context) {
  assert(!params.onlyReplaceNumerical);
  assert(floatPrecision == FloatPrecision::Single);
  return PrivateToCoordinates<float>(e, context);
}

template <>
Coordinate2D<double> Private::SpecificTo(const Tree* e,
                                         FloatPrecision floatPrecision,
                                         Parameters params,
                                         const Context& context) {
  assert(!params.onlyReplaceNumerical);
  assert(floatPrecision == FloatPrecision::Double);
  return PrivateToCoordinates<double>(e, context);
}

template <typename T>
Coordinate2D<T> Private::PrivateToCoordinates(const Tree* e,
                                              const Context& context) {
  assert(Dimension::DeepCheck(e, context.m_symbolContext) &&
         Dimension::Get(e, context.m_symbolContext).isPoint());
  if (context.m_localContext && (std::isnan(context.variable(0).real()) ||
                                 std::isnan(context.variable(0).imag()))) {
    return Coordinate2D<T>(NAN, NAN);
  }
  T xScalar, yScalar;
  Context pointEvaluationContext = context;
  pointEvaluationContext.m_pointElement = 0;
  xScalar = PrivateTo<T>(e, &pointEvaluationContext);
  pointEvaluationContext.m_pointElement = 1;
  yScalar = PrivateTo<T>(e, &pointEvaluationContext);
  return Coordinate2D<T>(xScalar, yScalar);
}

template <typename T>
Tree* Private::ToComplexTree(const Tree* e, const Context* ctx) {
  std::complex<T> value = PrivateToComplex<T>(e, ctx);
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

Tree* Private::PrepareTree(const Tree* e, FloatPrecision floatPrecision,
                           Parameters params, const Context& context) {
  // Only clone if necessary
  Tree* clone = nullptr;
  if (params.projectLocalVariables) {
    clone = e->cloneTree();
    Variables::ProjectLocalVariablesToId(clone);
  }
  if (params.prepare) {
    assert(Simplification::IsSystem(e));
    if (!clone) {
      clone = e->cloneTree();
    }
    PrepareExpressionForApproximation(clone);
  }
  return clone;
}

Tree* Private::PrivateToTree(const Tree* e, FloatPrecision floatPrecision,
                             Dimension dim, const Context* ctx) {
  /* TODO_PCJ: not all approximation methods come here, but this should always
   * be handled when approximating. */
  if (Projection::HasForbiddenDescendants(e)) {
    return KForbidden->cloneTree();
  }

  if (dim.isBoolean()) {
#if POINCARE_BOOLEAN
    BooleanOrUndefined boolean = PrivateToBoolean(e, floatPrecision, ctx);
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
    PrivateApproximateAndReplaceEveryScalar(result, floatPrecision, ctx);
    return result;
  }
  if (dim.isScalar()) {
    return floatPrecision == FloatPrecision::Single
               ? ToComplexTree<float>(e, ctx)
               : ToComplexTree<double>(e, ctx);
  }
  assert(dim.isPoint() || dim.isMatrix());
  Tree* result = dim.isPoint() ? PrivateToPoint(e, floatPrecision, ctx)
                               : ToMatrix(e, floatPrecision, ctx);
  Undefined::ShallowBubbleUpUndef(result);
  return result;
}

/* Helpers */

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
    // Other cases are left to the standard library, and might return NaN.
  }
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

template <typename T>
DefinedStatus Private::AreDependenciesDefined(const Tree* dep,
                                              const Context* ctx) {
  assert(ctx);
  // Dependency children may have different dimensions.
  DefinedStatus status = DefinedStatus::Defined;
  for (const Tree* child : Dependency::Dependencies(dep)->children()) {
    if (Dimension::IsList(child, ctx->m_symbolContext)) {
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
    Dimension dim = Dimension::Get(child, ctx->m_symbolContext);
    if (dim.isScalar()) {
      // Optimize most cases
      std::complex<T> c = PrivateToComplex<T>(child, ctx);
      // Only update to nonreal if there is no undef to respect priority
      if (IsNonReal(c) && status == DefinedStatus::Defined) {
        status = DefinedStatus::NonReal;
      } else if (std::isnan(c.real()) || std::isnan(c.imag())) {
        status = DefinedStatus::Undefined;
      }
    } else {
      Tree* a = PrivateToTree(child, Precision<T>::value,
                              Dimension::Get(child, ctx->m_symbolContext), ctx);
      if (a->isUndefined()) {
        status = DefinedStatus::Undefined;
      }
      a->removeTree();
    }
  }
  return status;
}

template <typename T>
std::complex<T> Private::ToUndefinedComplexValue(DefinedStatus status) {
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

template <typename T>
std::complex<T> Private::PrivateToComplex(const Tree* e, const Context* ctx) {
#ifdef POINCARE_MAX_TREE_SIZE_FOR_APPROXIMATION
  if (e->maxDepth() > POINCARE_MAX_TREE_SIZE_FOR_APPROXIMATION) {
    // Prevent recursive approximation algorithms from overflowing the stack
    return NAN;
  }
#endif
#if POINCARE_NO_FLOAT_APPROXIMATION
  return static_cast<std::complex<T>>(
      PrivateToComplexRecursive<double>(e, ctx));
#else
  return PrivateToComplexRecursive<T>(e, ctx);
#endif
}

template <typename T>
std::complex<T> Private::PrivateToComplexRecursive(const Tree* e,
                                                   const Context* ctx) {
#if POINCARE_NO_FLOAT_APPROXIMATION
  static_assert(sizeof(T) == sizeof(double));
#endif
  std::complex<T> value = ToComplexSwitch<T>(e, ctx);
  if (ctx && ctx->m_complexFormat == ComplexFormat::Real && value.imag() != 0 &&
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
std::complex<T> BasicToComplex(const Tree* e, const Context* ctx) {
  switch (e->type()) {
    case Type::Parentheses:
      return PrivateToComplexRecursive<T>(e->child(0), ctx);
    case Type::ComplexI:
      return std::complex<T>(0, 1);
    case Type::Add: {
      std::complex<T> result = 0;
      for (const Tree* child : e->children()) {
        result += PrivateToComplexRecursive<T>(child, ctx);
      }
      return result;
    }
    case Type::Mult: {
      std::complex<T> result = 1;
      for (const Tree* child : e->children()) {
        result = FloatMultiplication<T>(
            result, PrivateToComplexRecursive<T>(child, ctx));
      }
      return result;
    }
    case Type::Div:
      return FloatDivision<T>(PrivateToComplexRecursive<T>(e->child(0), ctx),
                              PrivateToComplexRecursive<T>(e->child(1), ctx));
    case Type::Sub:
      return PrivateToComplexRecursive<T>(e->child(0), ctx) -
             PrivateToComplexRecursive<T>(e->child(1), ctx);
    case Type::Pow: {
      return ApproximatePower<T>(
          e, ctx, ctx ? ctx->m_complexFormat : ComplexFormat::Cartesian);
    }
    case Type::GCD:
    case Type::LCM: {
      const Tree* child = e->child(0);
      T result = PositiveIntegerApproximation(PrivateTo<T>(child, ctx));
      if (std::isnan(result)) {
        return NAN;
      }
      for (int n = e->numberOfChildren() - 1; n > 0; n--) {
        child = child->nextTree();
        T current = PositiveIntegerApproximation(PrivateTo<T>(child, ctx));
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
      std::complex<T> c = PrivateToComplexRecursive<T>(e->child(0), ctx);
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
      return ApproximateRoot<T>(e, ctx);
    }
    case Type::Exp:
      return std::exp(PrivateToComplexRecursive<T>(e->child(0), ctx));
    case Type::Log:
    case Type::Ln: {
      return Private::ComplexLogarithm<T>(PrivateToComplex<T>(e->child(0), ctx),
                                          e->isLog());
    }
    case Type::LogBase: {
      std::complex<T> a = PrivateToComplexRecursive<T>(e->child(0), ctx);
      std::complex<T> b = PrivateToComplexRecursive<T>(e->child(1), ctx);
      return FloatDivision(Private::ComplexLogarithm<T>(a, false),
                           Private::ComplexLogarithm<T>(b, false));
    }
    case Type::Abs:
      return std::abs(PrivateToComplexRecursive<T>(e->child(0), ctx));
    case Type::Arg:
      return std::arg(PrivateToComplexRecursive<T>(e->child(0), ctx));
    case Type::Inf:
      return INFINITY;
    case Type::Conj:
      return std::conj(PrivateToComplexRecursive<T>(e->child(0), ctx));
    case Type::Opposite:
      return FloatMultiplication<T>(
          -1, PrivateToComplexRecursive<T>(e->child(0), ctx));
    case Type::Re: {
      /* TODO_PCJ: Complex NAN should be used in most of the code. Make sure a
       * NAN result cannot be lost. */
      // TODO_PCJ: We used to ignore NAN imag part and return just c.real()
      // TODO: undef are not bubbled-up?
      std::complex<T> c = PrivateToComplexRecursive<T>(e->child(0), ctx);
      return std::isnan(c.imag()) ? NAN : c.real();
    }
    case Type::Im: {
      // TODO: why not use std::im(c)?
      // TODO: undef are not bubbled-up?
      std::complex<T> c = PrivateToComplexRecursive<T>(e->child(0), ctx);
      return std::isnan(c.real()) ? NAN : c.imag();
    }
    default:
      OMG_UNREACHABLE;
  }
}

template <typename T>
std::complex<T> AllTrigToComplex(const Tree* e, const Context* ctx) {
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
      return TrigonometricToComplex(
          e->type(), PrivateToComplexRecursive<T>(e->child(0), ctx),
          ctx ? ctx->m_angleUnit : AngleUnit::Radian);
    case Type::SinH:
    case Type::CosH:
    case Type::TanH:
    case Type::ArSinH:
    case Type::ArCosH:
    case Type::ArTanH:
      return HyperbolicToComplex(
          e->type(), PrivateToComplexRecursive<T>(e->child(0), ctx));
    case Type::Trig:
    case Type::ATrig: {
      std::complex<T> a = PrivateToComplexRecursive<T>(e->child(0), ctx);
      std::complex<T> b = PrivateToComplexRecursive<T>(e->child(1), ctx);
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
      return TrigonometricToComplex(
          Type::ATan, PrivateToComplexRecursive<T>(e->child(0), ctx),
          AngleUnit::Radian);
    default:
      OMG_UNREACHABLE;
  }
}

template <typename T>
std::complex<T> UserSymbolToComplex(const Tree* e, const Context* ctx) {
  switch (e->type()) {
    case Type::Var: {
      // Local variable
      if (!ctx || !ctx->m_localContext) {
        return NAN;
      }
      // TODO: Use template with LocalContext
      std::complex<double> z = ctx->variable(Variables::Id(e));
      return std::complex<T>(z.real(), z.imag());
    }
    case Type::UserFunction:
    case Type::UserVariable: {
      // TODO: Factorize with PrivateToBoolean and ToMatrix
      // Global variable
      if (!ctx) {
        return NAN;
      }
      const Tree* definition = ctx->m_symbolContext.expressionForSymbol(e);
      if (!definition) {
        return NAN;
      }
      if (e->isUserVariable()) {
        return PrivateToComplex<T>(definition, ctx);
      }
      /* LocalContext only handles complex scalar, but non scalar children of
       * UserFunction are forbidden in dimension check anyway. */
      std::complex<T> x = PrivateToComplexRecursive<T>(e->child(0), ctx);
      if (std::isnan(x.real()) || std::isnan(x.imag())) {
        return NAN;
      }
      Tree* definitionClone = definition->cloneTree();
      // Only approximate child once and use local context.
      Variables::ReplaceUserVariable(definitionClone, KUnknownSymbol, 0,
                                     ComplexProperties::Unknown());
      Context ctxCopy = *ctx;
      LocalContext localCtx = LocalContext(x, ctx->m_localContext);
      ctxCopy.m_localContext = &localCtx;
      std::complex<T> result = PrivateToComplex<T>(definitionClone, &ctxCopy);
      definitionClone->removeTree();
      return result;
    }
    case Type::UserSequence: {
      T rank = PrivateTo<T>(e->child(0), ctx);
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

struct ParametricBoundOutput {
  int low;
  int up;
  bool isDefined;
};

// Extract the lower and upper bounds of a Parametric Sum or Product
template <typename T>
ParametricBoundOutput GetParametricBoundOutput(const Tree* e,
                                               const Context* ctx) {
  const Tree* lowerBoundChild = e->child(Parametric::k_lowerBoundIndex);
  std::complex<T> low = PrivateToComplex<T>(lowerBoundChild, ctx);
  if (low.imag() != 0 || (int)low.real() != low.real()) {
    return {0, 0, false};
  }
  assert(!Undefined::IsUndefined(low));
  const Tree* upperBoundChild = lowerBoundChild->nextTree();
  std::complex<T> up = PrivateToComplex<T>(upperBoundChild, ctx);
  if (up.imag() != 0 || (int)up.real() != up.real()) {
    return {0, 0, false};
  }
  assert(!Undefined::IsUndefined(up));
  return {static_cast<int>(low.real()), static_cast<int>(up.real()), true};
}

template <typename T>
std::complex<T> AnalysisToComplex(const Tree* e, const Context* ctx) {
  switch (e->type()) {
    case Type::Sum:
    case Type::Product: {
      ParametricBoundOutput bounds = GetParametricBoundOutput<T>(e, ctx);
      if (!bounds.isDefined) {
        return NAN;
      }
      // Cloning here to avoid modifying function argument `e`
      static_assert(Parametric::k_sumArgumentIndex ==
                    Parametric::k_prodArgumentIndex);
      Tree* child = e->child(Parametric::k_sumArgumentIndex)->cloneTree();
      Context ctxCopy = ctx ? *ctx : Context();
      /* We ApproximateAndReplaceEveryScalar here to avoid approximating complex
       * constants on every round of the sum/product computation */
      ApproximateAndReplaceEveryScalar(child, Precision<T>::value, ctxCopy);
      LocalContext localCtx = LocalContext(NAN, ctxCopy.m_localContext);
      ctxCopy.m_localContext = &localCtx;
      std::complex<T> result = e->isSum() ? 0 : 1;
      for (int k = bounds.low; k <= bounds.up; k++) {
        ctxCopy.setLocalValue(k);
        // Reset random context
        ctxCopy.m_randomContext = Random::Context(true);
        std::complex<T> value = PrivateToComplexRecursive<T>(child, &ctxCopy);
        if (e->isSum()) {
          result += value;
        } else {
          result *= value;
        }
        if (Undefined::IsUndefined(result)) {
          break;
        }
      }
      child->removeTree();
      return result;
    }
    case Type::Diff: {
      constexpr static int k_maxOrderForApproximation = 4;
      T orderReal = PrivateTo<T>(e->child(2), ctx);
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
      std::complex<T> at = PrivateToComplexRecursive<T>(e->child(1), ctx);
      if (std::isnan(at.real()) || at.imag() != 0) {
        return NAN;
      }
      assert(ctx);
      T result = ApproximateDerivative(derivand, at.real(), order, ctx);
      return result;
    }
    case Type::Integral:
      // TODO: assert(false) if we enforce preparation before approximation
    case Type::IntegralWithAlternatives:
      return ApproximateIntegral<T>(e, ctx);
    default:
      OMG_UNREACHABLE;
  }
}

template <typename T>
std::complex<T> MatrixToComplex(const Tree* e, const Context* ctx) {
  switch (e->type()) {
    case Type::Norm:
    case Type::Det: {
      Tree* m = ToMatrix(e->child(0), Precision<T>::value, ctx);
      Tree* value = nullptr;
      if (e->isDet()) {
        if (!Matrix::RowCanonize(m, true, &value, true, ctx)) {
          value = KUndefUnhandled->cloneTree();
        }
      } else {
        assert(e->isNorm());
        value = Vector::Norm(m);
      }
      std::complex<T> v = PrivateToComplexRecursive<T>(value, ctx);
      value->removeTree();
      m->removeTree();
      return v;
    }
    case Type::Trace:
      return ApproximateTrace<T>(e->child(0), ctx);
    case Type::Dot: {
      // TODO use complex conjugate ?
      Tree* u = ToMatrix(e->child(0), Precision<T>::value, ctx);
      Tree* v = ToMatrix(e->child(1), Precision<T>::value, ctx);
      Tree* r = Vector::Dot(u, v);
      std::complex<T> result = PrivateToComplexRecursive<T>(r, ctx);
      r->removeTree();
      v->removeTree();
      u->removeTree();
      return result;
    }
    case Type::Point:
      assert(ctx && ctx->m_pointElement != -1);
      return PrivateToComplexRecursive<T>(e->child(ctx->m_pointElement), ctx);
    default:
      OMG_UNREACHABLE;
  }
}

template <typename T>
std::complex<T> ListToComplex(const Tree* e, const Context* ctx) {
  assert(ctx);
  Context tempCtx(*ctx);
  switch (e->type()) {
    case Type::List:
      assert(ctx->m_listElement != -1);
      return PrivateToComplexRecursive<T>(e->child(ctx->m_listElement), ctx);
    case Type::ListSequence: {
      assert(ctx->m_listElement != -1);
      // epsilon sequences starts at one
      Context ctxCopy = *ctx;
      LocalContext localCtx =
          LocalContext(ctx->m_listElement + 1, ctx->m_localContext);
      ctxCopy.m_localContext = &localCtx;
      // Reset random context
      ctxCopy.m_randomContext = Random::Context();
      return PrivateToComplexRecursive<T>(
          e->child(Parametric::FunctionIndex(e)), &ctxCopy);
    }
    case Type::Dim: {
      int n = Dimension::ListLength(e->child(0), ctx->m_symbolContext);
      return n >= 0 ? n : NAN;
    }
    case Type::ListElement: {
      const Tree* values = e->child(0);
      const Tree* index = e->child(1);
      T indexValue = PrivateTo<T>(index, ctx);
      // Dimension check should only allow finite index
      assert(std::isfinite(indexValue));
      assert(indexValue == std::floor(indexValue));
      if (indexValue < 1 ||
          indexValue > Dimension::ListLength(values, ctx->m_symbolContext)) {
        return NAN;
      }
      assert(indexValue >= 1 && indexValue <= INT16_MAX);
      tempCtx.m_listElement = static_cast<int16_t>(indexValue) - 1;
      std::complex<T> result = PrivateToComplexRecursive<T>(values, &tempCtx);
      ctx->updateRandomContext(tempCtx.m_randomContext);
      return result;
    }
    case Type::ListSlice: {
      assert(ctx->m_listElement != -1);
      const Tree* values = e->child(0);
      const Tree* startIndex = e->child(1);
      assert(Integer::Is<uint8_t>(startIndex));
      assert(Integer::Is<uint8_t>(e->child(2)));
      int start = std::max(Integer::Handler(startIndex).to<uint8_t>() - 1, 0);
      assert(start >= 0);
      tempCtx.m_listElement += start;
      std::complex<T> result = PrivateToComplexRecursive<T>(values, &tempCtx);
      ctx->updateRandomContext(tempCtx.m_randomContext);
      return result;
    }
    case Type::ListSum:
    case Type::ListProduct: {
      const Tree* values = e->child(0);
      int length = Dimension::ListLength(values, ctx->m_symbolContext);
      std::complex<T> result = e->isListSum() ? 0 : 1;
      for (int i = 0; i < length; i++) {
        tempCtx.m_listElement = i;
        std::complex<T> v = PrivateToComplexRecursive<T>(values, &tempCtx);
        result = e->isListSum() ? result + v : result * v;
      }
      ctx->updateRandomContext(tempCtx.m_randomContext);
      return result;
    }
    case Type::Min:
    case Type::Max: {
      const Tree* values = e->child(0);
      int length = Dimension::ListLength(values, ctx->m_symbolContext);
      assert(length > 0);
      T result = 0.0;
      for (int i = 0; i < length; i++) {
        tempCtx.m_listElement = i;
        std::complex<T> v = PrivateToComplexRecursive<T>(values, &tempCtx);
        if (v.imag() != 0 || std::isnan(v.real())) {
          return NAN;
        }
        if (i == 0 ||
            (e->isMin() ? (v.real() < result) : (v.real() > result))) {
          result = v.real();
        }
      }
      ctx->updateRandomContext(tempCtx.m_randomContext);
      return result;
    }
    case Type::Mean:
    case Type::StdDev:
    case Type::SampleStdDev:
    case Type::Variance: {
      const Tree* values = e->child(0);
      const Tree* coefficients = e->child(1);
      int length = Dimension::ListLength(values, ctx->m_symbolContext);
      std::complex<T> sum = 0;
      std::complex<T> sumOfSquares = 0;
      T coefficientsSum = 0;
      for (int i = 0; i < length; i++) {
        tempCtx.m_listElement = i;
        std::complex<T> v = PrivateToComplexRecursive<T>(values, &tempCtx);
        std::complex<T> c =
            PrivateToComplexRecursive<T>(coefficients, &tempCtx);
        if (c.imag() != 0 || c.real() < 0) {
          return NAN;
        }
        sum += c.real() * v;
        // TODO v * conj(v) ?
        sumOfSquares += c.real() * v * v;
        coefficientsSum += c.real();
      }
      ctx->updateRandomContext(tempCtx.m_randomContext);
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
      Tree* list = ToList(e->child(0), Precision<T>::value, ctx);
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
          sortedList = KUndef->cloneTree();
        } else {
          TreeStackCheckpoint::Raise(exc);
        }
      }
      assert(sortedList);
      list->moveTreeOverTree(sortedList);
      std::complex<T> result = PrivateToComplexRecursive<T>(list, ctx);
      list->removeTree();
      return result;
    }
    case Type::Median: {
      Tree* list = ToList(e->child(0), Precision<T>::value, ctx);
      TreeDatasetColumn<T> values(list);
      T median;
      if (Dimension::IsList(e->child(1), ctx->m_symbolContext)) {
        Tree* weightsList = ToList(e->child(1), Precision<T>::value, ctx);
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
std::complex<T> MiscToComplex(const Tree* e, const Context* ctx) {
  switch (e->type()) {
    case Type::AngleUnitContext: {
      Context tempCtx(*ctx);
      tempCtx.m_angleUnit = static_cast<AngleUnit>(e->nodeValue(0));
      return PrivateToComplexRecursive<T>(e->child(0), &tempCtx);
    }
    case Type::Piecewise: {
      const Tree* branch = SelectPiecewiseBranch(e, Precision<T>::value, ctx);
      return branch ? PrivateToComplexRecursive<T>(branch, ctx) : NAN;
    }
    case Type::Distribution: {
      const Tree* child = e->child(0);
      DistributionMethod::Abscissae<T> abscissae;
      DistributionMethod::Type method = DistributionMethod::GetType(e);
      for (int i = 0; i < DistributionMethod::NumberOfParameters(method); i++) {
        std::complex<T> c = PrivateToComplexRecursive<T>(child, ctx);
        if (c.imag() != 0) {
          return NAN;
        }
        abscissae[i] = c.real();
        child = child->nextTree();
      }
      Distribution::ParametersArray<T> parameters;
      Distribution::Type distribType = Distribution::GetType(e);
      for (int i = 0; i < Distribution::NumberOfParameters(distribType); i++) {
        std::complex<T> c = PrivateToComplexRecursive<T>(child, ctx);
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
      DefinedStatus status = AreDependenciesDefined<T>(e, ctx);
      return (status == DefinedStatus::Defined)
                 ? PrivateToComplexRecursive<T>(Dependency::Main(e), ctx)
                 : ToUndefinedComplexValue<T>(status);
    }
    case Type::NonNull: {
      std::complex<T> x = PrivateToComplexRecursive<T>(e->child(0), ctx);
      return x == std::complex<T>(0.0) ? NAN : x;
    }
    case Type::Real: {
      std::complex<T> x = PrivateToComplexRecursive<T>(e->child(0), ctx);
      return x.imag() != 0.0 ? NAN : x;
    }
    case Type::RealPos: {
      std::complex<T> x = PrivateToComplexRecursive<T>(e->child(0), ctx);
      return x.real() < 0.0 || x.imag() != 0.0 ? NonReal<T>() : x;
    }
    case Type::RealInteger: {
      std::complex<T> x = PrivateToComplexRecursive<T>(e->child(0), ctx);
      return x.imag() != 0.0 ? NAN : std::floor(x.real()) == x ? x : NAN;
    }
    /* Handle units as their scalar value in basic SI so prefix and
     * representative homogeneity isn't necessary. Dimension is expected to be
     * handled at this point. */
    case Type::Unit: {
      T approxSI = Units::Unit::GetValue(e);
      assert(ctx);
      /* For angle units, convert to angle value in the context. Do not convert
       * if angle unit is None. */
      return (Units::IsPureAngleUnit(e) && ctx->m_angleUnit != AngleUnit::None)
                 ? Trigonometry::ConvertRadianToAngleUnit<T>(approxSI,
                                                             ctx->m_angleUnit)
                 : approxSI;
    }
    case Type::PhysicalConstant:
      return PhysicalConstant::GetProperties(e).m_value;
    case Type::LnUser: {
      std::complex<T> childApproximation =
          PrivateToComplex<T>(e->child(0), ctx);
      if (ctx && ctx->m_complexFormat == ComplexFormat::Real &&
          (childApproximation.real() < 0 || childApproximation.imag() != 0)) {
        return NonReal<T>();
      }
      if (childApproximation == std::complex<T>(0.0)) {
        return NAN;
      }
      return std::log(childApproximation);
    }
    case Type::Store:
      return PrivateToComplexRecursive<T>(e->child(0), ctx);
    default:
      OMG_UNREACHABLE;
  }
}

template <typename T>
std::complex<T> ToComplexSwitchOnlyReal(const Tree* e, const Context* ctx) {
  assert(e->numberOfChildren() <= 2);
  OMG::StaticVector<T, 2> children;
  for (IndexedChild<const Tree*> childNode : e->indexedChildren()) {
    std::complex<T> app = PrivateToComplexRecursive<T>(childNode, ctx);
    if (app.imag() != 0 || std::isnan(app.real())) {
      return NAN;
    }
    assert(!std::isnan(app.imag()));
    children.push(app.real());
  }
  switch (e->type()) {
    case Type::Decimal:
      return children[0] * std::pow(static_cast<T>(10.0), -children[1]);
    case Type::PowReal: {
      return ApproximatePower<T>(e, ctx, ComplexFormat::Real);
    }
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
std::complex<T> Private::ToComplexSwitch(const Tree* e, const Context* ctx) {
  /* TODO: the second part of this function and several ifs in different cases
   * act differently / more precisely on reals. We should have a dedicated,
   * faster, simpler and more precise real approximation to be used in every
   * cases where we know for sure there are no complexes. */
  assert(e->isExpression());
  if (e->isUndefined()) {
    // TODO: Find a way to pass exact undef type up to ToComplexTree.
    return e->isNonReal() ? NonReal<T>() : NAN;
  }
  if (e->isNumber()) {
    return Number::To<T>(e);
  }

  if (e->isRandomized()) {
    return ApproximateRandom<T>(e, ctx);
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
      return BasicToComplex<T>(e, ctx);
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
      return AllTrigToComplex<T>(e, ctx);
    case Type::Var:
    case Type::UserFunction:
    case Type::UserVariable:
    case Type::UserSequence:
      return UserSymbolToComplex<T>(e, ctx);
    case Type::Sum:
    case Type::Product:
    case Type::Diff:
    case Type::Integral:
    case Type::IntegralWithAlternatives:
      // TODO: Explicit parametrics if they contain random nodes?
      return AnalysisToComplex<T>(e, ctx);
    case Type::Norm:
    case Type::Det:
    case Type::Trace:
    case Type::Dot:
    case Type::Point:
      return MatrixToComplex<T>(e, ctx);
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
      return ListToComplex<T>(e, ctx);
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
      return MiscToComplex<T>(e, ctx);
    default:;
  }
  // The remaining operators are defined only on reals
  // TODO: handle potential types with more than 2 children
  return ToComplexSwitchOnlyReal<T>(e, ctx);
}

template <typename T>
BooleanOrUndefined InequalityToBoolean(const Tree* e, const Context* ctx) {
  T a = PrivateTo<T>(e->child(0), ctx);
  T b = PrivateTo<T>(e->child(1), ctx);
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
BooleanOrUndefined ComparisonToBoolean(const Tree* e, const Context* ctx) {
  std::complex<T> a = PrivateToComplex<T>(e->child(0), ctx);
  std::complex<T> b = PrivateToComplex<T>(e->child(1), ctx);
  if (std::isnan(a.real()) || std::isnan(a.imag()) || std::isnan(b.real()) ||
      std::isnan(b.imag())) {
    return BooleanOrUndefined::Undef();
  }
  return e->isEqual() == (a == b);
}

BooleanOrUndefined Private::PrivateToBoolean(const Tree* e,
                                             FloatPrecision floatPrecision,
                                             const Context* ctx) {
#if POINCARE_NO_FLOAT_APPROXIMATION
  if (floatPrecision == FloatPrecision::Single) {
    return PrivateToBoolean(e, FloatPrecision::Double, ctx);
  }
#endif
  if (e->isTrue()) {
    return true;
  }
  if (e->isFalse()) {
    return false;
  }
  if (e->isInequality()) {
    switch (floatPrecision) {
      case FloatPrecision::Single:
        return InequalityToBoolean<float>(e, ctx);
      case FloatPrecision::Double:
        return InequalityToBoolean<double>(e, ctx);
    }
  }
  if (e->isComparison()) {
    assert(e->isEqual() || e->isNotEqual());
    switch (floatPrecision) {
      case FloatPrecision::Single:
        return ComparisonToBoolean<float>(e, ctx);
      case FloatPrecision::Double:
        return ComparisonToBoolean<double>(e, ctx);
    }
  }
  if (e->isPiecewise()) {
    const Tree* branch = SelectPiecewiseBranch(e, floatPrecision, ctx);
    if (!branch || branch->isUndefined()) {
      // SelectPiecewiseBranch may return KUndef to signal no active branches
      return BooleanOrUndefined::Undef();
    }
    return PrivateToBoolean(branch, floatPrecision, ctx);
  }
  if (e->isList()) {
    assert(ctx && ctx->m_listElement != -1);
    return PrivateToBoolean(e->child(ctx->m_listElement), floatPrecision, ctx);
  }
  if (e->isParentheses()) {
    return PrivateToBoolean(e->child(0), floatPrecision, ctx);
  }
  if (e->isListSort()) {
    /* TODO we are computing all elements and sorting the list for all
     * elements, this is awful */
    Tree* list = ToList(e->child(0), floatPrecision, ctx);
    NAry::SortList(list);
    BooleanOrUndefined result = PrivateToBoolean(list, floatPrecision, ctx);
    list->removeTree();
    return result;
  }
  if (e->isListSequence()) {
    assert(ctx && ctx->m_listElement != -1);
    // epsilon sequences starts at one
    Context ctxCopy = *ctx;
    LocalContext localCtx =
        LocalContext(ctx->m_listElement + 1, ctx->m_localContext);
    ctxCopy.m_localContext = &localCtx;
    // Reset random context
    ctxCopy.m_randomContext = Random::Context();
    return PrivateToBoolean(e->child(Parametric::FunctionIndex(e)),
                            floatPrecision, &ctxCopy);
  }
  if (e->isUndefBoolean()) {
    return BooleanOrUndefined::Undef();
  }
  if (e->isDep()) {
    DefinedStatus areDependenciesDefined =
        (floatPrecision == FloatPrecision::Single)
            ? AreDependenciesDefined<float>(e, ctx)
            : AreDependenciesDefined<double>(e, ctx);
    if (areDependenciesDefined != DefinedStatus::Defined) {
      return BooleanOrUndefined::Undef();
    }
    return PrivateToBoolean(Dependency::Main(e), floatPrecision, ctx);
  }
  if (e->isUserFunction() || e->isUserVariable()) {
    // TODO: Factorize with ToMatrix and UserSymbolToComplex
    // Global variable
    // Function of symbol cannot have a boolean dimension without a context.
    assert(ctx);
    const Tree* definition = ctx->m_symbolContext.expressionForSymbol(e);
    // Function of symbol cannot have a boolean dimension without a definition.
    assert(definition);
    if (e->isUserVariable()) {
      return PrivateToBoolean(definition, floatPrecision, ctx);
    }
    /* LocalContext only handles complex scalar, but non scalar children of
     * UserFunction are forbidden in dimension check anyway. */

    // Local context uses double precision
    std::complex<double> x = PrivateToComplex<double>(e->child(0), ctx);
    if (std::isnan(x.real()) || std::isnan(x.imag())) {
      return BooleanOrUndefined::Undef();
    }
    Tree* definitionClone = definition->cloneTree();
    // Only approximate child once and use local context.
    Variables::ReplaceUserVariable(definitionClone, KUnknownSymbol, 0,
                                   ComplexProperties::Unknown());
    Context ctxCopy = *ctx;
    LocalContext localCtx = LocalContext(x, ctx->m_localContext);
    ctxCopy.m_localContext = &localCtx;
    BooleanOrUndefined result =
        PrivateToBoolean(definitionClone, floatPrecision, &ctxCopy);
    definitionClone->removeTree();
    return result;
  }
  assert(e->isLogicalOperator());
  BooleanOrUndefined a = PrivateToBoolean(e->child(0), floatPrecision, ctx);
  if (a.isUndefined()) {
    return a;
  }
  if (e->isLogicalNot()) {
    return !(a.value());
  }
  BooleanOrUndefined b = PrivateToBoolean(e->child(1), floatPrecision, ctx);
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

Tree* Private::ToList(const Tree* e, FloatPrecision floatPrecision,
                      const Context* ctx) {
  assert(ctx);
  Dimension dimension = Dimension::Get(e, ctx->m_symbolContext);
  int length = Dimension::ListLength(e, ctx->m_symbolContext);
  assert(length != Dimension::k_nonListListLength);
  Context tempCtx(*ctx);
  Tree* list = SharedTreeStack->pushList(length);
  for (int i = 0; i < length; i++) {
    tempCtx.m_listElement = i;
    PrivateToTree(e, floatPrecision, dimension, &tempCtx);
  }
  ctx->updateRandomContext(tempCtx.m_randomContext);
  return list;
}

Tree* Private::PrivateToPoint(const Tree* e, FloatPrecision floatPrecision,
                              const Context* ctx) {
  assert(ctx);
  Context tempCtx(*ctx);
  Tree* point = SharedTreeStack->pushPoint();
  tempCtx.m_pointElement = 0;
  floatPrecision == FloatPrecision::Single ? ToComplexTree<float>(e, &tempCtx)
                                           : ToComplexTree<double>(e, &tempCtx);
  tempCtx.m_pointElement = 1;
  floatPrecision == FloatPrecision::Single ? ToComplexTree<float>(e, &tempCtx)
                                           : ToComplexTree<double>(e, &tempCtx);
  ctx->updateRandomContext(tempCtx.m_randomContext);
  return point;
}

template <typename T>
Tree* EvaluateMatrixPower(const Tree* e, const Context* ctx) {
  const Tree* base = e->child(0);
  const Tree* index = base->nextTree();
  T value = PrivateTo<T>(index, ctx);
  if (std::isnan(value) || value != std::round(value)) {
    return Undefined::CreateTreeWithDimensionedType(e, Type::Undef);
  }
  Tree* result = ToMatrix(base, Precision<T>::value, ctx);
  result->moveTreeOverTree(Matrix::Power(result, value, true, ctx));
  return result;
}

Tree* Private::ToMatrix(const Tree* e, FloatPrecision floatPrecision,
                        const Context* ctx) {
  /* TODO: Normal matrix nodes and operations with approximated children are
   * used to carry matrix approximation. A dedicated node that knows its
   * children have a fixed size would be more efficient. */
  if (e->isMatrix()) {
    Tree* m = e->cloneNode();
    for (const Tree* child : e->children()) {
      floatPrecision == FloatPrecision::Single
          ? ToComplexTree<float>(child, ctx)
          : ToComplexTree<double>(child, ctx);
    }
    return m;
  }
  switch (e->type()) {
#if POINCARE_MATRIX
    case Type::Parentheses:
      return ToMatrix(e->child(0), floatPrecision, ctx);
    case Type::Add: {
      const Tree* child = e->child(0);
      int n = e->numberOfChildren() - 1;
      Tree* result = ToMatrix(child, floatPrecision, ctx);
      while (n--) {
        child = child->nextTree();
        Tree* approximatedChild = ToMatrix(child, floatPrecision, ctx);
        Matrix::Addition(result, approximatedChild, true, ctx);
        approximatedChild->removeTree();
        result->removeTree();
      }
      return result;
    }
    case Type::Sub: {
      Tree* a = ToMatrix(e->child(0), floatPrecision, ctx);
      Tree* b = ToMatrix(e->child(1), floatPrecision, ctx);
      b->moveTreeOverTree(Matrix::ScalarMultiplication(-1_e, b, true, ctx));
      Matrix::Addition(a, b, true, ctx);
      a->removeTree();
      a->removeTree();
      return a;
    }
    case Type::Opposite: {
      Tree* a = ToMatrix(e->child(0), floatPrecision, ctx);
      return a->moveTreeOverTree(
          Matrix::ScalarMultiplication(-1_e, a, true, ctx));
    }
    case Type::Mult: {
      bool resultIsMatrix = false;
      Tree* result = nullptr;
      for (const Tree* child : e->children()) {
        bool childIsMatrix = (ctx ? Dimension::Get(child, ctx->m_symbolContext)
                                  : Dimension::Get(child))
                                 .isMatrix();
        Tree* approx = childIsMatrix
                           ? ToMatrix(child, floatPrecision, ctx)
                           : (floatPrecision == FloatPrecision::Single
                                  ? ToComplexTree<float>(child, ctx)
                                  : ToComplexTree<double>(child, ctx));
        if (result == nullptr) {
          resultIsMatrix = childIsMatrix;
          result = approx;
          continue;
        }
        /* The multiplication can have both scalar and matrix children. Handle
         * all cases depending on the dimensions of the current result and the
         * next child. */
        if (resultIsMatrix && childIsMatrix) {
          Matrix::Multiplication(result, approx, true, ctx);
        } else if (resultIsMatrix) {
          Matrix::ScalarMultiplication(approx, result, true, ctx);
        } else if (childIsMatrix) {
          Matrix::ScalarMultiplication(result, approx, true, ctx);
        } else {
          Tree* mult = SharedTreeStack->pushMult(2);
          result->cloneTree();
          approx->cloneTree();
          Approximation::Private::ToComplexTreeInplace(mult, ctx);
        }
        resultIsMatrix |= childIsMatrix;
        approx->removeTree();
        result->removeTree();
      }
      return result;
    }
    case Type::Div: {
      Tree* a = ToMatrix(e->child(0), floatPrecision, ctx);
      Tree* s = KDiv->cloneNode();
      (1_e)->cloneTree();
      e->child(1)->cloneTree();
      floatPrecision == FloatPrecision::Single ? ToComplexTree<float>(s, ctx)
                                               : ToComplexTree<double>(s, ctx);
      s->removeTree();
      s->moveTreeOverTree(Matrix::ScalarMultiplication(s, a, true, ctx));
      a->removeTree();
      return a;
    }
    case Type::PowReal:
    case Type::Pow: {
      return floatPrecision == FloatPrecision::Single
                 ? EvaluateMatrixPower<float>(e, ctx)
                 : EvaluateMatrixPower<double>(e, ctx);
    }
    case Type::Inverse:
    case Type::Transpose: {
      Tree* result = ToMatrix(e->child(0), floatPrecision, ctx);
      result->moveTreeOverTree(e->isInverse()
                                   ? Matrix::Inverse(result, true, ctx)
                                   : Matrix::Transpose(result));
      return result;
    }
    case Type::Identity:
      return Matrix::Identity(e->child(0));
    case Type::Ref:
    case Type::Rref: {
      Tree* result = ToMatrix(e->child(0), floatPrecision, ctx);
      if (!Matrix::RowCanonize(result, e->isRref(), nullptr, true, ctx)) {
        result->moveTreeOverTree(
            Matrix::Undef(Matrix::GetMatrixDimension(result)));
      }
      return result;
    }
    case Type::Dim: {
      Dimension dim = ctx ? Dimension::Get(e->child(0), ctx->m_symbolContext)
                          : Dimension::Get(e->child(0));
      assert(dim.isMatrix());
      Tree* result = SharedTreeStack->pushMatrix(1, 2);
      switch (floatPrecision) {
        case FloatPrecision::Single: {
          SharedTreeStack->pushSingleFloat(float(dim.matrix.rows));
          SharedTreeStack->pushSingleFloat(float(dim.matrix.cols));
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
      Tree* u = ToMatrix(e->child(0), floatPrecision, ctx);
      Tree* v = ToMatrix(e->child(1), floatPrecision, ctx);
      Vector::Cross(u, v);
      u->removeTree();
      u->removeTree();
      return u;
    }
    case Type::Piecewise: {
      const Tree* branch = SelectPiecewiseBranch(e, floatPrecision, ctx);
      if (!branch || branch->isUndefined()) {
        // SelectPiecewiseBranch may return KUndef to signal no active branches
        return Undefined::CreateTreeWithDimensionedType(e, Type::Undef);
      }
      return ToMatrix(branch, floatPrecision, ctx);
    }
    case Type::Dep: {
      DefinedStatus dependencyDefinedStatus =
          floatPrecision == FloatPrecision::Single
              ? AreDependenciesDefined<float>(e, ctx)
              : AreDependenciesDefined<double>(e, ctx);
      switch (dependencyDefinedStatus) {
        case DefinedStatus::Defined:
          return ToMatrix(Dependency::Main(e), floatPrecision, ctx);
        case DefinedStatus::NonReal:
          return Undefined::CreateTreeWithDimensionedType(e, Type::NonReal);
        case DefinedStatus::Undefined:
          return Undefined::CreateTreeWithDimensionedType(e, Type::Undef);
        default:
          OMG_UNREACHABLE
      }
    }
    case Type::UserFunction:
    case Type::UserVariable: {
      // TODO: Factorize with PrivateToBoolean and UserSymbolToComplex
      // Global variable
      // Function of symbol cannot have a Matrix dimension without a context.
      assert(ctx);
      const Tree* definition = ctx->m_symbolContext.expressionForSymbol(e);
      // Function of symbol cannot have a Matrix dimension without a definition.
      assert(definition);
      if (e->isUserVariable()) {
        return ToMatrix(definition, floatPrecision, ctx);
      }
      /* LocalContext only handles complex scalar, but non scalar children of
       * UserFunction are forbidden in dimension check anyway. */
      // Local context uses a double abscissa
      std::complex<double> x = PrivateToComplex<double>(e->child(0), ctx);
      if (std::isnan(x.real()) || std::isnan(x.imag())) {
        return Undefined::CreateTreeWithDimensionedType(e, Type::Undef);
      }
      Tree* definitionClone = definition->cloneTree();
      // Only approximate child once and use local context.
      Variables::ReplaceUserVariable(definitionClone, KUnknownSymbol, 0,
                                     ComplexProperties::Unknown());
      Context ctxCopy = *ctx;
      LocalContext localCtx = LocalContext(x, ctx->m_localContext);
      ctxCopy.m_localContext = &localCtx;
      TreeRef result = ToMatrix(definitionClone, floatPrecision, &ctxCopy);
      definitionClone->removeTree();
      return result;
    }
#if POINCARE_SUM_AND_PRODUCT
    case Type::Sum:
    case Type::Product: {
      ParametricBoundOutput bounds =
          floatPrecision == FloatPrecision::Single
              ? GetParametricBoundOutput<float>(e, ctx)
              : GetParametricBoundOutput<double>(e, ctx);
      if (!bounds.isDefined) {
        return Undefined::CreateTreeWithDimensionedType(e, Type::Undef);
      }
      // TODO: Factorize with same code in AnalysisToComplex without slowing it
      // Cloning here to avoid modifying function argument `e`
      static_assert(Parametric::k_sumArgumentIndex ==
                    Parametric::k_prodArgumentIndex);
      Tree* child = e->child(Parametric::k_sumArgumentIndex)->cloneTree();
      Context ctxCopy = ctx ? *ctx : Context();
      /* We ApproximateAndReplaceEveryScalar here to avoid approximating complex
       * constants on every round of the sum/product computation */
      ApproximateAndReplaceEveryScalar(child, floatPrecision, ctxCopy);
      LocalContext localCtx = LocalContext(NAN, ctxCopy.m_localContext);
      ctxCopy.m_localContext = &localCtx;
      Dimension dim = Dimension::Get(child, ctxCopy.m_symbolContext);
      assert(dim.isMatrix());
      assert(e->isSum() || dim.isSquareMatrix());
      TreeRef result =
          e->isSum() ? Matrix::Zero(dim.matrix) : Matrix::Identity(dim.matrix);
      for (int k = bounds.low; k <= bounds.up; k++) {
        ctxCopy.setLocalValue(k);
        // Reset random context
        ctxCopy.m_randomContext = Random::Context(true);
        TreeRef value = ToMatrix(child, floatPrecision, &ctxCopy);
        if (e->isSum()) {
          result->moveTreeOverTree(
              Matrix::Addition(result, value, true, &ctxCopy));
        } else {
          result->moveTreeOverTree(
              Matrix::Multiplication(result, value, true, &ctxCopy));
        }
        // Unlike AnalysisToComplex, skip early undef escape case for simplicity
        value->removeTree();
      }
      child->removeTree();
      return result;
    }
#endif
#endif
    default:
      OMG_UNREACHABLE;
  }
  return Undefined::CreateTreeWithDimensionedType(e, Type::Undef);
}

template <typename T>
T Private::PrivateTo(const Tree* e, const Context* ctx) {
#if ASSERTIONS
  assert(ctx);
  Dimension dim = Dimension::Get(e, ctx->m_symbolContext);
  assert(dim.isScalar() || dim.isPoint() || dim.isUnit());
  // m_listElement is needed to approximate lists to scalars
  assert(ctx->m_listElement != -1 ||
         !Dimension::IsList(e, ctx->m_symbolContext));
  // m_pointElement is needed to approximate points to scalars
  assert(ctx->m_pointElement != -1 || !dim.isPoint());
#endif
  std::complex<T> value = PrivateToComplex<T>(e, ctx);
  // Remove signaling nan
  return value.imag() == 0 && !IsNonReal(value) ? value.real() : NAN;
}

template <typename T>
T ToLocalContext(const Tree* e, const Context* ctx, T x) {
  Context ctxCopy = ctx ? Context(*ctx) : Context();
  LocalContext localCtx(x, ctxCopy.m_localContext);
  ctxCopy.m_localContext = &localCtx;
  return PrivateTo<T>(e, &ctxCopy);
}

const Tree* Private::SelectPiecewiseBranch(const Tree* piecewise,
                                           FloatPrecision floatPrecision,
                                           const Context* ctx) {
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
    const BooleanOrUndefined conditionEvaluation =
        PrivateToBoolean(condition, floatPrecision, ctx);
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
  return KUndef;
}

/* TODO: users of this function just want to test equality of branch and do not
 * need the index */
template <typename T>
int IndexOfActivePiecewiseBranchAt(const Tree* piecewise, T x) {
  assert(piecewise->isPiecewise());
  /* TODO: Without randomContext, randomized nodes in piecewise's condition will
   * approximate to NAN. */
  LocalContext localCtx(x);
  Context ctx;
  ctx.m_localContext = &localCtx;
  const Tree* branch =
      SelectPiecewiseBranch(piecewise, Precision<T>::value, &ctx);
  return !branch            ? k_unknownActiveBranch
         : branch == KUndef ? k_noActiveBranch
                            : piecewise->indexOfChild(branch);
}

bool CanApproximate(const Tree* e, bool approxLocalVar) {
  return Private::CanApproximate(e, 0);
}

bool Private::CanApproximate(const Tree* e, int firstNonApproximableVarId) {
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

bool Private::SkipApproximation(TypeBlock type, FloatPrecision floatPrecision) {
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

bool Private::SkipApproximation(TypeBlock type, FloatPrecision floatPrecision,
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

bool ApproximateAndReplaceEveryScalar(Tree* e, FloatPrecision floatPrecision,
                                      const Context& context) {
  // Prevent float to double conversion
  if (floatPrecision == FloatPrecision::Double &&
      e->hasDescendantSatisfying(
          [](const Tree* e) { return e->isSingleFloat(); })) {
    return ApproximateAndReplaceEveryScalar(e, FloatPrecision::Single, context);
  }

  if (SkipApproximation(e->type(), floatPrecision)) {
    return false;
  }
  uint32_t hash = e->hash();
  bool result =
      PrivateApproximateAndReplaceEveryScalar(e, floatPrecision, &context);
  /* TODO: We compare the CRC32 to prevent expressions such as 1.0+i*1.0 from
   * returning true at every call. We should detect and skip them in
   * SkipApproximation instead. */
  return result && hash != e->hash();
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

bool Private::PrivateApproximateAndReplaceEveryScalar(
    Tree* e, FloatPrecision floatPrecision, const Context* ctx) {
  assert(ctx);
  if (Approximation::CanApproximate(e) &&
      Dimension::IsNonListScalar(e, ctx->m_symbolContext)) {
    e->moveTreeOverTree(PrivateToTree(e, floatPrecision,
                                      Dimension(DimensionType::Scalar), ctx));
    return true;
  }
  bool changed = false;
  bool previousChildWasApproximated = false;
  for (IndexedChild<Tree*> child : e->indexedChildren()) {
    if (!SkipApproximation(child->type(), floatPrecision, e->type(),
                           child.index, previousChildWasApproximated)) {
      changed =
          PrivateApproximateAndReplaceEveryScalar(child, floatPrecision, ctx) ||
          changed;
    }
    previousChildWasApproximated = child->isFloat();
  }
  changed = Undefined::ShallowBubbleUpUndef(e) || changed;
  // Merge addition and multiplication's children while preserving order
  if (e->isAdd() || e->isMult()) {
    changed = (floatPrecision == FloatPrecision::Single
                   ? MergeChildrenOfMultOrAdd<float>(e)
                   : MergeChildrenOfMultOrAdd<double>(e)) ||
              changed;
  }
  return changed;
}

Tree* ExtractRealPart(const Tree* e) {
  if (GetComplexProperties(e).isReal()) {
    return e->cloneTree();
  }
  std::complex<double> value = PrivateToComplex<double>(e, nullptr);
  return SharedTreeStack->pushDoubleFloat(value.real());
}

template float To(const Tree*, Parameters, const Context&);
template double To(const Tree*, Parameters, const Context&);
template float To(const Tree*, float, Parameters, const Context&);
template double To(const Tree*, double, Parameters, const Context&);
template Tree* To(const Tree*, FloatPrecision, Parameters, const Context&);
template std::complex<float> To(const Tree*, Parameters, const Context&);
template std::complex<double> To(const Tree*, Parameters, const Context&);
template Coordinate2D<float> To(const Tree*, Parameters, const Context&);
template Coordinate2D<double> To(const Tree*, Parameters, const Context&);
template PointOrRealScalar<float> To(const Tree*, float, Parameters,
                                     const Context&);
template PointOrRealScalar<double> To(const Tree*, double, Parameters,
                                      const Context&);
template BooleanOrUndefined To(const Tree*, FloatPrecision, Parameters,
                               const Context&);

template float FloatBinomial(float, float);
template double FloatBinomial(double, double);

template std::complex<float> Private::PrivateToComplex(const Tree*,
                                                       const Context*);
template std::complex<double> Private::PrivateToComplex(const Tree*,
                                                        const Context*);

#if !POINCARE_NO_FLOAT_APPROXIMATION
template std::complex<float> Private::PrivateToComplexRecursive(const Tree*,
                                                                const Context*);
#endif
template std::complex<double> Private::PrivateToComplexRecursive(
    const Tree*, const Context*);

template float Private::PrivateTo(const Tree*, const Context*);
template double Private::PrivateTo(const Tree*, const Context*);

template float ToLocalContext(const Tree*, const Context*, float);
template double ToLocalContext(const Tree*, const Context*, double);

template int IndexOfActivePiecewiseBranchAt(const Tree*, float);
template int IndexOfActivePiecewiseBranchAt(const Tree*, double);

}  // namespace Poincare::Internal::Approximation
