#include "n_ary.h"

#include <assert.h>
#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/memory/edition_reference.h>
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
void NAry::AddOrMergeChildAtIndex(Node* naryNode, Node* childNode, int index) {
  EditionReference nary = naryNode;
  EditionReference child = childNode;
  AddChildAtIndex(nary, child, index);
  if (nary->type() == child->type()) {
    size_t numberOfChildren =
        nary->numberOfChildren() + child->numberOfChildren() - 1;
    child->removeNode();
    SetNumberOfChildren(nary, numberOfChildren);
  }
}

Node* NAry::DetachChildAtIndex(Node* nary, int index) {
  assert(nary->isNAry());
  Node* child = nary->childAtIndex(index)->detachTree();
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

bool NAry::SquashIfUnary(Node* nary) {
  if (nary->numberOfChildren() == 1) {
    nary->moveTreeOverTree(nary->nextNode());
    return true;
  }
  return false;
}

bool NAry::SquashIfEmpty(Node* nary) {
  if (nary->numberOfChildren() >= 1) {
    return false;
  }
  // Return the neutral element
  BlockType type = nary->type();
  assert(type == BlockType::Addition || type == BlockType::Multiplication);
  nary->cloneTreeOverTree(
      Node::FromBlocks(type == BlockType::Addition ? &ZeroBlock : &OneBlock));
  return true;
}

bool NAry::Sanitize(Node* nary) {
  bool flattened = Flatten(nary);
  if (nary->numberOfChildren() == 0) {
    return SquashIfEmpty(nary) || flattened;
  }
  return SquashIfUnary(nary) || flattened;
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
      child0->moveTreeAtNode(child1);
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
  Node* newNAry = editionPool->clone(nary, false);
  for (int i = 0; i < numberOfChildren; i++) {
    children[indexes[i]]->clone();
  }
  assert(nary->treeSize() == newNAry->treeSize());
  // replace nary with the sorted one
  nary->moveTreeOverTree(newNAry);
  return true;
}

void NAry::SortedInsertChild(Node* nary, Node* child, Comparison::Order order) {
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
  AddChildAtIndex(nary, child, a);
}

}  // namespace PoincareJ
