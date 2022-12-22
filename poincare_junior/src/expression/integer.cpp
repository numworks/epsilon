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
  if (integer.isInt8()) {
    return EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(integer));
  }
  EditionPool * pool = EditionPool::sharedEditionPool();
  TypeBlock typeBlock = integer.sign() == StrictSign::Negative ? IntegerNegBigBlock : IntegerPosBigBlock;
  EditionReference reference = EditionReference(Node(pool->pushBlock(typeBlock)));
  pool->pushBlock(integer.numberOfDigits());
  integer.pushDigitsOnEditionPool();
  pool->pushBlock(integer.numberOfDigits());
  pool->pushBlock(typeBlock);
  return reference;
}

}

template float Poincare::IntegerHandler::to<float>();
template double Poincare::IntegerHandler::to<double>();
