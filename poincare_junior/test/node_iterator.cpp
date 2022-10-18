#include "print.h"
#include <poincare_junior/node_iterator.h>
#include <poincare_junior/constexpr_node.h>

using namespace Poincare;

void testChildrenIterator() {
  Node mult = createSimpleExpression();

  print();

  std::cout << "\n---------------- Scan children forward ----------------" << std::endl;
  for (const NodeIterator::IndexedNode indexedNode : NodeIterator(mult).forwardConstChildren()) {
    indexedNode.m_node.log(std::cout);
  }

  std::cout << "\n---------------- Scan children backward ----------------" << std::endl;
  for (const NodeIterator::IndexedNode indexedNode : NodeIterator(mult).backwardsConstChildren()) {
    indexedNode.m_node.log(std::cout);
  }

  std::cout << "\n---------------- Edit children forward ----------------" << std::endl;
  constexpr Tree integer = 42_n;
  for (NodeIterator::IndexedNode indexedNode : NodeIterator(mult).forwardEditableChildren()) {
    EditionReference(indexedNode.m_node).replaceTreeBy(integer);
  }

  print();

  std::cout << "\n---------------- Scan children backward ----------------" << std::endl;
  constexpr Tree pi = u'π'_n;
  for (NodeIterator::IndexedNode indexedNode : NodeIterator(mult).backwardsEditableChildren()) {
    EditionReference(indexedNode.m_node).replaceTreeBy(pi);
  }

  print();
}
