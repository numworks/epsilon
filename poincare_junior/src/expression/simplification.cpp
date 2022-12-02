#include "simplification.h"
#include <poincare_junior/src/memory/node_iterator.h>

namespace PoincareJ {

void Simplification::BasicReduction(EditionReference reference) {
  // TODO: Macro to automatically generate switch
  switch (reference.type()) {
    case BlockType::Division:
      return DivisionReduction(reference);
    case BlockType::Subtraction:
      return SubtractionReduction(reference);
    default:
      return;
  }
}

void Simplification::DivisionReduction(EditionReference reference) {
  assert(reference.type() == BlockType::Division);
  ProjectionReduction(reference,
      []() { return EditionReference::Push<BlockType::Multiplication>(2); },
      []() { return EditionReference::Push<BlockType::Power>(); }
    );
}

void Simplification::SubtractionReduction(EditionReference reference) {
  assert(reference.type() == BlockType::Subtraction);
  ProjectionReduction(reference,
      []() { return EditionReference::Push<BlockType::Addition>(2); },
      []() { return EditionReference::Push<BlockType::Multiplication>(2); }
    );
}

EditionReference Simplification::DistributeMultiplicationOverAddition(EditionReference reference) {
  for (auto [child, index] : NodeIterator::Children<Forward, Editable>(reference)) {
    if (child.type() == BlockType::Addition) {
      // Create new addition that will be filled in the following loop
      EditionReference add = EditionReference(EditionReference::Push<BlockType::Addition>(child.numberOfChildren()));
      for (auto [additionChild, additionIndex] : NodeIterator::Children<Forward, Editable>(child)) {
        // Copy a multiplication
        EditionReference multCopy = EditionReference::Clone(reference);
        // Find the addition to be replaced
        EditionReference additionCopy = EditionReference(multCopy.childAtIndex(index));
        // Find addition child to replace with
        EditionReference additionChildCopy = EditionReference(additionCopy.childAtIndex(additionIndex));
        // Replace addition per its child
        additionCopy.replaceTreeByTree(additionChildCopy);
        assert(multCopy.type() == BlockType::Multiplication);
        DistributeMultiplicationOverAddition(multCopy);
      }
      reference.replaceTreeByTree(add);
      return add;
    }
  }
  return reference;
}

void Simplification::ProjectionReduction(EditionReference division, EditionReference (*PushProjectedEExpression)(), EditionReference (*PushInverse)()) {
  /* Rule a / b --> a * b^-1 (or a - b --> a + b * -1) */
  // Create empty * (or +)
  EditionReference multiplication(PushProjectedEExpression());
  // Get references to children
  assert(division.numberOfChildren() == 2);
  EditionReference childrenReferences[2];
  for (auto [child, index] : NodeIterator::Children<Forward, Editable>(division)) {
    childrenReferences[index] = child;
  }
  // Move first child
  multiplication.insertTreeAfterNode(childrenReferences[0]);
  // Create empty ^ (or *)
  EditionReference power(PushInverse());
  // Move second child
  power.insertTreeAfterNode(childrenReferences[1]);
  // Complete: a * b^-1 (or a + b * -1)
  EditionReference::Push<BlockType::IntegerShort>(static_cast<int8_t>(-1));
  // Replace single-noded division (or subtraction) by the new multiplication (or addition)
  division.replaceNodeByTree(multiplication);
}

}
