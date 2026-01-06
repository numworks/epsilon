#include "rational.h"

#include <omg/unreachable.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/value_block.h>

#include "k_tree.h"
#include "poincare/sign.h"

#if POINCARE_TREE_STACK_VISUALIZATION
#include <poincare/src/memory/visualization.h>
#endif

namespace Poincare::Internal {

template <typename T>
T Rational::To(const Tree* e) {
  return Rational::Numerator(e).to<T>() / Rational::Denominator(e).to<T>();
}

IntegerHandler Rational::Numerator(const Tree* e) {
  assert(e->isRational());
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
      OMG_UNREACHABLE;
  }
}

IntegerHandler Rational::Denominator(const Tree* e) {
  assert(e->isRational());
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
      assert(numeratorNumberOfDigits <= IntegerHandler::k_maxNumberOfDigits);
      uint8_t denominatorNumberOfDigits = e->nodeValue(1);
      const uint8_t* digits = reinterpret_cast<const uint8_t*>(
          block->nextNth(3 + numeratorNumberOfDigits));
      return IntegerHandler(digits, denominatorNumberOfDigits,
                            NonStrictSign::Positive);
    }
    default:
      OMG_UNREACHABLE;
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
#if POINCARE_TREE_STACK_VISUALIZATION
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
  if (e1->isInteger() && e2->isInteger()) {
    // Early return if there are no denominators
    return IntegerHandler::Addition(Numerator(e1), Numerator(e2));
  }
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
  if (e1->isInteger() && e2->isInteger()) {
    // Early return if there are no denominators
    return newNumerator;
  }
  Tree* newDenominator =
      IntegerHandler::Multiplication(Denominator(e1), Denominator(e2));
  TreeRef result = Rational::Push(newNumerator, newDenominator);
  newDenominator->removeTree();
  newNumerator->removeTree();
  return result;
}

Tree* Rational::NonZeroDivision(const Tree* e1, const Tree* e2) {
  assert(e1->isRational() && e2->isRational() && !e2->isZero());
  Tree* newNumerator =
      IntegerHandler::Multiplication(Numerator(e1), Denominator(e2));
  Tree* newDenominator =
      IntegerHandler::Multiplication(Denominator(e1), Numerator(e2));
  TreeRef result = Rational::Push(newNumerator, newDenominator);
  newDenominator->removeTree();
  newNumerator->removeTree();
  return result;
}

