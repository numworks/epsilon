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

// Get numerator, denominator, opposite (if needed), complex I (if needed)
void Division::GetDivisionComponents(const Tree* e, TreeRef& numerator,
                                     TreeRef& denominator, bool* needOpposite,
                                     bool* needI) {
  assert(needOpposite && needI);
  numerator = SharedTreeStack->pushMult(0);
  denominator = SharedTreeStack->pushMult(0);
  // TODO replace NumberOfFactors and Factor with an iterable
  const int numberOfFactors = NumberOfFactors(e);
  for (int i = 0; i < numberOfFactors; i++) {
    const Tree* factor = Factor(e, i);
    TreeRef factorsNumerator;
    TreeRef factorsDenominator;
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
      if (!base->isUnit() && !base->isEulerE() &&
          MakePositiveAnyNegativeNumeralFactor(exponent)) {
        if (exponent->isOne()) {
          pow->moveTreeOverTree(base);
        }
        factorsDenominator = pow;
      } else {
        factorsNumerator = pow;
      }
    } else {
      factorsNumerator = factor->cloneTree();
    }
    if (factorsDenominator) {
      NAry::AddChild(denominator, factorsDenominator);
    }
    if (factorsNumerator) {
      NAry::AddChild(numerator, factorsNumerator);
    }
  }
  NAry::SquashIfPossible(numerator);
  NAry::SquashIfPossible(denominator);
}

void Division::GetNumeratorAndDenominator(const Tree* e, TreeRef& numerator,
                                          TreeRef& denominator) {
  bool needOpposite = false;
  bool needI = false;
  GetDivisionComponents(e, numerator, denominator, &needOpposite, &needI);
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
  TreeRef num;
  TreeRef den;
  bool needOpposite = false;
  bool needI = false;
  GetDivisionComponents(e, num, den, &needOpposite, &needI);
  if (den->isOne() && !needOpposite) {
    // no need to apply needI if den->isOne
    num->removeTree();
    den->removeTree();
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
  e->moveTreeOverTree(num);
  return true;
}

}  // namespace Poincare::Internal
