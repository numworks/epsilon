#include "n_ary.h"
#include <poincare_junior/src/memory/node_iterator.h>

namespace PoincareJ {

void NAry::AddChildAtIndex(EditionReference nary, EditionReference child, int index) {
  if (index == nary.numberOfChildren()) {
    nary.nextTree().insertTreeBeforeNode(child);
  } else {
    nary.childAtIndex(index).insertTreeBeforeNode(child);
  }
  SetNumberOfChildren(nary, nary.numberOfChildren() + 1);
}

void NAry::SetNumberOfChildren(EditionReference reference, size_t numberOfChildren) {
  assert(numberOfChildren < UINT8_MAX);
  if (static_cast<Node>(reference).nodeSize() > 1) {
    /* Increment the tail numberOfChildren block first because the nodeSize
     * computation might be altered by the head numberOfChildren Block. */
    Block * numberOfChildrenBlock = reference.nextNode().block()->previousNth(2);
    *numberOfChildrenBlock = numberOfChildren;
  }
  Block * numberOfChildrenBlock = reference.block()->next();
  *numberOfChildrenBlock = numberOfChildren;

}

EditionReference NAry::Flatten(EditionReference reference) {
  size_t numberOfChildren = 0;
  for (auto [child, index] : NodeIterator::Children<Forward, Editable>(reference)) {
    if (reference.node().type() == child.node().type()) {
      child.removeNode();
    }
    numberOfChildren++;
  }
  SetNumberOfChildren(reference, numberOfChildren);
  return reference;
}

}
