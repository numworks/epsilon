#include "integer.h"
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
  T sign = m_negative ? -1.0 : 1.0;
  if (m_numberOfDigits == 1) {
    return sign * static_cast<T>(m_digitAccessor.m_digit);
  }
  T approximation = 0.0f;
  for (uint8_t i = 0; i < m_numberOfDigits; i++) {
    approximation += m_digitAccessor.m_digits[i];
  }
  return sign * approximation;
}

EditionReference Integer::PushNode(IntegerHandler integer) {
  EditionPool * pool = EditionPool::sharedEditionPool();
  TypeBlock typeBlock = integer.sign() < 0 ? IntegerNegBigBlock : IntegerPosBigBlock;
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
