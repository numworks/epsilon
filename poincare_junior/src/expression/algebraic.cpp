#include "algebraic.h"

#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/number.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/n_ary.h>

#include "rational.h"
#include "simplification.h"

namespace PoincareJ {

// TODO: tests

TreeRef Algebraic::Rationalize(TreeRef expression) {
  if (Number::IsStrictRational(expression)) {
    TreeRef fraction(SharedEditionPool->push<Type::Multiplication>(2));
    Rational::Numerator(expression).pushOnEditionPool();
    SharedEditionPool->push(Type::Power);
    Rational::Denominator(expression).pushOnEditionPool();
    SharedEditionPool->push(Type::MinusOne);
    expression->moveTreeOverTree(fraction);
    return fraction;
  }
  if (expression->isPower()) {
    Rationalize(expression->child(0));
    return expression;  // TODO return basicReduction
  }
  if (expression->isMultiplication()) {
    for (std::pair<TreeRef, int> indexedNode :
         NodeIterator::Children<Editable>(expression)) {
      Rationalize(std::get<TreeRef>(indexedNode));
    }
    return expression;  // TODO return basicReduction
  }
  if (expression->isAddition()) {
    // Factorize on common denominator a/b + c/d
    return RationalizeAddition(expression);
  }
  return expression;
}

TreeRef Algebraic::RationalizeAddition(TreeRef expression) {
  assert(expression->isAddition());
  TreeRef commonDenominator(KMult());
  // Step 1: We want to compute the common denominator, b*d
  for (std::pair<TreeRef, int> indexedNode :
       NodeIterator::Children<Editable>(expression)) {
    TreeRef child = std::get<TreeRef>(indexedNode);
    child = Rationalize(child);
    TreeRef denominator = Denominator(SharedEditionPool->clone(child));
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
    TreeRef multiplication(SharedEditionPool->push<Type::Multiplication>(1));
    child->moveNodeBeforeNode(multiplication);
    child->nextTree()->moveTreeBeforeNode(
        SharedEditionPool->clone(commonDenominator));
    // TODO basicReduction of child
  }
  // Create Mult(expression, Pow)
  TreeRef fraction(SharedEditionPool->push<Type::Multiplication>(2));
  fraction->moveTreeAfterNode(expression);
  // Create Pow(commonDenominator, -1)
  TreeRef power(SharedEditionPool->push(Type::Power));
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
    TreeRef result = ator.pushOnEditionPool();
    expression->moveNodeOverNode(result);
    return result;
  }
  if (expression->isPower()) {
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
  if (expression->isMultiplication()) {
    for (std::pair<TreeRef, int> indexedNode :
         NodeIterator::Children<Editable>(expression)) {
      TreeRef child = std::get<TreeRef>(indexedNode);
      child = NormalFormator(child, numerator);
    }
    // TODO basicReduction of expression
  }
  return expression;
}

}  // namespace PoincareJ
