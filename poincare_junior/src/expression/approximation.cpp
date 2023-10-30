#include "approximation.h"

#include <math.h>
#include <poincare_junior/src/memory/node_iterator.h>

#include <bit>

#include "constant.h"
#include "decimal.h"
#include "float.h"
#include "rational.h"

namespace PoincareJ {

// TODO: tests

template <typename T>
T Approximation::To(const Tree* node) {
  assert(node->type().isExpression());
  if (node->type().isRational()) {
    return Rational::Numerator(node).to<T>() /
           Rational::Denominator(node).to<T>();
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
    case BlockType::Trig:
      // Do not approximate second term of Trig in case it isn't replaced.
      return (childIndex == 1);
    case BlockType::PowerMatrix:
    case BlockType::Power:
      // Note: After projection, Power's second term should always be integer.
      return (childIndex == 1 && childType.isInteger());
    case BlockType::Identity:
      return true;
  }
  return false;
}

template <typename T>
bool Approximation::ApproximateAndReplaceEveryScalarT(Tree* tree) {
  // These types are either already approximated or impossible to approximate.
  if (tree->type() == FloatType<T>::type ||
      tree->type().isOfType(
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
