#include "number.h"

#include "approximation.h"
#include "float.h"
#include "rational.h"

namespace PoincareJ {

Tree* Number::Addition(const Tree* i, const Tree* j) {
  if (i->type() == BlockType::DoubleFloat ||
      j->type() == BlockType::DoubleFloat) {
    return SharedEditionPool->push<BlockType::DoubleFloat>(
        Approximation::To<double>(i) + Approximation::To<double>(j));
  }
  if (i->type() == BlockType::SingleFloat ||
      j->type() == BlockType::SingleFloat) {
    return SharedEditionPool->push<BlockType::SingleFloat>(
        Approximation::To<float>(i) + Approximation::To<float>(j));
  }
  assert(i->type() != BlockType::Constant && j->type() != BlockType::Constant);
  Tree* result = Rational::Addition(i, j);
  Rational::MakeIrreducible(result);
  return result;
}
Tree* Number::Multiplication(const Tree* i, const Tree* j) {
  if (i->type() == BlockType::DoubleFloat ||
      j->type() == BlockType::DoubleFloat) {
    return SharedEditionPool->push<BlockType::DoubleFloat>(
        Approximation::To<double>(i) * Approximation::To<double>(j));
  }
  if (i->type() == BlockType::SingleFloat ||
      j->type() == BlockType::SingleFloat) {
    return SharedEditionPool->push<BlockType::SingleFloat>(
        Approximation::To<float>(i) * Approximation::To<float>(j));
  }
  assert(i->type() != BlockType::Constant && j->type() != BlockType::Constant);
  Tree* result = Rational::Multiplication(i, j);
  Rational::MakeIrreducible(result);
  return result;
}

Sign::Sign Number::Sign(const Tree* node) {
  switch (node->type()) {
    case BlockType::Constant:
      return Sign::Positive;
    case BlockType::DoubleFloat:
    case BlockType::SingleFloat: {
      double value = Float::To(node);
      // Floats are not considered integer since they may have been rounded
      return {value == 0, value > 0, value < 0, false};
    }
    default:
      assert(node->type().isRational());
      return Rational::Sign(node);
  }
}

bool Number::IsSanitized(const Tree* n) {
  if (!n->type().isOfType({BlockType::RationalShort, BlockType::RationalPosBig,
                           BlockType::RationalNegBig, BlockType::IntegerShort,
                           BlockType::IntegerPosBig,
                           BlockType::IntegerNegBig})) {
    assert(!n->type().isNumber() ||
           n->type().isOfType({BlockType::SingleFloat, BlockType::DoubleFloat,
                               BlockType::Constant, BlockType::Half,
                               BlockType::Zero, BlockType::One, BlockType::Two,
                               BlockType::MinusOne}));
    // Non numbers or optimal BlockType numbers
    return true;
  }
  // Re-push the optimized tree on the EditionPool, and compare with original.
  Tree* temp = Rational::Push(Rational::Numerator(n), Rational::Denominator(n));
  Rational::MakeIrreducible(temp);
  bool result = n->treeIsIdenticalTo(temp);
  temp->removeTree();
  return result;
}

}  // namespace PoincareJ
