#include "print.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

void testChildrenIterator() {
  Node mult = createSimpleExpression();

  print();

  std::cout << "\n---------------- Scan children forward ----------------" << std::endl;
  for (const std::pair<Node, int> indexedNode : NodeIterator::ForwardConstChildren(mult)) {
    std::get<Node>(indexedNode).log(std::cout);
  }

  std::cout << "\n---------------- Scan children backward ----------------" << std::endl;
  for (const std::pair<Node, int>indexedNode : NodeIterator::BackwardConstChildren(mult)) {
    std::get<Node>(indexedNode).log(std::cout);
  }

  std::cout << "\n---------------- Edit children forward ----------------" << std::endl;
  constexpr Tree integer = 42_n;
  EditionReference ref(mult);
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::ForwardEditableChildren(ref)) {
    std::get<EditionReference>(indexedRef).replaceTreeByNode(integer);
  }

  print();

  std::cout << "\n---------------- Edit children backward ----------------" << std::endl;
  constexpr Tree pi = u'π'_n;
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::BackwardEditableChildren(ref)) {
    std::get<EditionReference>(indexedRef).replaceTreeByNode(pi);
  }

  print();

  Node secondMult = createSimpleExpression();
  print();

  std::cout << "\n---------------- Scan two nodes children forward ----------------" << std::endl;
  int counter = 0;
  for (const std::pair<Node, Node> nodes : TwoNodesIterator::ForwardConstChildren(mult, secondMult)) {
    std::cout << "\n\nbtChildren indexed " << counter++ << std::endl;
    std::get<0>(nodes).log(std::cout);
    std::get<1>(nodes).log(std::cout);
  }
  print();

  std::cout << "\n---------------- Scan two nodes children backward ----------------" << std::endl;
  counter = 0;
  for (const std::pair<Node, Node> nodes : TwoNodesIterator::BackwardConstChildren(mult, secondMult)) {
    std::cout << "\n\nbtChildren indexed " << counter++ << std::endl;
    std::get<0>(nodes).log(std::cout);
    std::get<1>(nodes).log(std::cout);
  }
  print();

  std::cout << "\n---------------- Edit two nodes children forward ----------------" << std::endl;
  for (std::pair<EditionReference, EditionReference> refs : TwoNodesIterator::ForwardEditableChildren(mult, secondMult)) {
    std::get<0>(refs).replaceTreeByNode(integer);
    std::get<1>(refs).replaceTreeByNode(pi);
  }
  print();
}
