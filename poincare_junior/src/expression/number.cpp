#include "number.h"

#include "approximation.h"
#include "float.h"
#include "rational.h"

namespace PoincareJ {

Tree* Number::Addition(const Tree* i, const Tree* j) {
  if (i->isDoubleFloat() || j->isDoubleFloat()) {
    return SharedEditionPool->push<BlockType::DoubleFloat>(
        Approximation::To<double>(i, nullptr) +
        Approximation::To<double>(j, nullptr));
  }
  if (i->isSingleFloat() || j->isSingleFloat()) {
    return SharedEditionPool->push<BlockType::SingleFloat>(
        Approximation::To<float>(i, nullptr) +
        Approximation::To<float>(j, nullptr));
  }
  assert(!i->isConstant() && !j->isConstant());
  Tree* result = Rational::Addition(i, j);
  return result;
}
Tree* Number::Multiplication(const Tree* i, const Tree* j) {
  if (i->isDoubleFloat() || j->isDoubleFloat()) {
    return SharedEditionPool->push<BlockType::DoubleFloat>(
        Approximation::To<double>(i, nullptr) *
        Approximation::To<double>(j, nullptr));
  }
  if (i->isSingleFloat() || j->isSingleFloat()) {
    return SharedEditionPool->push<BlockType::SingleFloat>(
        Approximation::To<float>(i, nullptr) *
        Approximation::To<float>(j, nullptr));
  }
  assert(!i->isConstant() && !j->isConstant());
  Tree* result = Rational::Multiplication(i, j);
  return result;
}

Sign Number::Sign(const Tree* node) {
  switch (node->type()) {
    case BlockType::Constant:
      return Sign::Positive();
    case BlockType::DoubleFloat:
    case BlockType::SingleFloat: {
      double value = Float::To(node);
      // Floats are not considered integer since they may have been rounded
      return PoincareJ::Sign(value == 0, value > 0, value < 0, false);
    }
    default:
      assert(node->isRational());
      return Rational::Sign(node);
  }
}

}  // namespace PoincareJ
