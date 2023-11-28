#include "approximation.h"

#include <math.h>
#include <poincare_junior/src/memory/node_iterator.h>

#include <bit>

#include "constant.h"
#include "decimal.h"
#include "float.h"
#include "random.h"
#include "rational.h"

namespace PoincareJ {

// TODO: tests

template <typename T>
T Approximation::RootTreeTo(const Tree* node) {
  Random::Context context;
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
      return std::acos(To<T>(node->nextNode(), context));
    case BlockType::ArcSine:
      return std::asin(To<T>(node->nextNode(), context));
    case BlockType::ArcTangent:
      return std::atan(To<T>(node->nextNode(), context));
    case BlockType::SquareRoot:
      return std::sqrt(To<T>(node->nextNode(), context));
    case BlockType::Exponential:
      return std::exp(To<T>(node->nextNode(), context));
    case BlockType::Log:
      return std::log10(To<T>(node->nextNode(), context));
    case BlockType::Ln:
      return std::log(To<T>(node->nextNode(), context));
    case BlockType::Abs:
      return std::fabs(To<T>(node->nextNode(), context));
    case BlockType::Cosine:
      return std::cos(To<T>(node->nextNode(), context));
    case BlockType::Sine:
      return std::sin(To<T>(node->nextNode(), context));
    case BlockType::Tangent:
      return std::tan(To<T>(node->nextNode(), context));
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
    default:
      // TODO: Implement more BlockTypes
      return NAN;
  };
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
bool Approximation::ApproximateAndReplaceEveryScalarT(Tree* tree) {
  // These types are either already approximated or impossible to approximate.
  if (tree->type() == FloatType<T>::type || tree->isRandomNode() ||
      tree->isOfType(
          {BlockType::UserSymbol, BlockType::Variable, BlockType::Unit})) {
    return false;
  }
  bool changed = false;
  bool approximateNode = true;
  int childIndex = 0;
  for (Tree* child : tree->children()) {
    if (interruptApproximation(tree->type(), childIndex++, child->type())) {
      break;
    }
    changed = ApproximateAndReplaceEveryScalarT<T>(child) || changed;
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

template float Approximation::RootTreeTo<float>(const Tree*);
template double Approximation::RootTreeTo<double>(const Tree*);

template float Approximation::To<float>(const Tree*, Random::Context*);
template double Approximation::To<double>(const Tree*, Random::Context*);

template bool Approximation::ApproximateAndReplaceEveryScalarT<float>(Tree*);
template bool Approximation::ApproximateAndReplaceEveryScalarT<double>(Tree*);

}  // namespace PoincareJ
