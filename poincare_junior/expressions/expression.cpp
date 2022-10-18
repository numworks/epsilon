#include "approximation.h"
#include "expression.h"
#include "numbers/rational.h"
#include "../edition_reference.h"
#include "../node_iterator.h"

namespace Poincare {

void Expression::BasicReduction(TypeBlock * block) {
  // TODO: Macro to automatically generate switch
  switch (block->type()) {
    case BlockType::Addition:
      return Addition::BasicReduction(block);
    case BlockType::Division:
      return Division::BasicReduction(block);
    case BlockType::Power:
      return Power::BasicReduction(block);
    case BlockType::Subtraction:
      return Subtraction::BasicReduction(block);
    default:
      return;
  }
}

float Expression::Approximate(const TypeBlock * block) {
  if (block->isRational()) {
    return Rational::Numerator(block).approximate() / Rational::Denominator(block).approximate();
  }
  switch (block->type()) {
    case BlockType::Constant:
      return Constant::Value(block);
    case BlockType::Addition:
      Approximation::MapAndReduce(block, Addition::Reduce);
    case BlockType::Division:
      Approximation::MapAndReduce(block, Division::Reduce);
    case BlockType::Subtraction:
      Approximation::MapAndReduce(block, Subtraction::Reduce);
    default:
      assert(false);
  };
}

void Expression::ProjectionReduction(TypeBlock * block, TypeBlock * (*PushProjectedExpression)(), TypeBlock * (*PushInverse)()) {
  /* Rule a / b --> a * b^-1 (or a - b --> a + b * -1) */
  // Create a reference for future needs
  EditionReference division = EditionReference(Node(block));
  // Create empty * (or +)
  EditionReference multiplication(PushProjectedExpression());
  // Get references to children
  assert(Node(block).numberOfChildren() == 2);
  EditionReference childrenReferences[2];
  for (NodeIterator::IndexedNode indexedNode : NodeIterator(division.node()).forwardEditableChildren()) {
    childrenReferences[indexedNode.m_index] = EditionReference(indexedNode.m_node);
  }
  // Move first child
  childrenReferences[0].insertTreeAfter(multiplication);
  // Create empty ^ (or *)
  EditionReference power(PushInverse());
  // Move second child
  childrenReferences[1].insertTreeAfter(power);
  // Complete: a * b^-1 (or a + b * -1)
  Node::Push<IntegerShort>(-1);
  // Remove / node (or -)
  division.removeNode();
}

}
