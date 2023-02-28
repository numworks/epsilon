#include "algebraic.h"
#include <poincare_junior/src/n_ary.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/expression/constructor.h>
#include "rational.h"
#include "simplification.h"

namespace PoincareJ {

//TODO: tests

EditionReference Algebraic::Rationalize(EditionReference expression) {
  if (expression.block()->isRational()) {
    EditionReference fraction = EditionReference::Push<BlockType::Multiplication>(2);
    Rational::Numerator(expression).pushOnEditionPool();
    EditionReference::Push<BlockType::Power>();
    Rational::Denominator(expression).pushOnEditionPool();
    EditionReference::Push<BlockType::MinusOne>();
    expression.replaceTreeByTree(fraction);
    return fraction;
  }
  BlockType type = expression.type();
  if (type == BlockType::Power) {
    Rationalize(expression.childAtIndex(0));
    return expression; // TODO return basicReduction
  }
  if (type == BlockType::Multiplication) {
    for (std::pair<EditionReference, int> indexedNode : NodeIterator::Children<Forward, Editable>(expression)) {
      Rationalize(std::get<EditionReference>(indexedNode));
    }
    return expression; // TODO return basicReduction
  }
  if (type == BlockType::Addition) {
    // Factorize on common denominator a/b + c/d
    return RationalizeAddition(expression);
  }
  return expression;
}

EditionReference Algebraic::RationalizeAddition(EditionReference expression) {
  assert(expression.type() == BlockType::Addition);
  EditionReference commonDenominator = EditionReference(Mult());
  // Step 1: We want to compute the common denominator, b*d
  for (std::pair<EditionReference, int> indexedNode : NodeIterator::Children<Forward, Editable>(expression)) {
    EditionReference child = std::get<EditionReference>(indexedNode);
    child = Rationalize(child);
    EditionReference denominator = Denominator(EditionReference::Clone(child));
    NAry::AddChild(commonDenominator, denominator); // FIXME: do we need LCM?
  }
  // basic reduction commonDenominator
  assert(commonDenominator.type() != BlockType::Zero);
  if (commonDenominator.type() == BlockType::One) {
    return expression;
  }
  /* Step 2: Turn the expression into the numerator. We start with this being a/b+c/d and we
   * want to create numerator = a/b*b*d + c/d*b*d = a*d + c*b */
  for (std::pair<EditionReference, int> indexedNode : NodeIterator::Children<Forward, Editable>(expression)) {
    EditionReference child = std::get<EditionReference>(indexedNode);
    // Create Mult(child, commonDenominator) = a*b * b*d
    EditionReference multiplication = EditionReference::Push<BlockType::Multiplication>(1);
    child.insertNodeBeforeNode(multiplication);
    child.nextTree().insertTreeBeforeNode(EditionReference::Clone(commonDenominator));
    // TODO basicReduction of child
  }
  // Create Mult(expression, Pow)
  EditionReference fraction = EditionReference::Push<BlockType::Multiplication>(2);
  fraction.insertTreeAfterNode(expression);
  // Create Pow(commonDenominator, -1)
  EditionReference power = EditionReference::Push<BlockType::Power>();
  power.insertTreeAfterNode(commonDenominator);
  commonDenominator.nextTree().insertTreeBeforeNode(-1_e);
  // TODO basicReduction of power
  // TODO basicReduction of fraction
  return fraction;
}

EditionReference Algebraic::NormalFormator(EditionReference expression, bool numerator) {
  if (expression.block()->isRational()) {
    IntegerHandler ator = numerator ? Rational::Numerator(expression) : Rational::Denominator(expression);
    expression.replaceNodeByNode(ator.pushOnEditionPool());
    return expression;
  }
  BlockType type = expression.type();
  if (type == BlockType::Power) {
    EditionReference exponent = expression.childAtIndex(1);
    bool negativeRationalExponent = exponent.block()->isRational() && Rational::RationalStrictSign(exponent) == StrictSign::Negative;
    if (!numerator && negativeRationalExponent) {
      Rational::SetSign(exponent, NonStrictSign::Positive);
    }
    EditionReference result = numerator == negativeRationalExponent ? EditionReference(1_e) : expression; // TODO: BasicReduction(expression)
    expression.replaceTreeByTree(result);
    return result;
  }
  if (type == BlockType::Multiplication) {
    for (std::pair<EditionReference, int> indexedNode : NodeIterator::Children<Forward, Editable>(expression)) {
      EditionReference child = std::get<EditionReference>(indexedNode);
      child = NormalFormator(child, numerator);
    }
    // TODO basicReduction of expression
  }
  return expression;
}

}
