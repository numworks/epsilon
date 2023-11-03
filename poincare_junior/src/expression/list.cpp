#include "list.h"

#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/n_ary.h>

#include "k_tree.h"
#include "rational.h"
#include "simplification.h"
#include "variables.h"

namespace PoincareJ {

bool List::ProjectToNthElement(Tree* expr, int n) {
  switch (expr->type()) {
    case BlockType::SystemList:
      assert(n < expr->numberOfChildren());
      expr->moveTreeOverTree(expr->child(n));
      return true;
    case BlockType::ListSequence: {
      EditionReference value = Integer::Push(n + 1);
      Variables::Replace(expr->child(2), 0, value);
      value->removeTree();
      // sequence(k, max, f(k)) -> f(k)
      expr->removeNode();
      expr->removeTree();
      expr->removeTree();
      return true;
    }
    case BlockType::ListSort:
      assert(false);  // TODO
    default:
      if (expr->type().isListToScalar()) {
        return false;
      }
      bool changed = false;
      for (Tree* child : expr->children()) {
        changed = ProjectToNthElement(child, n) || changed;
      }
      return changed;
  }
}

Tree* List::Sum(const Tree* list) {
  Tree* result = (0_e)->clone();
  for (int i = 0; i < Size(list); i++) {
    Tree* element = list->clone();
    ProjectToNthElement(element, i);
    Simplification::DeepSystematicReduce(element);
    result->cloneNodeBeforeNode(KAdd.node<2>);
    Simplification::ShallowSystematicReduce(result);
  }
  return result;
}

Tree* List::Mean(const Tree* list) {
  Tree* result = KMult.node<2>->cloneNode();
  Sum(list);
  Rational::Push(1, Size(list));
  Simplification::ShallowSystematicReduce(result);
  return result;
}

bool List::ShallowApplyListOperators(Tree* e) {
  switch (e->type()) {
    case BlockType::Mean:
      e->moveTreeOverTree(List::Mean(e->child(0)));
      return true;
    case BlockType::ListSum:
      e->moveTreeOverTree(List::Sum(e->child(0)));
      return true;
    default:
      return false;
  }
}

}  // namespace PoincareJ
