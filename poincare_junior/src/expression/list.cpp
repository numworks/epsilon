#include "list.h"

#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/n_ary.h>

#include "k_tree.h"
#include "rational.h"
#include "variables.h"

namespace PoincareJ {

Tree* List::PushEmpty() { return KList.node<0>->cloneNode(); }

bool List::ProjectToNthElement(Tree* expr, int n, Tree::Operation reduction) {
  switch (expr->type()) {
    case Type::List:
      assert(n < expr->numberOfChildren());
      expr->moveTreeOverTree(expr->child(n));
      return true;
    case Type::ListSequence: {
      if (Parametric::HasLocalRandom(expr)) {
        return false;
      }
      TreeRef value = Integer::Push(n + 1);
      Variables::Replace(expr->child(2), 0, value);
      value->removeTree();
      // sequence(k, max, f(k)) -> f(k)
      expr->removeNode();
      expr->removeTree();
      expr->removeTree();
      return true;
    }
    case Type::RandIntNoRep:
      return false;  // Should be projected on approximation.
    case Type::ListSort:
    case Type::Median:
      assert(false);  // Must have been removed by simplification
    default:
      if (expr->type().isListToScalar()) {
        return false;
      }
      bool changed = false;
      for (Tree* child : expr->children()) {
        changed = ProjectToNthElement(child, n, reduction) || changed;
      }
      if (changed) {
        reduction(expr);
      }
      return changed;
  }
}

Tree* List::Fold(const Tree* list, TypeBlock type) {
  Tree* result = Tree::FromBlocks(SharedEditionPool->lastBlock());
  // TODO compute GetListLength less often
  int size = Dimension::GetListLength(list);
  assert(size >= 0);
  if (size == 0) {
    assert(type.isListSum() || type.isListProduct());
    (type.isListSum() ? 0_e : 1_e)->clone();
  }
  for (int i = 0; i < size; i++) {
    Tree* element = list->clone();
    if (!ProjectToNthElement(element, i,
                             Simplification::ShallowSystematicReduce)) {
      assert(false);
    }
    if (i == 0) {
      continue;
    }
    if (type.isListSum() || type.isListProduct()) {
      const Tree* node = type.isListSum() ? KAdd.node<2> : KMult.node<2>;
      result->cloneNodeBeforeNode(node);
      Simplification::ShallowSystematicReduce(result);
    } else {
      assert(type.isMinimum() || type.isMaximum());
      // TODO_PCJ: we need a natural order not a comparison
      if (Comparison::Compare(element, result) ==
          ((type.isMaximum()) ? 1 : -1)) {
        result->removeTree();
      } else {
        element->removeTree();
      }
    }
  }
  return result;
}

Tree* List::Variance(const Tree* list, const Tree* coefficients,
                     TypeBlock type) {
  // var(L) = mean(L^2) - mean(L)^2
  KTree variance =
      KAdd(KMean(KPow(KA, 2_e), KB), KMult(-1_e, KPow(KMean(KA, KB), 2_e)));
  // sqrt(var)
  KTree stdDev = KPow(variance, KHalf);
  // stdDev * sqrt(1 + 1 / (n - 1))
  KTree sampleStdDev =
      KPow(KMult(stdDev, KAdd(1_e, KPow(KAdd(KC, -1_e), -1_e))), KHalf);
  if (type.isSampleStdDev()) {
    Tree* n = coefficients->isOne()
                  ? Integer::Push(Dimension::GetListLength(list))
                  : Fold(coefficients, Type::ListSum);
    PatternMatching::CreateAndSimplify(
        sampleStdDev, {.KA = list, .KB = coefficients, .KC = n});
    n->removeTree();
    return n;
  } else {
    assert(type.isVariance() || type.isStdDev());
    return PatternMatching::CreateAndSimplify(
        type == Type::Variance ? variance : stdDev,
        {.KA = list, .KB = coefficients});
  }
}

Tree* List::Mean(const Tree* list, const Tree* coefficients) {
  if (coefficients->isOne()) {
    Tree* result = KMult.node<2>->cloneNode();
    Fold(list, Type::ListSum);
    Rational::Push(1, Dimension::GetListLength(list));
    Simplification::ShallowSystematicReduce(result);
    return result;
  }
  return PatternMatching::CreateAndSimplify(
      KMult(KListSum(KMult(KA, KB)), KPow(KListSum(KB), -1_e)),
      {.KA = list, .KB = coefficients});
}

bool List::BubbleUp(Tree* expr, Tree::Operation reduction) {
  int length = Dimension::GetListLength(expr);
  if (length < 0 || expr->isList()) {
    return false;
  }
  Tree* list = List::PushEmpty();
  for (int i = 0; i < length; i++) {
    Tree* element = expr->clone();
    if (!ProjectToNthElement(element, i, reduction)) {
      assert(i == 0);
      element->removeTree();
      list->removeTree();
      return false;
    }
  }
  NAry::SetNumberOfChildren(list, length);
  expr->moveTreeOverTree(list);
  return true;
}

bool List::ShallowApplyListOperators(Tree* e) {
  switch (e->type()) {
    case Type::ListSum:
    case Type::ListProduct:
    case Type::Minimum:
    case Type::Maximum:
      e->moveTreeOverTree(Fold(e->child(0), e->type()));
      return true;
    case Type::Mean:
      e->moveTreeOverTree(Mean(e->child(0), e->child(1)));
      return true;
    case Type::Variance:
    case Type::StdDev:
    case Type::SampleStdDev: {
      e->moveTreeOverTree(Variance(e->child(0), e->child(1), e->type()));
      return true;
    }
    case Type::ListSort:
    case Type::Median: {
      Tree* list = e->child(0);
      BubbleUp(list, Simplification::ShallowSystematicReduce);
      NAry::Sort(list);
      if (e->isMedian()) {
        if (!e->child(1)->isOne()) {
          // TODO_PCJ: not implemeted yet
          // Median with coefficients needs statistics_dataset
          assert(false);
        }
        int n = list->numberOfChildren();
        if (n % 2) {
          e->moveTreeOverTree(list->child(n / 2));
        } else {
          e->moveTreeOverTree(PatternMatching::CreateAndSimplify(
              KMult(KHalf, KAdd(KA, KB)),
              {.KA = list->child(n / 2 - 1), .KB = list->child(n / 2)}));
        }
      } else {
        e->removeNode();
      }
      return true;
    }
    case Type::ListElement: {
      int i = Integer::Handler(e->child(1)).to<uint8_t>();
      if (i < 1 || i > e->child(0)->numberOfChildren()) {
        e->cloneTreeOverTree(KUndef);
        return true;
      }
      if (!ProjectToNthElement(e->child(0), i - 1,
                               Simplification::ShallowSystematicReduce)) {
        return false;
      }
      e->moveTreeOverTree(e->child(0));
      return true;
    }
    case Type::ListSlice:
      // TODO PCJ
      return false;
    case Type::Dim:
      e->moveTreeOverTree(Integer::Push(Dimension::GetListLength(e->child(0))));
      return true;
    default:
      return false;
  }
}

}  // namespace PoincareJ
