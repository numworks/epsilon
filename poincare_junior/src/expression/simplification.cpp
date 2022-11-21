#include "simplification.h"
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/node_iterator.h>

namespace Poincare {

void Simplification::BasicReduction(TypeBlock * block) {
  // TODO: Macro to automatically generate switch
  switch (block->type()) {
    case BlockType::Division:
      return DivisionReduction(block);
    case BlockType::Subtraction:
      return SubtractionReduction(block);
    default:
      return;
  }
}

void Simplification::DivisionReduction(TypeBlock * block) {
  assert(block->type() == BlockType::Division);
  ProjectionReduction(block,
      []() { return Node::Push<BlockType::Multiplication>(2).block(); },
      []() { return Node::Push<BlockType::Power>().block(); }
    );
}

void Simplification::SubtractionReduction(TypeBlock * block) {
  assert(block->type() == BlockType::Subtraction);
  ProjectionReduction(block,
      []() { return Node::Push<BlockType::Addition>(2).block(); },
      []() { return Node::Push<BlockType::Multiplication>(2).block(); }
    );
}

TypeBlock * Simplification::DistributeMultiplicationOverAddition(TypeBlock * block) {
  EditionReference mult = EditionReference(Node(block));
  for (NodeIterator::IndexedNode indexedNode : NodeIterator(Node(block)).forwardEditableChildren()) {
    if (indexedNode.m_node.block()->type() == BlockType::Addition) {
      // Create new addition that will be filled in the following loop
      EditionReference add = EditionReference(Node(Node::Push<BlockType::Addition>(indexedNode.m_node.numberOfChildren())));
      for (NodeIterator::IndexedNode indexedAdditionChild : NodeIterator(indexedNode.m_node).forwardEditableChildren()) {
        // Copy a multiplication
        EditionReference multCopy = mult.clone();
        // Find the addition to be replaced
        EditionReference additionCopy = EditionReference(multCopy.node().childAtIndex(indexedNode.m_index));
        // Find addition child to replace with
        EditionReference additionChildCopy = EditionReference(additionCopy.childAtIndex(indexedAdditionChild.m_index));
        // Replace addition per its child
        additionCopy.replaceTreeByTree(additionChildCopy);
        assert(multCopy.block()->type() == BlockType::Multiplication);
        DistributeMultiplicationOverAddition(multCopy.block());
      }
      mult.replaceTreeByTree(add);
      return add.block();
    }
  }
  return block;
}

TypeBlock * Simplification::Flatten(TypeBlock * block) {
  uint8_t numberOfChildren = 0;
  for (NodeIterator::IndexedNode indexedNode : NodeIterator(Node(block)).forwardEditableChildren()) {
    if (block->type() == indexedNode.m_node.block()->type()) {
      EditionReference nAry = EditionReference(Node(Flatten(indexedNode.m_node.block())));
      numberOfChildren += nAry.node().numberOfChildren();
      nAry.removeNode();
    } else {
      numberOfChildren++;
    }
  }
  Block * numberOfChildrenBlock = block->next();
  *numberOfChildrenBlock = numberOfChildren;
  return block;
}

void Simplification::ProjectionReduction(TypeBlock * block, TypeBlock * (*PushProjectedEExpression)(), TypeBlock * (*PushInverse)()) {
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
  Node::Push<BlockType::IntegerShort>(-1);
  // Replace single-noded division (or subtraction) by the new multiplication (or addition)
  division.replaceNodeByTree(multiplication);
}

}
