#include "integer.h"
#include "rational.h"
#include <poincare_junior/src/memory/value_block.h>

namespace Poincare {

void IntegerHandler::pushDigitsOnEditionPool() {
  EditionPool * pool = EditionPool::sharedEditionPool();
  for (size_t i = 0; i < m_numberOfDigits; i++) {
    pool->pushBlock(ValueBlock(digit(i)));
  }
}

template <typename T>
T IntegerHandler::to() {
  T approximation = 0.0f;
  for (uint8_t i = 0; i < numberOfDigits(); i++) {
    approximation += static_cast<T>(digit(i));
  }
  return static_cast<int8_t>(m_sign) * approximation;
}

EditionReference Integer::PushNode(IntegerHandler integer) {
  if (integer.isZero()) {
    return EditionReference::Push<BlockType::Zero>();
  }
  if (integer.isOne()) {
    return EditionReference::Push<BlockType::One>();
  }
  if (integer.isTwo()) {
    return EditionReference::Push<BlockType::Two>();
  }
  if (integer.isMinusOne()) {
    return EditionReference::Push<BlockType::MinusOne>();
  }
  if (integer.isSignedType<int8_t>()) {
    return EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(integer));
  }
  EditionPool * pool = EditionPool::sharedEditionPool();
  TypeBlock typeBlock(integer.sign() == StrictSign::Negative ? BlockType::IntegerNegBig : BlockType::IntegerPosBig);
  EditionReference reference = EditionReference(Node(pool->pushBlock(typeBlock)));
  pool->pushBlock(integer.numberOfDigits());
  integer.pushDigitsOnEditionPool();
  pool->pushBlock(integer.numberOfDigits());
  pool->pushBlock(typeBlock);
  return reference;
}

// TODO: tests

bool Integer::IsUint8(const Node expression) {
  return expression.block()->isInteger() && Rational::Numerator(expression).isUnsignedType<uint8_t>();
}

uint8_t Integer::Uint8(const Node expression) {
  assert(IsUint8(expression));
  return static_cast<uint8_t>(Rational::Numerator(expression));
}

}

template float Poincare::IntegerHandler::to<float>();
template double Poincare::IntegerHandler::to<double>();
