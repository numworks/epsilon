#include "rational.h"

#include <omg/unreachable.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/value_block.h>

#include "k_tree.h"
#include "number.h"

#if POINCARE_POOL_VISUALIZATION
#include <poincare/src/memory/visualization.h>
#endif

namespace Poincare::Internal {

// TODO: tests

IntegerHandler Rational::Numerator(const Tree* e) {
  Type type = e->type();
  switch (type) {
    case Type::Zero:
      return IntegerHandler(static_cast<int8_t>(0));
    case Type::One:
      return IntegerHandler(1);
    case Type::Two:
      return IntegerHandler(2);
    case Type::MinusOne:
      return IntegerHandler(-1);
    case Type::Half:
      return IntegerHandler(1);
    case Type::IntegerPosShort:
    case Type::IntegerNegShort: {
      uint8_t value = e->nodeValue(0);
      return IntegerHandler(type == Type::IntegerPosShort ? value : -value);
    }
    case Type::IntegerPosBig:
    case Type::IntegerNegBig: {
      const Block* block = e->block();
      uint8_t numberOfDigits = e->nodeValue(0);
      const uint8_t* digits =
          reinterpret_cast<const uint8_t*>(block->nextNth(2));
      return IntegerHandler(digits, numberOfDigits,
                            type == Type::IntegerNegBig
                                ? NonStrictSign::Negative
                                : NonStrictSign::Positive);
    }
    case Type::RationalPosShort:
    case Type::RationalNegShort: {
      uint8_t value = e->nodeValue(0);
      return IntegerHandler(type == Type::RationalPosShort ? value : -value);
    }
    case Type::RationalPosBig:
    case Type::RationalNegBig: {
      const Block* block = e->block();
      uint8_t numberOfDigits = e->nodeValue(0);
      const uint8_t* digits =
          reinterpret_cast<const uint8_t*>(block->nextNth(3));
      return IntegerHandler(digits, numberOfDigits,
                            type == Type::RationalNegBig
                                ? NonStrictSign::Negative
                                : NonStrictSign::Positive);
    }
    default:
      OMG::unreachable();
  }
}

IntegerHandler Rational::Denominator(const Tree* e) {
  switch (e->type()) {
    case Type::Zero:
    case Type::One:
    case Type::Two:
    case Type::MinusOne:
    case Type::IntegerPosShort:
    case Type::IntegerNegShort:
    case Type::IntegerPosBig:
    case Type::IntegerNegBig:
      return IntegerHandler(1);
    case Type::Half:
      return IntegerHandler(2);
    case Type::RationalPosShort:
    case Type::RationalNegShort: {
      return IntegerHandler(e->nodeValue(1));
    }
    case Type::RationalPosBig:
    case Type::RationalNegBig: {
      const Block* block = e->block();
      uint8_t numeratorNumberOfDigits = e->nodeValue(0);
      uint8_t denominatorNumberOfDigits = e->nodeValue(1);
      const uint8_t* digits = reinterpret_cast<const uint8_t*>(
          block->nextNth(3 + numeratorNumberOfDigits));
      return IntegerHandler(digits, denominatorNumberOfDigits,
                            NonStrictSign::Positive);
    }
    default:
      OMG::unreachable();
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
  Tree* e;
  if (denominator.isOne() || numerator.isZero()) {
    e = numerator.pushOnTreeStack();
  } else if (numerator.isOne() && denominator.isTwo()) {
    e = SharedTreeStack->pushHalf();
  } else if (numerator.numberOfDigits() == 1 &&
             denominator.isUnsignedType<uint8_t>()) {
    if (numerator.sign() == NonStrictSign::Positive) {
      e = SharedTreeStack->pushBlock(Type::RationalPosShort);
    } else {
      e = SharedTreeStack->pushBlock(Type::RationalNegShort);
      numerator.setSign(NonStrictSign::Positive);
    }
    SharedTreeStack->pushBlock(ValueBlock(static_cast<uint8_t>(numerator)));
    SharedTreeStack->pushBlock(ValueBlock(static_cast<uint8_t>(denominator)));
  } else {
    e = SharedTreeStack->pushBlock(numeratorSign == NonStrictSign::Negative
                                       ? Type::RationalNegBig
                                       : Type::RationalPosBig);
    SharedTreeStack->pushBlock(ValueBlock(numerator.numberOfDigits()));
    SharedTreeStack->pushBlock(ValueBlock(denominator.numberOfDigits()));
    numerator.pushDigitsOnTreeStack();
    denominator.pushDigitsOnTreeStack();
  }
  assert(IsIrreducible(e));
#if POINCARE_POOL_VISUALIZATION
  Log("PushRational", e->block(), e->treeSize());
#endif
  return e;
}

Tree* Rational::Push(IntegerHandler numerator, IntegerHandler denominator) {
  /* Ensure unicity among all rationals. For example, convert 6/3 to Half
   * node. As a result there are many forbidden rational nodes. */
  assert(!denominator.isZero());
  Tree* result = Tree::FromBlocks(SharedTreeStack->lastBlock());
  // Push 1 temporary tree on the TreeStack.
  IntegerHandler gcd =
      Integer::Handler(IntegerHandler::GCD(numerator, denominator));
  if (!gcd.isOne()) {
    // Push 2 additional temporary trees on the TreeStack.
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

bool Rational::SetSign(Tree* e, NonStrictSign sign) {
  IntegerHandler numerator = Numerator(e);
  IntegerHandler denominator = Denominator(e);
  if (numerator.isZero() || sign == numerator.sign()) {
    return false;
  }
  numerator.setSign(sign);
  e->moveTreeOverTree(PushIrreducible(numerator, denominator));
  return true;
}

Tree* Rational::Addition(const Tree* e1, const Tree* e2) {
  // a/b + c/d
  Tree* ad = IntegerHandler::Multiplication(Numerator(e1), Denominator(e2));
  Tree* cb = IntegerHandler::Multiplication(Numerator(e2), Denominator(e1));
  TreeRef newNumerator =
      IntegerHandler::Addition(Integer::Handler(ad), Integer::Handler(cb));
  cb->removeTree();
  ad->removeTree();
  TreeRef newDenominator =
      IntegerHandler::Multiplication(Denominator(e1), Denominator(e2));
  TreeRef result = Rational::Push(newNumerator, newDenominator);
  newDenominator->removeTree();
  newNumerator->removeTree();
  return result;
}

Tree* Rational::Multiplication(const Tree* e1, const Tree* e2) {
  Tree* newNumerator =
      IntegerHandler::Multiplication(Numerator(e1), Numerator(e2));
  Tree* newDenominator =
      IntegerHandler::Multiplication(Denominator(e1), Denominator(e2));
  TreeRef result = Rational::Push(newNumerator, newDenominator);
  newDenominator->removeTree();
  newNumerator->removeTree();
  return result;
}

Tree* Rational::IntegerPower(const Tree* e1, const Tree* e2) {
  assert(!(e1->isZero() && Sign(e2).isNegative()));
  IntegerHandler absJ = Integer::Handler(e2);
  absJ.setSign(NonStrictSign::Positive);
  Tree* newNumerator = IntegerHandler::Power(Numerator(e1), absJ);
  Tree* newDenominator = IntegerHandler::Power(Denominator(e1), absJ);
  TreeRef result =
      Sign(e2).isNegative()
          ? Rational::PushIrreducible(newDenominator, newNumerator)
          : Rational::PushIrreducible(newNumerator, newDenominator);
  newDenominator->removeTree();
  newNumerator->removeTree();
  return result;
}

bool Rational::IsIrreducible(const Tree* e) {
  if (!e->isOfType({Type::RationalNegShort, Type::RationalPosShort,
                    Type::RationalNegBig, Type::RationalPosBig})) {
    return true;
  }
  TreeRef gcd = IntegerHandler::GCD(Numerator(e), Denominator(e));
  bool result = gcd->isOne();
  gcd->removeTree();
  return result;
}

bool Rational::IsGreaterThanOne(const Tree* e) {
  return IntegerHandler::Compare(Numerator(e), Denominator(e)) > 0;
}

Tree* Rational::CreateEuclideanDivision(const Tree* e) {
  IntegerHandler num = Numerator(e);
  IntegerHandler den = Denominator(e);
  DivisionResult<Tree*> division = IntegerHandler::Division(num, den);
  TreeRef numTree = num.pushOnTreeStack();
  TreeRef denTree = den.pushOnTreeStack();
  TreeRef result = PatternMatching::Create(KEqual(KA, KAdd(KMult(KB, KC), KD)),
                                           {.KA = numTree,
                                            .KB = denTree,
                                            .KC = division.quotient,
                                            .KD = division.remainder});
  denTree->removeTree();
  numTree->removeTree();
  division.remainder->removeTree();
  division.quotient->removeTree();
  return result;
}

Tree* Rational::CreateMixedFraction(const Tree* e,
                                    bool mixedFractionsAreEnabled) {
  IntegerHandler num = Numerator(e);
  IntegerHandler den = Denominator(e);
  bool numIsNegative = num.strictSign() == StrictSign::Negative;
  num.setSign(NonStrictSign::Positive);
  // Push quotient and remainder
  DivisionResult<Tree*> division = IntegerHandler::Division(num, den);
  Tree* integerPart = division.quotient;
  // Push the fraction
  TreeRef fractionPart =
      Rational::Push(Integer::Handler(division.remainder), den);
  division.remainder->removeTree();
  // If mixed fractions are enabled
  if (mixedFractionsAreEnabled) {
    integerPart->cloneNodeAtNode(KMixedFraction);
    if (numIsNegative) {
      integerPart->cloneNodeBeforeNode(KOpposite);
    }
    return integerPart;
  }
  // If mixed fractions don't exist in this country
  if (numIsNegative) {
    if (!integerPart->isZero()) {
      integerPart->cloneNodeBeforeNode(KOpposite);
    }
    fractionPart->cloneNodeBeforeNode(KOpposite);
  }
  integerPart->cloneNodeAtNode(KAdd.node<2>);
  return integerPart;
}

}  // namespace Poincare::Internal
