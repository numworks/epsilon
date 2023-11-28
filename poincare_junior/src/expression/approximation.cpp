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
T Approximation::To(const Tree* node) {
  assert(node->isExpression());
  if (node->isRational()) {
    return Rational::Numerator(node).to<T>() /
           Rational::Denominator(node).to<T>();
  }
  if (node->isRandomNode()) {
    // Seeded random node should have been handled at the root level.
    /* TODO : Random::ApproximateTreeNodes<T> should have been called beforehand
     * somehow. An other solution could be passing an EditionReference to the
     * approximated seeded random nodes along approximation. */
    assert(Random::GetSeed(node) == 0);
    Random::ApproximateIgnoringSeed<T>(node);
  }
  switch (node->type()) {
    case BlockType::Constant:
      return Constant::To<T>(Constant::Type(node));
    case BlockType::SingleFloat:
      return Float::FloatTo(node);
    case BlockType::DoubleFloat:
      return Float::DoubleTo(node);
    case BlockType::Addition:
      return MapAndReduce(node, FloatAddition<T>);
    case BlockType::Multiplication:
      return MapAndReduce(node, FloatMultiplication<T>);
    case BlockType::Division:
      return MapAndReduce(node, FloatDivision<T>);
    case BlockType::Subtraction:
      return MapAndReduce(node, FloatSubtraction<T>);
    case BlockType::PowerReal:
      return MapAndReduce(node, FloatPowerReal<T>);
    case BlockType::Power:
      return MapAndReduce(node, FloatPower<T>);
    case BlockType::Logarithm:
      return MapAndReduce(node, FloatLog<T>);
    case BlockType::Trig:
      return MapAndReduce(node, FloatTrig<T>);
    case BlockType::ATrig:
      return MapAndReduce(node, FloatATrig<T>);
    case BlockType::ArcCosine:
      return std::acos(To<T>(node->nextNode()));
    case BlockType::ArcSine:
      return std::asin(To<T>(node->nextNode()));
    case BlockType::ArcTangent:
      return std::atan(To<T>(node->nextNode()));
    case BlockType::SquareRoot:
      return std::sqrt(To<T>(node->nextNode()));
    case BlockType::Exponential:
      return std::exp(To<T>(node->nextNode()));
    case BlockType::Log:
      return std::log10(To<T>(node->nextNode()));
    case BlockType::Ln:
      return std::log(To<T>(node->nextNode()));
    case BlockType::Abs:
      return std::fabs(To<T>(node->nextNode()));
    case BlockType::Cosine:
      return std::cos(To<T>(node->nextNode()));
    case BlockType::Sine:
      return std::sin(To<T>(node->nextNode()));
    case BlockType::Tangent:
      return std::tan(To<T>(node->nextNode()));
    case BlockType::Decimal:
      return To<T>(node->nextNode()) *
             std::pow(10.0, -static_cast<T>(Decimal::DecimalOffset(node)));
    case BlockType::Infinity:
      return INFINITY;
    case BlockType::Opposite:
      return -To<T>(node->nextNode());
    case BlockType::Sign: {
      T c = To<T>(node->nextNode());
      // TODO why no epsilon in Poincare ?
      return c == 0 ? 0 : c < 0 ? -1 : 1;
    }
    case BlockType::Floor:
      // TODO low deviation
      return std::floor(To<T>(node->nextNode()));
    case BlockType::Ceiling:
      // TODO low deviation
      return std::ceil(To<T>(node->nextNode()));
    case BlockType::FracPart: {
      T child = To<T>(node->nextNode());
      return child - std::floor(child);
    }
    case BlockType::Round: {
      // TODO digits is integer
      T err = std::pow(10, std::round(To<T>(node->child(1))));
      return std::round(To<T>(node->nextNode()) * err) / err;
    }
    default:
      // TODO: Implement more BlockTypes
      return NAN;
  };
}

template <typename T>
T Approximation::MapAndReduce(const Tree* node, Reductor<T> reductor) {
  T res;
  for (auto [child, index] : NodeIterator::Children<NoEditable>(node)) {
    T app = To<T>(child);
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
      SharedEditionPool->push<FloatType<T>::type>(To<T>(tree)));
  return true;
}

template float Approximation::To<float>(const PoincareJ::Tree*);
template double Approximation::To<double>(const PoincareJ::Tree*);

template bool Approximation::ApproximateAndReplaceEveryScalarT<float>(
    Tree* tree);
template bool Approximation::ApproximateAndReplaceEveryScalarT<double>(
    Tree* tree);

}  // namespace PoincareJ
