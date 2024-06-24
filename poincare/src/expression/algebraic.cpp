#include "algebraic.h"

#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/node_iterator.h>

#include "k_tree.h"
#include "number.h"
#include "rational.h"
#include "sign.h"
#include "simplification.h"

namespace Poincare::Internal {

// TODO: tests

TreeRef Algebraic::Rationalize(TreeRef expression) {
  if (Number::IsStrictRational(expression)) {
    TreeRef fraction(SharedTreeStack->pushMult(2));
    Rational::Numerator(expression).pushOnTreeStack();
    SharedTreeStack->pushPow();
    Rational::Denominator(expression).pushOnTreeStack();
    SharedTreeStack->pushMinusOne();
    expression->moveTreeOverTree(fraction);
    return fraction;
  }
  if (expression->isPow()) {
    Rationalize(expression->child(0));
    return expression;  // TODO return basicReduction
  }
  if (expression->isMult()) {
    for (std::pair<TreeRef, int> indexedNode :
         NodeIterator::Children<Editable>(expression)) {
      Rationalize(std::get<TreeRef>(indexedNode));
    }
    return expression;  // TODO return basicReduction
  }
  if (expression->isAdd()) {
    // Factorize on common denominator a/b + c/d
    return RationalizeAddition(expression);
  }
  return expression;
}

TreeRef Algebraic::RationalizeAddition(TreeRef expression) {
  assert(expression->isAdd());
  TreeRef commonDenominator(KMult());
  // Step 1: We want to compute the common denominator, b*d
  for (std::pair<TreeRef, int> indexedNode :
       NodeIterator::Children<Editable>(expression)) {
    TreeRef child = std::get<TreeRef>(indexedNode);
    child = Rationalize(child);
    TreeRef denominator = Denominator(SharedTreeStack->clone(child));
    NAry::AddChild(commonDenominator, denominator);  // FIXME: do we need LCM?
  }
  // basic reduction commonDenominator
  assert(!commonDenominator->isZero());
  if (commonDenominator->isOne()) {
    return expression;
  }
  /* Step 2: Turn the expression into the numerator. We start with this being
   * a/b+c/d and we want to create numerator = a/b*b*d + c/d*b*d = a*d + c*b */
  for (std::pair<TreeRef, int> indexedNode :
       NodeIterator::Children<Editable>(expression)) {
    TreeRef child = std::get<TreeRef>(indexedNode);
    // Create Mult(child, commonDenominator) = a*b * b*d
    TreeRef multiplication(SharedTreeStack->pushMult(1));
    child->moveNodeBeforeNode(multiplication);
    child->nextTree()->moveTreeBeforeNode(
        SharedTreeStack->clone(commonDenominator));
    // TODO basicReduction of child
  }
  // Create Mult(expression, Pow)
  TreeRef fraction(SharedTreeStack->pushMult(2));
  fraction->moveTreeAfterNode(expression);
  // Create Pow(commonDenominator, -1)
  TreeRef power(SharedTreeStack->pushPow());
  power->moveTreeAfterNode(commonDenominator);
  commonDenominator->nextTree()->cloneTreeBeforeNode(-1_e);
  // TODO basicReduction of power
  // TODO basicReduction of fraction
  return fraction;
}

TreeRef Algebraic::NormalFormator(TreeRef expression, bool numerator) {
  if (expression->isRational()) {
    IntegerHandler ator = numerator ? Rational::Numerator(expression)
                                    : Rational::Denominator(expression);
    TreeRef result = ator.pushOnTreeStack();
    expression->moveNodeOverNode(result);
    return result;
  }
  if (expression->isPow()) {
    TreeRef exponent = expression->child(1);
    bool negativeRationalExponent =
        exponent->isRational() && Rational::Sign(exponent).isStrictlyNegative();
    if (!numerator && negativeRationalExponent) {
      Rational::SetSign(exponent, NonStrictSign::Positive);
    }
    if (numerator == negativeRationalExponent) {
      return expression->cloneTreeOverTree(1_e);
    }
    Simplification::DeepSystematicReduce(expression);
    return expression;
  }
  if (expression->isMult()) {
    for (std::pair<TreeRef, int> indexedNode :
         NodeIterator::Children<Editable>(expression)) {
      TreeRef child = std::get<TreeRef>(indexedNode);
      child = NormalFormator(child, numerator);
    }
    // TODO basicReduction of expression
  }
  return expression;
}

}  // namespace Poincare::Internal
