#include "rational.h"

#include <poincare_junior/include/poincare.h>
#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <poincare_junior/src/memory/value_block.h>

#include "number.h"

namespace PoincareJ {

// TODO: tests

IntegerHandler Rational::Numerator(const Tree* node) {
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

IntegerHandler Rational::Denominator(const Tree* node) {
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
      const Block* block = node->block();
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

Tree* Rational::Push(IntegerHandler numerator, IntegerHandler denominator) {
  assert(!denominator.isZero());
  /* Settle sign beforehand so that :
   *   x/-1 is -x
   *   -1/-2 is Half
   *   127/-255 can fit as a RationalShort */
  NonStrictSign numeratorSign = numerator.sign() == denominator.sign()
                                    ? NonStrictSign::Positive
                                    : NonStrictSign::Negative;
  numerator.setSign(numeratorSign);
  denominator.setSign(NonStrictSign::Positive);
  if (denominator.isOne() || numerator.isZero()) {
    return numerator.pushOnEditionPool();
  }
  if (numerator.isOne() && denominator.isTwo()) {
    return SharedEditionPool->push<BlockType::Half>();
  }
  if (numerator.isSignedType<int8_t>() &&
      denominator.isUnsignedType<uint8_t>()) {
    return SharedEditionPool->push<BlockType::RationalShort>(
        static_cast<int8_t>(numerator), static_cast<uint8_t>(denominator));
  }
  Tree* node = Tree::FromBlocks(SharedEditionPool->pushBlock(
      numeratorSign == NonStrictSign::Negative ? BlockType::RationalNegBig
                                               : BlockType::RationalPosBig));
  SharedEditionPool->pushBlock(ValueBlock(numerator.numberOfDigits()));
  SharedEditionPool->pushBlock(ValueBlock(denominator.numberOfDigits()));
  numerator.pushDigitsOnEditionPool();
  denominator.pushDigitsOnEditionPool();
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "PushRational", node->block(), node->treeSize());
#endif
  return node;
}

void Rational::SetSign(Tree* tree, NonStrictSign sign) {
  IntegerHandler numerator = Numerator(tree);
  IntegerHandler denominator = Denominator(tree);
  numerator.setSign(sign);
  tree->moveNodeOverNode(Push(numerator, denominator));
}

Tree* Rational::Addition(const Tree* i, const Tree* j) {
  // a/b + c/d
  Tree* ad = IntegerHandler::Multiplication(Numerator(i), Denominator(j));
  Tree* cb = IntegerHandler::Multiplication(Numerator(j), Denominator(i));
  EditionReference newNumerator =
      IntegerHandler::Addition(Integer::Handler(ad), Integer::Handler(cb));
  cb->removeTree();
  ad->removeTree();
  EditionReference newDenominator =
      IntegerHandler::Multiplication(Denominator(i), Denominator(j));
  EditionReference result = Rational::Push(newNumerator, newDenominator);
  newDenominator->removeTree();
  newNumerator->removeTree();
  return result;
}

Tree* Rational::Multiplication(const Tree* i, const Tree* j) {
  Tree* newNumerator =
      IntegerHandler::Multiplication(Numerator(i), Numerator(j));
  Tree* newDenominator =
      IntegerHandler::Multiplication(Denominator(i), Denominator(j));
  EditionReference result = Rational::Push(newNumerator, newDenominator);
  newDenominator->removeTree();
  newNumerator->removeTree();
  return result;
}

Tree* Rational::IntegerPower(const Tree* i, const Tree* j) {
  assert(!(Number::IsZero(i) && Sign(j) == NonStrictSign::Negative));
  IntegerHandler absJ = Integer::Handler(j);
  absJ.setSign(NonStrictSign::Positive);
  Tree* newNumerator = IntegerHandler::Power(Numerator(i), absJ);
  Tree* newDenominator = IntegerHandler::Power(Denominator(i), absJ);
  EditionReference result = Sign(j) == NonStrictSign::Negative
                                ? Rational::Push(newDenominator, newNumerator)
                                : Rational::Push(newNumerator, newDenominator);
  newDenominator->removeTree();
  newNumerator->removeTree();
  return result;
}

Tree* Rational::IrreducibleForm(const Tree* i) {
  Tree* gcd = IntegerHandler::GCD(Numerator(i), Denominator(i));
  if (IntegerHandler::Compare(Integer::Handler(gcd), Denominator(i)) == 0) {
    EditionReference numerator =
        IntegerHandler::Quotient(Numerator(i), Integer::Handler(gcd));
    gcd->removeTree();
    return numerator;
  }
  if (gcd->type() != BlockType::One) {
    Tree* numerator =
        IntegerHandler::Quotient(Numerator(i), Integer::Handler(gcd));
    Tree* denominator =
        IntegerHandler::Quotient(Denominator(i), Integer::Handler(gcd));
    EditionReference result = Rational::Push(numerator, denominator);
    denominator->removeTree();
    numerator->removeTree();
    gcd->removeTree();
    return result;
  }
  gcd->removeTree();
  return i->clone();
}

}  // namespace PoincareJ
