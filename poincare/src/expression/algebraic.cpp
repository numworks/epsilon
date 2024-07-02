#include "algebraic.h"

#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/node_iterator.h>

#include "k_tree.h"
#include "number.h"
#include "rational.h"
#include "sign.h"
#include "systematic_reduction.h"

namespace Poincare::Internal {

// TODO: tests

TreeRef Algebraic::Rationalize(TreeRef e) {
  if (Number::IsStrictRational(e)) {
    TreeRef fraction(SharedTreeStack->pushMult(2));
    Rational::Numerator(e).pushOnTreeStack();
    SharedTreeStack->pushPow();
    Rational::Denominator(e).pushOnTreeStack();
    SharedTreeStack->pushMinusOne();
    e->moveTreeOverTree(fraction);
    return fraction;
  }
  if (e->isPow()) {
    Rationalize(e->child(0));
    return e;  // TODO return basicReduction
  }
  if (e->isMult()) {
    for (Tree* child : e->children()) {
      Rationalize(child);
    }
    return e;  // TODO return basicReduction
  }
  if (e->isAdd()) {
    // Factorize on common denominator a/b + c/d
    return RationalizeAddition(e);
  }
  return e;
}

TreeRef Algebraic::RationalizeAddition(TreeRef e) {
  assert(e->isAdd());
  TreeRef commonDenominator(KMult());
  // Step 1: We want to compute the common denominator, b*d
  for (Tree* child : e->children()) {
    child = Rationalize(child);
    TreeRef denominator = Denominator(SharedTreeStack->clone(child));
    NAry::AddChild(commonDenominator, denominator);  // FIXME: do we need LCM?
  }
  // basic reduction commonDenominator
  assert(!commonDenominator->isZero());
  if (commonDenominator->isOne()) {
    return e;
  }
  /* Step 2: Turn the expression into the numerator. We start with this being
   * a/b+c/d and we want to create numerator = a/b*b*d + c/d*b*d = a*d + c*b */
  for (Tree* child : e->children()) {
    // Create Mult(child, commonDenominator) = a*b * b*d
    TreeRef multiplication(SharedTreeStack->pushMult(1));
    child->moveNodeBeforeNode(multiplication);
    child->nextTree()->moveTreeBeforeNode(
        SharedTreeStack->clone(commonDenominator));
    // TODO basicReduction of child
  }
  // Create Mult(e, Pow)
  TreeRef fraction(SharedTreeStack->pushMult(2));
  fraction->moveTreeAfterNode(e);
  // Create Pow(commonDenominator, -1)
  TreeRef power(SharedTreeStack->pushPow());
  power->moveTreeAfterNode(commonDenominator);
  commonDenominator->nextTree()->cloneTreeBeforeNode(-1_e);
  // TODO basicReduction of power
  // TODO basicReduction of fraction
  return fraction;
}

TreeRef Algebraic::NormalFormator(TreeRef e, bool numerator) {
  if (e->isRational()) {
    IntegerHandler ator =
        numerator ? Rational::Numerator(e) : Rational::Denominator(e);
    TreeRef result = ator.pushOnTreeStack();
    e->moveNodeOverNode(result);
    return result;
  }
  if (e->isPow()) {
    TreeRef exponent = e->child(1);
    bool negativeRationalExponent =
        exponent->isRational() && Rational::Sign(exponent).isStrictlyNegative();
    if (!numerator && negativeRationalExponent) {
      Rational::SetSign(exponent, NonStrictSign::Positive);
    }
    if (numerator == negativeRationalExponent) {
      return e->cloneTreeOverTree(1_e);
    }
    SystematicReduction::DeepReduce(e);
    return e;
  }
  if (e->isMult()) {
    for (Tree* child : e->children()) {
      NormalFormator(child, numerator);
    }
    // TODO basicReduction of e
  }
  return e;
}

}  // namespace Poincare::Internal
