#include "set.h"

#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

Tree* Set::PushEmpty() { return KSet.node<0>->cloneNode(); }

bool Set::Includes(const Tree* set, const Tree* expression) {
  for (const Tree* child : set->children()) {
    int comparison = Comparison::Compare(child, expression);
    if (comparison == 0) {
      return true;
    } else if (comparison > 0) {
      break;
    }
  }
  return false;
}

void Set::Add(Tree* set, const Tree* expression) {
  Tree* target = set->nextNode();
  // If set is empty, target is set's next tree
  int numberOfChildren = set->numberOfChildren();
  for (int i = 0; i < numberOfChildren; i++) {
    int comparison = Comparison::Compare(target, expression);
    if (comparison == 0) {
      return;
    } else if (comparison > 0) {
      break;
    }
    target = target->nextTree();
  }
  // target is either set's next tree or one of its children
  target->cloneTreeBeforeNode(expression);
  NAry::SetNumberOfChildren(set, numberOfChildren + 1);
}

Tree* Set::Pop(Tree* set) {
  assert(set->numberOfChildren() > 0);
  NAry::SetNumberOfChildren(set, set->numberOfChildren() - 1);
  return set->nextNode()->detachTree();
}

static Tree* MergeSets(EditionReference set0, EditionReference set1,
                       bool removeChildrenOnlyInSet0, bool pilferSet1Children,
                       bool removeCommonChildrenInSet0) {
  size_t numberOfChildren0 = set0->numberOfChildren();
  size_t numberOfChildren1 = set1->numberOfChildren();
  size_t numberOfChildren0ToScan = numberOfChildren0;
  size_t numberOfChildren1ToScan = numberOfChildren1;
  EditionReference currentChild0 = set0->nextNode();
  EditionReference currentChild1 = set1->nextNode();
  if (pilferSet1Children) {
    // Move set1 right after set0 to easily pilfer children
    set1 = set0->nextTree()->moveTreeBeforeNode(set1);
  }
  while (numberOfChildren0ToScan > 0 && numberOfChildren1ToScan > 0) {
    int comparison = Comparison::Compare(currentChild0, currentChild1);
    if (comparison < 0) {  // Increment child of set 0
      EditionReference nextChild0 = currentChild0->nextTree();
      if (removeChildrenOnlyInSet0) {
        currentChild0->removeTree();
        numberOfChildren0--;
      }
      currentChild0 = nextChild0;
      numberOfChildren0ToScan--;
    }
    if (comparison == 0) {  // Increment both children
      EditionReference nextChild0 = currentChild0->nextTree();
      EditionReference nextChild1 = currentChild1->nextTree();
      if (removeCommonChildrenInSet0) {
        currentChild0->removeTree();
        numberOfChildren0--;
      }
      if (pilferSet1Children) {
        currentChild1->removeTree();
        numberOfChildren1--;
      }
      currentChild0 = nextChild0;
      numberOfChildren0ToScan--;
      currentChild1 = nextChild1;
      numberOfChildren1ToScan--;
    }
    if (comparison > 0) {  // Increment child of set 1
      EditionReference nextChild1 = currentChild1->nextTree();
      if (pilferSet1Children) {
        currentChild0->moveTreeBeforeNode(currentChild1);
      }
      currentChild1 = nextChild1;
      numberOfChildren1ToScan--;
    }
  }
  if (pilferSet1Children) {
    set1->removeNode();
  } else {
    set1->removeTree();
    numberOfChildren1 = 0;
  }
  NAry::SetNumberOfChildren(set0, numberOfChildren0 + numberOfChildren1);
  return set0;
}

Tree* Set::Union(Tree* set0, Tree* set1) {
  return MergeSets(set0, set1, false, true, false);
}

Tree* Set::Intersection(Tree* set0, Tree* set1) {
  return MergeSets(set0, set1, true, false, false);
}

Tree* Set::Difference(Tree* set0, Tree* set1) {
  return MergeSets(set0, set1, false, false, true);
}

}  // namespace PoincareJ
