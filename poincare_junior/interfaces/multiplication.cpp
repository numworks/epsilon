#include "multiplication.h"
#include "../edition_reference.h"
#include "../node.h"

namespace Poincare {

float MultiplicationExpressionInterface::approximate(const TypeBlock * block) const {
  float res = 1.0f;
  for (const Iterator::IndexedNode indexedNode : Iterator(Node(block)).forwardConstChildren()) {
    res *= indexedNode.m_node.expressionInterface()->approximate(indexedNode.m_node.block());
  }
  return res;
}

TypeBlock * MultiplicationExpressionInterface::DistributeOverAddition(TypeBlock * block) {
  EditionReference mult = EditionReference(Node(block));
  for (Iterator::IndexedNode indexedNode : Iterator(Node(block)).forwardEditableChildren()) {
    if (indexedNode.m_node.block()->type() == BlockType::Addition) {
      // Create new addition that will be filled in the following loop
      EditionReference add = EditionReference(Node(AdditionInterface::PushNode(indexedNode.m_node.numberOfChildren())));
      for (Iterator::IndexedNode indexedAdditionChild : Iterator(indexedNode.m_node).forwardEditableChildren()) {
        // Copy a multiplication
        EditionReference multCopy = mult.clone();
        // Find the addition to be replaced
        EditionReference additionCopy = EditionReference(multCopy.node().childAtIndex(indexedNode.m_index));
        // Find addition child to replace with
        EditionReference additionChildCopy = EditionReference(additionCopy.childAtIndex(indexedAdditionChild.m_index));
        // Replace addition per its child
        additionCopy.replaceTreeBy(additionChildCopy);
        assert(multCopy.block()->type() == BlockType::Multiplication);
        MultiplicationExpressionInterface::DistributeOverAddition(multCopy.block());
      }
      mult.replaceTreeBy(add);
      return add.block();
    }
  }
  return block;
}

}
