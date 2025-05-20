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

Division::DivisionComponentsResult Division::GetDivisionComponents(
    const Tree* e) {
  TreeRef numerator = SharedTreeStack->pushMult(0);
  TreeRef denominator = SharedTreeStack->pushMult(0);
  TreeRef outerNumerator = SharedTreeStack->pushMult(0);
  bool needOpposite = false;

  // TODO replace NumberOfFactors and Factor with an iterable
  const int numberOfFactors = NumberOfFactors(e);
  for (int i = 0; i < numberOfFactors; i++) {
    const Tree* factor = Factor(e, i);
    TreeRef factorsNumerator;
    TreeRef factorsDenominator;
    TreeRef factorsOuterNumerator;
    if (factor->treeIsIdenticalTo(-1_fe) || factor->treeIsIdenticalTo(-1_de)) {
      factor = -1_e;
    }
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
          needOpposite = !needOpposite;
        } else if (rNum.sign() == NonStrictSign::Negative) {
          needOpposite = !needOpposite;
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
      if (base->isUnitOrPhysicalConstant()) {
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
    } else if (factor->isUnitOrPhysicalConstant()) {
      factorsOuterNumerator = factor->cloneTree();
    } else {
      factorsNumerator = factor->cloneTree();
    }
    if (factorsDenominator) {
      NAry::AddOrMergeChild(denominator, factorsDenominator);
    }
    if (factorsNumerator) {
      NAry::AddOrMergeChild(numerator, factorsNumerator);
    }
    if (factorsOuterNumerator) {
      NAry::AddOrMergeChild(outerNumerator, factorsOuterNumerator);
    }
  }
  NAry::SquashIfPossible(numerator);
  NAry::SquashIfPossible(denominator);
  NAry::SquashIfPossible(outerNumerator);
  return {numerator, denominator, outerNumerator, needOpposite};
}

Tree* Division::PushDenominatorAndComputeDegreeOfNumerator(
    const Tree* e, const char* symbol, int* numeratorDegree) {
  DivisionComponentsResult result = GetDivisionComponents(e);
  /* Numerator and outerNumerator are "merged" by summing degree.
   * needOpposite won't alter degree, so it is ignored.
   * Otherwise, numerator should be multiplied by -1 and flattened, or replaced
   * with -1 if it was 1. */
  int firstNumeratorDegree = Degree::Get(result.numerator, symbol);
  int outerNumeratorDegree = Degree::Get(result.outerNumerator, symbol);
  *numeratorDegree = (firstNumeratorDegree == Degree::k_unknown ||
                      outerNumeratorDegree == Degree::k_unknown)
                         ? Degree::k_unknown
                         : firstNumeratorDegree + outerNumeratorDegree;
  result.numerator->removeTree();
  result.outerNumerator->removeTree();
  return result.denominator;
}

bool Division::BeautifyIntoDivision(Tree* e) {
  DivisionComponentsResult result = GetDivisionComponents(e);
  assert(result.numerator->nextTree() == result.denominator &&
         result.denominator->nextTree() == result.outerNumerator);
  if (result.denominator->isOne() && !result.needOpposite) {
    // e is already num*outerNumerator
    result.numerator->removeTree();
    result.denominator->removeTree();
    result.outerNumerator->removeTree();
    return false;
  }
  if (result.needOpposite) {
    e->cloneNodeAtNode(KOpposite);
    e = e->child(0);
  }
  if (!result.denominator->isOne()) {
    result.numerator->cloneNodeAtNode(KDiv);
    if (result.outerNumerator->isOne()) {
      // return numerator/denominator
      result.outerNumerator->removeTree();
    } else {
      // return numerator/denominator * outerNumerator
      result.numerator->cloneNodeAtNode(KMult.node<2>);
      NAry::Flatten(result.numerator);
    }
    e->moveTreeOverTree(result.numerator);
  } else {
    result.denominator->removeTree();
    assert(result.needOpposite);
    bool outNumStartsWithUnit =
        result.outerNumerator->isUnitOrPhysicalConstant() ||
        (result.outerNumerator->isMult() &&
         result.outerNumerator->child(0)->isUnitOrPhysicalConstant());
    if (result.numerator->isOne() && !outNumStartsWithUnit) {
      // return outerNumerator
      result.numerator->removeTree();
      e->moveTreeOverTree(result.outerNumerator);
    } else if (result.outerNumerator->isOne()) {
      // return numerator
      result.outerNumerator->removeTree();
      e->moveTreeOverTree(result.numerator);
    } else {
      // return numerator*outerNumerator
      result.numerator->cloneNodeAtNode(KMult.node<2>);
      NAry::Flatten(result.numerator);
      e->moveTreeOverTree(result.numerator);
    }
  }
  return true;
}

}  // namespace Poincare::Internal
