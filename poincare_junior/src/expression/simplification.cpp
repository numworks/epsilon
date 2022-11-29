#include "approximation.h"
#include "simplification.h"
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/node_iterator.h>

namespace Poincare {

int Simplification::Compare(const Node node0, const Node node1) {
  BlockType type0 = node0.type();
  BlockType type1 = node1.type();
  TypeBlock * block0 = node0.block();
  TypeBlock * block1 = node1.block();
  if (type0 > type1) {
    // We handle the case first to implement only the upper diagonal of the comparison table
    return -Compare(node1, node0);
  } else if (type0 == type1 || (block0->isNumber() && block1->isNumber())) { // TODO: (block0->isUserNamed() && block1->isUserNamed()): do we want to group functions and symbols together?
    if (block0->isNumber()) {
      assert(block1->isNumber());
      return CompareNumbers(node0, node1);
    }
    if (block0->isUserNamed()) {
      int nameComparison = CompareNames(node0, node1);
      if (nameComparison != 0) {
        return nameComparison;
      }
      // Scan children
    }
    switch (type0) {
      case BlockType::Constant:
        return CompareConstants(node0, node1);
      case BlockType::Addition:
      case BlockType::Multiplication:
        return CompareChildren(node0, node1, ScanDirection::Backward);
      default:
        return CompareChildren(node0, node1, ScanDirection::Forward);
    }
  } else {
    assert(type0 < type1);
    switch (type0) {
      case BlockType::Addition:
      case BlockType::Multiplication:
        return CompareFirstChild(node0, node1, ScanDirection::Backward);
      case BlockType::Power:
      {
        int comparisonBase = Compare(node0.childAtIndex(0), node1);
        if (comparisonBase != 0) {
          return 1;
        }
        return Compare(node0.childAtIndex(1), &OneBlock);
      }
      case BlockType::Factorial:
        return CompareFirstChild(node0, node1, ScanDirection::Forward);
      default:
        return false;
    }
  }
}

int Simplification::CompareNumbers(const Node node0, const Node node1) {
  if (node0.block()->isRational() && node1.block()->isRational()) {
    // TODO
    //return Rational::NaturalOrder(block0, block1);
  }
  float approximation = Approximation::To<float>(node0) - Approximation::To<float>(node1);
  return approximation == 0.0f ? 0 : (approximation > 0.0f ? 1 : -1);
}

int Simplification::CompareNames(const Node node0, const Node node1) {
  // TODO
  return 0;
}

int Simplification::CompareConstants(const Node node0, const Node node1) {
  // TODO
  return 0;
}

template<typename ScanDirection>
int PrivateCompareChildren(const Node node0, const Node node1) {
  for (std::pair<std::array<Node, 2>, int> indexedNodes : MultipleNodesIterator::Children<ScanDirection, NoEditable, 2>({node0, node1})) {
    Node child0 = std::get<0>(indexedNodes)[0];
    Node child1 = std::get<0>(indexedNodes)[1];
    int order = Simplification::Compare(child0.block(), child1.block());
    if (order != 0) {
      return order;
    }
  }
  return 0;
}

int Simplification::CompareChildren(const Node node0, const Node node1, ScanDirection direction) {
  int comparison;
  if (direction == ScanDirection::Forward) {
    comparison = PrivateCompareChildren<Forward>(node0, node1);
  } else {
    comparison = PrivateCompareChildren<Backward>(node0, node1);
  }
  if (comparison) {
    return comparison;
  }
  int numberOfChildren0 = node0.numberOfChildren();
  int numberOfChildren1 = node1.numberOfChildren();
  // The NULL node is the least node type.
  if (numberOfChildren0 < numberOfChildren1) {
    return 1;
  }
  if (numberOfChildren1 < numberOfChildren0) {
    return -1;
  }
  return 0;
}

int Simplification::CompareFirstChild(const Node node0, Node node1, ScanDirection direction) {
  uint8_t indexOfChild = direction == ScanDirection::Forward ? 0 : node0.numberOfChildren() - 1;
  int comparisonWithChild = Compare(node0.childAtIndex(indexOfChild), node1);
  if (comparisonWithChild != 0) {
    return comparisonWithChild;
  }
  assert(node0.numberOfChildren() > 1);
  return 1;
}

void Simplification::BasicReduction(Node node) {
  // TODO: Macro to automatically generate switch
  switch (node.type()) {
    case BlockType::Division:
      return DivisionReduction(node);
    case BlockType::Subtraction:
      return SubtractionReduction(node);
    default:
      return;
  }
}

void Simplification::DivisionReduction(Node node) {
  assert(node.type() == BlockType::Division);
  ProjectionReduction(node,
      []() { return Node::Push<BlockType::Multiplication>(2); },
      []() { return Node::Push<BlockType::Power>(); }
    );
}

void Simplification::SubtractionReduction(Node node) {
  assert(node.type() == BlockType::Subtraction);
  ProjectionReduction(node,
      []() { return Node::Push<BlockType::Addition>(2); },
      []() { return Node::Push<BlockType::Multiplication>(2); }
    );
}

Node Simplification::DistributeMultiplicationOverAddition(Node node) {
  EditionReference mult = EditionReference(node);
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::Children<Forward, Editable>(mult)) {
    if (std::get<EditionReference>(indexedRef).node().type() == BlockType::Addition) {
      // Create new addition that will be filled in the following loop
      EditionReference add = EditionReference(Node::Push<BlockType::Addition>(std::get<EditionReference>(indexedRef).node().numberOfChildren()));
      for (std::pair<EditionReference, int> indexedAdditionChild : NodeIterator::Children<Forward, Editable>(std::get<EditionReference>(indexedRef))) {
        // Copy a multiplication
        EditionReference multCopy = mult.clone();
        // Find the addition to be replaced
        EditionReference additionCopy = EditionReference(multCopy.node().childAtIndex(std::get<int>(indexedRef)));
        // Find addition child to replace with
        EditionReference additionChildCopy = EditionReference(additionCopy.childAtIndex(std::get<int>(indexedAdditionChild)));
        // Replace addition per its child
        additionCopy.replaceTreeByTree(additionChildCopy);
        assert(multCopy.node().type() == BlockType::Multiplication);
        DistributeMultiplicationOverAddition(multCopy.node());
      }
      mult.replaceTreeByTree(add);
      return add.node();
    }
  }
  return node;
}

