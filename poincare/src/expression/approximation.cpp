#include "approximation.h"

#include <omg/float.h>
#include <omg/unreachable.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/statistics/distributions/distribution_method.h>
#include <poincare/src/statistics/statistics_dataset.h>
#include <poincare/trigonometry.h>

#include <bit>
#include <cmath>
#include <complex>

#include "arithmetic.h"
#include "beautification.h"
#include "context.h"
#include "decimal.h"
#include "dependency.h"
#include "dimension.h"
#include "float_helper.h"
#include "list.h"
#include "matrix.h"
#include "number.h"
#include "physical_constant.h"
#include "random.h"
#include "rational.h"
#include "simplification.h"
#include "symbol.h"
#include "undefined.h"
#include "units/representatives.h"
#include "units/unit.h"
#include "variables.h"
#include "vector.h"

namespace Poincare::Internal::Approximation {

using namespace Private;

static_assert(!POINCARE_NO_FLOAT_APPROXIMATION ||
                  !(POINCARE_MATRIX || POINCARE_LIST || POINCARE_POINT),
              "the double-only approximation is only available with scalars "
              "and booleans");

template <typename T>
Tree* ToTree(const Tree* e, Parameters params, Context context) {
  if (!Dimension::DeepCheck(e, context.m_symbolContext)) {
    return KUndefUnhandledDimension->cloneTree();
  }
  Tree* clone = PrepareTreeAndContext<T>(e, params, context);
  if (params.optimize) {
    // Tree has already been approximated in PrepareTreeAndContext
    assert(clone);
    return clone;
  }
  const Tree* target = clone ? clone : e;
  Tree* result;
  Dimension dim = Dimension::Get(target, context.m_symbolContext);
  int listLength = Dimension::ListLength(target, context.m_symbolContext);
  if (listLength != Dimension::k_nonListListLength) {
    assert(!dim.isMatrix());
    result = SharedTreeStack->pushList(listLength);
    for (int i = 0; i < listLength; i++) {
      context.m_listElement = i;
      PrivateToTree<T>(target, dim, &context);
    }
  } else {
    result = PrivateToTree<T>(target, dim, &context);
  }
  if (!clone) {
    return result;
  }
  assert(clone->nextTree() == result);
  clone->removeTree();
  return clone;
}

template <typename T>
std::complex<T> ToComplex(const Tree* e, Parameters params, Context context) {
  if (!Dimension::DeepCheck(e, context.m_symbolContext)) {
    return NAN;
  }
  assert(Dimension::IsNonListScalar(e, context.m_symbolContext));
  assert(!params.optimize);
  Tree* clone = PrepareTreeAndContext<T>(e, params, context);
  const Tree* target = clone ? clone : e;
  std::complex<T> c = PrivateToComplex<T>(target, &context);
  if (clone) {
    clone->removeTree();
  }
  return c;
};

template <typename T>
PointOrRealScalar<T> ToPointOrRealScalar(const Tree* e, Parameters params,
                                         Context context) {
  assert(Dimension::DeepCheck(e, context.m_symbolContext));
  assert(!params.optimize);
  Tree* clone = PrepareTreeAndContext<T>(e, params, context);
  const Tree* target = clone ? clone : e;
  Dimension dim = Dimension::Get(target, context.m_symbolContext);
  assert(dim.isScalar() || dim.isPoint() || dim.isUnit());
  PointOrRealScalar<T> result = dim.isScalar() ? PointOrRealScalar<T>(NAN)
                                               : PointOrRealScalar<T>(NAN, NAN);
  if (!context.m_localContext || !(std::isnan(context.variable(0).real()) ||
                                   std::isnan(context.variable(0).imag()))) {
    T xScalar;
    if (dim.isPoint()) {
      context.m_pointElement = 0;
      xScalar = PrivateTo<T>(target, &context);
      context.m_pointElement = 1;
    }
    T yScalar = PrivateTo<T>(target, &context);
    result = dim.isPoint() ? PointOrRealScalar<T>(xScalar, yScalar)
                           : PointOrRealScalar<T>(yScalar);
  }
  if (clone) {
    clone->removeTree();
  }
  return result;
};

template <typename T>
PointOrRealScalar<T> ToPointOrRealScalar(const Tree* e, T abscissa,
                                         Parameters params, Context context) {
  LocalContext localContext(abscissa, context.m_localContext);
  context.m_localContext = &localContext;
  return ToPointOrRealScalar<T>(e, params, context);
}

template <typename T>
bool ToBoolean(const Tree* e, Parameters params, Context context) {
  assert(Dimension::DeepCheck(e) && Dimension::Get(e).isBoolean());
  assert(!params.optimize);
  Tree* clone = PrepareTreeAndContext<T>(e, params, context);
  const Tree* target = clone ? clone : e;
  bool b = PrivateToBoolean<T>(target, &context);
  if (clone) {
    clone->removeTree();
  }
  return b;
};

template <typename T>
T To(const Tree* e, Parameters params, Context context) {
  // Units are tolerated in scalar approximation (replaced with SI ratios).
  assert(Dimension::DeepCheck(e, context.m_symbolContext));
  Dimension dim = Dimension::Get(e, context.m_symbolContext);
  if (!(dim.isScalar() || dim.isUnit())) {
    return NAN;
  }
  assert(context.m_listElement != -1 ||
         !Dimension::IsList(e, context.m_symbolContext));
  return ToPointOrRealScalar<T>(e, params, context).toRealScalar();
};

template <typename T>
T To(const Tree* e, T abscissa, Parameters params, Context context) {
  LocalContext localContext(abscissa, context.m_localContext);
  context.m_localContext = &localContext;
  return To<T>(e, params, context);
}

template <typename T>
Coordinate2D<T> ToPoint(const Tree* e, Parameters params, Context context) {
  assert(Dimension::DeepCheck(e, context.m_symbolContext) &&
         Dimension::Get(e, context.m_symbolContext).isPoint());
  return ToPointOrRealScalar<T>(e, params, context).toPoint();
};

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

template <typename T>
Tree* Private::PrepareTreeAndContext(const Tree* e, Parameters params,
                                     Context& context) {
  // Only clone if necessary
  Tree* clone = nullptr;
  if (params.projectLocalVariables) {
    clone = e->cloneTree();
    Variables::ProjectLocalVariablesToId(clone);
  }
  if (params.isRootAndCanHaveRandom &&
      !context.m_randomContext.m_isInitialized) {
    /* Initialize randomContext only on root expressions to catch unsafe
     * approximations of projected sub-expressions. */
    context.m_randomContext.m_isInitialized = true;
    // TODO_PCJ: RandIntNoRep needs to be seeded for a proper approximation.
  }
  if (params.prepare || params.optimize) {
    assert(Simplification::IsSystem(e));
    if (!clone) {
      clone = e->cloneTree();
    }
    assert(params.isRootAndCanHaveRandom);
    PrepareExpressionForApproximation(clone);
    if (params.optimize) {
      ApproximateAndReplaceEveryScalar<T>(clone, context);
      // TODO: factor common sub-expressions
      // TODO: apply Horner's method: a*x^2 + b*x + c => (a*x + b)*x + c ?
    }
  }
  return clone;
}

template <typename T>
Tree* Private::PrivateToTree(const Tree* e, Dimension dim, const Context* ctx) {
  /* TODO_PCJ: not all approximation methods come here, but this assert should
   * always be called when approximating. */
  assert(!e->hasDescendantSatisfying(Projection::IsForbidden));
  if (dim.isBoolean()) {
#if POINCARE_BOOLEAN
    return (PrivateToBoolean<T>(e, ctx) ? KTrue : KFalse)->cloneTree();
#else
    OMG::unreachable();
#endif
  }
  if (dim.isUnit()) {
    // Preserve units and only replace scalar values.
    Tree* result = e->cloneTree();
    PrivateApproximateAndReplaceEveryScalar<T>(result, ctx);
    return result;
  }
  if (dim.isScalar()) {
    return ToComplexTree<T>(e, ctx);
  }
  assert(dim.isPoint() || dim.isMatrix());
  Tree* result =
      dim.isPoint() ? PrivateToPoint<T>(e, ctx) : ToMatrix<T>(e, ctx);
  Undefined::ShallowBubbleUpUndef(result);
  return result;
}

/* Helpers */

template <typename T>
T FloatBinomial(T n, T k) {
  if (k != std::round(k)) {
    return NAN;
  }
  if (k < 0) {
    return 0;
  }
  // Generalized definition allows any n value
  bool generalized = (n != std::round(n) || n < k);
  // Take advantage of symmetry
  k = (!generalized && k > (n - k)) ? n - k : k;

  T result = 1;
  for (T i = 0; i < k; i++) {
    result *= (n - i) / (k - i);
    if (std::isinf(result) || std::isnan(result)) {
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

/* Return highest order of undefined dependencies if there is at least one, zero
 * otherwise */
std::complex<float> Private::HelperUndefDependencies(const Tree* dep,
                                                     const Context* ctx) {
  // Dependency children may have different dimensions.
  std::complex<float> undefValue = std::complex<float>(0);
  for (const Tree* child : Dependency::Dependencies(dep)->children()) {
    Dimension dim = Dimension::Get(child, ctx->m_symbolContext);
    if (dim.isScalar()) {
      // Optimize most cases
      std::complex<float> c = PrivateToComplex<float>(child, ctx);
      // Only update to nonreal if there is no undef to respect priority
      if (IsNonReal(c) && undefValue == std::complex<float>(0)) {
        undefValue = c;
      } else if (std::isnan(c.real())) {
        undefValue = NAN;
      } else {
        assert(!std::isnan(c.imag()));
      }
    } else {
      Tree* a = PrivateToTree<float>(
          child, Dimension::Get(child, ctx->m_symbolContext), ctx);
      if (a->isUndefined()) {
        a->removeTree();
        undefValue = NAN;
      }
      a->removeTree();
    }
  }
  return undefValue;
}

template <typename T>
std::complex<T> Private::UndefDependencies(const Tree* dep,
                                           const Context* ctx) {
  std::complex<float> res = HelperUndefDependencies(dep, ctx);
  return IsNonReal(res) ? NonReal<T>() : std::complex<T>(res);
}

template <typename T>
std::complex<T> Private::PrivateToComplex(const Tree* e, const Context* ctx) {
#if POINCARE_NO_FLOAT_APPROXIMATION
  std::complex<T> value;
  value = ToComplexSwitch<double>(e, ctx);
#else
  std::complex<T> value = ToComplexSwitch<T>(e, ctx);
#endif
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
      return PrivateToComplex<T>(e->child(0), ctx);
    case Type::ComplexI:
      return std::complex<T>(0, 1);
    case Type::Pi:
      return M_PI;
    case Type::EulerE:
      return M_E;
    case Type::SingleFloat:
      return FloatHelper::FloatTo(e);
    case Type::DoubleFloat:
      return FloatHelper::DoubleTo(e);
    case Type::Add: {
      std::complex<T> result = 0;
      for (const Tree* child : e->children()) {
        result += PrivateToComplex<T>(child, ctx);
      }
      return result;
    }
    case Type::Mult: {
      std::complex<T> result = 1;
      for (const Tree* child : e->children()) {
        result =
            FloatMultiplication<T>(result, PrivateToComplex<T>(child, ctx));
      }
      return result;
    }
    case Type::Div:
      return FloatDivision<T>(PrivateToComplex<T>(e->child(0), ctx),
                              PrivateToComplex<T>(e->child(1), ctx));
    case Type::Sub:
      return PrivateToComplex<T>(e->child(0), ctx) -
             PrivateToComplex<T>(e->child(1), ctx);
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
      std::complex<T> c = PrivateToComplex<T>(e->child(0), ctx);
      return NeglectRealOrImaginaryPartIfNegligible(std::sqrt(c), c);
    }
    case Type::Root: {
      return ApproximateRoot<T>(e, ctx);
    }
    case Type::Exp:
      return std::exp(PrivateToComplex<T>(e->child(0), ctx));
    case Type::Log:
    case Type::Ln: {
      std::complex<T> c = PrivateToComplex<T>(e->child(0), ctx);
      /* log has a branch cut on ]-inf, 0]: it is then multi-valued on this cut.
       * We followed the convention chosen by the lib c++ of llvm on ]-inf+0i,
       * 0+0i] (warning: log takes the other side of the cut values on ]-inf-0i,
       * 0-0i]). We manually handle the case where the argument is null, as the
       * lib c++ gives log(0) = -inf, which is only a generous shorthand for the
       * limit. */
      return c == std::complex<T>(0.0) ? -INFINITY
             : e->isLog()              ? std::log10(c)
                                       : std::log(c);
    }
    case Type::LogBase: {
      std::complex<T> a = PrivateToComplex<T>(e->child(0), ctx);
      std::complex<T> b = PrivateToComplex<T>(e->child(1), ctx);
      // TODO_PCJ: should we use log2 (we previously used log10)
      return a == static_cast<T>(0) || b == static_cast<T>(0)
                 ? NAN
                 : FloatDivision(std::log(a), std::log(b));
    }
    case Type::Abs:
      return std::abs(PrivateToComplex<T>(e->child(0), ctx));
    case Type::Arg:
      return std::arg(PrivateToComplex<T>(e->child(0), ctx));
    case Type::Inf:
      return INFINITY;
    case Type::Conj:
      return std::conj(PrivateToComplex<T>(e->child(0), ctx));
    case Type::Opposite:
      return FloatMultiplication<T>(-1, PrivateToComplex<T>(e->child(0), ctx));
    case Type::Re: {
      /* TODO_PCJ: Complex NAN should be used in most of the code. Make sure a
       * NAN result cannot be lost. */
      // TODO_PCJ: We used to ignore NAN imag part and return just c.real()
      // TODO: undef are not bubbled-up?
      std::complex<T> c = PrivateToComplex<T>(e->child(0), ctx);
      return std::isnan(c.imag()) ? NAN : c.real();
    }
    case Type::Im: {
      // TODO: why not use std::im(c)?
      // TODO: undef are not bubbled-up?
      std::complex<T> c = PrivateToComplex<T>(e->child(0), ctx);
      return std::isnan(c.real()) ? NAN : c.imag();
    }
    default:
      OMG::unreachable();
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
      return TrigonometricToComplex(e->type(),
                                    PrivateToComplex<T>(e->child(0), ctx),
                                    ctx ? ctx->m_angleUnit : AngleUnit::Radian);
    case Type::SinH:
    case Type::CosH:
    case Type::TanH:
    case Type::ArSinH:
    case Type::ArCosH:
    case Type::ArTanH:
      return HyperbolicToComplex(e->type(),
                                 PrivateToComplex<T>(e->child(0), ctx));
    case Type::Trig:
    case Type::ATrig: {
      std::complex<T> a = PrivateToComplex<T>(e->child(0), ctx);
      std::complex<T> b = PrivateToComplex<T>(e->child(1), ctx);
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
          Type::ATan, PrivateToComplex<T>(e->child(0), ctx), AngleUnit::Radian);
    default:
      OMG::unreachable();
  }
}

template <typename T>
std::complex<T> UserNamedToComplex(const Tree* e, const Context* ctx) {
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
    case Type::UserSymbol: {
      // Global variable
      if (!ctx || !ctx->m_symbolContext) {
        return NAN;
      }
      const Tree* definition = ctx->m_symbolContext->expressionForUserNamed(e);
      if (!definition) {
        return NAN;
      }
      if (e->isUserSymbol()) {
        return PrivateToComplex<T>(definition, ctx);
      }
      std::complex<T> x = PrivateToComplex<T>(e->child(0), ctx);
      if (std::isnan(x.real()) || std::isnan(x.imag())) {
        return NAN;
      }
      Tree* definitionClone = definition->cloneTree();
      // Only approximate child once and use local context.
      Variables::ReplaceSymbol(definitionClone, KUnknownSymbol, 0,
                               ComplexSign::Unknown());
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
      /* TODO_PCJ: Sequence are not replaced by their definition on projection,
       * GlobalContext is therefore used. */
      return Poincare::Context::GlobalContext->approximateSequenceAtRank(
          Symbol::GetName(e), rank);
    }
    default:
      OMG::unreachable();
  }
}

template <typename T>
std::complex<T> AnalysisToComplex(const Tree* e, const Context* ctx) {
  switch (e->type()) {
    case Type::Sum:
    case Type::Product: {
      const Tree* lowerBoundChild = e->child(Parametric::k_lowerBoundIndex);
      std::complex<T> low = PrivateToComplex<T>(lowerBoundChild, ctx);
      if (low.imag() != 0 || (int)low.real() != low.real()) {
        return NAN;
      }
      assert(!Undefined::IsUndefined(low));
      const Tree* upperBoundChild = lowerBoundChild->nextTree();
      std::complex<T> up = PrivateToComplex<T>(upperBoundChild, ctx);
      if (up.imag() != 0 || (int)up.real() != up.real()) {
        return NAN;
      }
      assert(!Undefined::IsUndefined(up));
      int lowerBound = low.real();
      int upperBound = up.real();
      // Cloning here to avoid modifying function argument `e`
      Tree* child = upperBoundChild->nextTree()->cloneTree();
      assert(ctx);
      Context ctxCopy = *ctx;
      /* We ApproximateAndReplaceEveryScalar here to avoid approximate complex
       * constants on every round of the sum/product computation */
      ApproximateAndReplaceEveryScalar<T>(child, *ctx);
      LocalContext localCtx = LocalContext(NAN, ctx->m_localContext);
      ctxCopy.m_localContext = &localCtx;
      std::complex<T> result = e->isSum() ? 0 : 1;
      for (int k = lowerBound; k <= upperBound; k++) {
        ctxCopy.setLocalValue(k);
        // Reset random context
        ctxCopy.m_randomContext = Random::Context(true);
        std::complex<T> value = PrivateToComplex<T>(child, &ctxCopy);
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
      std::complex<T> at = PrivateToComplex<T>(e->child(1), ctx);
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
      OMG::unreachable();
  }
}

template <typename T>
std::complex<T> MatrixToComplex(const Tree* e, const Context* ctx) {
  switch (e->type()) {
    case Type::Norm:
    case Type::Det: {
      Tree* m = ToMatrix<T>(e->child(0), ctx);
      Tree* value;
      if (e->isDet()) {
        Matrix::RowCanonize(m, true, &value, true, ctx);
      } else {
        assert(e->isNorm());
        value = Vector::Norm(m);
      }
      std::complex<T> v = PrivateToComplex<T>(value, ctx);
      value->removeTree();
      m->removeTree();
      return v;
    }
    case Type::Trace:
      return ApproximateTrace<T>(e->child(0), ctx);
    case Type::Dot: {
      // TODO use complex conjugate ?
      Tree* u = ToMatrix<T>(e->child(0), ctx);
      Tree* v = ToMatrix<T>(e->child(1), ctx);
      Tree* r = Vector::Dot(u, v);
      std::complex<T> result = PrivateToComplex<T>(r, ctx);
      r->removeTree();
      v->removeTree();
      u->removeTree();
      return result;
    }
    case Type::Point:
      assert(ctx && ctx->m_pointElement != -1);
      return PrivateToComplex<T>(e->child(ctx->m_pointElement), ctx);
    default:
      OMG::unreachable();
  }
}

template <typename T>
std::complex<T> ListToComplex(const Tree* e, const Context* ctx) {
  Context tempCtx(*ctx);
  switch (e->type()) {
    case Type::List:
      assert(ctx && ctx->m_listElement != -1);
      return PrivateToComplex<T>(e->child(ctx->m_listElement), ctx);
    case Type::ListSequence: {
      assert(ctx && ctx->m_listElement != -1);
      // epsilon sequences starts at one
      Context ctxCopy = *ctx;
      LocalContext localCtx =
          LocalContext(ctx->m_listElement + 1, ctx->m_localContext);
      ctxCopy.m_localContext = &localCtx;
      // Reset random context
      ctxCopy.m_randomContext = Random::Context();
      return PrivateToComplex<T>(e->child(2), &ctxCopy);
    }
    case Type::Dim: {
      int n = Dimension::ListLength(e->child(0), ctx->m_symbolContext);
      return n >= 0 ? n : NAN;
    }
    case Type::ListElement: {
      const Tree* values = e->child(0);
      const Tree* index = e->child(1);
      assert(Integer::Is<uint8_t>(index));
      int i = Integer::Handler(index).to<uint8_t>() - 1;
      if (i < 0 || i >= Dimension::ListLength(values, ctx->m_symbolContext)) {
        return NAN;
      }
      assert(ctx);
      tempCtx.m_listElement = i;
      return PrivateToComplex<T>(values, &tempCtx);
    }
    case Type::ListSlice: {
      assert(ctx && ctx->m_listElement != -1);
      const Tree* values = e->child(0);
      const Tree* startIndex = e->child(1);
      assert(Integer::Is<uint8_t>(startIndex));
      assert(Integer::Is<uint8_t>(e->child(2)));
      int start = std::max(Integer::Handler(startIndex).to<uint8_t>() - 1, 0);
      assert(start >= 0);
      tempCtx.m_listElement += start;
      return PrivateToComplex<T>(values, &tempCtx);
    }
    case Type::ListSum:
    case Type::ListProduct: {
      assert(ctx);
      const Tree* values = e->child(0);
      int length = Dimension::ListLength(values, ctx->m_symbolContext);
      std::complex<T> result = e->isListSum() ? 0 : 1;
      for (int i = 0; i < length; i++) {
        tempCtx.m_listElement = i;
        std::complex<T> v = PrivateToComplex<T>(values, &tempCtx);
        result = e->isListSum() ? result + v : result * v;
      }
      return result;
    }
    case Type::Min:
    case Type::Max: {
      assert(ctx);
      const Tree* values = e->child(0);
      int length = Dimension::ListLength(values, ctx->m_symbolContext);
      assert(length > 0);
      T result;
      for (int i = 0; i < length; i++) {
        tempCtx.m_listElement = i;
        std::complex<T> v = PrivateToComplex<T>(values, &tempCtx);
        if (v.imag() != 0 || std::isnan(v.real())) {
          return NAN;
        }
        if (i == 0 ||
            (e->isMin() ? (v.real() < result) : (v.real() > result))) {
          result = v.real();
        }
      }
      return result;
    }
    case Type::Mean:
    case Type::StdDev:
    case Type::SampleStdDev:
    case Type::Variance: {
      assert(ctx);
      const Tree* values = e->child(0);
      const Tree* coefficients = e->child(1);
      int length = Dimension::ListLength(values, ctx->m_symbolContext);
      std::complex<T> sum = 0;
      std::complex<T> sumOfSquares = 0;
      T coefficientsSum = 0;
      for (int i = 0; i < length; i++) {
        tempCtx.m_listElement = i;
        std::complex<T> v = PrivateToComplex<T>(values, &tempCtx);
        std::complex<T> c = PrivateToComplex<T>(coefficients, &tempCtx);
        if (c.imag() != 0 || c.real() < 0) {
          return NAN;
        }
        sum += c.real() * v;
        // TODO v * conj(v) ?
        sumOfSquares += c.real() * v * v;
        coefficientsSum += c.real();
      }
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
      Tree* list = ToList<T>(e->child(0), ctx);
      // TODO: Remove this clone
      Tree* sortedList;
      ExceptionTry {
        sortedList = list->cloneTree();
        NAry::Sort(sortedList, Order::OrderType::RealLine);
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
      list->moveTreeOverTree(sortedList);
      std::complex<T> result = PrivateToComplex<T>(list, ctx);
      list->removeTree();
      return result;
    }
    case Type::Median: {
      Tree* list = ToList<T>(e->child(0), ctx);
      TreeDatasetColumn<T> values(list);
      T median;
      if (Dimension::IsList(e->child(1), ctx->m_symbolContext)) {
        Tree* weightsList = ToList<T>(e->child(1), ctx);
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
      OMG::unreachable();
  }
}

template <typename T>
std::complex<T> MiscToComplex(const Tree* e, const Context* ctx) {
  switch (e->type()) {
    case Type::AngleUnitContext: {
      Context tempCtx(*ctx);
      tempCtx.m_angleUnit = static_cast<AngleUnit>(e->nodeValue(0));
      return PrivateToComplex<T>(e->child(0), &tempCtx);
    }
    case Type::Piecewise:
      return PrivateToComplex<T>(SelectPiecewiseBranch<T>(e, ctx), ctx);
    case Type::Distribution: {
      const Tree* child = e->child(0);
      T abscissa[DistributionMethod::k_maxNumberOfParameters];
      DistributionMethod::Type method = DistributionMethod::Get(e);
      for (int i = 0; i < DistributionMethod::numberOfParameters(method); i++) {
        std::complex<T> c = PrivateToComplex<T>(child, ctx);
        if (c.imag() != 0) {
          return NAN;
        }
        abscissa[i] = c.real();
        child = child->nextTree();
      }
      T parameters[Distribution::k_maxNumberOfParameters];
      Distribution::Type distribution = Distribution::Get(e);
      for (int i = 0; i < Distribution::numberOfParameters(distribution); i++) {
        std::complex<T> c = PrivateToComplex<T>(child, ctx);
        if (c.imag() != 0) {
          return NAN;
        }
        parameters[i] = c.real();
        child = child->nextTree();
      }
      return DistributionMethod::Get(method)->EvaluateAtAbscissa(
          abscissa, Distribution::Get(distribution), parameters);
    }
    case Type::Dep: {
      std::complex<T> undef = UndefDependencies<T>(e, ctx);
      return (undef == std::complex<T>(0.0))
                 ? PrivateToComplex<T>(Dependency::Main(e), ctx)
                 : undef;
    }
    case Type::NonNull: {
      std::complex<T> x = PrivateToComplex<T>(e->child(0), ctx);
      return x == std::complex<T>(0.0) ? NAN : x;
    }
    case Type::Real: {
      std::complex<T> x = PrivateToComplex<T>(e->child(0), ctx);
      return x.imag() != 0.0 ? NAN : x;
    }
    case Type::RealPos: {
      std::complex<T> x = PrivateToComplex<T>(e->child(0), ctx);
      return x.real() < 0.0 || x.imag() != 0.0 ? NonReal<T>() : x;
    }
    /* Handle units as their scalar value in basic SI so prefix and
     * representative homogeneity isn't necessary. Dimension is expected to be
     * handled at this point. */
    case Type::Unit: {
      T approxSI = Units::Unit::GetValue(e);
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
      std::complex<T> x = PrivateToComplex<T>(e->child(0), ctx);
      if (ctx && ctx->m_complexFormat == ComplexFormat::Real &&
          (x.real() < 0 || x.imag() != 0)) {
        return NonReal<T>();
      }
      return x == std::complex<T>(0.0) ? NAN : std::log(x);
    }
    case Type::Store:
      return PrivateToComplex<T>(e->child(0), ctx);
    default:
      OMG::unreachable();
  }
}

template <typename T>
std::complex<T> ToComplexSwitchOnlyReal(const Tree* e, const Context* ctx) {
  T child[2];
  for (IndexedChild<const Tree*> childNode : e->indexedChildren()) {
    std::complex<T> app = PrivateToComplex<T>(childNode, ctx);
    if (app.imag() != 0 || std::isnan(app.real())) {
      return NAN;
    }
    assert(!std::isnan(app.imag()));
    child[childNode.index] = app.real();
  }
  switch (e->type()) {
    case Type::Decimal:
      return child[0] * std::pow(static_cast<T>(10.0), -child[1]);
    case Type::PowReal: {
      return ApproximatePower<T>(e, ctx, ComplexFormat::Real);
    }
    case Type::Sign: {
      // TODO why no epsilon in Poincare ?
      return child[0] == 0 ? 0 : child[0] < 0 ? -1 : 1;
    }
    case Type::Floor:
    case Type::Ceil: {
      /* Assume low deviation from natural numbers are errors */
      T delta = std::fabs((std::round(child[0]) - child[0]) / child[0]);
      if (delta <= OMG::Float::Epsilon<T>()) {
        return std::round(child[0]);
      }
      return e->isFloor() ? std::floor(child[0]) : std::ceil(child[0]);
    }
    case Type::Frac: {
      return child[0] - std::floor(child[0]);
    }
    case Type::Round: {
      assert(!std::isnan(child[1]));
      if (child[1] != std::round(child[1])) {
        return NAN;
      }
      T err = std::pow(10, std::round(child[1]));
      return std::round(child[0] * err) / err;
    }
    case Type::EuclideanDivision:
    case Type::EuclideanDivisionResult:  // FIXME: should be unreachable
    case Type::Quo:
    case Type::Rem: {
      T a = child[0];
      T b = child[1];
      assert(!std::isnan(a) && !std::isnan(b));
      if (a != (int)a || b != (int)b) {
        return NAN;
      }
      // TODO: is this really better than std::remainder ?
      T quotient = b >= 0 ? std::floor(a / b) : -std::floor(a / (-b));
      return e->isRem() ? std::round(a - b * quotient) : quotient;
    }

    case Type::Fact: {
      T n = child[0];
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
      T n = child[0];
      T k = child[1];
      return FloatBinomial<T>(n, k);
    }
    case Type::Permute: {
      T n = child[0];
      T k = child[1];
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
      T integerPart = child[0];
      T fractionPart = child[1];
      if (fractionPart < 0.0 || integerPart != std::fabs(integerPart)) {
        // TODO how can this happen ?
        return NAN;
      }
      return child[0] + child[1];
    }
    case Type::Factor:
      // Useful for the beautification only
      return child[0];
    case Type::PercentSimple:
      return child[0] / 100.0;
    case Type::PercentAddition:
      return child[0] * (1.0 + child[1] / 100.0);

    default:
      if (e->isParametric()) {
        // TODO: Explicit e if it contains random nodes.
      }
      // TODO: Implement more Types
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
  if (e->isRational()) {
    return Rational::Numerator(e).to<T>() / Rational::Denominator(e).to<T>();
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
    case Type::Pi:
    case Type::EulerE:
    case Type::SingleFloat:
    case Type::DoubleFloat:
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
    case Type::UserSymbol:
    case Type::UserSequence:
      return UserNamedToComplex<T>(e, ctx);
    case Type::Sum:
    case Type::Product:
    case Type::Diff:
    case Type::Integral:
    case Type::IntegralWithAlternatives:
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
    case Type::Unit:
    case Type::PhysicalConstant:
    case Type::LnUser:
    case Type::Store:
      return MiscToComplex<T>(e, ctx);
    default:;
  }
  // The remaining operators are defined only on reals
  // assert(e->numberOfChildren() <= 2);
  if (e->numberOfChildren() > 2) {
    return NAN;
  }
  return ToComplexSwitchOnlyReal<T>(e, ctx);
}

template <typename T>
bool Private::PrivateToBoolean(const Tree* e, const Context* ctx) {
#if POINCARE_NO_FLOAT_APPROXIMATION
  if (sizeof(T) == sizeof(float)) {
    return PrivateToBoolean<double>(e, ctx);
  }
#endif
  if (e->isTrue()) {
    return true;
  }
  if (e->isFalse()) {
    return false;
  }
  if (e->isInequality()) {
    T a = PrivateTo<T>(e->child(0), ctx);
    T b = PrivateTo<T>(e->child(1), ctx);
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
  if (e->isComparison()) {
    assert(e->isEqual() || e->isNotEqual());
    std::complex<T> a = PrivateToComplex<T>(e->child(0), ctx);
    std::complex<T> b = PrivateToComplex<T>(e->child(1), ctx);
    return e->isEqual() == (a == b);
  }
  if (e->isPiecewise()) {
    return PrivateToBoolean<T>(SelectPiecewiseBranch<T>(e, ctx), ctx);
  }
  if (e->isList()) {
    assert(ctx && ctx->m_listElement != -1);
    return PrivateToBoolean<T>(e->child(ctx->m_listElement), ctx);
  }
  if (e->isParentheses()) {
    return PrivateToBoolean<T>(e->child(0), ctx);
  }
  if (e->isListSort()) {
    /* TODO we are computing all elements and sorting the list for all
     * elements, this is awful */
    Tree* list = ToList<T>(e->child(0), ctx);
    NAry::Sort(list);
    bool result = PrivateToBoolean<T>(list, ctx);
    list->removeTree();
    return result;
  }
  assert(e->isLogicalOperator());
  bool a = PrivateToBoolean<T>(e->child(0), ctx);
  if (e->isLogicalNot()) {
    return !a;
  }
  if (e->isDep()) {
    // TODO: Undefined boolean return false for now.
    return (UndefDependencies<T>(e, ctx) == std::complex<T>(0.0));
  }
  bool b = PrivateToBoolean<T>(e->child(1), ctx);
  switch (e->type()) {
    case Type::LogicalAnd:
      return a && b;
    case Type::LogicalNand:
      return !(a && b);
    case Type::LogicalOr:
      return a || b;
    case Type::LogicalNor:
      return !(a || b);
    case Type::LogicalXor:
      return a ^ b;
    default:
      OMG::unreachable();
  }
}

template <typename T>
Tree* Private::ToList(const Tree* e, const Context* ctx) {
  assert(ctx);
  Dimension dimension = Dimension::Get(e, ctx->m_symbolContext);
  int length = Dimension::ListLength(e, ctx->m_symbolContext);
  assert(length != Dimension::k_nonListListLength);
  Context tempCtx(*ctx);
  Tree* list = SharedTreeStack->pushList(length);
  for (int i = 0; i < length; i++) {
    tempCtx.m_listElement = i;
    PrivateToTree<T>(e, dimension, &tempCtx);
  }
  return list;
}

template <typename T>
Tree* Private::PrivateToPoint(const Tree* e, const Context* ctx) {
  assert(ctx);
  Context tempCtx(*ctx);
  Tree* point = SharedTreeStack->pushPoint();
  tempCtx.m_pointElement = 0;
  ToComplexTree<T>(e, &tempCtx);
  tempCtx.m_pointElement = 1;
  ToComplexTree<T>(e, &tempCtx);
  return point;
}

template <typename T>
Tree* Private::ToMatrix(const Tree* e, const Context* ctx) {
  /* TODO: Normal matrix nodes and operations with approximated children are
   * used to carry matrix approximation. A dedicated node that knows its
   * children have a fixed size would be more efficient. */
  if (e->isMatrix()) {
    Tree* m = e->cloneNode();
    for (const Tree* child : e->children()) {
      ToComplexTree<T>(child, ctx);
    }
    return m;
  }
  switch (e->type()) {
    case Type::Parentheses:
      return ToMatrix<T>(e->child(0), ctx);
    case Type::Add: {
      const Tree* child = e->child(0);
      int n = e->numberOfChildren() - 1;
      Tree* result = ToMatrix<T>(child, ctx);
      while (n--) {
        child = child->nextTree();
        Tree* approximatedChild = ToMatrix<T>(child, ctx);
        Matrix::Addition(result, approximatedChild, true, ctx);
        approximatedChild->removeTree();
        result->removeTree();
      }
      return result;
    }
    case Type::Sub: {
      Tree* a = ToMatrix<T>(e->child(0), ctx);
      Tree* b = ToMatrix<T>(e->child(1), ctx);
      b->moveTreeOverTree(Matrix::ScalarMultiplication(-1_e, b, true, ctx));
      Matrix::Addition(a, b, true, ctx);
      a->removeTree();
      a->removeTree();
      return a;
    }
    case Type::Mult: {
      bool resultIsMatrix = false;
      Tree* result = nullptr;
      for (const Tree* child : e->children()) {
        bool childIsMatrix =
            Dimension::Get(child, ctx->m_symbolContext).isMatrix();
        Tree* approx = childIsMatrix ? ToMatrix<T>(child, ctx)
                                     : ToComplexTree<T>(child, ctx);
        if (result == nullptr) {
          resultIsMatrix = childIsMatrix;
          result = approx;
          continue;
        }
        if (resultIsMatrix && childIsMatrix) {
          Matrix::Multiplication(result, approx, true, ctx);
        } else if (resultIsMatrix) {
          Matrix::ScalarMultiplication(approx, result, true, ctx);
        } else {
          Matrix::ScalarMultiplication(result, approx, true, ctx);
        }
        resultIsMatrix |= childIsMatrix;
        approx->removeTree();
        result->removeTree();
      }
      return result;
    }
    case Type::Div: {
      Tree* a = ToMatrix<T>(e->child(0), ctx);
      Tree* s = KDiv->cloneNode();
      (1_e)->cloneTree();
      e->child(1)->cloneTree();
      ToComplexTree<T>(s, ctx);
      s->removeTree();
      s->moveTreeOverTree(Matrix::ScalarMultiplication(s, a, true, ctx));
      a->removeTree();
      return a;
    }
    case Type::Pow: {
      const Tree* base = e->child(0);
      const Tree* index = base->nextTree();
      T value = PrivateTo<T>(index, ctx);
      if (std::isnan(value) || value != std::round(value)) {
        return KUndef->cloneTree();
      }
      Tree* result = ToMatrix<T>(base, ctx);
      result->moveTreeOverTree(Matrix::Power(result, value, true, ctx));
      return result;
    }
    case Type::Inverse:
    case Type::Transpose: {
      Tree* result = ToMatrix<T>(e->child(0), ctx);
      result->moveTreeOverTree(e->isInverse()
                                   ? Matrix::Inverse(result, true, ctx)
                                   : Matrix::Transpose(result));
      return result;
    }
    case Type::Identity:
      return Matrix::Identity(e->child(0));
    case Type::Ref:
    case Type::Rref: {
      Tree* result = ToMatrix<T>(e->child(0), ctx);
      Matrix::RowCanonize(result, e->isRref(), nullptr, true, ctx);
      return result;
    }
    case Type::Dim: {
      Dimension dim = Dimension::Get(e->child(0), ctx->m_symbolContext);
      assert(dim.isMatrix());
      Tree* result = SharedTreeStack->pushMatrix(1, 2);
      SharedTreeStack->pushFloat(T(dim.matrix.rows));
      SharedTreeStack->pushFloat(T(dim.matrix.cols));
      return result;
    }
    case Type::Cross: {
      Tree* u = ToMatrix<T>(e->child(0), ctx);
      Tree* v = ToMatrix<T>(e->child(1), ctx);
      Vector::Cross(u, v);
      u->removeTree();
      u->removeTree();
      return u;
    }
    case Type::Piecewise:
      return ToMatrix<T>(SelectPiecewiseBranch<T>(e, ctx), ctx);
    case Type::Dep: {
      std::complex<T> undef = UndefDependencies<T>(e, ctx);
      return (undef == std::complex<T>(0.0))
                 ? ToMatrix<T>(Dependency::Main(e), ctx)
                 : (IsNonReal(undef) ? KNonReal->cloneTree()
                                     : KUndef->cloneTree());
    }
    default:;
  }
  return KUndef->cloneTree();
}

template <typename T>
T Private::PrivateTo(const Tree* e, const Context* ctx) {
#if ASSERTIONS
  Dimension dim = Dimension::Get(e, ctx->m_symbolContext);
#endif
  assert((dim.isScalar() && (ctx->m_listElement != -1 ||
                             !Dimension::IsList(e, ctx->m_symbolContext))) ||
         (dim.isPoint() && ctx->m_pointElement != -1) || dim.isUnit());
  std::complex<T> value = PrivateToComplex<T>(e, ctx);
  // Remove signaling nan
  return value.imag() == 0 && !IsNonReal(value) ? value.real() : NAN;
}

template <typename T>
T ToLocalContext(const Tree* e, const Context* ctx, T x) {
  assert(ctx);
  Context ctxCopy(*ctx);
  LocalContext localCtx(x, ctxCopy.m_localContext);
  ctxCopy.m_localContext = &localCtx;
  return PrivateTo<T>(e, &ctxCopy);
}

template <typename T>
const Tree* Private::SelectPiecewiseBranch(const Tree* piecewise,
                                           const Context* ctx) {
  assert(piecewise->isPiecewise());
  int n = piecewise->numberOfChildren();
  int i = 0;
  const Tree* child = piecewise->child(0);
  while (i < n) {
    const Tree* condition = child->nextTree();
    i++;
    if (i == n || PrivateToBoolean<T>(condition, ctx)) {
      return child;
    }
    child = condition->nextTree();
    i++;
  }
  // No clause matched
  return KUndef;
}

/* TODO: users of this function just want to test equality of branch and do not
 * need the index */
template <typename T>
int IndexOfActivePiecewiseBranchAt(const Tree* piecewise, T x) {
  assert(piecewise->isPiecewise());
  /* TODO: Without randomContext, randomized nodes in piecewise's condition will
   *       approximate to NAN. */
  LocalContext localCtx(x);
  Context ctx;
  ctx.m_localContext = &localCtx;
  const Tree* branch = SelectPiecewiseBranch<T>(piecewise, &ctx);
  return branch == KUndef ? -1 : piecewise->indexOfChild(branch);
}

bool CanApproximate(const Tree* e, bool approxLocalVar) {
  return Private::CanApproximate(e, 0);
}

bool Private::CanApproximate(const Tree* e, int firstNonApproximableVarId) {
  if (e->isRandomized() || e->isUserSymbol() || e->isUserFunction() ||
      (e->isVar() && Variables::Id(e) >= firstNonApproximableVarId) ||
      e->isDepList()) {
    return false;
  }
  int childIndex = 0;
  for (const Tree* child : e->children()) {
    bool enterScope = false;
    if (e->isParametric()) {
      if (childIndex == Parametric::k_variableIndex) {
        // Parametric's variable cant be approximated, but we never want to.
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

template <>
bool Private::SkipApproximation<float>(TypeBlock type) {
  return type.isSingleFloat() || type.isComplexI();
}

template <>
bool Private::SkipApproximation<double>(TypeBlock type) {
  return type.isDoubleFloat() || type.isComplexI();
}

template <typename T>
bool Private::SkipApproximation(TypeBlock type, TypeBlock parentType,
                                int indexInParent,
                                bool previousChildWasApproximated) {
  if (SkipApproximation<T>(type)) {
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
    case Type::Identity:
      return true;
    default:
      return false;
  }
}

template <typename T>
bool ApproximateAndReplaceEveryScalar(Tree* e, Context context) {
  // Prevent float to double conversion
  if (sizeof(T) == sizeof(double) &&
      e->hasDescendantSatisfying(
          [](const Tree* e) { return e->isSingleFloat(); })) {
    return ApproximateAndReplaceEveryScalar<float>(e, context);
  }

  if (SkipApproximation<T>(e->type())) {
    return false;
  }
  uint32_t hash = e->hash();
  bool result = PrivateApproximateAndReplaceEveryScalar<T>(e, &context);
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

template <typename T>
bool Private::PrivateApproximateAndReplaceEveryScalar(Tree* e,
                                                      const Context* ctx) {
  if (Approximation::CanApproximate(e) &&
      Dimension::IsNonListScalar(e, ctx->m_symbolContext)) {
    e->moveTreeOverTree(PrivateToTree<T>(e, Dimension(), ctx));
    return true;
  }
  bool changed = false;
  bool previousChildWasApproximated = false;
  for (IndexedChild<Tree*> child : e->indexedChildren()) {
    if (!SkipApproximation<T>(child->type(), e->type(), child.index,
                              previousChildWasApproximated)) {
      changed =
          PrivateApproximateAndReplaceEveryScalar<T>(child, ctx) || changed;
    }
    previousChildWasApproximated = child->isFloat();
  }
  changed = Undefined::ShallowBubbleUpUndef(e) || changed;
  // Merge addition and multiplication's children while preserving order
  if (e->isAdd() || e->isMult()) {
    changed = MergeChildrenOfMultOrAdd<T>(e) || changed;
  }
  return changed;
}

Tree* ExtractRealPartIfImaginaryPartNegligible(const Tree* e) {
  if (GetComplexSign(e).isReal()) {
    return e->cloneTree();
  }
  std::complex<double> value = PrivateToComplex<double>(e, nullptr);
  constexpr float precision = 100 * OMG::Float::EpsilonLax<double>();
  if (std::abs(value.imag()) < precision) {
    return SharedTreeStack->pushDoubleFloat(value.real());
  }
  return nullptr;
}

/* TODO: not all this functions are worth templating on float and
 * double. ToComplex needs it but ToMatrix could take a bool and call the
 * correct ToComplex<T> as needed since the code is mostly independent of the
 * float type used in the tree. */

template Tree* ToTree<float>(const Tree*, Parameters, Context);
template Tree* ToTree<double>(const Tree*, Parameters, Context);

template std::complex<float> ToComplex(const Tree*, Parameters, Context);
template std::complex<double> ToComplex(const Tree*, Parameters, Context);

template PointOrRealScalar<float> ToPointOrRealScalar(const Tree*, Parameters,
                                                      Context);
template PointOrRealScalar<double> ToPointOrRealScalar(const Tree*, Parameters,
                                                       Context);

template PointOrRealScalar<float> ToPointOrRealScalar(const Tree*, float,
                                                      Parameters, Context);
template PointOrRealScalar<double> ToPointOrRealScalar(const Tree*, double,
                                                       Parameters, Context);

template bool ToBoolean<float>(const Tree*, Parameters, Context);
template bool ToBoolean<double>(const Tree*, Parameters, Context);

template float To(const Tree*, Parameters, Context);
template double To(const Tree*, Parameters, Context);

template float To(const Tree*, float, Parameters, Context);
template double To(const Tree*, double, Parameters, Context);

template Coordinate2D<float> ToPoint(const Tree*, Parameters, Context);
template Coordinate2D<double> ToPoint(const Tree*, Parameters, Context);

template float FloatBinomial(float, float);
template double FloatBinomial(double, double);

template std::complex<float> Private::PrivateToComplex(const Tree*,
                                                       const Context*);
template std::complex<double> Private::PrivateToComplex(const Tree*,
                                                        const Context*);

template Tree* Private::PrivateToPoint<float>(const Tree*, const Context*);
template Tree* Private::PrivateToPoint<double>(const Tree*, const Context*);

template float Private::PrivateTo(const Tree*, const Context*);
template double Private::PrivateTo(const Tree*, const Context*);

template float ToLocalContext(const Tree*, const Context*, float);
template double ToLocalContext(const Tree*, const Context*, double);

template int IndexOfActivePiecewiseBranchAt(const Tree*, float);
template int IndexOfActivePiecewiseBranchAt(const Tree*, double);

template bool ApproximateAndReplaceEveryScalar<float>(Tree*, Context);
template bool ApproximateAndReplaceEveryScalar<double>(Tree*, Context);

template bool Private::PrivateApproximateAndReplaceEveryScalar<float>(
    Tree*, const Context*);
template bool Private::PrivateApproximateAndReplaceEveryScalar<double>(
    Tree*, const Context*);

template bool Private::SkipApproximation<float>(TypeBlock, TypeBlock, int,
                                                bool);
template bool Private::SkipApproximation<double>(TypeBlock, TypeBlock, int,
                                                 bool);

template Tree* Private::PrepareTreeAndContext<float>(const Tree*, Parameters,
                                                     Context&);
template Tree* Private::PrepareTreeAndContext<double>(const Tree*, Parameters,
                                                      Context&);

}  // namespace Poincare::Internal::Approximation
