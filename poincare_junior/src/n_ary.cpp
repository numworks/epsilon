#include "n_ary.h"

#include <assert.h>
#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/memory/node_iterator.h>

namespace PoincareJ {

void NAry::AddChildAtIndex(EditionReference nary, EditionReference child,
                           int index) {
  assert(static_cast<Node*>(nary)->isNAry());
  /* Child will be moved, it should be detached from his parent beforehand.
   * Otherwise, the parent structure will get corrupted.
   * TODO: Polynomial temporarily deal with corrupted structures to optimize
   * operations. This assert is therefore commented, maybe there could be a
   * private AddChildAtIndex method bypassing this check. */
  // assert(child.parent().isUninitialized());
  if (index == nary.numberOfChildren()) {
    nary.nextTree()->moveTreeBeforeNode(child);
  } else {
    nary.childAtIndex(index)->moveTreeBeforeNode(child);
  }
  SetNumberOfChildren(nary, nary.numberOfChildren() + 1);
}

void NAry::AddOrMergeChildAtIndex(EditionReference nary, EditionReference child,
                                  int index) {
  AddChildAtIndex(nary, child, index);
  if (static_cast<Node*>(nary)->type() == static_cast<Node*>(child)->type()) {
    size_t numberOfChildren =
        nary.numberOfChildren() + child.numberOfChildren() - 1;
    child.removeNode();
    SetNumberOfChildren(nary, numberOfChildren);
  }
}

EditionReference NAry::DetachChildAtIndex(EditionReference nary, int index) {
  assert(static_cast<Node*>(nary)->isNAry());
  EditionReference child = nary.childAtIndex(index);
  child.detachTree();
  SetNumberOfChildren(nary, nary.numberOfChildren() - 1);
  return child;
}

void NAry::RemoveChildAtIndex(EditionReference nary, int index) {
  assert(static_cast<Node*>(nary)->isNAry());
  EditionReference child = nary.childAtIndex(index);
  child.removeTree();
  SetNumberOfChildren(nary, nary.numberOfChildren() - 1);
}

void NAry::SetNumberOfChildren(EditionReference reference,
                               size_t numberOfChildren) {
  assert(static_cast<Node*>(reference)->isNAry());
  assert(numberOfChildren < UINT8_MAX);
  Block* numberOfChildrenBlock = reference.block()->next();
  *numberOfChildrenBlock = numberOfChildren;
}

bool NAry::Flatten(EditionReference* reference) {
  bool modified = false;
  assert(static_cast<Node*>(*reference)->isNAry());
  size_t numberOfChildren = reference->numberOfChildren();
  size_t childIndex = 0;
  Node* child = reference->nextNode();
  while (childIndex < numberOfChildren) {
    if (reference->type() == child->type()) {
      modified = true;
      numberOfChildren += child->numberOfChildren() - 1;
      child->removeNode();
    } else {
      child = child->nextTree();
      childIndex++;
    }
  }
  if (modified) {
    SetNumberOfChildren(*reference, numberOfChildren);
    return true;
  }
  return false;
}

bool NAry::SquashIfUnary(EditionReference* reference) {
  if (reference->numberOfChildren() == 1) {
    *reference = reference->moveTreeOverTree(reference->nextNode());
    return true;
  }
  return false;
}

bool NAry::SquashIfEmpty(EditionReference* reference) {
  if (reference->numberOfChildren() >= 1) {
    return false;
  }
  // Return the neutral element
  BlockType type = reference->type();
  assert(type == BlockType::Addition || type == BlockType::Multiplication);
  *reference = reference->cloneTreeOverTree(
      Node::FromBlocks(type == BlockType::Addition ? &ZeroBlock : &OneBlock));
  return true;
}

bool NAry::Sanitize(EditionReference* reference) {
  bool flattened = Flatten(reference);
  if (reference->numberOfChildren() == 0) {
    return SquashIfEmpty(reference) || flattened;
  }
  return SquashIfUnary(reference) || flattened;
}

bool NAry::Sort(Node* nary, Comparison::Order order) {
  // avoid the switch of numberOfChildren() since we know the type ?
  const uint8_t numberOfChildren = nary->numberOfChildren();
  if (numberOfChildren < 2) {
    return false;
  }
  if (numberOfChildren == 2) {
    Node* child0 = nary->nextNode();
    Node* child1 = child0->nextTree();
    if (Comparison::Compare(child0, child1, order) > 0) {
      child0->moveTreeBeforeNode(child1);
      return true;
    }
    return false;
  }
  const Node* children[k_maxNumberOfChildren];
  uint8_t indexes[k_maxNumberOfChildren];
  for (uint8_t index = 0; const Node* child : nary->children()) {
    children[index] = child;
    indexes[index] = index;
    index++;
  }
  // sort a list of indexes first
  std::sort(&indexes[0], &indexes[numberOfChildren], [&](uint8_t a, uint8_t b) {
    return Comparison::Compare(children[a], children[b], order) < 0;
  });
  // test if something has changed
  for (int i = 0; i < numberOfChildren; i++) {
    if (indexes[i] != i) {
      goto push;
    }
  }
  return false;
push:
  // push children in their destination order
  Node* newNAry = EditionPool::sharedEditionPool()->clone(nary, false);
  for (int i = 0; i < numberOfChildren; i++) {
    children[indexes[i]]->clone();
  }
  assert(nary->treeSize() == newNAry->treeSize());
  // replace nary with the sorted one
  nary->moveTreeOverTree(newNAry);
  return true;
}

bool NAry::Sort(EditionReference* reference, Comparison::Order order) {
  Node* u = *reference;
  bool result = Sort(u, order);
  *reference = u;
  return result;
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
        EditionReference refI = nary->childAtIndex(i);
        EditionReference refJ = nary->childAtIndex(j);
        EditionReference refJNext = refJ.nextTree();
        refI.moveTreeBeforeNode(refJ);
        refJNext.moveTreeBeforeNode(refI);
      },
      [](int i, int j, void* context, int numberOfElements) {
        void** contextArray = static_cast<void**>(context);
        Node* nary = *static_cast<Node**>(contextArray[0]);
        Comparison::Order order =
            *static_cast<Comparison::Order*>(contextArray[1]);
        return Comparison::Compare(nary->childAtIndex(i), nary->childAtIndex(j),
                                   order) >= 0;
      },
      contextArray, reference.numberOfChildren());
}

void NAry::SortedInsertChild(EditionReference ref, EditionReference child,
                             Comparison::Order order) {
  Node* nary = ref;
  Node* children[k_maxNumberOfChildren];
  for (uint8_t index = 0; const Node* child : nary->children()) {
    children[index++] = const_cast<Node*>(child);
  }
  uint8_t a = 0;
  uint8_t b = nary->numberOfChildren();
  while (b - a > 0) {
    uint8_t m = (a + b) / 2;
    if (Comparison::Compare(children[m], child, order) < 0) {
      a = (a + b + 1) / 2;
    } else {
      b = m;
    }
  }
  AddChildAtIndex(ref, child, a);
}

}  // namespace PoincareJ
