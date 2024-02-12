#include "number.h"

#include "approximation.h"
#include "float.h"
#include "rational.h"

namespace PoincareJ {

Tree* Number::Addition(const Tree* i, const Tree* j) {
  if (i->isDoubleFloat() || j->isDoubleFloat()) {
    return SharedEditionPool->push<BlockType::DoubleFloat>(
        Approximation::To<double>(i) + Approximation::To<double>(j));
  }
  if (i->isSingleFloat() || j->isSingleFloat()) {
    return SharedEditionPool->push<BlockType::SingleFloat>(
        Approximation::To<float>(i) + Approximation::To<float>(j));
  }
  assert(!i->isMathematicalConstant() && !j->isMathematicalConstant());
  Tree* result = Rational::Addition(i, j);
  return result;
}
Tree* Number::Multiplication(const Tree* i, const Tree* j) {
  if (i->isDoubleFloat() || j->isDoubleFloat()) {
    return SharedEditionPool->push<BlockType::DoubleFloat>(
        Approximation::To<double>(i) * Approximation::To<double>(j));
  }
  if (i->isSingleFloat() || j->isSingleFloat()) {
    return SharedEditionPool->push<BlockType::SingleFloat>(
        Approximation::To<float>(i) * Approximation::To<float>(j));
  }
  assert(!i->isMathematicalConstant() && !j->isMathematicalConstant());
  Tree* result = Rational::Multiplication(i, j);
  return result;
}

Sign Number::Sign(const Tree* node) {
  switch (node->type()) {
    case BlockType::Pi:
    case BlockType::ExponentialE:
      return Sign::Positive();
    case BlockType::DoubleFloat:
    case BlockType::SingleFloat: {
      double value = FloatNode::To(node);
      // Floats are not considered integer since they may have been rounded
      return PoincareJ::Sign(value == 0, value > 0, value < 0, true);
    }
    default:
      assert(node->isRational());
      return Rational::Sign(node);
  }
}

}  // namespace PoincareJ
