#include "print.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

void testChildrenIterator() {
  Node mult = createSimpleExpression();

  print();
  std::cout << "\n---------------- Scan children forward ----------------" << std::endl;
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(mult)) {
    std::get<Node>(indexedNode).log(std::cout);
  }

  std::cout << "\n---------------- Scan children backward ----------------" << std::endl;
  for (const std::pair<Node, int>indexedNode : NodeIterator::Children<Backward, NoEditable>(mult)) {
    std::get<Node>(indexedNode).log(std::cout);
  }

  std::cout << "\n---------------- Edit children forward ----------------" << std::endl;
  constexpr Tree integer = 42_n;
  EditionReference ref(mult);
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::Children<Forward, Editable>(ref)) {
    std::get<EditionReference>(indexedRef).replaceTreeByNode(integer);
  }

  print();

  std::cout << "\n---------------- Edit children backward ----------------" << std::endl;
  constexpr Tree pi = u'π'_n;
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::Children<Backward, Editable>(ref)) {
    std::get<EditionReference>(indexedRef).replaceTreeByNode(pi);
  }

  print();

  Node secondMult = createSimpleExpression();
  print();

  std::cout << "\n---------------- Scan two nodes children forward ----------------" << std::endl;
  for (std::pair<std::array<Node, 2>, int> indexedArray : MultipleNodesIterator::Children<Forward, NoEditable, 2>(std::array<Node, 2>({mult, secondMult}))) {
    std::cout << "\n\nbtChildren indexed " << std::get<1>(indexedArray) << std::endl;
    std::get<0>(indexedArray)[0].log(std::cout);
    std::get<0>(indexedArray)[1].log(std::cout);
  }
  print();

  std::cout << "\n---------------- Scan two nodes children backward ----------------" << std::endl;
  for (std::pair<std::array<Node, 2>, int> indexedArray : MultipleNodesIterator::Children<Backward, NoEditable, 2>(std::array<Node, 2>({mult, secondMult}))) {
    std::cout << "\n\nbtChildren indexed " << std::get<1>(indexedArray) << std::endl;
    std::get<0>(indexedArray)[0].log(std::cout);
    std::get<0>(indexedArray)[1].log(std::cout);
  }
  print();

  std::cout << "\n---------------- Edit two nodes children forward ----------------" << std::endl;
  for (std::pair<std::array<EditionReference, 2>, int> indexedRefs : MultipleNodesIterator::Children<Forward, Editable, 2>(std::array<EditionReference, 2>({mult, secondMult}))) {
    std::get<0>(indexedRefs)[0].replaceTreeByNode(integer);
    std::get<0>(indexedRefs)[1].replaceTreeByNode(pi);
  }
  print();

  std::cout << "\n---------------- Edit two nodes children backward ----------------" << std::endl;
  for (std::pair<std::array<EditionReference, 2>, int> indexedRefs : MultipleNodesIterator::Children<Backward, Editable, 2>(std::array<EditionReference, 2>({mult, secondMult}))) {
    std::get<0>(indexedRefs)[0].replaceTreeByNode(pi);
    std::get<0>(indexedRefs)[1].replaceTreeByNode(integer);
  }
  print();
}
