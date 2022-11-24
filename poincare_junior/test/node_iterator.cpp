#include "print.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

void testChildrenIterator() {
  Node mult = createSimpleExpression();

  print();

  std::cout << "\n---------------- Scan children forward ----------------" << std::endl;
  for (const NodeIterator::IndexedNode indexedNode : NodeIterator(mult).forwardConstChildren()) {
    indexedNode.m_node.log(std::cout);
  }

  std::cout << "\n---------------- Scan children backward ----------------" << std::endl;
  for (const NodeIterator::IndexedNode indexedNode : NodeIterator(mult).backwardConstChildren()) {
    indexedNode.m_node.log(std::cout);
  }

  std::cout << "\n---------------- Edit children forward ----------------" << std::endl;
  constexpr Tree integer = 42_n;
  for (NodeIterator::IndexedNode indexedNode : NodeIterator(mult).forwardEditableChildren()) {
    EditionReference(indexedNode.m_node).replaceTreeByNode(integer);
  }

  print();

  std::cout << "\n---------------- Edit children backward ----------------" << std::endl;
  constexpr Tree pi = u'π'_n;
  for (NodeIterator::IndexedNode indexedNode : NodeIterator(mult).backwardEditableChildren()) {
    EditionReference(indexedNode.m_node).replaceTreeByNode(pi);
  }

  print();
}
