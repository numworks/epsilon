#include "algebraic.h"

#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/number.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/n_ary.h>

#include "rational.h"
#include "simplification.h"

namespace PoincareJ {

// TODO: tests

EditionReference Algebraic::Rationalize(EditionReference expression) {
  if (expression->block()->isRational()) {
    EditionReference fraction(
        SharedEditionPool->push<BlockType::Multiplication>(2));
    Rational::Numerator(expression).pushOnEditionPool();
    SharedEditionPool->push<BlockType::Power>();
    Rational::Denominator(expression).pushOnEditionPool();
    SharedEditionPool->push<BlockType::MinusOne>();
    expression->moveTreeOverTree(fraction);
    return fraction;
  }
  BlockType type = expression->type();
  if (type == BlockType::Power) {
    Rationalize(expression->childAtIndex(0));
    return expression;  // TODO return basicReduction
  }
  if (type == BlockType::Multiplication) {
    for (std::pair<EditionReference, int> indexedNode :
         NodeIterator::Children<Editable>(expression)) {
      Rationalize(std::get<EditionReference>(indexedNode));
    }
    return expression;  // TODO return basicReduction
  }
  if (type == BlockType::Addition) {
    // Factorize on common denominator a/b + c/d
    return RationalizeAddition(expression);
  }
  return expression;
}

EditionReference Algebraic::RationalizeAddition(EditionReference expression) {
  assert(expression->type() == BlockType::Addition);
  EditionReference commonDenominator = EditionReference(KMult());
  // Step 1: We want to compute the common denominator, b*d
  for (std::pair<EditionReference, int> indexedNode :
       NodeIterator::Children<Editable>(expression)) {
    EditionReference child = std::get<EditionReference>(indexedNode);
    child = Rationalize(child);
    EditionReference denominator = Denominator(SharedEditionPool->clone(child));
    NAry::AddChild(commonDenominator, denominator);  // FIXME: do we need LCM?
  }
  // basic reduction commonDenominator
  assert(!Number::IsZero(commonDenominator));
  if (Number::IsOne(commonDenominator)) {
    return expression;
  }
  /* Step 2: Turn the expression into the numerator. We start with this being
   * a/b+c/d and we want to create numerator = a/b*b*d + c/d*b*d = a*d + c*b */
  for (std::pair<EditionReference, int> indexedNode :
       NodeIterator::Children<Editable>(expression)) {
    EditionReference child = std::get<EditionReference>(indexedNode);
    // Create Mult(child, commonDenominator) = a*b * b*d
    EditionReference multiplication(
        SharedEditionPool->push<BlockType::Multiplication>(1));
    child->moveNodeBeforeNode(multiplication);
    child->nextTree()->moveTreeBeforeNode(
        SharedEditionPool->clone(commonDenominator));
    // TODO basicReduction of child
  }
  // Create Mult(expression, Pow)
  EditionReference fraction(
      SharedEditionPool->push<BlockType::Multiplication>(2));
  fraction->moveTreeAfterNode(expression);
  // Create Pow(commonDenominator, -1)
  EditionReference power(SharedEditionPool->push<BlockType::Power>());
  power->moveTreeAfterNode(commonDenominator);
  commonDenominator->nextTree()->cloneTreeBeforeNode(-1_e);
  // TODO basicReduction of power
  // TODO basicReduction of fraction
  return fraction;
}

EditionReference Algebraic::NormalFormator(EditionReference expression,
                                           bool numerator) {
  if (expression->block()->isRational()) {
    IntegerHandler ator = numerator ? Rational::Numerator(expression)
                                    : Rational::Denominator(expression);
    EditionReference result = ator.pushOnEditionPool();
    expression->moveNodeOverNode(result);
    return result;
  }
  BlockType type = expression->type();
  if (type == BlockType::Power) {
    EditionReference exponent = expression->childAtIndex(1);
    bool negativeRationalExponent =
        exponent->block()->isRational() &&
        Rational::StrictSign(exponent) == StrictSign::Negative;
    if (!numerator && negativeRationalExponent) {
      Rational::SetSign(exponent, NonStrictSign::Positive);
    }
    if (numerator == negativeRationalExponent) {
      return expression->cloneTreeOverTree(1_e);
    }
    Simplification::DeepSystematicReduce(expression);
    return expression;
  }
  if (type == BlockType::Multiplication) {
    for (std::pair<EditionReference, int> indexedNode :
         NodeIterator::Children<Editable>(expression)) {
      EditionReference child = std::get<EditionReference>(indexedNode);
      child = NormalFormator(child, numerator);
    }
    // TODO basicReduction of expression
  }
  return expression;
}

}  // namespace PoincareJ
