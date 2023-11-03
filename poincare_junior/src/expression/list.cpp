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

Tree* List::Fold(const Tree* list, BlockType type) {
  Tree* result = Tree::FromBlocks(SharedEditionPool->lastBlock());
  // TODO compute GetListLength less often
  size_t size = Dimension::GetListLength(list);
  for (int i = 0; i < size; i++) {
    Tree* element = list->clone();
    ProjectToNthElement(element, i);
    Simplification::DeepSystematicReduce(element);
    if (i == 0) {
      continue;
    }
    if (type == BlockType::ListSum || type == BlockType::ListProduct) {
      const Tree* node =
          type == BlockType::ListSum ? KAdd.node<2> : KMult.node<2>;
      result->cloneNodeBeforeNode(node);
      Simplification::ShallowSystematicReduce(result);
    } else {
      assert(type == BlockType::Minimum || type == BlockType::Maximum);
      // TODO we need a natural order not a comparison
      if (Comparison::Compare(element, result) ==
          ((type == BlockType::Maximum) ? 1 : -1)) {
        result->removeTree();
      } else {
        element->removeTree();
      }
    }
  }
  return result;
}

Tree* List::Mean(const Tree* list) {
  Tree* result = KMult.node<2>->cloneNode();
  Fold(list, BlockType::ListSum);
  Rational::Push(1, Dimension::GetListLength(list));
  Simplification::ShallowSystematicReduce(result);
  return result;
}

bool List::ShallowApplyListOperators(Tree* e) {
  switch (e->type()) {
    case BlockType::ListSum:
    case BlockType::ListProduct:
    case BlockType::Minimum:
    case BlockType::Maximum:
      e->moveTreeOverTree(List::Fold(e->child(0), e->type()));
      return true;
    case BlockType::Mean:
      e->moveTreeOverTree(List::Mean(e->child(0)));
      return true;
    case BlockType::Variance: {
      /* var(L) = mean(L^2) - mean(L)^2 */
      Tree* variance = PatternMatching::CreateAndSimplify(
          KAdd(KMean(KPow(KA, 2_e)), KMult(-1_e, KPow(KMean(KA), 2_e))),
          {.KA = e->child(0)});
      if (variance->type().isRational()) {
        e->moveTreeOverTree(variance);
      }
      return true;
    }
    default:
      return false;
  }
}

}  // namespace PoincareJ
