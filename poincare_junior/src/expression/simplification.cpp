#include "simplification.h"
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/node_iterator.h>

namespace Poincare {

void Simplification::BasicReduction(TypeBlock * block) {
  // TODO: Macro to automatically generate switch
  switch (block->type()) {
    case BlockType::Division:
      return DivisionReduction(block);
    case BlockType::Subtraction:
      return SubtractionReduction(block);
    default:
      return;
  }
}

void Simplification::DivisionReduction(TypeBlock * block) {
  assert(block->type() == BlockType::Division);
  ProjectionReduction(block,
      []() { return Node::Push<BlockType::Multiplication>(2).block(); },
      []() { return Node::Push<BlockType::Power>().block(); }
    );
}

void Simplification::SubtractionReduction(TypeBlock * block) {
  assert(block->type() == BlockType::Subtraction);
  ProjectionReduction(block,
      []() { return Node::Push<BlockType::Addition>(2).block(); },
      []() { return Node::Push<BlockType::Multiplication>(2).block(); }
    );
}

TypeBlock * Simplification::DistributeMultiplicationOverAddition(TypeBlock * block) {
  EditionReference mult = EditionReference(Node(block));
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::Children<Forward, Editable>(mult)) {
    if (std::get<EditionReference>(indexedRef).node().block()->type() == BlockType::Addition) {
      // Create new addition that will be filled in the following loop
      EditionReference add = EditionReference(Node(Node::Push<BlockType::Addition>(std::get<EditionReference>(indexedRef).node().numberOfChildren())));
      for (std::pair<EditionReference, int> indexedAdditionChild : NodeIterator::Children<Forward, Editable>(std::get<EditionReference>(indexedRef))) {
        // Copy a multiplication
        EditionReference multCopy = mult.clone();
        // Find the addition to be replaced
        EditionReference additionCopy = EditionReference(multCopy.node().childAtIndex(std::get<int>(indexedRef)));
        // Find addition child to replace with
        EditionReference additionChildCopy = EditionReference(additionCopy.childAtIndex(std::get<int>(indexedAdditionChild)));
        // Replace addition per its child
        additionCopy.replaceTreeByTree(additionChildCopy);
        assert(multCopy.block()->type() == BlockType::Multiplication);
        DistributeMultiplicationOverAddition(multCopy.block());
      }
      mult.replaceTreeByTree(add);
      return add.block();
    }
  }
  return block;
}

TypeBlock * Simplification::Flatten(TypeBlock * block) {
  uint8_t numberOfChildren = 0;
  EditionReference ref = EditionReference(Node(block));
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::Children<Forward, Editable>(ref)) {
    if (block->type() == std::get<EditionReference>(indexedRef).node().block()->type()) {
      EditionReference nAry = EditionReference(Node(Flatten(std::get<EditionReference>(indexedRef).node().block())));
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

void Simplification::ProjectionReduction(TypeBlock * block, TypeBlock * (*PushProjectedEExpression)(), TypeBlock * (*PushInverse)()) {
  /* Rule a / b --> a * b^-1 (or a - b --> a + b * -1) */
  // Create a reference for future needs
  EditionReference division = EditionReference(Node(block));
  // Create empty * (or +)
  EditionReference multiplication(PushProjectedEExpression());
  // Get references to children
  assert(Node(block).numberOfChildren() == 2);
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
