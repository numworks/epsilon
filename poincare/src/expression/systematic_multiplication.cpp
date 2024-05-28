#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "matrix.h"
#include "number.h"
#include "rational.h"
#include "simplification.h"
#include "systematic_operation.h"

namespace Poincare::Internal {

const Tree* Base(const Tree* u) { return u->isPow() ? u->child(0) : u; }

const Tree* Exponent(const Tree* u) { return u->isPow() ? u->child(1) : 1_e; }

static bool MergeMultiplicationChildWithNext(Tree* child) {
  Tree* next = child->nextTree();
  Tree* merge = nullptr;
  if (child->isNumber() && next->isNumber() &&
      !((child->isMathematicalConstant()) || next->isMathematicalConstant())) {
    // Merge numbers
    merge = Number::Multiplication(child, next);
  } else if (Base(child)->treeIsIdenticalTo(Base(next))) {
    // t^m * t^n -> t^(m+n)
    merge = PatternMatching::CreateSimplify(
        KPow(KA, KAdd(KB, KC)),
        {.KA = Base(child), .KB = Exponent(child), .KC = Exponent(next)});
    assert(!merge->isMult());
  } else if (next->isMatrix()) {
    // TODO: Maybe this should go in advanced reduction.
    merge =
        (child->isMatrix() ? Matrix::Multiplication
                           : Matrix::ScalarMultiplication)(child, next, false);
  }
  if (!merge) {
    return false;
  }
  // Replace both child and next with merge
  next->moveTreeOverTree(merge);
  child->removeTree();
  return true;
}

static bool MergeMultiplicationChildrenFrom(Tree* child, int index,
                                            int* numberOfSiblings, bool* zero) {
  bool changed = false;
  while (index < *numberOfSiblings) {
    if (child->isZero()) {
      *zero = true;
      return false;
    }
    if (child->isOne()) {
      child->removeTree();
    } else if (!(index + 1 < *numberOfSiblings &&
                 MergeMultiplicationChildWithNext(child))) {
      // Child is neither 0, 1 and can't be merged with next child (or is last).
      return changed;
    }
    (*numberOfSiblings)--;
    changed = true;
  }
  return changed;
}

static bool SimplifyMultiplicationChildRec(Tree* child, int index,
                                           int* numberOfSiblings,
                                           bool* multiplicationChanged,
                                           bool* zero) {
  assert(index < *numberOfSiblings);
  // Merge child with right siblings as much as possible.
  bool childChanged =
      MergeMultiplicationChildrenFrom(child, index, numberOfSiblings, zero);
  // Simplify starting from next child.
  if (!*zero && index + 1 < *numberOfSiblings &&
      SimplifyMultiplicationChildRec(child->nextTree(), index + 1,
                                     numberOfSiblings, multiplicationChanged,
                                     zero)) {
    // Next child changed, child may now merge with it.
    assert(!*zero);
    childChanged =
        MergeMultiplicationChildrenFrom(child, index, numberOfSiblings, zero) ||
        childChanged;
  }
  if (*zero) {
    return false;
  }
  *multiplicationChanged = *multiplicationChanged || childChanged;
  return childChanged;
}

static bool SimplifySortedMultiplication(Tree* multiplication) {
  int n = multiplication->numberOfChildren();
  bool changed = false;
  bool zero = false;
  /* Recursively merge children.
   * Keep track of n, changed status and presence of zero child. */
  SimplifyMultiplicationChildRec(multiplication->child(0), 0, &n, &changed,
                                 &zero);
  NAry::SetNumberOfChildren(multiplication, n);
  if (zero) {
    // 0 * {1, 2, 4} -> {0, 0, 0}. Same for matrices.
    Tree* zeroTree;
    Dimension dim = Dimension::GetDimension(multiplication);
    if (dim.isMatrix()) {
      zeroTree = Matrix::Zero(dim.matrix);
    } else {
      int length = Dimension::GetListLength(multiplication);
      if (length >= 0) {
        // Push ListSequence of 0s instead of a list to delay its expansion.
        zeroTree = Integer::Push(length);
        zeroTree->moveTreeOverTree(PatternMatching::Create(
            KListSequence(KVarK, KA, 0_e), {.KA = zeroTree}));
      } else {
        zeroTree = (0_e)->clone();
      }
    }
    multiplication->moveTreeOverTree(zeroTree);
    return true;
  }
  if (!changed || NAry::SquashIfPossible(multiplication)) {
    return changed;
  }
  /* Merging children can un-sort the multiplication. It must then be simplified
   * again once sorted again. For example:
   * 3*a*i*i -> Simplify -> 3*a*-1 -> Sort -> -1*3*a -> Simplify -> -3*a */
  if (NAry::Sort(multiplication, Comparison::Order::PreserveMatrices)) {
    SimplifySortedMultiplication(multiplication);
  }
  return true;
}

bool SystematicOperation::SimplifyMultiplication(Tree* u) {
  assert(u->isMult());
  bool changed = NAry::Flatten(u);
  if (changed && Simplification::CanApproximateTree(u, &changed)) {
    /* In case of successful flatten, approximateAndReplaceEveryScalar must be
     * tried again to properly handle possible new float children. */
    return true;
  }
  if (NAry::SquashIfPossible(u)) {
    return true;
  }
  changed = NAry::Sort(u, Comparison::Order::PreserveMatrices) || changed;
  changed = SimplifySortedMultiplication(u) || changed;
  if (changed && u->isMult()) {
    // Bubble-up may be unlocked after merging identical bases.
    Simplification::BubbleUpFromChildren(u);
    assert(!Simplification::ShallowSystematicReduce(u));
  }
  return changed;
}

}  // namespace Poincare::Internal
