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
      int8_t value = static_cast<int8_t>(node->nodeValue(0));
      return IntegerHandler(value);
    }
    case BlockType::IntegerPosBig:
    case BlockType::IntegerNegBig: {
      const Block* block = node->block();
      uint8_t numberOfDigits = node->nodeValue(0);
      const uint8_t* digits =
          reinterpret_cast<const uint8_t*>(block->nextNth(2));
      return IntegerHandler(digits, numberOfDigits,
                            type == BlockType::IntegerNegBig
                                ? NonStrictSign::Negative
                                : NonStrictSign::Positive);
    }
    case BlockType::RationalShort: {
      int8_t value = static_cast<int8_t>(node->nodeValue(0));
      return IntegerHandler(value);
    }
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig: {
      const Block* block = node->block();
      uint8_t numberOfDigits = node->nodeValue(0);
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
      return IntegerHandler(node->nodeValue(1));
    }
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig: {
      const Block* block = node->block();
      uint8_t numeratorNumberOfDigits = node->nodeValue(0);
      uint8_t denominatorNumberOfDigits = node->nodeValue(1);
      const uint8_t* digits = reinterpret_cast<const uint8_t*>(
          block->nextNth(3 + numeratorNumberOfDigits));
      return IntegerHandler(digits, denominatorNumberOfDigits,
                            NonStrictSign::Positive);
    }
    default:
      assert(false);
  }
}

Tree* Rational::PushIrreducible(IntegerHandler numerator,
                                IntegerHandler denominator) {
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
  Tree* node;
  if (denominator.isOne() || numerator.isZero()) {
    node = numerator.pushOnEditionPool();
  } else if (numerator.isOne() && denominator.isTwo()) {
    node = SharedEditionPool->push(BlockType::Half);
  } else if (numerator.isSignedType<int8_t>() &&
             denominator.isUnsignedType<uint8_t>()) {
    node = SharedEditionPool->push(BlockType::RationalShort);
    SharedEditionPool->push(ValueBlock(static_cast<int8_t>(numerator)));
    SharedEditionPool->push(ValueBlock(static_cast<uint8_t>(denominator)));
  } else {
    node = SharedEditionPool->push(numeratorSign == NonStrictSign::Negative
                                       ? BlockType::RationalNegBig
                                       : BlockType::RationalPosBig);
    SharedEditionPool->push(ValueBlock(numerator.numberOfDigits()));
    SharedEditionPool->push(ValueBlock(denominator.numberOfDigits()));
    numerator.pushDigitsOnEditionPool();
    denominator.pushDigitsOnEditionPool();
  }
  assert(IsIrreducible(node));
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "PushRational", node->block(), node->treeSize());
#endif
  return node;
}

Tree* Rational::Push(IntegerHandler numerator, IntegerHandler denominator) {
  /* Ensure unicity among all rationals. For example, convert 6/3 to Half node.
   * As a result there are many forbidden rational nodes. */
  assert(!denominator.isZero());
  Tree* result = Tree::FromBlocks(SharedEditionPool->lastBlock());
  // Push 1 temporary tree on the EditionPool.
  IntegerHandler gcd =
      Integer::Handler(IntegerHandler::GCD(numerator, denominator));
  if (!gcd.isOne()) {
    // Push 2 additional temporary trees on the EditionPool.
    numerator = Integer::Handler(IntegerHandler::Quotient(numerator, gcd));
    denominator = Integer::Handler(IntegerHandler::Quotient(denominator, gcd));
  }
  PushIrreducible(numerator, denominator);
  if (!gcd.isOne()) {
    // Remove 2 out of the 3 of temporary trees.
    result->removeTree();
    result->removeTree();
  }
  // Remove the only remaining temporary tree.
  result->removeTree();
  return result;
}

void Rational::SetSign(Tree* tree, NonStrictSign sign) {
  IntegerHandler numerator = Numerator(tree);
  IntegerHandler denominator = Denominator(tree);
  numerator.setSign(sign);
  tree->moveTreeOverTree(PushIrreducible(numerator, denominator));
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
  assert(!(i->isZero() && Sign(j).isNegative()));
  IntegerHandler absJ = Integer::Handler(j);
  absJ.setSign(NonStrictSign::Positive);
  Tree* newNumerator = IntegerHandler::Power(Numerator(i), absJ);
  Tree* newDenominator = IntegerHandler::Power(Denominator(i), absJ);
  EditionReference result =
      Sign(j).isNegative()
          ? Rational::PushIrreducible(newDenominator, newNumerator)
          : Rational::PushIrreducible(newNumerator, newDenominator);
  newDenominator->removeTree();
  newNumerator->removeTree();
  return result;
}

bool Rational::IsIrreducible(const Tree* i) {
  if (!i->isOfType({BlockType::RationalShort, BlockType::RationalNegBig,
                    BlockType::RationalPosBig})) {
    return true;
  }
  EditionReference gcd = IntegerHandler::GCD(Numerator(i), Denominator(i));
  bool result = gcd->isOne();
  gcd->removeTree();
  return result;
}

}  // namespace PoincareJ
