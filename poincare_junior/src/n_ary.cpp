#include "n_ary.h"

#include <assert.h>
#include <poincare_junior/src/memory/node_iterator.h>

namespace PoincareJ {

void NAry::AddChildAtIndex(EditionReference nary, EditionReference child,
                           int index) {
  assert(static_cast<Node>(nary).isNAry());
  /* Child will be moved, it should be detached from his parent beforehand.
   * Otherwise, the parent structure will get corrupted.
   * TODO: Polynomial temporarily deal with corrupted structures to optimize
   * operations. This assert is therefore commented, maybe there could be a
   * private AddChildAtIndex method bypassing this check. */
  // assert(child.parent().isUninitialized());
  if (index == nary.numberOfChildren()) {
    nary.nextTree().insertTreeBeforeNode(child);
  } else {
    nary.childAtIndex(index).insertTreeBeforeNode(child);
  }
  SetNumberOfChildren(nary, nary.numberOfChildren() + 1);
}

void NAry::AddOrMergeChildAtIndex(EditionReference nary, EditionReference child,
                                  int index) {
  AddChildAtIndex(nary, child, index);
  if (static_cast<Node>(nary).type() == static_cast<Node>(child).type()) {
    size_t numberOfChildren =
        nary.numberOfChildren() + child.numberOfChildren() - 1;
    child.removeNode();
    SetNumberOfChildren(nary, numberOfChildren);
  }
}

EditionReference NAry::DetachChildAtIndex(EditionReference nary, int index) {
  assert(static_cast<Node>(nary).isNAry());
  EditionReference child = nary.childAtIndex(index);
  child.detachTree();
  SetNumberOfChildren(nary, nary.numberOfChildren() - 1);
  return child;
}

void NAry::RemoveChildAtIndex(EditionReference nary, int index) {
  assert(static_cast<Node>(nary).isNAry());
  EditionReference child = nary.childAtIndex(index);
  child.removeTree();
  SetNumberOfChildren(nary, nary.numberOfChildren() - 1);
}

void NAry::SetNumberOfChildren(EditionReference reference,
                               size_t numberOfChildren) {
  assert(static_cast<Node>(reference).isNAry());
  assert(numberOfChildren < UINT8_MAX);
  if (static_cast<Node>(reference).nodeSize() > 1) {
    /* Increment the tail numberOfChildren block first because the nodeSize
     * computation might be altered by the head numberOfChildren Block. */
    Block* numberOfChildrenBlock = reference.nextNode().block()->previousNth(2);
    *numberOfChildrenBlock = numberOfChildren;
  }
  Block* numberOfChildrenBlock = reference.block()->next();
  *numberOfChildrenBlock = numberOfChildren;
}

EditionReference NAry::Flatten(EditionReference reference) {
  assert(static_cast<Node>(reference).isNAry());
  size_t numberOfChildren = 0;
  for (auto [child, index] :
       NodeIterator::Children<Forward, Editable>(reference)) {
    if (reference.type() == child.type()) {
      child.removeNode();
    }
    numberOfChildren++;
  }
  SetNumberOfChildren(reference, numberOfChildren);
  return reference;
}

EditionReference NAry::SquashIfUnary(EditionReference reference) {
  if (reference.numberOfChildren() == 1) {
    EditionReference child = reference.nextNode();
    reference.replaceTreeByTree(child);
    return child;
  }
  return reference;
}

}  // namespace PoincareJ
