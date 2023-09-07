#include "approximation.h"

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
    case BlockType::Float:
      return Float::To(node);
    case BlockType::Addition:
      return Approximation::MapAndReduce(node, FloatAddition<T>);
    case BlockType::Multiplication:
      return Approximation::MapAndReduce(node, FloatMultiplication<T>);
    case BlockType::Division:
      return Approximation::MapAndReduce(node, FloatDivision<T>);
    case BlockType::Subtraction:
      return Approximation::MapAndReduce(node, FloatSubtraction<T>);
    case BlockType::PowerReal:
      return Approximation::MapAndReduce(node, FloatPowerReal<T>);
    case BlockType::Power:
      return Approximation::MapAndReduce(node, FloatPower<T>);
    case BlockType::Logarithm:
      return Approximation::MapAndReduce(node, FloatLog<T>);
    case BlockType::Trig:
      return Approximation::MapAndReduce(node, FloatTrig<T>);
    case BlockType::TrigDiff:
      return Approximation::MapAndReduce(node, FloatTrigDiff<T>);
    case BlockType::SquareRoot:
      return std::sqrt(Approximation::To<T>(node->nextNode()));
    case BlockType::Exponential:
      return std::exp(Approximation::To<T>(node->nextNode()));
    case BlockType::Log:
      return std::log10(Approximation::To<T>(node->nextNode()));
    case BlockType::Ln:
      return std::log(Approximation::To<T>(node->nextNode()));
    case BlockType::Abs:
      return std::fabs(Approximation::To<T>(node->nextNode()));
    case BlockType::Cosine:
      return std::cos(Approximation::To<T>(node->nextNode()));
    case BlockType::Sine:
      return std::sin(Approximation::To<T>(node->nextNode()));
    case BlockType::Tangent:
      return std::tan(Approximation::To<T>(node->nextNode()));
    case BlockType::Decimal:
      return Approximation::To<T>(node->nextNode()) *
             std::pow(10.0, -static_cast<T>(Decimal::DecimalOffset(node)));
    default:
      // TODO: Implement more BlockTypes
      return NAN;
  };
}

template <typename T>
T Approximation::MapAndReduce(const Tree* node, Reductor<T> reductor) {
  T res;
  for (auto [child, index] : NodeIterator::Children<NoEditable>(node)) {
    T app = Approximation::To<T>(child);
    if (index == 0) {
      res = app;
    } else {
      res = reductor(res, app);
    }
  }
  return res;
}

bool Approximation::ApproximateAndReplaceEveryScalar(Tree* tree) {
  if (tree->type() == BlockType::Float) {
    return false;
  }
  bool changed = false;
  bool approximateNode = true;
  for (Tree* child : tree->children()) {
    changed = ApproximateAndReplaceEveryScalar(child) || changed;
    approximateNode = approximateNode && child->type() == BlockType::Float;
  }
  if (!approximateNode) {
    // TODO: Partially approximate additions and multiplication anyway
    return changed;
  }
  float approx = Approximation::To<float>(tree);
  if (std::isnan(approx)) {
    return changed;
  }
  tree->moveTreeOverTree(SharedEditionPool->push<BlockType::Float>(approx));
  return true;
}

}  // namespace PoincareJ

template float PoincareJ::Approximation::To<float>(const PoincareJ::Tree*);
template double PoincareJ::Approximation::To<double>(const PoincareJ::Tree*);
