#include "approximation.h"

#include <math.h>
#include <poincare/approximation_helper.h>
#include <poincare_junior/src/memory/node_iterator.h>

#include <bit>
#include <complex>

#include "constant.h"
#include "decimal.h"
#include "float.h"
#include "list.h"
#include "random.h"
#include "rational.h"

namespace PoincareJ {

// TODO: tests

AngleUnit Approximation::angleUnit;

template <typename T>
T Approximation::RootTreeTo(const Tree* node, AngleUnit angleUnit) {
  Random::Context context;
  Approximation::angleUnit = angleUnit;
  return To<T>(node, &context);
}

template <typename T>
T Approximation::To(const Tree* node, Random::Context* context) {
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
      return Constant::To<T>(Constant::Type(node));
    case BlockType::SingleFloat:
      return Float::FloatTo(node);
    case BlockType::DoubleFloat:
      return Float::DoubleTo(node);
    case BlockType::Addition:
      return MapAndReduce(node, FloatAddition<T>, context);
    case BlockType::Multiplication:
      return MapAndReduce(node, FloatMultiplication<T>, context);
    case BlockType::Division:
      return MapAndReduce(node, FloatDivision<T>, context);
    case BlockType::Subtraction:
      return MapAndReduce(node, FloatSubtraction<T>, context);
    case BlockType::PowerReal:
      return MapAndReduce(node, FloatPowerReal<T>, context);
    case BlockType::Power:
      return MapAndReduce(node, FloatPower<T>, context);
    case BlockType::Logarithm:
      return MapAndReduce(node, FloatLog<T>, context);
    case BlockType::Trig:
      return MapAndReduce(node, FloatTrig<T>, context);
    case BlockType::ATrig:
      return MapAndReduce(node, FloatATrig<T>, context);
    case BlockType::ArcCosine:
      return ConvertFromRadian(std::acos(To<T>(node->nextNode(), context)));
    case BlockType::ArcSine:
      return ConvertToRadian(std::asin(To<T>(node->nextNode(), context)));
    case BlockType::ArcTangent:
      return ConvertFromRadian(std::atan(To<T>(node->nextNode(), context)));
    case BlockType::SquareRoot:
      return std::sqrt(To<T>(node->nextNode(), context));
    case BlockType::Exponential:
      return std::exp(To<T>(node->nextNode(), context));
    case BlockType::Log:
      return std::log10(To<T>(node->nextNode(), context));
    case BlockType::LnReal:
      return FloatLnReal<T>(To<T>(node->nextNode(), context));
    case BlockType::Ln:
      return FloatLn<T>(To<T>(node->nextNode(), context));
    case BlockType::Abs:
      return std::fabs(To<T>(node->nextNode(), context));
    // TODO: Handle AngleUnits in context as well.
    case BlockType::Cosine:
      return std::cos(ConvertToRadian(To<T>(node->nextNode(), context)));
      // return ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(
      // res, angleInput);

    case BlockType::Sine:
      return std::sin(ConvertToRadian(To<T>(node->nextNode(), context)));
      // return ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(
      // res, angleInput);

    case BlockType::Tangent: {
      T angle = ConvertToRadian(To<T>(node->nextNode(), context));
      /* tan should be undefined at (2n+1)*pi/2 for any integer n.
       * std::tan is not reliable at these values because it is diverging and
       * any approximation errors on pi could easily yield a finite result. At
       * these values, cos yields 0, but is also greatly affected by
       * approximation error and could yield a non-null value : cos(pi/2+e) ~=
       * -e On the other hand, sin, which should yield either 1 or -1 around
       * these values is much more resilient : sin(pi/2+e) ~= 1 - (e^2)/2. We
       * therefore use sin to identify values at which tan should be undefined.
       */
      T sin = std::sin(angle);
      if (sin == 1 || sin == -1) {
        return NAN;
      }
      return std::tan(angle);
      // return ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(
      // res, angleInput);
    }
    case BlockType::Cosecant: {
      T c = ConvertToRadian(To<T>(node->nextNode(), context));
      // std::complex<T> denominator =
      // SineNode::computeOnComplex<T>(c, complexFormat, angleUnit);
      T denominator = std::sin(c);
      if (denominator == static_cast<T>(0.0)) {
        return NAN;  // complexNAN<T>();
      }
      return /*std::complex<T>(1)*/ 1 / denominator;
    }
    case BlockType::Cotangent: {
      T c = ConvertToRadian(To<T>(node->nextNode(), context));
      // std::complex<T> denominator =
      // SineNode::computeOnComplex<T>(c, complexFormat, angleUnit);
      // std::complex<T> numerator =
      // CosineNode::computeOnComplex<T>(c, complexFormat, angleUnit);
      T denominator = std::sin(c);
      T numerator = std::cos(c);
      if (denominator == static_cast<T>(0.0)) {
        return NAN;  // complexNAN<T>();
      }
      return numerator / denominator;
    }
    case BlockType::Secant: {
      T c = ConvertToRadian(To<T>(node->nextNode(), context));
      // std::complex<T> denominator =
      // CosineNode::computeOnComplex<T>(c, complexFormat, angleUnit);
      T denominator = std::cos(c);
      if (denominator == static_cast<T>(0.0)) {
        return NAN;  // complexNAN<T>();
      }
      return /*std::complex<T>(1)*/ 1 / denominator;
    }
    case BlockType::Decimal:
      return To<T>(node->nextNode(), context) *
             std::pow(10.0, -static_cast<T>(Decimal::DecimalOffset(node)));
    case BlockType::Infinity:
      return INFINITY;
    case BlockType::Opposite:
      return -To<T>(node->nextNode(), context);
    case BlockType::Sign: {
      T c = To<T>(node->nextNode(), context);
      // TODO why no epsilon in Poincare ?
      return c == 0 ? 0 : c < 0 ? -1 : 1;
    }
    case BlockType::Floor:
      // TODO low deviation
      return std::floor(To<T>(node->nextNode(), context));
    case BlockType::Ceiling:
      // TODO low deviation
      return std::ceil(To<T>(node->nextNode(), context));
    case BlockType::FracPart: {
      T child = To<T>(node->nextNode(), context);
      return child - std::floor(child);
    }
    case BlockType::Round: {
      // TODO digits is integer
      T err = std::pow(10, std::round(To<T>(node->child(1), context)));
      return std::round(To<T>(node->nextNode(), context) * err) / err;
    }
    case BlockType::Factorial: {
      T n = To<T>(node->nextNode(), context);
      if (/*c.imag() != 0 ||*/ std::isnan(n) || n != (int)n || n < 0) {
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
      T n = To<T>(node->child(0), context);
      T k = To<T>(node->child(1), context);
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
      // If not generalized, the output must be round
      return generalized ? result : std::round(result);
    }
    case BlockType::Permute: {
      T n = To<T>(node->child(0), context);
      T k = To<T>(node->child(1), context);
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
  return angle * (angleUnit == AngleUnit::Degree ? M_PI / 180.0 : M_PI / 200.0);
}

template <typename T>
T Approximation::ConvertFromRadian(T angle) {
  if (angleUnit == AngleUnit::Radian) {
    return angle;
  }
  return angle * (angleUnit == AngleUnit::Degree ? 180.0 / M_PI : 200.0 / M_PI);
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

template <typename T>
T Approximation::MapAndReduce(const Tree* node, Reductor<T> reductor,
                              Random::Context* context) {
  T res;
  for (auto [child, index] : NodeIterator::Children<NoEditable>(node)) {
    T app = To<T>(child, context);
    if (index == 0) {
      res = app;
    } else {
      res = reductor(res, app);
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

template float Approximation::RootTreeTo<float>(const Tree*, AngleUnit);
template double Approximation::RootTreeTo<double>(const Tree*, AngleUnit);

template float Approximation::To<float>(const Tree*, Random::Context*);
template double Approximation::To<double>(const Tree*, Random::Context*);

template Tree* Approximation::ToList<float>(const Tree*, AngleUnit);
template Tree* Approximation::ToList<double>(const Tree*, AngleUnit);

template bool Approximation::ApproximateAndReplaceEveryScalarT<float>(Tree*,
                                                                      bool);
template bool Approximation::ApproximateAndReplaceEveryScalarT<double>(Tree*,
                                                                       bool);

}  // namespace PoincareJ
