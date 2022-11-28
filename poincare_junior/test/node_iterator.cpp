#include "print.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

void testChildrenIterator() {
  Node mult = createSimpleExpression();

  print();

  std::cout << "\n---------------- Scan children forward ----------------" << std::endl;
  for (const std::pair<Node, int> indexedNode : NodeIterator::Children<ScanDirection::Forward, Editable::False>(mult)) {
    std::get<Node>(indexedNode).log(std::cout);
  }

  std::cout << "\n---------------- Scan children backward ----------------" << std::endl;
  for (const std::pair<Node, int>indexedNode : NodeIterator::Children<ScanDirection::Backward, Editable::False, const Node>(mult)) {
    std::get<Node>(indexedNode).log(std::cout);
  }

  std::cout << "\n---------------- Edit children forward ----------------" << std::endl;
  constexpr Tree integer = 42_n;
  EditionReference ref(mult);
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::Children<ScanDirection::Forward, Editable::True>(ref)) {
    std::get<EditionReference>(indexedRef).replaceTreeByNode(integer);
  }

  print();

  std::cout << "\n---------------- Edit children backward ----------------" << std::endl;
  constexpr Tree pi = u'π'_n;
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::Children<ScanDirection::Backward, Editable::True>(ref)) {
    std::get<EditionReference>(indexedRef).replaceTreeByNode(pi);
  }

  print();

  Node secondMult = createSimpleExpression();
  print();

  std::cout << "\n---------------- Scan two nodes children forward ----------------" << std::endl;
  for (std::pair<std::array<const Node, 2>, int> indexedArray : MultipleNodesIterator::Children<ScanDirection::Forward, Editable::False>(std::array<const Node, 2>({mult, secondMult}))) {
    std::cout << "\n\nbtChildren indexed " << std::get<1>(indexedArray) << std::endl;
    std::get<0>(indexedArray)[0].log(std::cout);
    std::get<0>(indexedArray)[1].log(std::cout);
  }
  print();

  std::cout << "\n---------------- Scan two nodes children backward ----------------" << std::endl;
  for (std::pair<std::array<const Node, 2>, int> indexedArray : MultipleNodesIterator::Children<ScanDirection::Backward, Editable::False>(std::array<const Node, 2>({mult, secondMult}))) {
    std::cout << "\n\nbtChildren indexed " << std::get<1>(indexedArray) << std::endl;
    std::get<0>(indexedArray)[0].log(std::cout);
    std::get<0>(indexedArray)[1].log(std::cout);
  }
  print();

  std::cout << "\n---------------- Edit two nodes children forward ----------------" << std::endl;
  for (std::pair<std::array<EditionReference, 2>, int> indexedRefs : MultipleNodesIterator::Children<ScanDirection::Forward, Editable::True>(std::array<EditionReference, 2>({mult, secondMult}))) {
    std::get<0>(indexedRefs)[0].replaceTreeByNode(integer);
    std::get<0>(indexedRefs)[1].replaceTreeByNode(pi);
  }
  print();

  std::cout << "\n---------------- Edit two nodes children backward ----------------" << std::endl;
  for (std::pair<std::array<EditionReference, 2>, int> indexedRefs : MultipleNodesIterator::Children<ScanDirection::Backward, Editable::True>(std::array<EditionReference, 2>({mult, secondMult}))) {
    std::get<0>(indexedRefs)[0].replaceTreeByNode(pi);
    std::get<0>(indexedRefs)[1].replaceTreeByNode(integer);
  }
  print();
}
