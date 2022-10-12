#include "n_ary.h"
#include "../edition_reference.h"
#include "../node_iterator.h"

namespace Poincare {

void NAryInterface::logAttributes(const TypeBlock * block, std::ostream & stream) const {
  stream << " numberOfChildren=\"" << Node(block).numberOfChildren() << "\"";
}

TypeBlock * NAryExpressionInterface::Flatten(TypeBlock * block) {
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

}
