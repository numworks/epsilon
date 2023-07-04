#include "n_ary.h"

#include <assert.h>
#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/memory/node_iterator.h>

namespace PoincareJ {

void NAry::AddChildAtIndex(Node* nary, Node* child, int index) {
  assert(nary->isNAry());
  Node* insertionPoint = index == nary->numberOfChildren()
                             ? nary->nextTree()
                             : nary->childAtIndex(index);
  SetNumberOfChildren(nary, nary->numberOfChildren() + 1);
  insertionPoint->moveTreeBeforeNode(child);
}

// Should these useful refs be hidden in the function ?
void NAry::AddOrMergeChildAtIndex(EditionReference nary, EditionReference child,
                                  int index) {
  AddChildAtIndex(nary, child, index);
  if (static_cast<Node*>(nary)->type() == static_cast<Node*>(child)->type()) {
    size_t numberOfChildren =
        nary->numberOfChildren() + child->numberOfChildren() - 1;
    child->removeNode();
    SetNumberOfChildren(nary, numberOfChildren);
  }
}

EditionReference NAry::DetachChildAtIndex(Node* nary, int index) {
  assert(nary->isNAry());
  EditionReference child = nary->childAtIndex(index);
  child->detachTree();
  SetNumberOfChildren(nary, nary->numberOfChildren() - 1);
  return child;
}

void NAry::RemoveChildAtIndex(Node* nary, int index) {
  assert(nary->isNAry());
  nary->childAtIndex(index)->removeTree();
  SetNumberOfChildren(nary, nary->numberOfChildren() - 1);
}

void NAry::SetNumberOfChildren(Node* nary, size_t numberOfChildren) {
  assert(nary->isNAry());
  assert(numberOfChildren < UINT8_MAX);
  Block* numberOfChildrenBlock = nary->block()->next();
  *numberOfChildrenBlock = numberOfChildren;
}

bool NAry::Flatten(Node* nary) {
  assert(nary->isNAry());
  bool modified = false;
  size_t numberOfChildren = nary->numberOfChildren();
  size_t childIndex = 0;
  Node* child = nary->nextNode();
  while (childIndex < numberOfChildren) {
    if (nary->type() == child->type()) {
      modified = true;
      numberOfChildren += child->numberOfChildren() - 1;
      child->removeNode();
    } else {
      child = child->nextTree();
      childIndex++;
    }
  }
  if (modified) {
    SetNumberOfChildren(nary, numberOfChildren);
    return true;
  }
  return false;
}

bool NAry::SquashIfUnary(EditionReference& reference) {
  if (reference->numberOfChildren() == 1) {
    reference = reference->moveTreeOverTree(reference->nextNode());
    return true;
  }
  return false;
}

bool NAry::SquashIfEmpty(EditionReference& reference) {
  if (reference->numberOfChildren() >= 1) {
    return false;
  }
  // Return the neutral element
  BlockType type = reference->type();
  assert(type == BlockType::Addition || type == BlockType::Multiplication);
  reference = reference->cloneTreeOverTree(
      Node::FromBlocks(type == BlockType::Addition ? &ZeroBlock : &OneBlock));
  return true;
}

bool NAry::Sanitize(EditionReference& reference) {
  bool flattened = Flatten(reference);
  if (reference->numberOfChildren() == 0) {
    return SquashIfEmpty(reference) || flattened;
  }
  return SquashIfUnary(reference) || flattened;
}

bool NAry::Sort(Node* nary, Comparison::Order order) {
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
  // Sort a list of indexes first
  void* contextArray[] = {&indexes, &children, &order};
  List::Sort(
      [](int i, int j, void* context, int numberOfElements) {
        void** contextArray = static_cast<void**>(context);
        uint8_t* indexes = static_cast<decltype(indexes)>(contextArray[0]);
        uint8_t s = indexes[i];
        indexes[i] = indexes[j];
        indexes[j] = s;
        // std::swap(&indexes[i], &indexes[j]);
      },
      [](int i, int j, void* context, int numberOfElements) {
        void** contextArray = static_cast<void**>(context);
        uint8_t* indexes = static_cast<decltype(indexes)>(contextArray[0]);
        const Node** children =
            static_cast<decltype(children)>(contextArray[1]);
        Comparison::Order order =
            *static_cast<Comparison::Order*>(contextArray[2]);
        return Comparison::Compare(children[indexes[i]], children[indexes[j]],
                                   order) >= 0;
      },
      contextArray, numberOfChildren);
  // TODO use the sort from stdlib instead
  /* std::sort(&indexes[0], &indexes[numberOfChildren], [&](uint8_t a, uint8_t
   * b) { return Comparison::Compare(children[a], children[b], order) < 0;
   * }); */
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

bool NAry::Sort(EditionReference& reference, Comparison::Order order) {
  Node* u = reference;
  bool result = Sort(u, order);
  reference = u;
  return result;
}

void NAry::SortChildren(EditionReference reference, Comparison::Order order) {
  // Non simple NArys (Polynomial) rely on children order.
  assert(reference->block()->isSimpleNAry());
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
        EditionReference refJNext = refJ->nextTree();
        refI->moveTreeBeforeNode(refJ);
        refJNext->moveTreeBeforeNode(refI);
      },
      [](int i, int j, void* context, int numberOfElements) {
        void** contextArray = static_cast<void**>(context);
        Node* nary = *static_cast<Node**>(contextArray[0]);
        Comparison::Order order =
            *static_cast<Comparison::Order*>(contextArray[1]);
        return Comparison::Compare(nary->childAtIndex(i), nary->childAtIndex(j),
                                   order) >= 0;
      },
      contextArray, reference->numberOfChildren());
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
