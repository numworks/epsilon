#include "rational.h"
#include <poincare_junior/src/memory/value_block.h>

namespace Poincare {

// TODO: tests

IntegerHandler Rational::Numerator(const Node node) {
  BlockType type = node.type();
  switch (type) {
    case BlockType::Zero:
      return IntegerHandler(0);
    case BlockType::One:
      return IntegerHandler(1);
    case BlockType::Two:
      return IntegerHandler(2);
    case BlockType::MinusOne:
      return IntegerHandler(-1);
    case BlockType::Half:
      return IntegerHandler(1);
    case BlockType::IntegerShort:
    {
      int8_t value = static_cast<int8_t>(*(node.block()->next()));
      return IntegerHandler(value);
    }
    case BlockType::IntegerPosBig:
    case BlockType::IntegerNegBig:
    {
      Block * block = node.block();
      uint8_t numberOfDigits = static_cast<uint8_t>(*(block->next()));
      const uint8_t * digits = reinterpret_cast<const uint8_t *>(block->nextNth(2));
      return IntegerHandler(digits, numberOfDigits, type == BlockType::IntegerNegBig ? NonStrictSign::Negative : NonStrictSign::Positive);
    }
    case BlockType::RationalShort:
    {
      int8_t value = static_cast<int8_t>(*(node.block()->next()));
      return IntegerHandler(value);
    }
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig:
    {
      Block * block = node.block();
      uint8_t numberOfDigits = static_cast<uint8_t>(*(block->next()));
      const uint8_t * digits = reinterpret_cast<const uint8_t *>(block->nextNth(3));
      return IntegerHandler(digits, numberOfDigits, type == BlockType::RationalNegBig ? NonStrictSign::Negative : NonStrictSign::Positive);
    }
    default:
      assert(false);
  }
}

IntegerHandler Rational::Denominator(const Node node) {
  switch (node.type()) {
    case BlockType::Zero:
    case BlockType::One:
    case BlockType::Two:
    case BlockType::MinusOne:
    case BlockType::IntegerShort:
    case BlockType::IntegerPosBig:
    case BlockType::IntegerNegBig:
      return IntegerHandler(1);
    case BlockType::Half:
      return IntegerHandler(2);
    case BlockType::RationalShort:
    {
      uint8_t value = static_cast<uint8_t>(*(node.block()->next()));
      return IntegerHandler(value);
    }
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig:
    {
      Block * block = node.block();
      uint8_t numeratorNumberOfDigits = static_cast<uint8_t>(*(block->next()));
      uint8_t denominatorNumberOfDigits = static_cast<uint8_t>(*(block->nextNth(2)));
      const uint8_t * digits = reinterpret_cast<const uint8_t *>(block->nextNth(3 + numeratorNumberOfDigits));
      return IntegerHandler(digits, denominatorNumberOfDigits, NonStrictSign::Positive);
    }
    default:
      assert(false);
  }
}

EditionReference Rational::PushNode(IntegerHandler numerator, IntegerHandler denominator) {
  assert(!denominator.isZero());
  if (denominator.isOne()) {
    return Integer::PushNode(numerator);
  }
  if (numerator.isOne() && denominator.isTwo()) {
    return EditionReference::Push<BlockType::Half>();
  }
  if (numerator.isSignedType<int8_t>() && denominator.isUnsignedType<uint8_t>()) {
    return EditionReference::Push<BlockType::RationalShort>(static_cast<int8_t>(numerator), static_cast<uint8_t>(denominator));
  }
  EditionPool * pool = EditionPool::sharedEditionPool();
  TypeBlock typeBlock(numerator.sign() == StrictSign::Negative ? BlockType::RationalNegBig : BlockType::RationalPosBig);
  EditionReference reference = EditionReference(Node(pool->pushBlock(typeBlock)));
  uint8_t numberOfDigitsOfNumerator = numerator.numberOfDigits();
  uint8_t numberOfDigitsOfDenominator = numerator.numberOfDigits();
  if (numberOfDigitsOfNumerator > UINT8_MAX - numberOfDigitsOfDenominator) {
    // TODO: RAISE EXCEPTION rational overflows
  }
  pool->pushBlock(ValueBlock(numberOfDigitsOfNumerator));
  pool->pushBlock(ValueBlock(numberOfDigitsOfDenominator));
  numerator.pushDigitsOnEditionPool();
  denominator.pushDigitsOnEditionPool();
  pool->pushBlock(ValueBlock(numberOfDigitsOfNumerator + numberOfDigitsOfDenominator));
  pool->pushBlock(typeBlock);
  return reference;
}

void Rational::SetSign(EditionReference reference, NonStrictSign sign) {
  IntegerHandler numerator = Numerator(reference.node());
  IntegerHandler denominator = Denominator(reference.node());
  numerator.setSign(sign);
  reference.replaceNodeByNode(PushNode(numerator, denominator));
}

}
