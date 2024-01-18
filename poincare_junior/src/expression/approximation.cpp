#include "approximation.h"

#include <math.h>
#include <poincare/approximation_helper.h>
#include <poincare/float.h>
#include <poincare_junior/src/memory/node_iterator.h>

#include <bit>
#include <complex>

#include "constant.h"
#include "decimal.h"
#include "float.h"
#include "list.h"
#include "random.h"
#include "rational.h"

using Poincare::ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable;

namespace PoincareJ {

// TODO: tests

AngleUnit Approximation::angleUnit;

template <typename T>
std::complex<T> Approximation::ComplexRootTreeTo(const Tree* node,
                                                 AngleUnit angleUnit) {
  Random::Context context;
  Approximation::angleUnit = angleUnit;
  return ComplexTo<T>(node, &context);
}

template <typename T>
std::complex<T> Approximation::ComplexTo(const Tree* node,
                                         Random::Context* context) {
  assert(node->isExpression());
  if (node->isRational()) {
    return Rational::Numerator(node).to<T>() /
           Rational::Denominator(node).to<T>();
  }
  if (node->isRandomNode()) {
    return Random::Approximate<T>(node, context);
  }
  switch (node->type()) {
    case BlockType::Constant:
      if (Constant::Type(node) == Constant::Type::I) {
        return std::complex<T>(0, 1);
      }
      return Constant::To<T>(Constant::Type(node));
    case BlockType::SingleFloat:
      return Float::FloatTo(node);
    case BlockType::DoubleFloat:
      return Float::DoubleTo(node);
    case BlockType::Addition:
      return MapAndReduce<T, std::complex<T>>(
          node, FloatAddition<std::complex<T>>, context);
    case BlockType::Multiplication:
      return MapAndReduce<T, std::complex<T>>(
          node, FloatMultiplication<std::complex<T>>, context);
    case BlockType::Division:
      return MapAndReduce<T, std::complex<T>>(
          node, FloatDivision<std::complex<T>>, context);
    case BlockType::Subtraction:
      return MapAndReduce<T, std::complex<T>>(
          node, FloatSubtraction<std::complex<T>>, context);
    case BlockType::PowerReal:
      return MapAndReduce<T, std::complex<T>>(node, FloatPowerReal<T>, context);
    case BlockType::Power:
      return MapAndReduce<T, std::complex<T>>(node, FloatPower<std::complex<T>>,
                                              context);
    case BlockType::Logarithm:
      return MapAndReduce<T, std::complex<T>>(node, FloatLog<std::complex<T>>,
                                              context);
    case BlockType::Trig:
      return MapAndReduce<T, std::complex<T>>(node, FloatTrig<std::complex<T>>,
                                              context);
    case BlockType::ATrig:
      return MapAndReduce<T, std::complex<T>>(node, FloatATrig<std::complex<T>>,
                                              context);
    case BlockType::GCD:
      return MapAndReduce<T, T>(node, FloatGCD<T>, context,
                                PositiveIntegerApproximation<T>);
    case BlockType::LCM:
      return MapAndReduce<T, T>(node, FloatLCM<T>, context,
                                PositiveIntegerApproximation<T>);
    case BlockType::ArcCosine:
      return ConvertFromRadian(
          std::acos(ComplexTo<T>(node->nextNode(), context)));
    case BlockType::ArcSine:
      return ConvertToRadian(
          std::asin(ComplexTo<T>(node->nextNode(), context)));
    case BlockType::ArcTangent:
      return ConvertFromRadian(
          std::atan(ComplexTo<T>(node->nextNode(), context)));
    case BlockType::SquareRoot:
      return std::sqrt(ComplexTo<T>(node->nextNode(), context));
    case BlockType::Exponential:
      return std::exp(ComplexTo<T>(node->nextNode(), context));
    case BlockType::Log:
      return std::log10(ComplexTo<T>(node->nextNode(), context));
    case BlockType::LnReal:
      return FloatLnReal<T>(To<T>(node->nextNode(), context));
    case BlockType::Ln:
      return FloatLn<std::complex<T>>(ComplexTo<T>(node->nextNode(), context));
    case BlockType::Abs:
      return std::abs(ComplexTo<T>(node->nextNode(), context));
    // TODO: Handle AngleUnits in context as well.
    case BlockType::Cosine:
    case BlockType::Sine: {
      std::complex<T> angleInput =
          ConvertToRadian(ComplexTo<T>(node->nextNode(), context));
      std::complex<T> res =
          node->isCosine() ? std::cos(angleInput) : std::sin(angleInput);
      return NeglectRealOrImaginaryPartIfNeglectable(res, angleInput);
    }
    case BlockType::Tangent: {
      std::complex<T> angleInput =
          ConvertToRadian(ComplexTo<T>(node->nextNode(), context));
      /* tan should be undefined at (2n+1)*pi/2 for any integer n.
       * std::tan is not reliable at these values because it is diverging and
       * any approximation errors on pi could easily yield a finite result. At
       * these values, cos yields 0, but is also greatly affected by
       * approximation error and could yield a non-null value : cos(pi/2+e) ~=
       * -e On the other hand, sin, which should yield either 1 or -1 around
       * these values is much more resilient : sin(pi/2+e) ~= 1 - (e^2)/2. We
       * therefore use sin to identify values at which tan should be undefined.
       */
      std::complex<T> sin = std::sin(angleInput);
      if (sin == std::complex<T>(1) || sin == std::complex<T>(-1)) {
        return NAN;
      }
      std::complex<T> res = std::tan(angleInput);
      return NeglectRealOrImaginaryPartIfNeglectable(res, angleInput);
    }
    case BlockType::Cosecant: {
      std::complex<T> c =
          ConvertToRadian(ComplexTo<T>(node->nextNode(), context));
      // std::complex<T> denominator =
      // SineNode::computeOnComplex<T>(c, complexFormat, angleUnit);
      std::complex<T> denominator = std::sin(c);
      if (denominator == static_cast<T>(0.0)) {
        return NAN;  // complexNAN<T>();
      }
      return std::complex<T>(1) / denominator;
    }
    case BlockType::Cotangent: {
      std::complex<T> c =
          ConvertToRadian(ComplexTo<T>(node->nextNode(), context));
      // std::complex<T> denominator =
      // SineNode::computeOnComplex<T>(c, complexFormat, angleUnit);
      // std::complex<T> numerator =
      // CosineNode::computeOnComplex<T>(c, complexFormat, angleUnit);
      std::complex<T> denominator = std::sin(c);
      std::complex<T> numerator = std::cos(c);
      if (denominator == static_cast<T>(0.0)) {
        return NAN;  // complexNAN<T>();
      }
      return numerator / denominator;
    }
    case BlockType::Secant: {
      std::complex<T> c =
          ConvertToRadian(ComplexTo<T>(node->nextNode(), context));
      // std::complex<T> denominator =
      // CosineNode::computeOnComplex<T>(c, complexFormat, angleUnit);
      std::complex<T> denominator = std::cos(c);
      if (denominator == static_cast<T>(0.0)) {
        return NAN;  // complexNAN<T>();
      }
      return std::complex<T>(1) / denominator;
    }
    case BlockType::Infinity:
      return INFINITY;
    case BlockType::Opposite:
      return -ComplexTo<T>(node->nextNode(), context);
    case BlockType::RealPart:
      return ComplexTo<T>(node->nextNode(), context).real();
    case BlockType::ImaginaryPart:
      return ComplexTo<T>(node->nextNode(), context).imag();
  }

  // The remaining operators are defined only on reals
  // assert(node->numberOfChildren() <= 2);
  if (node->numberOfChildren() > 2) {
    return NAN;
  }
  T child[2];
  for (int i = 0; const Tree* childNode : node->children()) {
    std::complex<T> app = ComplexTo<T>(childNode, context);
    if (app.imag() != 0) {
      return NAN;
    }
    child[i++] = app.real();
  }
  switch (node->type()) {
    case BlockType::Decimal:
      return child[0] *
             std::pow(10.0, -static_cast<T>(Decimal::DecimalOffset(node)));
    case BlockType::Sign: {
      // TODO why no epsilon in Poincare ?
      if (std::isnan(child[0])) {
        return NAN;
      }
      return child[0] == 0 ? 0 : child[0] < 0 ? -1 : 1;
    }
    case BlockType::Floor:
    case BlockType::Ceiling: {
      /* Assume low deviation from natural numbers are errors */
      T delta = std::fabs((std::round(child[0]) - child[0]) / child[0]);
      if (delta <= Poincare::Float<T>::Epsilon()) {
        return std::round(child[0]);
      }
      return node->isFloor() ? std::floor(child[0]) : std::ceil(child[0]);
    }
    case BlockType::FracPart: {
      return child[0] - std::floor(child[0]);
    }
    case BlockType::Round: {
      if (std::isnan(child[1]) || child[1] != std::round(child[1])) {
        return NAN;
      }
      T err = std::pow(10, std::round(child[1]));
      return std::round(child[0] * err) / err;
    }
    case BlockType::Factorial: {
      T n = child[0];
      if (std::isnan(n) || n != std::round(n) || n < 0) {
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
    case BlockType::Binomial: {
      T n = child[0];
      T k = child[1];
      if (std::isnan(n) || std::isnan(k) || k != std::round(k)) {
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
    case BlockType::Permute: {
      T n = child[0];
      T k = child[1];
      if (std::isnan(n) || std::isnan(k) || n != std::round(n) ||
          k != std::round(k) || n < 0.0f || k < 0.0f) {
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

    default:
      if (node->isParametric()) {
        // TODO: Explicit tree if it contains random nodes.
      }
      // TODO: Implement more BlockTypes
      return NAN;
  };
}

template <typename T>
T Approximation::ConvertToRadian(T angle) {
  if (angleUnit == AngleUnit::Radian) {
    return angle;
  }
  return angle * (angleUnit == AngleUnit::Degree
                      ? static_cast<T>(M_PI / 180.0)
                      : static_cast<T>(M_PI / 200.0));
}

template <typename T>
T Approximation::ConvertFromRadian(T angle) {
  if (angleUnit == AngleUnit::Radian) {
    return angle;
  }
  return angle * (angleUnit == AngleUnit::Degree
                      ? static_cast<T>(180.0 / M_PI)
                      : static_cast<T>(200.0 / M_PI));
}

template <typename T>
Tree* Approximation::ToList(const Tree* node, AngleUnit angleUnit) {
  Tree* l = node->clone();
  Approximation::angleUnit = angleUnit;
  List::BubbleUp(l, [](Tree* e) -> bool {
    return Approximation::ApproximateAndReplaceEveryScalarT<T>(e, true);
  });
  return l;
}

template <typename T, typename U>
U Approximation::MapAndReduce(const Tree* node, Reductor<U> reductor,
                              Random::Context* context,
                              Mapper<std::complex<T>, U> mapper) {
  U res;
  for (auto [child, index] : NodeIterator::Children<NoEditable>(node)) {
    std::complex<T> app = ComplexTo<T>(child, context);
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

bool interruptApproximation(TypeBlock type, int childIndex,
                            TypeBlock childType) {
  switch (type) {
    case BlockType::ATrig:
    case BlockType::Trig:
      // Do not approximate second term in case tree isn't replaced.
      return (childIndex == 1);
    case BlockType::PowerMatrix:
    case BlockType::Power:
      // Note: After projection, Power's second term should always be integer.
      return (childIndex == 1 && childType.isInteger());
    case BlockType::Identity:
      return true;
    default:
      return false;
  }
}

template <typename T>
bool Approximation::ApproximateAndReplaceEveryScalarT(Tree* tree,
                                                      bool collapse) {
  // These types are either already approximated or impossible to approximate.
  if (tree->type() == FloatType<T>::type || tree->isRandomNode() ||
      tree->isOfType(
          {BlockType::UserSymbol, BlockType::Variable, BlockType::Unit})) {
    return false;
  }
  bool changed = false;
  bool approximateNode = collapse || (tree->numberOfChildren() == 0);
  int childIndex = 0;
  for (Tree* child : tree->children()) {
    if (interruptApproximation(tree->type(), childIndex++, child->type())) {
      break;
    }
    changed = ApproximateAndReplaceEveryScalarT<T>(child, collapse) || changed;
    approximateNode = approximateNode && child->type() == FloatType<T>::type;
  }
  if (!approximateNode) {
    // TODO: Partially approximate additions and multiplication anyway
    return changed;
  }
  tree->moveTreeOverTree(
      SharedEditionPool->push<FloatType<T>::type>(To<T>(tree, nullptr)));
  return true;
}

template std::complex<float> Approximation::ComplexRootTreeTo<float>(
    const Tree*, AngleUnit);
template std::complex<double> Approximation::ComplexRootTreeTo<double>(
    const Tree*, AngleUnit);

template std::complex<float> Approximation::ComplexTo<float>(const Tree*,
                                                             Random::Context*);
template std::complex<double> Approximation::ComplexTo<double>(
    const Tree*, Random::Context*);

template Tree* Approximation::ToList<float>(const Tree*, AngleUnit);
template Tree* Approximation::ToList<double>(const Tree*, AngleUnit);

template bool Approximation::ApproximateAndReplaceEveryScalarT<float>(Tree*,
                                                                      bool);
template bool Approximation::ApproximateAndReplaceEveryScalarT<double>(Tree*,
                                                                       bool);

}  // namespace PoincareJ
