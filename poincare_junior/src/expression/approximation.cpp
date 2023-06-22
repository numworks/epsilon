#include "approximation.h"

#include <poincare_junior/src/memory/node_iterator.h>

#include <bit>

#include "constant.h"
#include "rational.h"

namespace PoincareJ {

// TODO: tests

template <typename T>
T Approximation::To(const Node* node) {
  assert(node->block()->isExpression());
  if (node->block()->isRational()) {
    return Rational::Numerator(node).to<T>() /
           Rational::Denominator(node).to<T>();
  }
  switch (node->type()) {
    case BlockType::Constant:
      return Constant::To<T>(Constant::Type(node));
    case BlockType::Float: {
      volatile const uint32_t value =
          *reinterpret_cast<const uint32_t*>(node->block()->next());
      return std::bit_cast<float>(value);
    }
    case BlockType::Addition:
      return Approximation::MapAndReduce(node, FloatAddition<T>);
    case BlockType::Multiplication:
      return Approximation::MapAndReduce(node, FloatMultiplication<T>);
    case BlockType::Division:
      return Approximation::MapAndReduce(node, FloatDivision<T>);
    case BlockType::Subtraction:
      return Approximation::MapAndReduce(node, FloatSubtraction<T>);
    case BlockType::Power:
      return Approximation::MapAndReduce(node, FloatPower<T>);
    case BlockType::Logarithm:
      return Approximation::MapAndReduce(node, FloatLog<T>);
    case BlockType::Trig:
      return Approximation::MapAndReduce(node, FloatTrig<T>);
    case BlockType::TrigDiff:
      return Approximation::MapAndReduce(node, FloatTrigDiff<T>);
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
    default:
      // TODO: Implement more BlockTypes
      return NAN;
  };
}

template <typename T>
T Approximation::MapAndReduce(const Node* node, Reductor<T> reductor) {
  T res;
  for (auto [child, index] :
       NodeIterator::Children<Forward, NoEditable>(node)) {
    T app = Approximation::To<T>(child);
    if (index == 0) {
      res = app;
    } else {
      res = reductor(res, app);
    }
  }
  return res;
}

EditionReference Approximation::ReplaceWithApproximation(EditionReference ref) {
  Node* root = ref;
  ApproximateAndReplaceEveryScalar(ref);
  return EditionReference(root);
}

bool Approximation::ApproximateAndReplaceEveryScalar(EditionReference ref) {
  bool hasApproximatedEveryChild = true;
  Node* node = ref.nextNode();
  int numberOfChildren = ref.numberOfChildren();
  for (int i = 0; i < numberOfChildren; i++) {
    // Approximate anyway
    hasApproximatedEveryChild =
        ApproximateAndReplaceEveryScalar(node) && hasApproximatedEveryChild;
    node = node->nextTree();
  }
  if (!hasApproximatedEveryChild) {
    // TODO: Partially approximate additions and multiplication anyway
    return false;
  }
  float approx = Approximation::To<float>(ref);
  if (std::isnan(approx)) {
    return false;
  }
  ref = ref.replaceTreeByTree(
      EditionPool::sharedEditionPool()->push<BlockType::Float>(approx));
  return true;
}

}  // namespace PoincareJ

template float PoincareJ::Approximation::To<float>(const PoincareJ::Node*);
template double PoincareJ::Approximation::To<double>(const PoincareJ::Node*);
