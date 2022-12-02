#include "set.h"
#include <poincare_junior/src/n_ary.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/node_iterator.h>

namespace Poincare {

bool Set::Includes(EditionReference set, Node expression) {
  for (auto [setChild, index] : NodeIterator::Children<Forward, NoEditable>(set.node())) {
    if (Simplification::Compare(setChild, expression) == 0) {
      return true;
    }
  }
  return false;
}

EditionReference Set::Add(EditionReference set, Node expression) {
  EditionReference child = set;
  for (auto [ref, index] : NodeIterator::Children<Forward, Editable>(set)) {
    child = ref;
    int comparison = Simplification::Compare(ref.node(), expression);
    if (comparison == 0) {
      return set;
    } else if (comparison > 0) {
      ref.insertTreeBeforeNode(expression);
      break;
    }
  }
  if (child == set || Simplification::Compare(child.node(), expression) < 0) {
    // Empty set or all elements are < expression
    child.nextTree().insertTreeBeforeNode(expression);
  }
  NAry::SetNumberOfChildren(set, set.numberOfChildren() + 1);
  return set;
}

EditionReference Set::Union(EditionReference set0, EditionReference set1) {
  size_t numberOfChildren0 = set0.numberOfChildren();
  size_t numberOfChildren1 = set1.numberOfChildren();
  size_t numberOfChildren0ToScan = numberOfChildren0;
  size_t numberOfChildren1ToScan = numberOfChildren1;
  EditionReference currentChild0 = set0.nextNode();
  EditionReference currentChild1 = set1.nextNode();
  while (numberOfChildren0ToScan > 0 && numberOfChildren1ToScan > 0) {
    int comparison = Simplification::Compare(currentChild0.node(), currentChild1.node());
    if (comparison >= 0) {
      EditionReference nextChild1 = currentChild1.nextTree();
      if (comparison > 0) {
        currentChild0.insertTreeBeforeNode(currentChild1);
      } else if (comparison == 0) {
        currentChild1.removeTree();
        numberOfChildren1--;
      }
      currentChild1 = nextChild1;
      numberOfChildren1ToScan--;
    } else {
      currentChild0 = currentChild0.nextTree();
      numberOfChildren0ToScan--;
    }
  }
  set1.removeNode();
  NAry::SetNumberOfChildren(set0, numberOfChildren0 + numberOfChildren1);
  return set0;
}

EditionReference Set::Intersection(EditionReference set0, EditionReference set1) {
  size_t numberOfChildren0 = set0.numberOfChildren();
  size_t numberOfChildren1 = set1.numberOfChildren();
  size_t numberOfChildren0ToScan = numberOfChildren0;
  size_t numberOfChildren1ToScan = numberOfChildren1;
  EditionReference currentChild0 = set0.nextNode();
  EditionReference currentChild1 = set1.nextNode();
  while (numberOfChildren0ToScan > 0 && numberOfChildren1ToScan > 0) {
    int comparison = Simplification::Compare(currentChild0.node(), currentChild1.node());
    if (comparison >= 0) {
      currentChild1 = currentChild1.nextTree();
      numberOfChildren1ToScan--;
    }
    if (comparison <= 0) {
      EditionReference nextChild0 = currentChild0.nextTree();
      if (comparison < 0) {
        currentChild0.removeTree();
        numberOfChildren0--;
      }
      currentChild0 = nextChild0;
      numberOfChildren0ToScan--;
    }

  }
  set1.removeTree();
  NAry::SetNumberOfChildren(set0, numberOfChildren0);
  return set0;
}

EditionReference Set::Difference(EditionReference set0, EditionReference set1) {
  size_t numberOfChildren0 = set0.numberOfChildren();
  size_t numberOfChildren1 = set1.numberOfChildren();
  size_t numberOfChildren0ToScan = numberOfChildren0;
  size_t numberOfChildren1ToScan = numberOfChildren1;
  EditionReference currentChild0 = set0.nextNode();
  EditionReference currentChild1 = set1.nextNode();
  while (numberOfChildren0ToScan > 0 && numberOfChildren1ToScan > 0) {
    int comparison = Simplification::Compare(currentChild0.node(), currentChild1.node());
    if (comparison < 0) {
      currentChild0 = currentChild0.nextTree();
      numberOfChildren0ToScan--;
    }
    if (comparison == 0) {
      EditionReference nextChild0 = currentChild0.nextTree();
      currentChild0.removeTree();
      numberOfChildren0--;
      currentChild0 = nextChild0;
      numberOfChildren0ToScan--;
    }
    if (comparison > 0) {
      currentChild1 = currentChild1.nextTree();
      numberOfChildren1ToScan--;
    }

  }
  set1.removeTree();
  NAry::SetNumberOfChildren(set0, numberOfChildren0);
  return set0;
}

}