Rational::IntegerOperationResult Rational::IntegerPower(const Tree* e1,
                                                        const Tree* e2) {
  assert(!(e1->isZero() && Properties(e2).sign().isNegative()));
  ExceptionTry {
    IntegerHandler absJ = Integer::Handler(e2);
    absJ.setSign(NonStrictSign::Positive);
    Tree* newNumerator = IntegerHandler::Power(Numerator(e1), absJ);
    Tree* newDenominator = IntegerHandler::Power(Denominator(e1), absJ);
    TreeRef result =
        Properties(e2).sign().isNegative()
            ? Rational::PushIrreducible(newDenominator, newNumerator)
            : Rational::PushIrreducible(newNumerator, newDenominator);
    newDenominator->removeTree();
    newNumerator->removeTree();
    return IntegerOperationResult{.tree = result, .hasOverflown = false};
  }
  ExceptionCatch(type) {
    if (type == ExceptionType::IntegerOverflow) {
      // Return the unchanged power
      return IntegerOperationResult{
          .tree = PatternMatching::Create(KPow(KA, KB), {.KA = e1, .KB = e2}),
          .hasOverflown = true};
    }
    TreeStackCheckpoint::Raise(type);
  }
  OMG_UNREACHABLE;
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

bool Rational::IsStrictlyPositiveUnderOne(const Tree* e) {
  IntegerHandler num = Numerator(e);
  return IntegerHandler::Compare(num, IntegerHandler(0)) > 0 &&
         IntegerHandler::Compare(num, Denominator(e)) < 0;
}

int Rational::CompareHandlers(IntegerHandler num1, IntegerHandler denom1,
                              IntegerHandler num2, IntegerHandler denom2) {
  assert(denom1.strictSign() == StrictSign::Positive &&
         denom2.strictSign() == StrictSign::Positive);
  // Compare signs
  if (num1.strictSign() == StrictSign::Null) {
    return -static_cast<int>(num2.strictSign());
  } else if (num2.strictSign() == StrictSign::Null) {
    return static_cast<int>(num1.strictSign());
  } else if (num1.strictSign() != num2.strictSign()) {
    return num1.strictSign() > num2.strictSign() ? 1 : -1;
  }
  assert(num1.strictSign() == num2.strictSign());
  uint8_t n1 = num1.numberOfDigits();
  uint8_t n2 = num2.numberOfDigits();
  uint8_t d1 = denom1.numberOfDigits();
  uint8_t d2 = denom2.numberOfDigits();
  /* Compare orders of magnitude to escape obvious cases and avoid unnecessary
   * big multiplications.
   * 10^(n1-d1-1) < |num1/denom1| < 10^(n1-d1+1) and
   * 10^(n2-d2-1) < |num2/denom2| < 10^(n2-d2-1)
   */
  if (n1 - d1 - 1 >= n2 - d2 + 1) {
    return num1.strictSign() == StrictSign::Positive ? 1 : -1;
  } else if (n2 - d2 - 1 > n1 - d1 + 1) {
    return num2.strictSign() == StrictSign::Positive ? -1 : 1;
  }
  // num1/denom1 > num2/denom2 <=> num1*denom2 > num2*denom1
  Tree* m1 = IntegerHandler::Multiplication(num1, denom2);
  Tree* m2 = IntegerHandler::Multiplication(denom1, num2);
  int result =
      IntegerHandler::Compare(Integer::Handler(m1), Integer::Handler(m2));
  m2->removeTree();
  m1->removeTree();
  return result;
}

int Rational::Compare(const Tree* e1, const Tree* e2) {
  assert(e1->isRational() && e2->isRational());
  return CompareHandlers(Numerator(e1), Denominator(e1), Numerator(e2),
                         Denominator(e2));
}

int Rational::CompareAbs(const Tree* e1, const Tree* e2) {
  assert(e1->isRational() && e2->isRational());
  IntegerHandler num1 = Numerator(e1);
  IntegerHandler num2 = Numerator(e2);
  num1.setSign(NonStrictSign::Positive);
  num2.setSign(NonStrictSign::Positive);
  return CompareHandlers(num1, Denominator(e1), num2, Denominator(e2));
}

OMG::Troolean Rational::AbsSmallerThanPi(const Tree* e) {
  assert(e->isRational());
  // 157/50 < π < 79/25. Improve these numbers for less unknown results.
  const Tree* piMinus = 157_e / 50_e;
  const Tree* piPlus = 79_e / 25_e;
  return CompareAbs(e, piMinus) <= 0 ? OMG::Troolean::True
         : CompareAbs(e, piPlus) < 0 ? OMG::Troolean::Unknown
                                     : OMG::Troolean::False;
}

ComplexProperties Rational::ComplexPropertiesOfLn(const Tree* e) {
  assert(e->isLn());
  const Tree* child = e->child(0);
  assert(child->isRational());
  if (child->isOne()) {
    return ComplexProperties(Properties::Zero(), Properties::Zero());
  }
  if (child->isStrictlyPositiveRational()) {
    /* Check if the child is greater than one to determine sign */
    class Properties realProperties =
        Internal::Rational::IsGreaterThanOne(child)
            ? Properties::FiniteStrictlyPositive()
            : Properties::FiniteStrictlyNegative();
    return ComplexProperties(realProperties, Properties::Zero());
  }
  if (child->isMinusOne()) {
    return ComplexProperties(Properties::Zero(),
                             Properties::FiniteStrictlyPositive());
  }
  if (child->isStrictlyNegativeRational()) {
    /* Reverse the sign of Numerator to be able to compare it with Denominator
     * and see if the absolute value is greater than one */
    IntegerHandler numerator = Rational::Numerator(child);
    numerator.setSign(NonStrictSign::Positive);
    class Properties realProperties =
        IntegerHandler::Compare(numerator, Rational::Denominator(child)) > 0
            ? Properties::FiniteStrictlyPositive()
            : Properties::FiniteStrictlyNegative();
    return ComplexProperties(realProperties,
                             Properties::FiniteStrictlyPositive());
  }
  assert(child->isZero());
  return ComplexProperties::Unknown();
}

bool Rational::IsMinusHalf(const Tree* e) {
  return e->treeIsIdenticalTo(-1_e / 2_e);
}

template float Rational::To<float>(const Tree* e);
template double Rational::To<double>(const Tree* e);

}  // namespace Poincare::Internal
