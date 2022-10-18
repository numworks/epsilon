#include "multiplication.h"
#include "../node_iterator.h"

namespace Poincare {

TypeBlock * Multiplication::DistributeOverAddition(TypeBlock * block) {
  EditionReference mult = EditionReference(Node(block));
  for (NodeIterator::IndexedNode indexedNode : NodeIterator(Node(block)).forwardEditableChildren()) {
    if (indexedNode.m_node.block()->type() == BlockType::Addition) {
      // Create new addition that will be filled in the following loop
      EditionReference add = EditionReference(Node(Node::Push<Addition>(indexedNode.m_node.numberOfChildren())));
      for (NodeIterator::IndexedNode indexedAdditionChild : NodeIterator(indexedNode.m_node).forwardEditableChildren()) {
        // Copy a multiplication
        EditionReference multCopy = mult.clone();
        // Find the addition to be replaced
        EditionReference additionCopy = EditionReference(multCopy.node().childAtIndex(indexedNode.m_index));
        // Find addition child to replace with
        EditionReference additionChildCopy = EditionReference(additionCopy.childAtIndex(indexedAdditionChild.m_index));
        // Replace addition per its child
        additionCopy.replaceTreeBy(additionChildCopy);
        assert(multCopy.block()->type() == BlockType::Multiplication);
        Multiplication::DistributeOverAddition(multCopy.block());
      }
      mult.replaceTreeBy(add);
      return add.block();
    }
  }
  return block;
}

}
