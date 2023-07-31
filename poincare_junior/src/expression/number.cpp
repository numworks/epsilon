#include "number.h"

#include "float.h"
#include "rational.h"

namespace PoincareJ {

EditionReference Number::Addition(const Tree* i, const Tree* j) {
  // TODO: handle Float
  assert(i->type() != BlockType::Float && j->type() != BlockType::Float);
  return Rational::Addition(i, j);
}
EditionReference Number::Multiplication(const Tree* i, const Tree* j) {
  // TODO: handle Float
  assert(i->type() != BlockType::Float && j->type() != BlockType::Float);
  return Rational::Multiplication(i, j);
}

NonStrictSign Number::Sign(const Tree* node) {
  switch (node->type()) {
    case BlockType::Constant:
      return NonStrictSign::Positive;
    case BlockType::Float:
      return Float::To(node) >= 0 ? NonStrictSign::Positive
                                  : NonStrictSign::Negative;
    default:
      assert(node->block()->isRational());
      return Rational::Sign(node);
  }
}

StrictSign Number::StrictSign(const Tree* node) {
  switch (node->type()) {
    case BlockType::Constant:
      return StrictSign::Positive;
    case BlockType::Float: {
      float value = Float::To(node);
      return value == 0
                 ? StrictSign::Null
                 : (value > 0 ? StrictSign::Positive : StrictSign::Negative);
    }
    default:
      assert(node->block()->isRational());
      return Rational::StrictSign(node);
  }
}

bool Number::IsSanitized(const Tree* n) {
  if (!n->block()->isOfType(
          {BlockType::RationalShort, BlockType::RationalPosBig,
           BlockType::RationalNegBig, BlockType::IntegerShort,
           BlockType::IntegerPosBig, BlockType::IntegerNegBig})) {
    assert(
        !n->block()->isNumber() ||
        n->block()->isOfType({BlockType::Float, BlockType::Constant,
                              BlockType::Half, BlockType::Zero, BlockType::One,
                              BlockType::Two, BlockType::MinusOne}));
    // Non numbers or optimal BlockType numbers
    return true;
  }
  // Re-push the optimized tree on the EditionPool, and compare with original.
  Tree* temp =
      (n->block()->isRational())
          ? Rational::Push(Rational::Numerator(n), Rational::Denominator(n))
          : Integer::Handler(n).pushOnEditionPool();
  bool result = n->treeIsIdenticalTo(temp);
  temp->removeTree();
  return result;
}

}  // namespace PoincareJ
