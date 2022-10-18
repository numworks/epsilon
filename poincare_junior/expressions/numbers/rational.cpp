#include "rational.h"

namespace Poincare {

IntegerHandler Rational::Numerator(const TypeBlock * block) {
  BlockType type = block->type();
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
      return IntegerHandler(IntegerShort::Value(block));
    case BlockType::IntegerPosBig:
    case BlockType::IntegerNegBig:
      return IntegerHandler(IntegerBig::Digits(block), IntegerBig::NumberOfDigits(block), type == BlockType::IntegerNegBig);
    case BlockType::RationalShort:
      return IntegerHandler(RationalShort::NumeratorValue(block));
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig:
      return IntegerHandler(RationalBig::NumeratorDigits(block), RationalBig::NumeratorNumberOfDigits(block), type == BlockType::RationalNegBig);
    default:
      assert(false);
  }
}

IntegerHandler Rational::Denominator(const TypeBlock * block) {
  switch (block->type()) {
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
      return IntegerHandler(RationalShort::DenominatorValue(block));
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig:
      return IntegerHandler(RationalBig::DenominatorDigits(block), RationalBig::DenominatorNumberOfDigits(block), true);
    default:
      assert(false);
  }
}

}
