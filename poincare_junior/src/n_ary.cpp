#include "n_ary.h"

#include <assert.h>
#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/memory/node_iterator.h>

namespace PoincareJ {

void NAry::AddChildAtIndex(EditionReference nary, EditionReference child,
                           int index) {
  assert(static_cast<Node*>(nary).isNAry());
  /* Child will be moved, it should be detached from his parent beforehand.
   * Otherwise, the parent structure will get corrupted.
   * TODO: Polynomial temporarily deal with corrupted structures to optimize
   * operations. This assert is therefore commented, maybe there could be a
   * private AddChildAtIndex method bypassing this check. */
  // assert(child.parent().isUninitialized());
  if (index == nary.numberOfChildren()) {
    EditionReference(nary.nextTree()).insertTreeBeforeNode(child);
  } else {
    EditionReference(nary.childAtIndex(index)).insertTreeBeforeNode(child);
  }
  SetNumberOfChildren(nary, nary.numberOfChildren() + 1);
}

void NAry::AddOrMergeChildAtIndex(EditionReference nary, EditionReference child,
                                  int index) {
  AddChildAtIndex(nary, child, index);
  if (static_cast<Node*>(nary).type() == static_cast<Node*>(child).type()) {
    size_t numberOfChildren =
        nary.numberOfChildren() + child.numberOfChildren() - 1;
    child.removeNode();
    SetNumberOfChildren(nary, numberOfChildren);
  }
}

EditionReference NAry::DetachChildAtIndex(EditionReference nary, int index) {
  assert(static_cast<Node*>(nary).isNAry());
  EditionReference child = nary.childAtIndex(index);
  child.detachTree();
  SetNumberOfChildren(nary, nary.numberOfChildren() - 1);
  return child;
}

void NAry::RemoveChildAtIndex(EditionReference nary, int index) {
  assert(static_cast<Node*>(nary).isNAry());
  EditionReference child = nary.childAtIndex(index);
  child.removeTree();
  SetNumberOfChildren(nary, nary.numberOfChildren() - 1);
}

void NAry::SetNumberOfChildren(EditionReference reference,
                               size_t numberOfChildren) {
  assert(static_cast<Node*>(reference).isNAry());
  assert(numberOfChildren < UINT8_MAX);
  if (static_cast<Node*>(reference).nodeSize() > 1) {
    /* Increment the tail numberOfChildren block first because the nodeSize
     * computation might be altered by the head numberOfChildren Block. */
    Block* numberOfChildrenBlock = reference.nextNode().block()->previousNth(2);
    *numberOfChildrenBlock = numberOfChildren;
  }
  Block* numberOfChildrenBlock = reference.block()->next();
  *numberOfChildrenBlock = numberOfChildren;
}

EditionReference NAry::Flatten(EditionReference reference) {
  Flatten(&reference);
  return reference;
}

bool NAry::Flatten(EditionReference* reference) {
  bool modified = false;
  assert(Node * (*reference).isNAry());
  size_t numberOfChildren = reference->numberOfChildren();
  size_t childIndex = 0;
  Node* child = reference->nextNode();
  while (childIndex < numberOfChildren) {
    if (reference->type() == child.type()) {
      modified = true;
      numberOfChildren += child.numberOfChildren() - 1;
      EditionReference(child).removeNode();
    } else {
      child = child.nextTree();
      childIndex++;
    }
  }
  if (modified) {
    SetNumberOfChildren(*reference, numberOfChildren);
    return true;
  }
  return false;
}

EditionReference NAry::SquashIfUnary(EditionReference reference) {
  SquashIfUnary(&reference);
  return reference;
}

bool NAry::SquashIfUnary(EditionReference* reference) {
  if (reference->numberOfChildren() == 1) {
    *reference = reference->replaceTreeByTree(reference->nextNode());
    return true;
  }
  return false;
}

EditionReference NAry::SquashIfEmpty(EditionReference reference) {
  SquashIfEmpty(&reference);
  return reference;
}

bool NAry::SquashIfEmpty(EditionReference* reference) {
  if (reference->numberOfChildren() >= 1) {
    return false;
  }
  // Return the neutral element
  BlockType type = reference->type();
  assert(type == BlockType::Addition || type == BlockType::Multiplication);
  *reference = reference->replaceTreeByTree(
      type == BlockType::Addition ? &ZeroBlock : &OneBlock);
  return true;
}

EditionReference NAry::Sanitize(EditionReference reference) {
  Sanitize(&reference);
  return reference;
}

bool NAry::Sanitize(EditionReference* reference) {
  bool flattened = Flatten(reference);
  if (reference->numberOfChildren() == 0) {
    return SquashIfEmpty(reference) || flattened;
  }
  return SquashIfUnary(reference) || flattened;
}

void NAry::SortChildren(EditionReference reference, Comparison::Order order) {
  // Non simple NArys (Polynomial) rely on children order.
  assert(reference.block()->isSimpleNAry());
  Node* nary = reference;
  void* contextArray[2] = {&nary, &order};
  /* TODO : This sort is far from being optimized. Calls of childAtIndex are
   *        very expensive here. A better swap could also be implemented. */
  List::Sort(
      [](int i, int j, void* context, int numberOfElements) {
        void** contextArray = static_cast<void**>(context);
        Node* nary = *static_cast<Node**>(contextArray[0]);
        EditionReference refI = nary.childAtIndex(i);
        EditionReference refJ = nary.childAtIndex(j);
        EditionReference refJNext = refJ.nextTree();
        refI.insertTreeBeforeNode(refJ);
        refJNext.insertTreeBeforeNode(refI);
      },
      [](int i, int j, void* context, int numberOfElements) {
        void** contextArray = static_cast<void**>(context);
        Node* nary = *static_cast<Node**>(contextArray[0]);
        Comparison::Order order =
            *static_cast<Comparison::Order*>(contextArray[1]);
        return Comparison::Compare(nary.childAtIndex(i), nary.childAtIndex(j),
                                   order) >= 0;
      },
      contextArray, reference.numberOfChildren());
}

}  // namespace PoincareJ
