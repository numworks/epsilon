#include "rational.h"

#include <poincare_junior/include/poincare.h>
#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <poincare_junior/src/memory/value_block.h>

#include "number.h"

namespace PoincareJ {

// TODO: tests

IntegerHandler Rational::Numerator(const Node* node) {
  BlockType type = node->type();
  switch (type) {
    case BlockType::Zero:
      return IntegerHandler(static_cast<int8_t>(0));
    case BlockType::One:
      return IntegerHandler(1);
    case BlockType::Two:
      return IntegerHandler(2);
    case BlockType::MinusOne:
      return IntegerHandler(-1);
    case BlockType::Half:
      return IntegerHandler(1);
    case BlockType::IntegerShort: {
      int8_t value = static_cast<int8_t>(*(node->block()->next()));
      return IntegerHandler(value);
    }
    case BlockType::IntegerPosBig:
    case BlockType::IntegerNegBig: {
      const Block* block = node->block();
      uint8_t numberOfDigits = static_cast<uint8_t>(*(block->next()));
      const uint8_t* digits =
          reinterpret_cast<const uint8_t*>(block->nextNth(2));
      return IntegerHandler(digits, numberOfDigits,
                            type == BlockType::IntegerNegBig
                                ? NonStrictSign::Negative
                                : NonStrictSign::Positive);
    }
    case BlockType::RationalShort: {
      int8_t value = static_cast<int8_t>(*(node->block()->next()));
      return IntegerHandler(value);
    }
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig: {
      const Block* block = node->block();
      uint8_t numberOfDigits = static_cast<uint8_t>(*(block->next()));
      const uint8_t* digits =
          reinterpret_cast<const uint8_t*>(block->nextNth(3));
      return IntegerHandler(digits, numberOfDigits,
                            type == BlockType::RationalNegBig
                                ? NonStrictSign::Negative
                                : NonStrictSign::Positive);
    }
    default:
      assert(false);
  }
}

IntegerHandler Rational::Denominator(const Node* node) {
  switch (node->type()) {
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
    case BlockType::RationalShort: {
      uint8_t value = static_cast<uint8_t>(*(node->block()->nextNth(2)));
      return IntegerHandler(value);
    }
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig: {
      Block* block = node->block();
      uint8_t numeratorNumberOfDigits = static_cast<uint8_t>(*(block->next()));
      uint8_t denominatorNumberOfDigits =
          static_cast<uint8_t>(*(block->nextNth(2)));
      const uint8_t* digits = reinterpret_cast<const uint8_t*>(
          block->nextNth(3 + numeratorNumberOfDigits));
      return IntegerHandler(digits, denominatorNumberOfDigits,
                            NonStrictSign::Positive);
    }
    default:
      assert(false);
  }
}

Node* Rational::Push(IntegerHandler numerator, IntegerHandler denominator) {
  assert(!denominator.isZero());
  if (denominator.isOne()) {
    return numerator.pushOnEditionPool();
  }
  EditionPool* pool = EditionPool::sharedEditionPool();
  if (numerator.isOne() && denominator.isTwo()) {
    return pool->push<BlockType::Half>();
  }
  if (numerator.isSignedType<int8_t>() &&
      denominator.isUnsignedType<uint8_t>()) {
    return pool->push<BlockType::RationalShort>(
        static_cast<int8_t>(numerator), static_cast<uint8_t>(denominator));
  }
  TypeBlock typeBlock(numerator.sign() == NonStrictSign::Negative
                          ? BlockType::RationalNegBig
                          : BlockType::RationalPosBig);
  Node* node(pool->pushBlock(typeBlock));
  uint8_t numberOfDigitsOfNumerator = numerator.numberOfDigits();
  uint8_t numberOfDigitsOfDenominator = numerator.numberOfDigits();
  if (numberOfDigitsOfNumerator > UINT8_MAX - numberOfDigitsOfDenominator) {
    // TODO: set error type to be "Unrepresentable rational"
    ExceptionCheckpoint::Raise();
    return nullptr;
  }
  pool->pushBlock(ValueBlock(numberOfDigitsOfNumerator));
  pool->pushBlock(ValueBlock(numberOfDigitsOfDenominator));
  numerator.pushDigitsOnEditionPool();
  denominator.pushDigitsOnEditionPool();
  pool->pushBlock(
      ValueBlock(numberOfDigitsOfNumerator + numberOfDigitsOfDenominator));
  pool->pushBlock(typeBlock);
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "PushRational", node->block(), node->treeSize());
#endif
  return node;
}

void Rational::SetSign(EditionReference reference, NonStrictSign sign) {
  IntegerHandler numerator = Numerator(reference);
  IntegerHandler denominator = Denominator(reference);
  numerator.setSign(sign);
  reference.replaceNodeByNode(Push(numerator, denominator));
}

Node* Rational::Addition(const Node* i, const Node* j) {
  // a/b + c/d
  EditionReference ad =
      IntegerHandler::Multiplication(Numerator(i), Denominator(j));
  EditionReference cb =
      IntegerHandler::Multiplication(Numerator(j), Denominator(i));
  EditionReference newNumerator =
      IntegerHandler::Addition(Integer::Handler(ad), Integer::Handler(cb));
  ad.removeTree();
  cb.removeTree();
  EditionReference newDenominator =
      IntegerHandler::Multiplication(Denominator(i), Denominator(j));
  EditionReference result = Rational::Push(newNumerator, newDenominator);
  newNumerator.removeTree();
  newDenominator.removeTree();
  return result;
}

Node* Rational::Multiplication(const Node* i, const Node* j) {
  EditionReference newNumerator =
      IntegerHandler::Multiplication(Numerator(i), Numerator(j));
  EditionReference newDenominator =
      IntegerHandler::Multiplication(Denominator(i), Denominator(j));
  EditionReference result = Rational::Push(newNumerator, newDenominator);
  newNumerator.removeTree();
  newDenominator.removeTree();
  return result;
}

Node* Rational::IntegerPower(const Node* i, const Node* j) {
  assert(!(Number::IsZero(i) && Sign(j) == NonStrictSign::Negative));
  IntegerHandler absJ = Integer::Handler(j);
  absJ.setSign(NonStrictSign::Positive);
  EditionReference newNumerator = IntegerHandler::Power(Numerator(i), absJ);
  EditionReference newDenominator = IntegerHandler::Power(Denominator(i), absJ);
  EditionReference result = Sign(j) == NonStrictSign::Negative
                                ? Rational::Push(newDenominator, newNumerator)
                                : Rational::Push(newNumerator, newDenominator);
  newNumerator.removeTree();
  newDenominator.removeTree();
  return result;
}

Node* Rational::IrreducibleForm(const Node* i) {
  EditionReference gcd = IntegerHandler::GCD(Numerator(i), Denominator(i));
  if (IntegerHandler::Compare(Integer::Handler(gcd), Denominator(i)) == 0) {
    EditionReference numerator =
        IntegerHandler::Quotient(Numerator(i), Integer::Handler(gcd));
    gcd.removeTree();
    return numerator;
  }
  if (gcd.type() != BlockType::One) {
    EditionReference numerator =
        IntegerHandler::Quotient(Numerator(i), Integer::Handler(gcd));
    EditionReference denominator =
        IntegerHandler::Quotient(Denominator(i), Integer::Handler(gcd));
    EditionReference result = Rational::Push(numerator, denominator);
    gcd.removeTree();
    numerator.removeTree();
    denominator.removeTree();
    return result;
  }
  gcd.removeTree();
  return EditionPool::sharedEditionPool()->clone(i);
}

}  // namespace PoincareJ
