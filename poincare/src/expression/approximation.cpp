#include "approximation.h"

#include <omg/float.h>
#include <omg/unreachable.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/numeric/statistics_dataset.h>
#include <poincare/src/probability/distribution_method.h>

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
#include "physical_constant.h"
#include "random.h"
#include "rational.h"
#include "symbol.h"
#include "undefined.h"
#include "units/representatives.h"
#include "units/unit.h"
#include "variables.h"
#include "vector.h"

namespace Poincare::Internal {

/* Approximation::Context */

Approximation::Context::Context(Random::Context* randomContext,
                                AngleUnit angleUnit,
                                ComplexFormat complexFormat,
                                VariableType abscissa, int listElement)
    : m_angleUnit(angleUnit),
      m_complexFormat(complexFormat),
      m_localVariable(abscissa),
      m_listElement(listElement),
      m_pointElement(-1),
      m_randomContext(randomContext),
      m_parentContext(nullptr) {}

Approximation::Context::Context(const Context* parentContext,
                                VariableType abscissa)
    : Context(*parentContext) {
  m_parentContext = parentContext;
  m_localVariable = abscissa;
}

template <typename T>
PointOrScalar<T> Approximation::RootPreparedToPointOrScalar(
    const Tree* preparedFunction, T abscissa) {
  Dimension dimension = Dimension::Get(preparedFunction);
  assert(dimension.isScalar() || dimension.isPoint());
  return RootToPointOrScalarPrivate<T>(preparedFunction, dimension.isPoint(),
                                       true, abscissa);
}

template <typename T>
Tree* Approximation::RootTreeToTree(const Tree* e, AngleUnit angleUnit,
                                    ComplexFormat complexFormat) {
  if (!Dimension::DeepCheck(e)) {
    return KUndefUnhandledDimension->cloneTree();
  }
  return RootTreeToTreePrivate<T>(e, angleUnit, complexFormat,
                                  Dimension::Get(e), Dimension::ListLength(e));
}

template <typename T>
Tree* Approximation::RootTreeToTreePrivate(const Tree* e, AngleUnit angleUnit,
                                           ComplexFormat complexFormat,
                                           Dimension dim, int listLength) {
  assert(Dimension::DeepCheck(e));
  assert(listLength == Dimension::ListLength(e));
  assert(dim == Dimension::Get(e));

  Random::Context randomContext;
  Context context(&randomContext, angleUnit, complexFormat);
  Tree* clone = e->cloneTree();
  // TODO we should rather assume variable projection has already been done
  Variables::ProjectLocalVariablesToId(clone);

  if (listLength != Dimension::k_nonListListLength) {
    assert(!dim.isMatrix());
    SharedTreeStack->pushList(listLength);
    for (int i = 0; i < listLength; i++) {
      context.m_listElement = i;
      ToTree<T>(clone, dim, &context);
    }
  } else {
    ToTree<T>(clone, dim, &context);
  }

  clone->removeTree();
  return clone;
}

template <typename T>
Tree* Approximation::ToBeautifiedComplex(const Tree* e, const Context* ctx) {
  std::complex<T> value = ToComplex<T>(e, ctx);
  /* TODO: no s_context => complexFormat = cartesian for now, and it is only
   * used with OutOfContext matrix operations, we should impose a context
   * instead. */
  if (IsNonReal<T>(value)) {
    return KNonReal->cloneTree();
  }
  return Beautification::PushBeautifiedComplex(
      value, ctx ? ctx->m_complexFormat : ComplexFormat::Cartesian);
}

template <typename T>
Tree* Approximation::ToTree(const Tree* e, Dimension dim, const Context* ctx) {
  /* TODO_PCJ: not all approximation methods comes here, but this assert should
   * always be called when approximating. */
  assert(!e->hasDescendantSatisfying(Projection::IsForbidden));
  if (dim.isBoolean()) {
    return (ToBoolean<T>(e, ctx) ? KTrue : KFalse)->cloneTree();
  }
  if (dim.isUnit()) {
    // Preserve units and only replace scalar values.
    Tree* result = e->cloneTree();
    PrivateApproximateAndReplaceEveryScalar(result, ctx);
    return result;
  }
  if (dim.isScalar()) {
    Tree* result = ToBeautifiedComplex<T>(e, ctx);
    return result;
  }
  assert(dim.isPoint() || dim.isMatrix());
  Tree* result = dim.isPoint() ? ToPoint<T>(e, ctx) : ToMatrix<T>(e, ctx);
  if (Undefined::ShallowBubbleUpUndef(result)) {
    return result;
  }
  for (Tree* child : result->children()) {
    child->moveTreeOverTree(ToBeautifiedComplex<T>(child, ctx));
  }
  return result;
}

/* Entry points */

/* TODO rework and factorize entry points :
 *   - move beautification out of the approximation
 *   - move variable projection and other reductions (integral substitution)
 *     inside a PrepareForApproximation method
 */

template <typename T>
PointOrScalar<T> Approximation::RootToPointOrScalarPrivate(
    const Tree* e, bool isPoint, bool isPrepared, T abscissa, int listElement,
    AngleUnit angleUnit, ComplexFormat complexFormat) {
  Random::Context randomContext;
  Context context(&randomContext, angleUnit, complexFormat, abscissa,
                  listElement);
  Tree* clone;
  if (!isPrepared) {
    clone = e->cloneTree();
    // TODO we should rather assume variable projection has already been done
    Variables::ProjectLocalVariablesToId(clone);
    e = clone;
  }
  T xScalar;
  if (isPoint) {
    context.m_pointElement = 0;
    xScalar = To<T>(e, &context);
    context.m_pointElement = 1;
  }
  T yScalar = To<T>(e, &context);
  if (!isPrepared) {
    clone->removeTree();
  }
  return isPoint ? PointOrScalar<T>(xScalar, yScalar)
                 : PointOrScalar<T>(yScalar);
}

template <typename T>
std::complex<T> Approximation::RootPreparedToComplex(
    const Tree* preparedFunction, T abscissa) {
  Random::Context randomContext;
  Context context(&randomContext, AngleUnit::Radian, ComplexFormat::Cartesian,
                  abscissa);
  std::complex<T> result = ToComplex<T>(preparedFunction, &context);
  return result;
}

/* Helpers */

template <typename T>
T Approximation::FloatBinomial(T n, T k) {
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
std::complex<T> FloatMultiplication(std::complex<T> c, std::complex<T> d) {
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
std::complex<T> FloatDivision(std::complex<T> c, std::complex<T> d) {
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

// Return true if one of the dependencies is undefined
bool UndefDependencies(const Tree* dep, const Approximation::Context* ctx) {
  // Dependency children may have different dimensions.
  for (const Tree* child : Dependency::Dependencies(dep)->children()) {
    Dimension dim = Dimension::Get(child);
    if (dim.isScalar()) {
      // Optimize most cases
      std::complex<float> c = Approximation::ToComplex<float>(child, ctx);
      if (std::isnan(c.real())) {
        return true;
      }
      assert(!std::isnan(c.imag()));
    } else {
      Tree* a = Approximation::ToTree<float>(child, Dimension::Get(child), ctx);
      if (a->isUndefined()) {
        a->removeTree();
        return true;
      }
      a->removeTree();
    }
  }
  // None of the dependencies are NAN.
  return false;
}

template <typename T>
std::complex<T> Approximation::ToComplex(const Tree* e, const Context* ctx) {
  std::complex<T> value = ToComplexSwitch<T>(e, ctx);
  if (ctx && ctx->m_complexFormat == ComplexFormat::Real && value.imag() != 0 &&
      !e->isComplexI()) {
    /* Some operations in reduction can introduce i, but when complex format is
     * real and the factor or i approximates to 0, we don't want to return
     * nonreal. We thus decided not to return nonreal when we approximate i.
     * TODO: tolerate any complex encountered in approximation intermediary
     * steps? */
    return NonReal<T>();
  }
  // We used to flush negative zeros here but it was not worth
  return value;
}

template <typename T>
std::complex<T> Approximation::ToComplexSwitch(const Tree* e,
                                               const Context* ctx) {
  /* TODO: the second part of this function and several ifs in different cases
   * act differently / more precisely on reals. We should have a dedicated,
   * faster, simpler and more precise real approximation to be used in every
   * cases where we know for sure there are no complexes. */
  assert(e->isExpression());
  if (e->isUndefined()) {
    // TODO: Find a way to pass exact undef type up to PushBeautifiedComplex.
    return e->isNonReal() ? NonReal<T>() : NAN;
  }
  if (e->isRational()) {
    return Rational::Numerator(e).to<T>() / Rational::Denominator(e).to<T>();
  }

  if (e->isRandomized()) {
    return ApproximateRandom<T>(e, ctx);
  }
  switch (e->type()) {
    case Type::Parentheses:
      return ToComplex<T>(e->child(0), ctx);
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
        result += ToComplex<T>(child, ctx);
      }
      return result;
    }
    case Type::Mult: {
      std::complex<T> result = 1;
      for (const Tree* child : e->children()) {
        result = FloatMultiplication<T>(result, ToComplex<T>(child, ctx));
      }
      return result;
    }
    case Type::Div:
      return FloatDivision<T>(ToComplex<T>(e->child(0), ctx),
                              ToComplex<T>(e->child(1), ctx));
    case Type::Sub:
      return ToComplex<T>(e->child(0), ctx) - ToComplex<T>(e->child(1), ctx);
    case Type::Pow: {
      // how is this called without a context ?
      Context c;
      c.m_complexFormat = ComplexFormat::Real;
      return ApproximatePower<T>(e, ctx ? ctx : &c);
    }
    case Type::GCD:
    case Type::LCM: {
      const Tree* child = e->child(0);
      T result = PositiveIntegerApproximation(To<T>(child, ctx));
      if (std::isnan(result)) {
        return NAN;
      }
      for (int n = e->numberOfChildren() - 1; n > 0; n--) {
        child = child->nextTree();
        T current = PositiveIntegerApproximation(To<T>(child, ctx));
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
      std::complex<T> c = ToComplex<T>(e->child(0), ctx);
      return NeglectRealOrImaginaryPartIfNegligible(std::sqrt(c), c);
    }
    case Type::Root: {
      std::complex<T> base = ToComplex<T>(e->child(0), ctx);
      std::complex<T> exp = ToComplex<T>(e->child(1), ctx);
      /* If the complexFormat is Real, we look for nth root of form root(x,q)
       * with x real and q integer because they might have a real form which
       * does not correspond to the principal angle. */
      if (ctx && ctx->m_complexFormat == Preferences::ComplexFormat::Real &&
          exp.imag() == 0.0 && std::round(exp.real()) == exp.real()) {
        // root(x, q) with q integer and x real
        std::complex<T> result =
            ComputeNotPrincipalRealRootOfRationalPow<T>(base, 1, exp.real());
        if (!std::isnan(result.real()) && !std::isnan(result.imag())) {
          return result;
        }
      }
      return ComputeComplexPower<T>(
          base, std::complex<T>(1.0) / (exp),
          ctx ? ctx->m_complexFormat : ComplexFormat::Cartesian);
    }
    case Type::Exp:
      return std::exp(ToComplex<T>(e->child(0), ctx));
    case Type::Log:
    case Type::Ln: {
      std::complex<T> c = ToComplex<T>(e->child(0), ctx);
      /* log has a branch cut on ]-inf, 0]: it is then multi-valued on this cut.
       * We followed the convention chosen by the lib c++ of llvm on ]-inf+0i,
       * 0+0i] (warning: log takes the other side of the cut values on ]-inf-0i,
       * 0-0i]). We manually handle the case where the argument is null, as the
       * lib c++ gives log(0) = -inf, which is only a generous shorthand for the
       * limit. */
      return c == std::complex<T>(0) ? -INFINITY
             : e->isLog()            ? std::log10(c)
                                     : std::log(c);
    }
    case Type::LogBase: {
      std::complex<T> a = ToComplex<T>(e->child(0), ctx);
      std::complex<T> b = ToComplex<T>(e->child(1), ctx);
      // TODO_PCJ: should we use log2 (we previously used log10)
      return a == static_cast<T>(0) || b == static_cast<T>(0)
                 ? NAN
                 : FloatDivision(std::log(a), std::log(b));
    }
    case Type::Abs:
      return std::abs(ToComplex<T>(e->child(0), ctx));
    case Type::Arg:
      return std::arg(ToComplex<T>(e->child(0), ctx));
    case Type::Inf:
      return INFINITY;
    case Type::Conj:
      return std::conj(ToComplex<T>(e->child(0), ctx));
    case Type::Opposite:
      return FloatMultiplication<T>(-1, ToComplex<T>(e->child(0), ctx));
    case Type::Re: {
      /* TODO_PCJ: Complex NAN should be used in most of the code. Make sure a
       * NAN result cannot be lost. */
      // TODO_PCJ: We used to ignore NAN imag part and return just c.real()
      // TODO: undef are not bubbled-up?
      std::complex<T> c = ToComplex<T>(e->child(0), ctx);
      return std::isnan(c.imag()) ? NAN : c.real();
    }
    case Type::Im: {
      // TODO: why not use std::im(c)?
      // TODO: undef are not bubbled-up?
      std::complex<T> c = ToComplex<T>(e->child(0), ctx);
      return std::isnan(c.real()) ? NAN : c.imag();
    }

    /* Trigonometry */
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
      return TrigonometricToComplex(e->type(), ToComplex<T>(e->child(0), ctx),
                                    ctx ? ctx->m_angleUnit : AngleUnit::Radian);
    case Type::SinH:
    case Type::CosH:
    case Type::TanH:
    case Type::ArSinH:
    case Type::ArCosH:
    case Type::ArTanH:
      return HyperbolicToComplex(e->type(), ToComplex<T>(e->child(0), ctx));
    case Type::Trig:
    case Type::ATrig: {
      std::complex<T> a = ToComplex<T>(e->child(0), ctx);
      std::complex<T> b = ToComplex<T>(e->child(1), ctx);
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
      return TrigonometricToComplex(Type::ATan, ToComplex<T>(e->child(0), ctx),
                                    AngleUnit::Radian);
    case Type::Var: {
      if (!ctx) {
        return NAN;
      }
      // Local variable
      return ctx->variable(Variables::Id(e));
    }
    case Type::UserSymbol:
    case Type::UserFunction:
      // Global variable
      return NAN;
    case Type::UserSequence: {
      T rank = To<T>(e->child(0), ctx);
      if (std::isnan(rank) || std::floor(rank) != rank) {
        return NAN;
      }
      return Poincare::Context::GlobalContext->approximateSequenceAtRank(
          Internal::Symbol::GetName(e), rank);
    }
    /* Analysis */
    case Type::Sum:
    case Type::Product: {
      const Tree* lowerBoundChild = e->child(Parametric::k_lowerBoundIndex);
      std::complex<T> low = ToComplex<T>(lowerBoundChild, ctx);
      if (low.imag() != 0 || (int)low.real() != low.real()) {
        return NAN;
      }
      assert(!std::isnan(low.real()) && !std::isnan(low.imag()));
      const Tree* upperBoundChild = lowerBoundChild->nextTree();
      std::complex<T> up = ToComplex<T>(upperBoundChild, ctx);
      if (up.imag() != 0 || (int)up.real() != up.real()) {
        return NAN;
      }
      assert(!std::isnan(up.real()) && !std::isnan(up.imag()));
      int lowerBound = low.real();
      int upperBound = up.real();
      const Tree* child = upperBoundChild->nextTree();
      assert(ctx);
      Context childCtx(ctx, NAN);
      std::complex<T> result = e->isSum() ? 0 : 1;
      for (int k = lowerBound; k <= upperBound; k++) {
        childCtx.setLocalValue(k);
        std::complex<T> value = ToComplex<T>(child, &childCtx);
        if (e->isSum()) {
          result += value;
        } else {
          result *= value;
        }
        if (std::isnan(result.real()) || std::isnan(result.imag())) {
          break;
        }
      }
      return result;
    }
    case Type::Diff: {
      constexpr static int k_maxOrderForApproximation = 4;
      T orderReal = To<T>(e->child(2), ctx);
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
      std::complex<T> at = ToComplex<T>(e->child(1), ctx);
      if (std::isnan(at.real()) || at.imag() != 0) {
        return NAN;
      }
      assert(ctx);
      Context childCtx(ctx, NAN);
      T result = ApproximateDerivative(derivand, at.real(), order, &childCtx);
      return result;
    }
    case Type::Integral:
      // TODO: assert(false) if we enforce preparation before approximation
    case Type::IntegralWithAlternatives:
      return ApproximateIntegral<T>(e, ctx);

    /* Matrices */
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
      std::complex<T> v = ToComplex<T>(value, ctx);
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
      std::complex<T> result = ToComplex<T>(r, ctx);
      r->removeTree();
      v->removeTree();
      u->removeTree();
      return result;
    }
    case Type::Point:
      assert(ctx && ctx->m_pointElement != -1);
      return ToComplex<T>(e->child(ctx->m_pointElement), ctx);
    /* Lists */
    case Type::List:
      assert(ctx && ctx->m_listElement != -1);
      return ToComplex<T>(e->child(ctx->m_listElement), ctx);
    case Type::ListSequence: {
      assert(ctx && ctx->m_listElement != -1);
      // epsilon sequences starts at one
      Context childCtx(ctx, ctx->m_listElement + 1);
      std::complex<T> result = ToComplex<T>(e->child(2), &childCtx);
      return result;
    }
    case Type::Dim: {
      int n = Dimension::ListLength(e->child(0));
      return n >= 0 ? n : NAN;
    }
    case Type::ListElement: {
      const Tree* values = e->child(0);
      const Tree* index = e->child(1);
      assert(Integer::Is<uint8_t>(index));
      int i = Integer::Handler(index).to<uint8_t>() - 1;
      if (i < 0 || i >= Dimension::ListLength(values)) {
        return NAN;
      }
      assert(ctx);
      Context childCtx(*ctx);
      childCtx.m_listElement = i;
      std::complex<T> result = ToComplex<T>(values, &childCtx);
      return result;
    }
    case Type::ListSlice: {
      assert(ctx && ctx->m_listElement != -1);
      const Tree* values = e->child(0);
      const Tree* startIndex = e->child(1);
      assert(Integer::Is<uint8_t>(startIndex));
      assert(Integer::Is<uint8_t>(e->child(2)));
      int start = std::max(Integer::Handler(startIndex).to<uint8_t>() - 1, 0);
      assert(start >= 0);
      Context childCtx(*ctx);
      childCtx.m_listElement += start;
      std::complex<T> result = ToComplex<T>(values, &childCtx);
      return result;
    }
    case Type::ListSum:
    case Type::ListProduct: {
      assert(ctx);
      const Tree* values = e->child(0);
      int length = Dimension::ListLength(values);
      Context childCtx(*ctx);
      std::complex<T> result = e->isListSum() ? 0 : 1;
      for (int i = 0; i < length; i++) {
        childCtx.m_listElement = i;
        std::complex<T> v = ToComplex<T>(values, &childCtx);
        result = e->isListSum() ? result + v : result * v;
      }
      return result;
    }
    case Type::Min:
    case Type::Max: {
      assert(ctx);
      const Tree* values = e->child(0);
      int length = Dimension::ListLength(values);
      Context childCtx(*ctx);
      T result;
      for (int i = 0; i < length; i++) {
        childCtx.m_listElement = i;
        std::complex<T> v = ToComplex<T>(values, &childCtx);
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
      int length = Dimension::ListLength(values);
      Context childCtx(*ctx);
      std::complex<T> sum = 0;
      std::complex<T> sumOfSquares = 0;
      T coefficientsSum = 0;
      for (int i = 0; i < length; i++) {
        childCtx.m_listElement = i;
        std::complex<T> v = ToComplex<T>(values, &childCtx);
        std::complex<T> c = ToComplex<T>(coefficients, &childCtx);
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
      std::complex<T> result = ToComplex<T>(list, ctx);
      list->removeTree();
      return result;
    }
    case Type::Median: {
      Tree* list = ToList<T>(e->child(0), ctx);
      TreeDatasetColumn<T> values(list);
      T median;
      if (Dimension::IsList(e->child(1))) {
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
    case Type::Piecewise:
      return ToComplex<T>(SelectPiecewiseBranch<T>(e, ctx), ctx);
    case Type::Distribution: {
      const Tree* child = e->child(0);
      T abscissa[DistributionMethod::k_maxNumberOfParameters];
      DistributionMethod::Type method = DistributionMethod::Get(e);
      for (int i = 0; i < DistributionMethod::numberOfParameters(method); i++) {
        std::complex<T> c = ToComplex<T>(child, ctx);
        if (c.imag() != 0) {
          return NAN;
        }
        abscissa[i] = c.real();
        child = child->nextTree();
      }
      T parameters[Distribution::k_maxNumberOfParameters];
      Distribution::Type distribution = Distribution::Get(e);
      for (int i = 0; i < Distribution::numberOfParameters(distribution); i++) {
        std::complex<T> c = ToComplex<T>(child, ctx);
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
      return UndefDependencies(e, ctx) ? NAN
                                       : ToComplex<T>(Dependency::Main(e), ctx);
    }
    case Type::NonNull: {
      std::complex<T> x = ToComplex<T>(e->child(0), ctx);
      return x != std::complex<T>(0) ? std::complex<T>(0) : NAN;
    }
    case Type::RealPos: {
      std::complex<T> x = ToComplex<T>(e->child(0), ctx);
      return x.real() >= 0 && x.imag() == 0 ? std::complex<T>(0) : NonReal<T>();
    }
    /* Handle units as their scalar value in basic SI so prefix and
     * representative homogeneity isn't necessary. Dimension is expected to be
     * handled at this point. */
    case Type::Unit:
      return Units::Unit::GetValue(e);
    case Type::PhysicalConstant:
      return PhysicalConstant::GetProperties(e).m_value;
    case Type::LnUser: {
      std::complex<T> x = ToComplex<T>(e->child(0), ctx);
      if (ctx && ctx->m_complexFormat == ComplexFormat::Real &&
          (x.real() < 0 || x.imag() != 0)) {
        return NonReal<T>();
      }
      return x == std::complex<T>(0) ? NAN : std::log(x);
    }
    default:;
  }
  // The remaining operators are defined only on reals
  // assert(e->numberOfChildren() <= 2);
  if (e->numberOfChildren() > 2) {
    return NAN;
  }
  T child[2];
  for (IndexedChild<const Tree*> childNode : e->indexedChildren()) {
    std::complex<T> app = ToComplex<T>(childNode, ctx);
    if (app.imag() != 0) {
      return NAN;
    }
    if (std::isnan(app.real())) {
      return NAN;
    }
    child[childNode.index] = app.real();
  }
  switch (e->type()) {
    case Type::Decimal:
      return child[0] * std::pow(static_cast<T>(10.0), -child[1]);
    case Type::PowReal: {
      Context childCtx(*ctx);
      childCtx.m_complexFormat = ComplexFormat::Real;
      return ApproximatePower<T>(e, &childCtx);
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
      return e->isQuo() ? quotient : std::round(a - b * quotient);
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

bool Approximation::ApproximateToComplexTree(Tree* e, const Context* ctx) {
  e->moveTreeOverTree(ToBeautifiedComplex<double>(e, ctx));
  return true;
}

template <typename T>
bool Approximation::ToBoolean(const Tree* e, const Context* ctx) {
  if (e->isTrue()) {
    return true;
  }
  if (e->isFalse()) {
    return false;
  }
  if (e->isInequality()) {
    T a = To<T>(e->child(0), ctx);
    T b = To<T>(e->child(1), ctx);
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
    std::complex<T> a = ToComplex<T>(e->child(0), ctx);
    std::complex<T> b = ToComplex<T>(e->child(1), ctx);
    return e->isEqual() == (a == b);
  }
  if (e->isPiecewise()) {
    return ToBoolean<T>(SelectPiecewiseBranch<T>(e, ctx), ctx);
  }
  if (e->isList()) {
    assert(ctx && ctx->m_listElement != -1);
    return ToBoolean<T>(e->child(ctx->m_listElement), ctx);
  }
  if (e->isParentheses()) {
    return ToBoolean<T>(e->child(0), ctx);
  }
  if (e->isListSort()) {
    /* TODO we are computing all elements and sorting the list for all
     * elements, this is awful */
    Tree* list = ToList<T>(e->child(0), ctx);
    NAry::Sort(list);
    bool result = ToBoolean<T>(list, ctx);
    list->removeTree();
    return result;
  }
  assert(e->isLogicalOperator());
  bool a = ToBoolean<T>(e->child(0), ctx);
  if (e->isLogicalNot()) {
    return !a;
  }
  if (e->isDep()) {
    // TODO: Undefined boolean return false for now.
    return !UndefDependencies(e, ctx) && a;
  }
  bool b = ToBoolean<T>(e->child(1), ctx);
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
Tree* Approximation::ToList(const Tree* e, const Context* ctx) {
  assert(ctx);
  Dimension dimension = Dimension::Get(e);
  int length = Dimension::ListLength(e);
  assert(length != Dimension::k_nonListListLength);
  Context childCtx(*ctx);
  Tree* list = SharedTreeStack->pushList(length);
  for (int i = 0; i < length; i++) {
    childCtx.m_listElement = i;
    ToTree<T>(e, dimension, &childCtx);
  }
  return list;
}

template <typename T>
Tree* Approximation::ToPoint(const Tree* e, const Context* ctx) {
  assert(ctx);
  Context childCtx(*ctx);
  Tree* point = SharedTreeStack->pushPoint();
  childCtx.m_pointElement = 0;
  ToBeautifiedComplex<T>(e, &childCtx);
  childCtx.m_pointElement = 1;
  ToBeautifiedComplex<T>(e, &childCtx);
  return point;
}

/* Using our consteval operator- inside a template<float> does not work with
 * llvm14 it works with 17. */
constexpr KTree minusOne = -1_e;
constexpr KTree one = 1_e;

template <typename T>
Tree* Approximation::ToMatrix(const Tree* e, const Context* ctx) {
  /* TODO: Normal matrix nodes and operations with approximated children are
   * used to carry matrix approximation. A dedicated node that knows its
   * children have a fixed size would be more efficient. */
  if (e->isMatrix()) {
    Tree* m = e->cloneNode();
    for (const Tree* child : e->children()) {
      ToBeautifiedComplex<T>(child, ctx);
    }
    return m;
  }
  switch (e->type()) {
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
      b->moveTreeOverTree(Matrix::ScalarMultiplication(minusOne, b, true, ctx));
      Matrix::Addition(a, b, true, ctx);
      a->removeTree();
      a->removeTree();
      return a;
    }
    case Type::Mult: {
      bool resultIsMatrix = false;
      Tree* result = nullptr;
      for (const Tree* child : e->children()) {
        bool childIsMatrix = Dimension::Get(child).isMatrix();
        Tree* approx = childIsMatrix ? ToMatrix<T>(child, ctx)
                                     : ToBeautifiedComplex<T>(child, ctx);
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
      one->cloneTree();
      e->child(1)->cloneTree();
      ToBeautifiedComplex<T>(s, ctx);
      s->removeTree();
      s->moveTreeOverTree(Matrix::ScalarMultiplication(s, a, true, ctx));
      a->removeTree();
      return a;
    }
    case Type::PowMatrix: {
      const Tree* base = e->child(0);
      const Tree* index = base->nextTree();
      T value = To<T>(index, ctx);
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
      Dimension dim = Dimension::Get(e->child(0));
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
    case Type::Dep:
      return UndefDependencies(e, ctx) ? KUndef->cloneTree()
                                       : ToMatrix<T>(Dependency::Main(e), ctx);
    default:;
  }
  return KUndef->cloneTree();
}

template <typename T>
T Approximation::To(const Tree* e, T x, const Context* ctx) {
  T result;
  if (!ctx) {
    Context context(nullptr, AngleUnit::Radian, ComplexFormat::Cartesian, x);
    ctx = &context;
    result = To<T>(e, &context);
  } else {
    Context childCtx(*ctx);
    childCtx.setLocalValue(x);
    result = To<T>(e, &childCtx);
  }
  return result;
}

template <typename T>
const Tree* Approximation::SelectPiecewiseBranch(const Tree* piecewise,
                                                 const Context* ctx) {
  assert(piecewise->isPiecewise());
  int n = piecewise->numberOfChildren();
  int i = 0;
  const Tree* child = piecewise->child(0);
  while (i < n) {
    const Tree* condition = child->nextTree();
    i++;
    if (i == n || ToBoolean<T>(condition, ctx)) {
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
int Approximation::IndexOfActivePiecewiseBranchAt(const Tree* piecewise, T x,
                                                  const Context* ctx) {
  assert(!ctx);
  assert(piecewise->isPiecewise());
  // TODO piecewises depending on a random will not work
  Context context(nullptr, AngleUnit::Radian, ComplexFormat::Cartesian, x);
  ctx = &context;
  const Tree* branch = SelectPiecewiseBranch<T>(piecewise, ctx);
  ctx = nullptr;
  if (branch == KUndef) {
    return -1;
  }
  return piecewise->indexOfChild(branch);
}

bool Approximation::CanApproximate(const Tree* e,
                                   int firstNonApproximableVarId) {
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

bool SkipApproximation(TypeBlock type) {
  return type.isFloat() || type.isComplexI();
}

bool SkipApproximation(TypeBlock type, TypeBlock parentType,
                       int indexInParent) {
  if (SkipApproximation(type)) {
    return true;
  }
  switch (parentType) {
    case Type::ATrig:
    case Type::Trig:
      // Do not approximate second term in case tree isn't replaced.
      return (indexInParent == 1);
    case Type::PowMatrix:
    case Type::Pow:
      // Note: After projection, Power's second term should always be integer.
      return (indexInParent == 1 && type.isInteger());
    case Type::Identity:
      return true;
    default:
      return false;
  }
}

bool Approximation::ApproximateAndReplaceEveryScalar(
    Tree* e, const ProjectionContext* ctx) {
  Context context(nullptr, ctx ? ctx->m_angleUnit : AngleUnit::Radian,
                  ctx ? ctx->m_complexFormat : ComplexFormat::Cartesian);
  if (SkipApproximation(e->type())) {
    return false;
  }
  uint32_t hash = e->hash();
  bool result = PrivateApproximateAndReplaceEveryScalar(e, &context);
  /* TODO: We compare the CRC32 to prevent expressions such as 1.0+i*1.0 from
   * returning true at every call. We should detect and skip them in
   * SkipApproximation instead. */
  return result && hash != e->hash();
}

bool Approximation::PrivateApproximateAndReplaceEveryScalar(
    Tree* e, const Context* ctx) {
  if (CanApproximate(e) && Dimension::IsNonListScalar(e)) {
    e->moveTreeOverTree(ToTree<double>(e, Dimension(), ctx));
    return true;
  }
  bool changed = false;
  for (IndexedChild<Tree*> child : e->indexedChildren()) {
    if (!SkipApproximation(child->type(), e->type(), child.index)) {
      changed = PrivateApproximateAndReplaceEveryScalar(child, ctx) || changed;
    }
  }
  // TODO: Merge additions and multiplication's children if possible.
  return changed;
}

Tree* Approximation::ExtractRealPartIfImaginaryPartNegligible(const Tree* e) {
  if (GetComplexSign(e).isReal()) {
    return e->cloneTree();
  }
  std::complex<double> value = ToComplex<double>(e, nullptr);
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

template PointOrScalar<float> Approximation::RootPreparedToPointOrScalar(
    const Tree*, float);
template PointOrScalar<double> Approximation::RootPreparedToPointOrScalar(
    const Tree*, double);
template PointOrScalar<float> Approximation::RootToPointOrScalarPrivate(
    const Tree*, bool, bool, float, int, AngleUnit, ComplexFormat);
template PointOrScalar<double> Approximation::RootToPointOrScalarPrivate(
    const Tree*, bool, bool, double, int, AngleUnit, ComplexFormat);

template std::complex<float> Approximation::RootPreparedToComplex<float>(
    const Tree*, float);
template std::complex<double> Approximation::RootPreparedToComplex<double>(
    const Tree*, double);

template float Approximation::FloatBinomial<float>(float, float);
template double Approximation::FloatBinomial<double>(double, double);

template std::complex<float> Approximation::ToComplex<float>(
    const Tree*, const Context* ctx);
template std::complex<double> Approximation::ToComplex<double>(
    const Tree*, const Context* ctx);

template Tree* Approximation::ToPoint<float>(const Tree*, const Context* ctx);
template Tree* Approximation::ToPoint<double>(const Tree*, const Context* ctx);

template Tree* Approximation::RootTreeToTree<float>(const Tree*, AngleUnit,
                                                    ComplexFormat);
template Tree* Approximation::RootTreeToTree<double>(const Tree*, AngleUnit,
                                                     ComplexFormat);
template float Approximation::To(const Tree* e, float x, const Context* ctx);
template double Approximation::To(const Tree* e, double x, const Context* ctx);

template int Approximation::IndexOfActivePiecewiseBranchAt<float>(
    const Tree* piecewise, float x, const Context* ctx);
template int Approximation::IndexOfActivePiecewiseBranchAt<double>(
    const Tree* piecewise, double x, const Context* ctx);

}  // namespace Poincare::Internal
