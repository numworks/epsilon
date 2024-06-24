#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "infinity.h"
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
  if (child->isZero()) {
    if (next->isInf()) {
      // 0 * inf -> undef
      merge = KUndef->cloneTree();
    } else {
      Dimension nextDim = Dimension::Get(next);
      if (!nextDim.isMatrix() && !nextDim.isUnit() &&
          !Dimension::IsList(next)) {
        // 0 * x -> 0
        next->removeTree();
        return true;
      }
    }
  }
  if (child->isOne() || (child->isInf() && next->isInf())) {
    // 1 * x -> x
    // inf * inf -> inf
    child->removeTree();
    return true;
  } else if (child->isRationalOrFloat() && next->isRationalOrFloat()) {
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
                                            int* numberOfSiblings) {
  assert(*numberOfSiblings > 0);
  bool changed = false;
  while (index < *numberOfSiblings) {
    if (!(index + 1 < *numberOfSiblings &&
          MergeMultiplicationChildWithNext(child))) {
      // Child is neither 0, 1 and can't be merged with next child (or is last).
      return changed;
    }
    (*numberOfSiblings)--;
    assert(*numberOfSiblings > 0);
    changed = true;
  }
  return changed;
}

static bool SimplifyMultiplicationChildRec(Tree* child, int index,
                                           int* numberOfSiblings,
                                           bool* multiplicationChanged) {
  assert(*numberOfSiblings > 0);
  assert(index < *numberOfSiblings);
  // Merge child with right siblings as much as possible.
  bool childChanged =
      MergeMultiplicationChildrenFrom(child, index, numberOfSiblings);
  // Simplify starting from next child.
  if (index + 1 < *numberOfSiblings &&
      SimplifyMultiplicationChildRec(child->nextTree(), index + 1,
                                     numberOfSiblings, multiplicationChanged)) {
    // Next child changed, child may now merge with it.
    childChanged =
        MergeMultiplicationChildrenFrom(child, index, numberOfSiblings) ||
        childChanged;
  }
  *multiplicationChanged = *multiplicationChanged || childChanged;
  return childChanged;
}

static bool SimplifyMultiplicationWithInf(Tree* e) {
  // x*inf -> sign(x)*inf
  // except when x = -1,0,1 or sign (to avoid infinite loop)
  PatternMatching::Context ctx;
  if (PatternMatching::Match(KMult(KA, KInf), e, &ctx) ||
      PatternMatching::Match(KMult(KInf, KA), e, &ctx)) {
    const Tree* x = ctx.getTree(KA);
    assert(!x->isZero() && !x->isOne());
    if (x->isMinusOne() || x->isSign()) {
      return false;
    }
  }
  if (PatternMatching::MatchReplaceSimplify(
          e, KMult(KA_s, KInf, KB_s), KMult(KSign(KMult(KA_s, KB_s)), KInf))) {
    /* Warning: it works because sign(z)=undef if z is complex and we don't
     * handle i*inf.*/
    return true;
  }

  return false;
}

static bool SimplifySortedMultiplication(Tree* multiplication) {
  int n = multiplication->numberOfChildren();
  bool changed = false;
  assert(n > 1);
  /* Recursively merge children.
   * Keep track of n and changed status. */
  SimplifyMultiplicationChildRec(multiplication->child(0), 0, &n, &changed);
  assert(n > 0);
  NAry::SetNumberOfChildren(multiplication, n);
  if (multiplication->child(0)->isZero()) {
    Dimension dim = Dimension::Get(multiplication);
    if (dim.isUnit()) {
      // 0 * 0 * 2 * (m + km) * m -> 0 * m^2
      // Use hash because change is too complex to track.
      uint32_t hash = changed ? 0 : multiplication->hash();
      if (dim.hasNonKelvinTemperatureUnit()) {
        /* Temperature exception : 0*_°C != 0*K : unit must be preserved.
         * Taking advantage of the fact only very simple expressions of such
         * temperatures are allowed. */
        assert(dim.unit.vector.supportSize() == 1 &&
               dim.unit.vector.temperature == 1);
        multiplication->moveTreeOverTree(PatternMatching::Create(
            KMult(0_e, KA),
            {.KA = Units::Unit::Push(dim.unit.representative)}));
      } else {
        // Since all units are equivalent, use base SI.
        multiplication->moveTreeOverTree(dim.unit.vector.toBaseUnits());
        if (multiplication->isMult()) {
          NAry::Sort(multiplication, Comparison::Order::PreserveMatrices);
        } else {
          multiplication->cloneNodeAtNode(KMult.node<1>);
        }
        NAry::AddChildAtIndex(multiplication, (0_e)->cloneTree(), 0);
      }
      return changed || (hash != multiplication->hash());
    }
  }

  if (changed && NAry::SquashIfPossible(multiplication)) {
    return true;
  }

  if (SimplifyMultiplicationWithInf(multiplication)) {
    changed = true;
  }

  if (!changed) {
    return false;
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
  if (changed && CanApproximateTree(u, &changed)) {
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
