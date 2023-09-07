#include "number.h"

#include "float.h"
#include "rational.h"

namespace PoincareJ {

Tree* Number::Addition(const Tree* i, const Tree* j) {
  // TODO: handle Float
  assert(i->type() != BlockType::Float && j->type() != BlockType::Float);
  return Rational::Addition(i, j);
}
Tree* Number::Multiplication(const Tree* i, const Tree* j) {
  // TODO: handle Float
  assert(i->type() != BlockType::Float && j->type() != BlockType::Float);
  return Rational::Multiplication(i, j);
}

Sign::Sign Number::Sign(const Tree* node) {
  switch (node->type()) {
    case BlockType::Constant:
      return Sign::Positive;
    case BlockType::Float: {
      float value = Float::To(node);
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
           n->type().isOfType({BlockType::Float, BlockType::Constant,
                               BlockType::Half, BlockType::Zero, BlockType::One,
                               BlockType::Two, BlockType::MinusOne}));
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
