#include "n_ary_interface.h"
#include "../edition_reference.h"

namespace Poincare {

void NAryInterface::logAttributes(const TypeBlock * block, std::ostream & stream) const {
  stream << " numberOfChildren=\"" << Node(block).numberOfChildren() << "\"";
}

TypeBlock * NAryExpressionInterface::Flatten(TypeBlock * block) {
  uint8_t numberOfChildren = 0;
  for (NodeIterator::IndexedNode indexedNode : NodeIterator(Node(block)).directChildren()) {
    if (block->type() == indexedNode.m_node.block()->type()) {
      EditionReference nAry = EditionReference(Node(Flatten(indexedNode.m_node.block())));
      numberOfChildren += nAry.node().numberOfChildren();
      nAry.remove();
    } else {
      numberOfChildren++;
    }
  }
  Block * numberOfChildrenBlock = block->next();
  *numberOfChildrenBlock = numberOfChildren;
  return block;
}

}
