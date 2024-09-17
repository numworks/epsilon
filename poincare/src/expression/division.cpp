#include "division.h"

#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree.h>

#include "degree.h"
#include "infinity.h"
#include "k_tree.h"
#include "number.h"
#include "rational.h"

namespace Poincare::Internal {

Tree* Factor(Tree* e, int index) {
  if (e->isMult()) {
    return e->child(index);
  }
  return e;
}

const Tree* Factor(const Tree* e, int index) {
  if (e->isMult()) {
    assert(e->numberOfChildren() > index);
    return e->child(index);
  }
  return e;
}

int NumberOfFactors(const Tree* e) {
  if (e->isMult()) {
    assert(e->numberOfChildren() > 0);
    return e->numberOfChildren();
  }
  return 1;
}

bool MakePositiveAnyNegativeNumeralFactor(Tree* e) {
  // The expression is a negative number
  Tree* factor = Factor(e, 0);
  if (factor->isMinusOne() && e->isMult()) {
    NAry::RemoveChildAtIndex(e, 0);
    NAry::SquashIfUnary(e);
    return true;
  }
  return factor->isNumber() && Number::SetSign(factor, NonStrictSign::Positive);
}

void Division::GetDivisionComponents(const Tree* e, TreeRef& numerator,
                                     TreeRef& denominator,
                                     TreeRef& outerNumerator,
                                     bool* needOpposite) {
  assert(needOpposite);
  assert(!numerator.isUninitialized() && numerator->isMult());
  assert(!denominator.isUninitialized() && denominator->isMult());
  assert(!outerNumerator.isUninitialized() && outerNumerator->isMult());
  // TODO replace NumberOfFactors and Factor with an iterable
  const int numberOfFactors = NumberOfFactors(e);
  for (int i = 0; i < numberOfFactors; i++) {
    const Tree* factor = Factor(e, i);
    TreeRef factorsNumerator;
    TreeRef factorsDenominator;
    TreeRef factorsOuterNumerator;
    if (factor->isComplexI() && i == numberOfFactors - 1) {
      /* Move the final i out of the multiplication e.g. 2^(-1)×i → (1/2)×i. If
       * i is not in the last position, it is either intentional or a bug in the
       * order, so leave it where it is. */
      factorsOuterNumerator = factor->cloneTree();
    } else if (factor->isRational()) {
      if (factor->isOne()) {
        // Special case: add a unary numeral factor if r = 1
        factorsNumerator = factor->cloneTree();
      } else {
        IntegerHandler rNum = Rational::Numerator(factor);
        if (rNum.isMinusOne()) {
          *needOpposite = !*needOpposite;
        } else if (rNum.sign() == NonStrictSign::Negative) {
          *needOpposite = !*needOpposite;
          rNum.setSign(NonStrictSign::Positive);
          factorsNumerator = rNum.pushOnTreeStack();
        } else if (!rNum.isOne()) {
          factorsNumerator = rNum.pushOnTreeStack();
        }
        IntegerHandler rDen = Rational::Denominator(factor);
        if (!rDen.isOne()) {
          // TODO_PCJ: if rDen is overflow, return -inf
          factorsDenominator = rDen.pushOnTreeStack();
        }
      }
    } else if (factor->isPow() || factor->isPowReal()) {
      Tree* pow = factor->cloneTree();
      Tree* base = pow->child(0);
      Tree* exponent = base->nextTree();
      /* Preserve m^(-2) and e^(-2) and x^(-inf)
       * Indeed x^(-inf) can be different from 1/x^inf
       * for example (-2)^inf is undef but (-2)^-inf is 0 (and not undef) */
      assert(!Infinity::IsPlusOrMinusInfinity(exponent));
      if (base->isUnit()) {
        factorsOuterNumerator = pow;
      } else if (!base->isEulerE() &&
                 MakePositiveAnyNegativeNumeralFactor(exponent)) {
        if (exponent->isOne()) {
          pow->moveTreeOverTree(base);
        }
        factorsDenominator = pow;
      } else {
        factorsNumerator = pow;
      }
    } else if (factor->isUnit()) {
      factorsOuterNumerator = factor->cloneTree();
    } else {
      factorsNumerator = factor->cloneTree();
    }
    if (factorsDenominator) {
      NAry::AddChild(denominator, factorsDenominator);
    }
    if (factorsNumerator) {
      NAry::AddChild(numerator, factorsNumerator);
    }
    if (factorsOuterNumerator) {
      NAry::AddChild(outerNumerator, factorsOuterNumerator);
    }
  }
  NAry::SquashIfPossible(numerator);
  NAry::SquashIfPossible(denominator);
  NAry::SquashIfPossible(outerNumerator);
}

void Division::GetNumeratorAndDenominator(const Tree* e, TreeRef& numerator,
                                          TreeRef& denominator) {
  bool needOpposite = false;
  // Anything expected in outerNumerator is put back in numerator.
  numerator = SharedTreeStack->pushMult(0);
  denominator = SharedTreeStack->pushMult(0);
  GetDivisionComponents(e, numerator, denominator, numerator, &needOpposite);
  if (needOpposite) {
    return;
  }
  if (!numerator->isMult()) {
    if (numerator->isOne()) {
      CloneNodeOverTree(numerator, KMult.node<0>);
    } else {
      CloneNodeAtNode(numerator, KMult.node<1>);
    }
  }
  NAry::AddChild(numerator, (-1_e)->cloneTree());
  NAry::SquashIfPossible(numerator);
}

bool Division::BeautifyIntoDivision(Tree* e) {
  TreeRef num = SharedTreeStack->pushMult(0);
  TreeRef den = SharedTreeStack->pushMult(0);
  TreeRef outNum = SharedTreeStack->pushMult(0);
  bool needOpposite = false;
  GetDivisionComponents(e, num, den, outNum, &needOpposite);
  if (den->isOne() && !needOpposite) {
    // e is already num*outNum
    num->removeTree();
    den->removeTree();
    outNum->removeTree();
    return false;
  }
  if (needOpposite) {
    e->cloneNodeBeforeNode(KOpposite);
    e = e->child(0);
  }
  if (!den->isOne()) {
    num->cloneNodeAtNode(KDiv);
    if (outNum->isOne()) {
      // return num/den
      outNum->removeTree();
    } else {
      // return num/den * outNum
      num->cloneNodeAtNode(KMult.node<2>);
    }
    e->moveTreeOverTree(num);
  } else {
    den->removeTree();
    assert(needOpposite);
    if (num->isOne()) {
      // return outNum
      num->removeTree();
      e->moveTreeOverTree(outNum);
    } else if (outNum->isOne()) {
      // return num
      outNum->removeTree();
      e->moveTreeOverTree(num);
    } else {
      // return num*outNum
      num->cloneNodeAtNode(KMult.node<2>);
      e->moveTreeOverTree(num);
    }
  }
  return true;
}

}  // namespace Poincare::Internal
