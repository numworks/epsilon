#include "simplification.h"
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/node_iterator.h>

namespace Poincare {

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
