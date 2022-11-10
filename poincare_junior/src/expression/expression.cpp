#include "approximation.h"
#include "expression.h"
#include <poincare_junior/src/expression/type/number/rational.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/node_iterator.h>

namespace Poincare {

void EExpression::BasicReduction(TypeBlock * block) {
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

float EExpression::Approximate(const TypeBlock * block) {
  if (block->isRational()) {
    return Rational::Numerator(block).approximate() / Rational::Denominator(block).approximate();
  }
  switch (block->type()) {
    case BlockType::Constant:
      return Constant::Value(block);
    case BlockType::Addition:
      return Approximation::MapAndReduce(block, Addition::Reduce);
    case BlockType::Division:
      return Approximation::MapAndReduce(block, Division::Reduce);
    case BlockType::Subtraction:
      return Approximation::MapAndReduce(block, Subtraction::Reduce);
    default:
      assert(false);
  };
}

void EExpression::ProjectionReduction(TypeBlock * block, TypeBlock * (*PushProjectedEExpression)(), TypeBlock * (*PushInverse)()) {
  /* Rule a / b --> a * b^-1 (or a - b --> a + b * -1) */
  // Create a reference for future needs
  EditionReference division = EditionReference(Node(block));
  // Create empty * (or +)
  EditionReference multiplication(PushProjectedEExpression());
  // Get references to children
  assert(Node(block).numberOfChildren() == 2);
  EditionReference childrenReferences[2];
  for (NodeIterator::IndexedNode indexedNode : NodeIterator(division.node()).forwardEditableChildren()) {
    childrenReferences[indexedNode.m_index] = EditionReference(indexedNode.m_node);
  }
  // Move first child
  childrenReferences[0].insertTreeAfterNode(multiplication);
  // Create empty ^ (or *)
  EditionReference power(PushInverse());
  // Move second child
  childrenReferences[1].insertTreeAfterNode(power);
  // Complete: a * b^-1 (or a + b * -1)
  Node::Push<IntegerShort>(-1);
  // Replace single-noded division (or subtraction) by the new multiplication (or addition)
  division.replaceNodeByTree(multiplication);
}

}
