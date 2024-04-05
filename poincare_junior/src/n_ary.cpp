#include "n_ary.h"

#include <assert.h>
#include <omgpj/list.h>
#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/tree_ref.h>

namespace PoincareJ {

void NAry::AddChildAtIndex(Tree* nary, Tree* child, int index) {
  assert(nary->isNAry());
  Tree* insertionPoint =
      index == nary->numberOfChildren() ? nary->nextTree() : nary->child(index);
  SetNumberOfChildren(nary, nary->numberOfChildren() + 1);
  insertionPoint->moveTreeBeforeNode(child);
}

// Should these useful refs be hidden in the function ?
void NAry::AddOrMergeChildAtIndex(Tree* naryNode, Tree* childNode, int index) {
  TreeRef nary = naryNode;
  TreeRef child = childNode;
  AddChildAtIndex(nary, child, index);
  if (nary->type() == child->type()) {
    size_t numberOfChildren =
        nary->numberOfChildren() + child->numberOfChildren() - 1;
    child->removeNode();
    SetNumberOfChildren(nary, numberOfChildren);
  }
}

Tree* NAry::DetachChildAtIndex(Tree* nary, int index) {
  assert(nary->isNAry());
  TreeRef child = nary->child(index);
  child->detachTree();
  SetNumberOfChildren(nary, nary->numberOfChildren() - 1);
  return child;
}

void NAry::RemoveChildAtIndex(Tree* nary, int index) {
  assert(nary->isNAry());
  nary->child(index)->removeTree();
  SetNumberOfChildren(nary, nary->numberOfChildren() - 1);
}

void NAry::SetNumberOfChildren(Tree* nary, size_t numberOfChildren) {
  assert(nary->isNAry());
  assert(numberOfChildren < UINT16_MAX);
  if (nary->isNAry16()) {
    nary->setNodeValue(0, numberOfChildren % 256);
    nary->setNodeValue(1, numberOfChildren / 256);
  } else {
    nary->setNodeValue(0, numberOfChildren);
  }
}

bool NAry::Flatten(Tree* nary) {
  assert(nary->isNAry());
  bool modified = false;
  size_t numberOfChildren = nary->numberOfChildren();
  size_t childIndex = 0;
  Tree* child = nary->nextNode();
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

bool NAry::SquashIfUnary(Tree* nary) {
  if (nary->numberOfChildren() == 1) {
    nary->moveTreeOverTree(nary->nextNode());
    return true;
  }
  return false;
}

bool NAry::SquashIfEmpty(Tree* nary) {
  if (nary->numberOfChildren() >= 1) {
    return false;
  }
  // Return the neutral element
  Type type = nary->type();
  assert(type == Type::Addition || type == Type::Multiplication);
  nary->cloneTreeOverTree(type == Type::Addition ? 0_e : 1_e);
  return true;
}

bool NAry::Sanitize(Tree* nary) {
  bool flattened = Flatten(nary);
  return SquashIfPossible(nary) || flattened;
}

bool NAry::Sort(Tree* nary, Comparison::Order order) {
  const uint8_t numberOfChildren = nary->numberOfChildren();
  if (numberOfChildren < 2) {
    return false;
  }
  if (numberOfChildren == 2) {
    Tree* child0 = nary->nextNode();
    Tree* child1 = child0->nextTree();
    if (Comparison::Compare(child0, child1, order) > 0) {
      child0->moveTreeAtNode(child1);
      return true;
    }
    return false;
  }
  const Tree* children[k_maxNumberOfChildren];
  uint8_t indexes[k_maxNumberOfChildren];
  for (uint8_t index = 0; const Tree* child : nary->children()) {
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
        const Tree** children =
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
  Tree* newNAry = SharedTreeStack->clone(nary, false);
  for (int i = 0; i < numberOfChildren; i++) {
    children[indexes[i]]->clone();
  }
  assert(nary->treeSize() == newNAry->treeSize());
  // replace nary with the sorted one
  nary->moveTreeOverTree(newNAry);
  return true;
}

void NAry::SortedInsertChild(Tree* nary, Tree* child, Comparison::Order order) {
  Tree* children[k_maxNumberOfChildren];
  for (uint8_t index = 0; const Tree* child : nary->children()) {
    children[index++] = const_cast<Tree*>(child);
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

Tree* NAry::CloneSubRange(const Tree* nary, int startIndex, int endIndex) {
  Tree* result = nary->cloneNode();
  int nb = endIndex - startIndex;
  SetNumberOfChildren(result, nb);
  const Tree* child = nary->child(startIndex);
  for (int i = 0; i < nb; i++) {
    child->clone();
    child = child->nextTree();
  }
  return result;
}

}  // namespace PoincareJ
