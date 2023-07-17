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
  if (node->type() != BlockType::Float) {
    assert(node->block()->isRational());
    return Rational::Sign(node);
  }
  return Float::To(node) >= 0 ? NonStrictSign::Positive
                              : NonStrictSign::Negative;
}

StrictSign Number::StrictSign(const Tree* node) {
  if (node->type() != BlockType::Float) {
    assert(node->block()->isRational());
    return Rational::RationalStrictSign(node);
  }
  float value = Float::To(node);
  return value == 0 ? StrictSign::Null
                    : (value > 0 ? StrictSign::Positive : StrictSign::Negative);
}

}  // namespace PoincareJ
