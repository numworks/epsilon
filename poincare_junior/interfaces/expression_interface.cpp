#include "expression_interface.h"
#include "../edition_reference.h"
#include "../node.h"

namespace Poincare {

void ExpressionInterface::projectionReduction(TypeBlock * block, TypeBlock * (*PushNode)(), TypeBlock * (*PushInverseNode)()) const {
  /* Rule a / b --> a * b^-1 (or a - b --> a + b * -1) */
  // Create a reference for future needs
  EditionReference division = EditionReference(Node(block));
  // Create empty * (or +)
  EditionReference multiplication(PushNode());
  // Get references to children
  assert(Node(block).numberOfChildren() == 2);
  EditionReference childrenReferences[2];
  for (NodeIterator::IndexedNode indexedNode : NodeIterator(division.node()).directChildren()) {
    childrenReferences[indexedNode.m_index] = EditionReference(indexedNode.m_node);
  }
  // Move first child
  childrenReferences[0].insertAfter(multiplication);
  // Create empty ^ (or *)
  EditionReference power(PushInverseNode());
  // Move second child
  childrenReferences[1].insertAfter(power);
  // Complete: a * b^-1 (or a + b * -1)
  IntegerInterface::PushNode(111); // TODO: implement negative number
  // Remove / node (or -)
  division.remove();
}


}
