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

// Get numerator, denominator, out, opposite (if needed), complex I (if needed)
void Division::GetDivisionComponents(const Tree* e, TreeRef& numerator,
                                     TreeRef& denominator, TreeRef& out,
                                     bool* needOpposite, bool* needI) {
  /* TODO: instead of needI, add i in out. Handle this in
   * GetNumeratorAndDenominator too, by passing num in both num and out maybe ?
   */
  assert(needOpposite && needI);
  assert(!numerator.isUninitialized() && numerator->isMult());
  assert(!denominator.isUninitialized() && denominator->isMult());
  assert(!out.isUninitialized() && out->isMult());
  // TODO replace NumberOfFactors and Factor with an iterable
  const int numberOfFactors = NumberOfFactors(e);
  for (int i = 0; i < numberOfFactors; i++) {
    const Tree* factor = Factor(e, i);
    TreeRef factorsNumerator;
    TreeRef factorsDenominator;
    TreeRef factorsOut;
    if (factor->isComplexI() && i == numberOfFactors - 1 &&
        denominator->numberOfChildren() > 0) {
      /* Move the final i out of the multiplication e.g. 2^(-1)×i → (1/2)×i. If
       * i is not in the last position, it is either intentional or a bug in the
       * order, so leave it where it is. */
      assert(*needI == false);
      *needI = true;
      continue;
    }
    if (factor->isRational()) {
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
        factorsOut = pow;
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
      factorsOut = factor->cloneTree();
    } else {
      factorsNumerator = factor->cloneTree();
    }
    if (factorsDenominator) {
      NAry::AddChild(denominator, factorsDenominator);
    }
    if (factorsNumerator) {
      NAry::AddChild(numerator, factorsNumerator);
    }
    if (factorsOut) {
      NAry::AddChild(out, factorsOut);
    }
  }
  NAry::SquashIfPossible(numerator);
  NAry::SquashIfPossible(denominator);
  NAry::SquashIfPossible(out);
}

void Division::GetNumeratorAndDenominator(const Tree* e, TreeRef& numerator,
                                          TreeRef& denominator) {
  bool needOpposite = false;
  bool needI = false;
  // Anything expected in out is put back in numerator.
  numerator = SharedTreeStack->pushMult(0);
  denominator = SharedTreeStack->pushMult(0);
  GetDivisionComponents(e, numerator, denominator, numerator, &needOpposite,
                        &needI);
  if (!needOpposite && !needI) {
    return;
  }
  if (!numerator->isMult()) {
    if (numerator->isOne()) {
      CloneNodeOverNode(numerator, KMult.node<0>);
    } else {
      CloneNodeAtNode(numerator, KMult.node<1>);
    }
  }
  if (needOpposite) {
    NAry::AddChild(numerator, (-1_e)->cloneTree());
  }
  if (needI) {
    NAry::AddChild(numerator, (i_e)->cloneTree());
  }
  NAry::SquashIfPossible(numerator);
}

bool Division::BeautifyIntoDivision(Tree* e) {
  TreeRef num = SharedTreeStack->pushMult(0);
  TreeRef den = SharedTreeStack->pushMult(0);
  TreeRef out = SharedTreeStack->pushMult(0);
  bool needOpposite = false;
  bool needI = false;
  GetDivisionComponents(e, num, den, out, &needOpposite, &needI);
  if (den->isOne() && !needOpposite) {
    // no need to apply needI if den->isOne
    num->removeTree();
    den->removeTree();
    out->removeTree();
    return false;
  }
  if (needOpposite) {
    e->cloneNodeBeforeNode(KOpposite);
    e = e->child(0);
  }
  if (needI) {
    e->cloneNodeBeforeNode(KMult.node<2>);
    e = e->child(0);
    // TODO: create method cloneTreeAfterTree
    e->nextTree()->cloneTreeBeforeNode(i_e);
  }
  if (!den->isOne()) {
    num->cloneNodeAtNode(KDiv);
  } else {
    den->removeTree();
  }
  if (!out->isOne()) {
    e->cloneNodeBeforeNode(KMult.node<2>);
    e = e->child(0);
    // TODO: create method cloneTreeAfterTree
    e->nextTree()->moveTreeBeforeNode(out);
  } else {
    out->removeTree();
  }
  e->moveTreeOverTree(num);
  return true;
}

}  // namespace Poincare::Internal
