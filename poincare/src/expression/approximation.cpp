#include "approximation.h"

#include <apps/shared/global_context.h>
#include <math.h>
#include <omg/float.h>
#include <omg/signaling_nan.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/node_iterator.h>
#include <poincare/src/numeric/statistics_dataset.h>
#include <poincare/src/probability/distribution_method.h>

#include <bit>
#include <complex>

#include "arithmetic.h"
#include "beautification.h"
#include "context.h"
#include "decimal.h"
#include "dependency.h"
#include "dimension.h"
#include "float.h"
#include "list.h"
#include "matrix.h"
#include "physical_constant.h"
#include "random.h"
#include "rational.h"
#include "symbol.h"
#include "undefined.h"
#include "unit.h"
#include "variables.h"
#include "vector.h"

namespace Poincare::Internal {

/* For function calls that may alter s_context.
 * TODO : There is no clear indication s_context will be altered or not, and
 * this could be avoided by reworking how s_context is handled. */
#define OutOfContext(F)           \
  [&]() {                         \
    Context* context = s_context; \
    s_context = nullptr;          \
    auto r = F;                   \
    s_context = context;          \
    return r;                     \
  }()

/* Static members */

Approximation::Context* Approximation::s_context = nullptr;

// With a nullptr context, seeded random will be undef.
Random::Context* Approximation::s_randomContext = nullptr;

/* Approximation::Context */

Approximation::Context::Context(AngleUnit angleUnit,
                                ComplexFormat complexFormat,
                                VariableType abscissa)
    : m_angleUnit(angleUnit),
      m_complexFormat(complexFormat),
      m_variablesOffset(k_maxNumberOfVariables),
      m_encounteredComplex(false)
#if ASSERTIONS
      ,
      m_listElement(-1),
      m_pointElement(-1)
#endif
{
  for (int i = 0; i < k_maxNumberOfVariables; i++) {
    m_variables[i] = NAN;
  }
  setLocalValue(abscissa);
}

template <typename T>
PointOrScalar<T> Approximation::RootPreparedToPointOrScalar(
    const Tree* preparedFunction, T abscissa) {
  Dimension dimension = Dimension::GetDimension(preparedFunction);
  assert(dimension.isScalar() || dimension.isPoint());
  return RootToPointOrScalarPrivate<T>(preparedFunction, dimension.isPoint(),
                                       true, abscissa);
}

template <typename T>
Tree* Approximation::RootTreeToTree(const Tree* node, AngleUnit angleUnit,
                                    ComplexFormat complexFormat) {
  if (!Dimension::DeepCheckDimensions(node) ||
      !Dimension::DeepCheckListLength(node)) {
    return KUndefUnhandledDimension->clone();
  }
  return RootTreeToTreePrivate<T>(node, angleUnit, complexFormat,
                                  Dimension::GetDimension(node),
                                  Dimension::GetListLength(node));
}

template <typename T>
Tree* Approximation::RootTreeToTreePrivate(const Tree* node,
                                           AngleUnit angleUnit,
                                           ComplexFormat complexFormat,
                                           Dimension dim, int listLength) {
  assert(Dimension::DeepCheckDimensions(node) &&
         Dimension::DeepCheckListLength(node));
  assert(listLength == Dimension::GetListLength(node));
  assert(dim == Dimension::GetDimension(node));

  Random::Context randomContext;
  s_randomContext = &randomContext;
  Context context(angleUnit, complexFormat);
  s_context = &context;
  Tree* clone = node->clone();
  // TODO we should rather assume variable projection has already been done
  Variables::ProjectLocalVariablesToId(clone);

  if (listLength != Dimension::k_nonListListLength) {
    assert(!dim.isMatrix());
    int old = s_context->m_listElement;
    SharedTreeStack->push<Type::List>(listLength);
    for (int i = 0; i < listLength; i++) {
      s_context->m_listElement = i;
      ToTree<T>(clone, dim);
    }
    s_context->m_listElement = old;
  } else {
    ToTree<T>(clone, dim);
  }

  clone->removeTree();
  s_randomContext = nullptr;
  s_context = nullptr;
  return clone;
}

template <typename T>
Tree* Approximation::ToBeautifiedComplex(const Tree* node) {
  std::complex<T> value = ToComplex<T>(node);
  /* TODO: no s_context => complexFormat = cartesian for now, and it is only
   * used with OutOfContext matrix operations, we should impose a context
   * instead. */
  // Return nonreal if not undef and a complex was encountered in real mode
  if (s_context && s_context->m_encounteredComplex) {
    if (s_context->m_complexFormat == ComplexFormat::Real &&
        !std::isnan(value.real()) && !std::isnan(value.imag())) {
      return KNonReal->clone();
    }
    s_context->m_encounteredComplex = false;
  }
  return Beautification::PushBeautifiedComplex(
      value, s_context ? s_context->m_complexFormat : ComplexFormat::Cartesian);
}

template <typename T>
Tree* Approximation::ToTree(const Tree* node, Dimension dim) {
  if (dim.isBoolean()) {
    return (ToBoolean<T>(node) ? KTrue : KFalse)->clone();
  }
  if (dim.isScalar()) {
    return ToBeautifiedComplex<T>(node);
  }
  assert(dim.isPoint() || dim.isMatrix());
  Tree* result = dim.isPoint() ? ToPoint<T>(node) : ToMatrix<T>(node);
  if (Undefined::ShallowBubbleUpUndef(result)) {
    return result;
  }
  for (Tree* child : result->children()) {
    child->moveTreeOverTree(ToBeautifiedComplex<T>(child));
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
    const Tree* node, bool isPoint, bool isPrepared, T abscissa,
    AngleUnit angleUnit, ComplexFormat complexFormat) {
  Random::Context randomContext;
  s_randomContext = &randomContext;
  Context context(angleUnit, complexFormat, abscissa);
  s_context = &context;
  Tree* clone;
  if (!isPrepared) {
    clone = node->clone();
    // TODO we should rather assume variable projection has already been done
    Variables::ProjectLocalVariablesToId(clone);
    node = clone;
  }
  T xScalar;
  if (isPoint) {
    s_context->m_pointElement = 0;
    xScalar = To<T>(node);
    s_context->m_pointElement = 1;
  }
  T yScalar = To<T>(node);
  if (!isPrepared) {
    clone->removeTree();
  }
  s_randomContext = nullptr;
  s_context = nullptr;
  return isPoint ? PointOrScalar<T>(xScalar, yScalar)
                 : PointOrScalar<T>(yScalar);
}

/* Helpers */

template <typename T>
static T FloatAddition(T a, T b) {
  return a + b;
}

template <typename T>
static T FloatSubtraction(T a, T b) {
  return a - b;
}

template <typename T>
T Approximation::FloatBinomial(T k, T n) {
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
T Approximation::FloatGCD(T a, T b) {
  T result = Arithmetic::GCD(a, b);
  if (!IsIntegerRepresentationAccurate(result)) {
    return NAN;
  }
  return result;
}

template <typename T>
T Approximation::FloatLCM(T a, T b) {
  bool overflow = false;
  T result = Arithmetic::LCM(a, b, &overflow);
  if (overflow || !IsIntegerRepresentationAccurate(result)) {
    return NAN;
  }
  return result;
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

template <typename T>
static T FloatLog(T a, T b) {
  return a == static_cast<T>(0) || b == static_cast<T>(0)
             ? NAN
             : FloatDivision(std::log(a), std::log(b));
}

// Return true if one of the dependencies is undefined
bool UndefDependencies(const Tree* dep) {
  // Dependency children may have different dimensions.
  for (const Tree* child : Dependency::Dependencies(dep)->children()) {
    Dimension dim = Dimension::GetDimension(child);
    if (dim.isScalar()) {
      // Optimize most cases
      std::complex<float> c = Approximation::ToComplex<float>(child);
      if (std::isnan(c.real())) {
        return true;
      }
      assert(!std::isnan(c.imag()));
    } else {
      Tree* a =
          Approximation::ToTree<float>(child, Dimension::GetDimension(child));
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
std::complex<T> Approximation::ToComplex(const Tree* node) {
  std::complex<T> value = ToComplexSwitch<T>(node);
  if (s_context && value.imag() != 0) {
    s_context->m_encounteredComplex = true;
  }
  // We used to flush negative zeros here but it was not worth
  return value;
}

template <typename T>
std::complex<T> Approximation::ToComplexSwitch(const Tree* node) {
  /* TODO: the second part of this function and several ifs in different cases
   * act differently / more precisely on reals. We should have a dedicated,
   * faster, simpler and more precise real approximation to be used in every
   * cases where we know for sure there are no complexes. */
  assert(node->isExpression());
  if (node->isUndefined()) {
    /* TODO: Find a way to pass exact type up to PushBeautifiedComplex, at least
     * to have something similar to similar to old SetEncounteredComplex method
     * and distinguish undef from nonreal. */
    return NAN;
  }
  if (node->isRational()) {
    return Rational::Numerator(node).to<T>() /
           Rational::Denominator(node).to<T>();
  }

  if (node->isRandomNode()) {
    return Random::Approximate<T>(node, s_randomContext,
                                  s_context ? s_context->m_listElement : -1);
  }
  switch (node->type()) {
    case Type::Parenthesis:
      return ToComplex<T>(node->child(0));
    case Type::ComplexI:
      return std::complex<T>(0, 1);
    case Type::Pi:
      return M_PI;
    case Type::EulerE:
      return M_E;
    case Type::SingleFloat:
      return FloatNode::FloatTo(node);
    case Type::DoubleFloat:
      return FloatNode::DoubleTo(node);
    case Type::Add:
      return MapAndReduce<T, std::complex<T>>(node,
                                              FloatAddition<std::complex<T>>);
    case Type::Mult:
      return MapAndReduce<T, std::complex<T>>(node, FloatMultiplication<T>);
    case Type::Div:
      return MapAndReduce<T, std::complex<T>>(node, FloatDivision<T>);
    case Type::Sub:
      return MapAndReduce<T, std::complex<T>>(
          node, FloatSubtraction<std::complex<T>>);
    case Type::Pow:
      return ApproximatePower<T>(node, s_context ? s_context->m_complexFormat
                                                 : ComplexFormat::Cartesian);
    case Type::Logarithm:
      return MapAndReduce<T, std::complex<T>>(node, FloatLog<std::complex<T>>);
    case Type::GCD:
      return MapAndReduce<T, T>(node, FloatGCD<T>,
                                PositiveIntegerApproximation<T>);
    case Type::LCM:
      return MapAndReduce<T, T>(node, FloatLCM<T>,
                                PositiveIntegerApproximation<T>);
    case Type::Sqrt: {
      std::complex<T> c = ToComplex<T>(node->child(0));
      return NeglectRealOrImaginaryPartIfNeglectable(std::sqrt(c), c);
    }
    case Type::Root: {
      std::complex<T> base = ToComplex<T>(node->child(0));
      std::complex<T> exp = ToComplex<T>(node->child(1));
      /* If the complexFormat is Real, we look for nthroot of form root(x,q)
       * with x real and q integer because they might have a real form which
       * does not correspond to the principale angle. */
      if (s_context->m_complexFormat == Preferences::ComplexFormat::Real &&
          exp.imag() == 0.0 && std::round(exp.real()) == exp.real()) {
        // root(x, q) with q integer and x real
        std::complex<T> result =
            ComputeNotPrincipalRealRootOfRationalPow<T>(base, 1, exp.real());
        if (!std::isnan(result.real()) && !std::isnan(result.imag())) {
          return result;
        }
      }
      return ComputeComplexPower<T>(base, std::complex<T>(1.0) / (exp),
                                    s_context->m_complexFormat);
    }
    case Type::Exp:
      return std::exp(ToComplex<T>(node->child(0)));
    case Type::Log:
    case Type::Ln: {
      std::complex<T> c = ToComplex<T>(node->child(0));
      /* log has a branch cut on ]-inf, 0]: it is then multivalued on this cut.
       * We followed the convention chosen by the lib c++ of llvm on ]-inf+0i,
       * 0+0i] (warning: log takes the other side of the cut values on ]-inf-0i,
       * 0-0i]). We manually handle the case where the argument is null, as the
       * lib c++ gives log(0) = -inf, which is only a generous shorthand for the
       * limit. */
      return c == std::complex<T>(0) ? NAN
             : node->isLog()         ? std::log10(c)
                                     : std::log(c);
    }
    case Type::Abs:
      return std::abs(ToComplex<T>(node->child(0)));
    case Type::Arg:
      return std::arg(ToComplex<T>(node->child(0)));
    case Type::Inf:
      return INFINITY;
    case Type::Conj:
      return std::conj(ToComplex<T>(node->child(0)));
    case Type::Opposite:
      return FloatMultiplication<T>(-1, ToComplex<T>(node->child(0)));
    case Type::Re: {
      /* TODO_PCJ: Complex NAN should be used in most of the code. Make sure a
       * NAN result cannot be lost. */
      std::complex<T> c = ToComplex<T>(node->child(0));
      return std::isnan(c.imag()) ? NAN : c.real();
    }
    case Type::Im: {
      std::complex<T> c = ToComplex<T>(node->child(0));
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
      return TrigonometricToComplex(node->type(), ToComplex<T>(node->child(0)),
                                    s_context->m_angleUnit);
    case Type::SinH:
    case Type::CosH:
    case Type::TanH:
    case Type::ArSinH:
    case Type::ArCosH:
    case Type::ArTanH:
      return HyperbolicToComplex(node->type(), ToComplex<T>(node->child(0)));
    case Type::Trig:
    case Type::ATrig: {
      std::complex<T> a = ToComplex<T>(node->child(0));
      std::complex<T> b = ToComplex<T>(node->child(1));
      assert(b == static_cast<T>(0.0) || b == static_cast<T>(1.0));
      bool isCos = b == static_cast<T>(0.0);
      if (node->isTrig()) {
        return TrigonometricToComplex(isCos ? Type::Cos : Type::Sin, a,
                                      AngleUnit::Radian);
      }
      return TrigonometricToComplex(isCos ? Type::ACos : Type::ASin, a,
                                    AngleUnit::Radian);
    }
    case Type::ATanRad:
      return TrigonometricToComplex(Type::ATan, ToComplex<T>(node->child(0)),
                                    AngleUnit::Radian);
    case Type::Var: {
      if (!s_context) {
        return NAN;
      }
      // Local variable
      return s_context->variable(Variables::Id(node));
    }
    case Type::UserSymbol:
    case Type::UserFunction:
      // Global variable
      return NAN;
    case Type::UserSequence: {
      int index = Shared::GlobalContext::sequenceStore->SequenceIndexForName(
          Internal::Symbol::GetName(node)[0]);
      Shared::Sequence sequence =
          Shared::GlobalContext::sequenceStore->sequenceAtIndex(index);
      T rank = To<T>(node->child(0));
      if (std::isnan(rank) || std::floor(rank) != rank) {
        return NAN;
      }
      double result =
          Shared::GlobalContext::sequenceCache->storedValueOfSequenceAtRank(
              index, rank);
      if (OMG::IsSignalingNan(result)) {
        // compute value if not in cache
        result = OutOfContext(sequence.approximateAtRank(
            rank, Shared::GlobalContext::sequenceCache));
      }
      return result;
    }
    /* Analysis */
    case Type::Sum:
    case Type::Product: {
      const Tree* lowerBoundChild = node->child(Parametric::k_lowerBoundIndex);
      std::complex<T> low = ToComplex<T>(lowerBoundChild);
      if (low.imag() != 0 || (int)low.real() != low.real()) {
        return NAN;
      }
      const Tree* upperBoundChild = lowerBoundChild->nextTree();
      std::complex<T> up = ToComplex<T>(upperBoundChild);
      if (up.imag() != 0 || (int)up.real() != up.real()) {
        return NAN;
      }
      int lowerBound = low.real();
      int upperBound = up.real();
      const Tree* child = upperBoundChild->nextTree();
      s_context->shiftVariables();
      std::complex<T> result = node->isSum() ? 0 : 1;
      for (int k = lowerBound; k <= upperBound; k++) {
        s_context->setLocalValue(k);
        std::complex<T> value = ToComplex<T>(child);
        if (node->isSum()) {
          result += value;
        } else {
          result *= value;
        }
        if (std::isnan(result.real()) || std::isnan(result.imag())) {
          break;
        }
      }
      s_context->unshiftVariables();
      return result;
    }
    case Type::Diff:
    case Type::NthDiff: {
      constexpr static int k_maxOrderForApproximation = 4;
      int order;
      const Tree* derivand;
      if (node->isNthDiff()) {
        T orderReal = To<T>(node->child(2));
        if (orderReal != std::floor(orderReal)) {
          return NAN;
        }
        order = orderReal;
        derivand = node->child(3);
      } else {
        order = 1;
        derivand = node->child(2);
      }
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
      std::complex<T> at = ToComplex<T>(node->child(1));
      if (std::isnan(at.real()) || at.imag() != 0) {
        return NAN;
      }
      s_context->shiftVariables();
      T result = ApproximateDerivative(derivand, at.real(), order);
      s_context->unshiftVariables();
      return result;
    }
    case Type::Integral:
      return ApproximateIntegral<T>(node);

    /* Matrices */
    case Type::Norm:
    case Type::Trace:
    case Type::Det: {
      Tree* m = ToMatrix<T>(node->child(0));
      Tree* value;
      if (node->isDet()) {
        OutOfContext(Matrix::RowCanonize(m, true, &value, true));
      } else if (node->isNorm()) {
        value = OutOfContext(Vector::Norm(m));
      } else {
        value = OutOfContext(Matrix::Trace(m));
      }
      std::complex<T> v = ToComplex<T>(value);
      value->removeTree();
      m->removeTree();
      return v;
    }
    case Type::Dot: {
      // TODO use complex conjugate ?
      Tree* u = ToMatrix<T>(node->child(0));
      Tree* v = ToMatrix<T>(node->child(1));
      Tree* r = OutOfContext(Vector::Dot(u, v));
      std::complex<T> result = ToComplex<T>(r);
      r->removeTree();
      v->removeTree();
      u->removeTree();
      return result;
    }
    case Type::Point:
      assert(s_context->m_pointElement != -1);
      return ToComplex<T>(node->child(s_context->m_pointElement));
    /* Lists */
    case Type::List:
      assert(s_context->m_listElement != -1);
      return ToComplex<T>(node->child(s_context->m_listElement));
    case Type::ListSequence: {
      s_context->shiftVariables();
      // epsilon sequences starts at one
      assert(s_context->m_listElement != -1);
      s_context->setLocalValue(s_context->m_listElement + 1);
      std::complex<T> result = ToComplex<T>(node->child(2));
      s_context->unshiftVariables();
      return result;
    }
    case Type::Dim: {
      int n = Dimension::GetListLength(node->child(0));
      return n >= 0 ? n : NAN;
    }
    case Type::ListElement: {
      const Tree* values = node->child(0);
      const Tree* index = node->child(1);
      assert(Integer::Is<uint8_t>(index));
      int i = Integer::Handler(index).to<uint8_t>() - 1;
      if (i < 0 || i >= Dimension::GetListLength(values)) {
        return NAN;
      }
      int old = s_context->m_listElement;
      s_context->m_listElement = i;
      std::complex<T> result = ToComplex<T>(values);
      s_context->m_listElement = old;
      return result;
    }
    case Type::ListSlice: {
      assert(s_context->m_listElement != -1);
      const Tree* values = node->child(0);
      const Tree* startIndex = node->child(1);
      assert(Integer::Is<uint8_t>(startIndex));
      int start = std::max(Integer::Handler(startIndex).to<uint8_t>() - 1, 0);
      int old = s_context->m_listElement;
      s_context->m_listElement += start;
      std::complex<T> result = ToComplex<T>(values);
      s_context->m_listElement = old;
      return result;
    }
    case Type::ListSum:
    case Type::ListProduct: {
      const Tree* values = node->child(0);
      int length = Dimension::GetListLength(values);
      int old = s_context->m_listElement;
      std::complex<T> result = node->isListSum() ? 0 : 1;
      for (int i = 0; i < length; i++) {
        s_context->m_listElement = i;
        std::complex<T> v = ToComplex<T>(values);
        result = node->isListSum() ? result + v : result * v;
      }
      s_context->m_listElement = old;
      return result;
    }
    case Type::Min:
    case Type::Max: {
      const Tree* values = node->child(0);
      int length = Dimension::GetListLength(values);
      int old = s_context->m_listElement;
      T result;
      for (int i = 0; i < length; i++) {
        s_context->m_listElement = i;
        std::complex<T> v = ToComplex<T>(values);
        if (v.imag() != 0 || std::isnan(v.real())) {
          return NAN;
        }
        if (i == 0 ||
            (node->isMin() ? (v.real() < result) : (v.real() > result))) {
          result = v.real();
        }
      }
      s_context->m_listElement = old;
      return result;
    }
    case Type::Mean:
    case Type::StdDev:
    case Type::SampleStdDev:
    case Type::Variance: {
      const Tree* values = node->child(0);
      const Tree* coefficients = node->child(1);
      int length = Dimension::GetListLength(values);
      int old = s_context->m_listElement;
      std::complex<T> sum = 0;
      std::complex<T> sumOfSquares = 0;
      T coefficientsSum = 0;
      for (int i = 0; i < length; i++) {
        s_context->m_listElement = i;
        std::complex<T> v = ToComplex<T>(values);
        std::complex<T> c = ToComplex<T>(coefficients);
        if (c.imag() != 0 || c.real() < 0) {
          return NAN;
        }
        sum += c.real() * v;
        // TODO v * conj(v) ?
        sumOfSquares += c.real() * v * v;
        coefficientsSum += c.real();
      }
      s_context->m_listElement = old;
      if (coefficientsSum == 0) {
        return NAN;
      }
      sum /= coefficientsSum;
      if (node->isMean()) {
        return sum;
      }
      sumOfSquares /= coefficientsSum;
      std::complex<T> var = sumOfSquares - sum * sum;
      if (node->isVariance()) {
        return var;
      }
      std::complex<T> stdDev = std::pow(var, std::complex<T>(0.5));
      if (node->isStdDev()) {
        return stdDev;
      }
      T sampleStdDevCoef = std::pow(1 + 1 / (coefficientsSum - 1), 0.5);
      return stdDev * sampleStdDevCoef;
    }
    case Type::ListSort: {
      /* TODO we are computing all elements and sorting the list for all
       * elements, this is awful */
      Tree* list = ToList<T>(node->child(0));
      OutOfContext(NAry::Sort(list));
      std::complex<T> result = ToComplex<T>(list);
      list->removeTree();
      return result;
    }
    case Type::Median: {
      Tree* list = ToList<T>(node->child(0));
      TreeDatasetColumn<T> values(list);
      T median;
      if (Dimension::GetListLength(node->child(1)) !=
          Dimension::k_nonListListLength) {
        Tree* weightsList = ToList<T>(node->child(1));
        TreeDatasetColumn<T> weights(weightsList);
        median = OutOfContext(StatisticsDataset<T>(&values, &weights).median());
        weightsList->removeTree();
      } else {
        median = OutOfContext(StatisticsDataset<T>(&values).median());
      }
      list->removeTree();
      return median;
    }
    case Type::Piecewise:
      return ToComplex<T>(SelectPiecewiseBranch<T>(node));
    case Type::Distribution: {
      const Tree* child = node->child(0);
      T abscissa[DistributionMethod::k_maxNumberOfParameters];
      DistributionMethod::Type method = DistributionMethod::Get(node);
      for (int i = 0; i < DistributionMethod::numberOfParameters(method); i++) {
        std::complex<T> c = ToComplex<T>(child);
        if (c.imag() != 0) {
          return NAN;
        }
        abscissa[i] = c.real();
        child = child->nextTree();
      }
      T parameters[Distribution::k_maxNumberOfParameters];
      Distribution::Type distribution = Distribution::Get(node);
      for (int i = 0; i < Distribution::numberOfParameters(distribution); i++) {
        std::complex<T> c = ToComplex<T>(child);
        if (c.imag() != 0) {
          return NAN;
        }
        parameters[i] = c.real();
        child = child->nextTree();
      }
      return DistributionMethod::Get(method)->EvaluateAtAbscissa(
          abscissa, Distribution::Get(distribution), parameters);
    }
    case Type::Dependency: {
      return UndefDependencies(node) ? NAN
                                     : ToComplex<T>(Dependency::Main(node));
    }
    /* At this point, the overall expression is expected to be Scalar, but it
     * could be 1_m/1_ft. Approximating to ratios is enough. */
    case Type::Unit:
      return Units::Unit::GetRepresentative(node)->ratio();
    case Type::PhysicalConstant:
      return PhysicalConstant::GetProperties(node).m_value;
    default:;
  }
  // The remaining operators are defined only on reals
  // assert(node->numberOfChildren() <= 2);
  if (node->numberOfChildren() > 2) {
    return NAN;
  }
  T child[2];
  for (int i = 0; const Tree* childNode : node->children()) {
    std::complex<T> app = ToComplex<T>(childNode);
    if (app.imag() != 0) {
      return NAN;
    }
    if (std::isnan(app.real())) {
      return NAN;
    }
    child[i++] = app.real();
  }
  switch (node->type()) {
    case Type::Decimal:
      return child[0] *
             std::pow(10.0, -static_cast<T>(Decimal::DecimalOffset(node)));
    case Type::PowReal: {
      T a = child[0];
      T b = child[1];
      if ((std::fabs(a) == INFINITY && b == static_cast<T>(0.0)) ||
          (std::fabs(a) == static_cast<T>(1.0) && std::fabs(b) == INFINITY)) {
        /* std::pow(±Inf,0) = std::pow(±1,±Inf) = 1 but we want undef. */
        return NAN;
      }
      if ((a == -INFINITY && b == INFINITY) ||
          (a < static_cast<T>(-1.0) && b == INFINITY) ||
          (static_cast<T>(-1.0) < a && a <= static_cast<T>(0.0) &&
           b == -INFINITY)) {
        /* (-inf)^inf, a^inf with a <-1 and a^(-inf) with -1 < a <= 0 should be
         * approximated to complex infinity but we do not handle it for now. */
        return NAN;
      }
      /* PowerReal could not be reduced, b's reductions cannot be safely
       * interpreted as a rational. As a consequence, return NAN if a is
       * negative and b isn't an integer. */
      return (a < 0.0 && b != std::round(b)) ? NAN : std::pow(a, b);
    }
    case Type::Sign: {
      // TODO why no epsilon in Poincare ?
      return child[0] == 0 ? 0 : child[0] < 0 ? -1 : 1;
    }
    case Type::LnReal:
      // TODO unreal
      return child[0] <= 0 ? NAN : std::log(child[0]);
    case Type::Floor:
    case Type::Ceil: {
      /* Assume low deviation from natural numbers are errors */
      T delta = std::fabs((std::round(child[0]) - child[0]) / child[0]);
      if (delta <= OMG::Float::Epsilon<T>()) {
        return std::round(child[0]);
      }
      return node->isFloor() ? std::floor(child[0]) : std::ceil(child[0]);
    }
    case Type::Frac: {
      return child[0] - std::floor(child[0]);
    }
    case Type::Round: {
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
      if (a != (int)a || b != (int)b) {
        return NAN;
      }
      // TODO: is this really better than std::remainder ?
      T quotient = b >= 0 ? std::floor(a / b) : -std::floor(a / (-b));
      return node->isQuo() ? quotient : std::round(a - b * quotient);
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
      return FloatBinomial<T>(k, n);
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
      if (node->isParametric()) {
        // TODO: Explicit tree if it contains random nodes.
      }
      // TODO: Implement more Types
      assert(false);
      return NAN;
  }
}

bool Approximation::ApproximateToComplexTree(Tree* node) {
  node->moveTreeOverTree(ToBeautifiedComplex<double>(node));
  return true;
}

template <typename T>
bool Approximation::ToBoolean(const Tree* node) {
  if (node->isTrue()) {
    return true;
  }
  if (node->isFalse()) {
    return false;
  }
  if (node->isInequality()) {
    T a = To<T>(node->child(0));
    T b = To<T>(node->child(1));
    if (node->isInferior()) {
      return a < b;
    }
    if (node->isInferiorEqual()) {
      return a <= b;
    }
    if (node->isSuperior()) {
      return a > b;
    }
    assert(node->isSuperiorEqual());
    return a >= b;
  }
  if (node->isComparison()) {
    assert(node->isEqual() || node->isNotEqual());
    std::complex<T> a = ToComplex<T>(node->child(0));
    std::complex<T> b = ToComplex<T>(node->child(1));
    return node->isEqual() == (a == b);
  }
  if (node->isPiecewise()) {
    return ToBoolean<T>(SelectPiecewiseBranch<T>(node));
  }
  if (node->isList()) {
    assert(s_context->m_listElement != -1);
    return ToBoolean<T>(node->child(s_context->m_listElement));
  }
  if (node->isParenthesis()) {
    return ToBoolean<T>(node->child(0));
  }
  assert(node->isLogicalOperator());
  bool a = ToBoolean<T>(node->child(0));
  if (node->isLogicalNot()) {
    return !a;
  }
  if (node->isDependency()) {
    // TODO: Undefined boolean return false for now.
    return !UndefDependencies(node) && a;
  }
  bool b = ToBoolean<T>(node->child(1));
  switch (node->type()) {
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
      assert(false);
  }
}

template <typename T>
Tree* Approximation::ToList(const Tree* node) {
  int length = Dimension::GetListLength(node);
  int old = s_context->m_listElement;
  Tree* list = SharedTreeStack->push<Type::List>(length);
  for (int i = 0; i < length; i++) {
    s_context->m_listElement = i;
    ToBeautifiedComplex<T>(node);
  }
  s_context->m_listElement = old;
  return list;
}

template <typename T>
Tree* Approximation::ToPoint(const Tree* node) {
  int old = s_context->m_pointElement;
  Tree* point = SharedTreeStack->push(Type::Point);
  s_context->m_pointElement = 0;
  ToBeautifiedComplex<T>(node);
  s_context->m_pointElement = 1;
  ToBeautifiedComplex<T>(node);
  s_context->m_pointElement = old;
  return point;
}

/* Using our consteval operator- inside a template<float> does not work with
 * llvm14 it works with 17. */
constexpr KTree minusOne = -1_e;

template <typename T>
Tree* Approximation::ToMatrix(const Tree* node) {
  /* TODO: Normal matrix nodes and operations with approximated children are
   * used to carry matrix approximation. A dedicated node that knows its
   * children have a fixed size would be more efficient. */
  if (node->isMatrix()) {
    Tree* m = node->cloneNode();
    for (const Tree* child : node->children()) {
      ToBeautifiedComplex<T>(child);
    }
    return m;
  }
  switch (node->type()) {
    case Type::Add: {
      const Tree* child = node->child(0);
      int n = node->numberOfChildren() - 1;
      Tree* result = ToMatrix<T>(child);
      while (n--) {
        child = child->nextTree();
        Tree* approximatedChild = ToMatrix<T>(child);
        OutOfContext(Matrix::Addition(result, approximatedChild, true));
        approximatedChild->removeTree();
        result->removeTree();
      }
      return result;
    }
    case Type::Sub: {
      Tree* a = ToMatrix<T>(node->child(0));
      Tree* b = ToMatrix<T>(node->child(1));
      b->moveTreeOverTree(
          OutOfContext(Matrix::ScalarMultiplication(minusOne, b, true)));
      OutOfContext(Matrix::Addition(a, b));
      a->removeTree();
      a->removeTree();
      return a;
    }
    case Type::Mult: {
      bool resultIsMatrix = false;
      Tree* result = nullptr;
      for (const Tree* child : node->children()) {
        bool childIsMatrix = Dimension::GetDimension(child).isMatrix();
        Tree* approx =
            childIsMatrix ? ToMatrix<T>(child) : ToBeautifiedComplex<T>(child);
        if (result == nullptr) {
          resultIsMatrix = childIsMatrix;
          result = approx;
          continue;
        }
        if (resultIsMatrix && childIsMatrix) {
          OutOfContext(Matrix::Multiplication(result, approx, true));
        } else if (resultIsMatrix) {
          OutOfContext(Matrix::ScalarMultiplication(approx, result, true));
        } else {
          OutOfContext(Matrix::ScalarMultiplication(result, approx, true));
        }
        resultIsMatrix |= childIsMatrix;
        approx->removeTree();
        result->removeTree();
      }
      return result;
    }
    case Type::Div: {
      Tree* a = ToMatrix<T>(node->child(0));
      Tree* s = KDiv->cloneNode();
      (1_e)->clone();
      node->child(1)->clone();
      ToBeautifiedComplex<T>(s);
      s->removeTree();
      s->moveTreeOverTree(
          OutOfContext(Matrix::ScalarMultiplication(s, a, true)));
      a->removeTree();
      return a;
    }
    case Type::PowMatrix: {
      const Tree* base = node->child(0);
      const Tree* index = base->nextTree();
      T value = To<T>(index);
      if (std::isnan(value) || value != std::round(value)) {
        return KUndef->clone();
      }
      Tree* result = ToMatrix<T>(base);
      result->moveTreeOverTree(
          OutOfContext(Matrix::Power(result, value, true)));
      return result;
    }
    case Type::Inverse:
    case Type::Transpose: {
      Tree* result = ToMatrix<T>(node->child(0));
      result->moveTreeOverTree(node->isInverse()
                                   ? OutOfContext(Matrix::Inverse(result, true))
                                   : OutOfContext(Matrix::Transpose(result)));
      return result;
    }
    case Type::Ref:
    case Type::Rref: {
      Tree* result = ToMatrix<T>(node->child(0));
      OutOfContext(Matrix::RowCanonize(result, node->isRref(), nullptr, true));
      return result;
    }
    case Type::Dim: {
      Dimension dim = Dimension::GetDimension(node->child(0));
      assert(dim.isMatrix());
      Tree* result = SharedTreeStack->push<Type::Matrix>(1, 2);
      FloatNode::Push(T(dim.matrix.rows));
      FloatNode::Push(T(dim.matrix.cols));
      return result;
    }
    case Type::Cross: {
      Tree* u = ToMatrix<T>(node->child(0));
      Tree* v = ToMatrix<T>(node->child(1));
      OutOfContext(Vector::Cross(u, v));
      u->removeTree();
      u->removeTree();
      return u;
    }
    case Type::Piecewise:
      return ToMatrix<T>(SelectPiecewiseBranch<T>(node));
    case Type::Dependency:
      return UndefDependencies(node) ? KUndef->clone()
                                     : ToMatrix<T>(Dependency::Main(node));
    default:;
  }
  return KUndef->clone();
}

template <typename T>
const Tree* Approximation::SelectPiecewiseBranch(const Tree* piecewise) {
  int n = piecewise->numberOfChildren();
  int i = 0;
  const Tree* child = piecewise->child(0);
  while (i < n) {
    const Tree* condition = child->nextTree();
    i++;
    if (i == n || ToBoolean<T>(condition)) {
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
int Approximation::IndexOfActivePiecewiseBranchAt(const Tree* piecewise, T x) {
  s_context->setLocalValue(x);
  const Tree* branch = SelectPiecewiseBranch<T>(piecewise);
  if (branch == KUndef) {
    return -1;
  }
  return piecewise->indexOfChild(branch);
}

template <typename T, typename U>
U Approximation::MapAndReduce(const Tree* node, Reductor<U> reductor,
                              Mapper<std::complex<T>, U> mapper) {
  /* TODO: this function, the use of function pointers and the general
   * recursive design of ToComplex incurs some overhead when approximating. For
   * instance (a+b)*c, will execute nextNode() on a twice (one for + and one
   * for *). We should use a non-recursive and more C-like algorithm. */
  U res;
  for (auto [child, index] : NodeIterator::Children<NoEditable>(node)) {
    std::complex<T> app = ToComplex<T>(child);
    if (std::isnan(app.real()) || std::isnan(app.imag())) {
      return NAN;
    }
    U mapped;
    if constexpr (std::is_same_v<std::complex<T>, U>) {
      mapped = app;
    } else {
      assert(mapper);
      mapped = mapper(app);
      if (std::isnan(mapped)) {
        return NAN;
      }
    }
    if (index == 0) {
      res = mapped;
    } else {
      res = reductor(res, mapped);
    }
  }
  return res;
}

bool Approximation::CanApproximate(const Tree* tree,
                                   int firstNonApproximableVarId) {
  if (tree->isRandomNode() || tree->isUserNamed() ||
      (tree->isVar() && Variables::Id(tree) >= firstNonApproximableVarId) ||
      tree->isSet()) {
    return false;
  }
  int childIndex = 0;
  for (const Tree* child : tree->children()) {
    bool enterScope = false;
    if (tree->isParametric()) {
      if (childIndex == Parametric::k_variableIndex) {
        // Parametric's variable cant be approximated, but we never want to.
        childIndex++;
        continue;
      }
      if (childIndex == Parametric::FunctionIndex(tree)) {
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

bool IsNonListScalar(const Tree* tree) {
  return Dimension::GetDimension(tree).isScalar() && !Dimension::IsList(tree);
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
    Tree* tree, const ProjectionContext* ctx) {
  Context context(ctx ? ctx->m_angleUnit : AngleUnit::Radian,
                  ctx ? ctx->m_complexFormat : ComplexFormat::Cartesian);
  if (SkipApproximation(tree->type())) {
    return false;
  }
  s_context = &context;
  uint32_t hash = tree->hash();
  bool result = false;
  if (CanApproximate(tree) && IsNonListScalar(tree)) {
    tree->moveTreeOverTree(ToTree<double>(tree, Dimension()));
    result = true;
  } else {
    result = PrivateApproximateAndReplaceEveryScalar(tree);
  }
  s_context = nullptr;
  /* TODO: We compare the CRC32 to prevent expressions such as 1.0+i*1.0 from
   * returning true at every call. We should detect and skip them in
   * SkipApproximation instead. */
  return result && hash != tree->hash();
}

bool Approximation::PrivateApproximateAndReplaceEveryScalar(Tree* tree) {
  assert(!CanApproximate(tree) || !IsNonListScalar(tree));
  bool changed = false;
  int childIndex = 0;
  for (Tree* child : tree->children()) {
    if (SkipApproximation(child->type(), tree->type(), childIndex++)) {
      continue;
    }
    if (CanApproximate(child) && IsNonListScalar(child)) {
      child->moveTreeOverTree(ToTree<double>(child, Dimension()));
      changed = true;
    } else {
      changed = PrivateApproximateAndReplaceEveryScalar(child) || changed;
    }
  }
  // TODO: Merge additions and multiplication's children if possible.
  return changed;
}

/* TODO: not all this functions are worth templating on float and
 * double. ToComplex needs it but ToMatrix could take a bool and call the
 * correct ToComplex<T> as needed since the code is mostly independant of the
 * float type used in the tree. */

template PointOrScalar<float> Approximation::RootPreparedToPointOrScalar(
    const Tree*, float);
template PointOrScalar<double> Approximation::RootPreparedToPointOrScalar(
    const Tree*, double);
template PointOrScalar<float> Approximation::RootToPointOrScalarPrivate(
    const Tree*, bool, bool, float, AngleUnit, ComplexFormat);
template PointOrScalar<double> Approximation::RootToPointOrScalarPrivate(
    const Tree*, bool, bool, double, AngleUnit, ComplexFormat);

template std::complex<float> Approximation::ToComplex<float>(const Tree*);
template std::complex<double> Approximation::ToComplex<double>(const Tree*);

template Tree* Approximation::ToPoint<float>(const Tree*);
template Tree* Approximation::ToPoint<double>(const Tree*);

template Tree* Approximation::RootTreeToTree<float>(const Tree*, AngleUnit,
                                                    ComplexFormat);
template Tree* Approximation::RootTreeToTree<double>(const Tree*, AngleUnit,
                                                     ComplexFormat);

template int Approximation::IndexOfActivePiecewiseBranchAt<float>(
    const Tree* piecewise, float x);
template int Approximation::IndexOfActivePiecewiseBranchAt<double>(
    const Tree* piecewise, double x);

}  // namespace Poincare::Internal