Node Simplification::Flatten(Node node) {
  uint8_t numberOfChildren = 0;
  EditionReference ref = EditionReference(node);
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::Children<Forward, Editable>(ref)) {
    if (node.type() == std::get<EditionReference>(indexedRef).node().type()) {
      EditionReference nAry = EditionReference(Flatten(std::get<EditionReference>(indexedRef).node()));
      numberOfChildren += nAry.node().numberOfChildren();
      nAry.removeNode();
    } else {
      numberOfChildren++;
    }
  }
  Block * numberOfChildrenBlock = node.block()->next();
  *numberOfChildrenBlock = numberOfChildren;
  return node;
}

void Simplification::ProjectionReduction(Node node, Node (*PushProjectedEExpression)(), Node (*PushInverse)()) {
  /* Rule a / b --> a * b^-1 (or a - b --> a + b * -1) */
  // Create a reference for future needs
  EditionReference division = EditionReference(node);
  // Create empty * (or +)
  EditionReference multiplication(PushProjectedEExpression());
  // Get references to children
  assert(node.numberOfChildren() == 2);
  EditionReference childrenReferences[2];
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::Children<Forward, Editable>(division)) {
    childrenReferences[std::get<int>(indexedRef)] = std::get<EditionReference>(indexedRef);
  }
  // Move first child
  childrenReferences[0].insertTreeAfterNode(multiplication);
  // Create empty ^ (or *)
  EditionReference power(PushInverse());
  // Move second child
  childrenReferences[1].insertTreeAfterNode(power);
  // Complete: a * b^-1 (or a + b * -1)
  Node::Push<BlockType::IntegerShort>(-1);
  // Replace single-noded division (or subtraction) by the new multiplication (or addition)
  division.replaceNodeByTree(multiplication);
}

}
