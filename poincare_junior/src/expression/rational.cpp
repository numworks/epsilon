#include "rational.h"

namespace Poincare {

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
      return IntegerHandler(digits, numberOfDigits, type == BlockType::IntegerNegBig);
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
      return IntegerHandler(digits, numberOfDigits, type == BlockType::RationalNegBig);
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
      return IntegerHandler(digits, denominatorNumberOfDigits, false);
    }
    default:
      assert(false);
  }
}

}
