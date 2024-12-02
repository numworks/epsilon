#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "dependency.h"
#include "infinity.h"
#include "k_tree.h"
#include "matrix.h"
#include "number.h"
#include "rational.h"
#include "systematic_operation.h"
#include "systematic_reduction.h"
#include "units/unit.h"

namespace Poincare::Internal {

const Tree* Base(const Tree* e) { return e->isPow() ? e->child(0) : e; }

const Tree* Exponent(const Tree* e) { return e->isPow() ? e->child(1) : 1_e; }

static bool approximationIsFinite(const Tree* e) {
  if (!Approximation::CanApproximate(e)) {
    return false;
  }
  std::complex<double> approx =
      Approximation::ToComplex<double>(e, Approximation::Parameters{});
  return std::isfinite(approx.real()) && std::isfinite(approx.imag());
}

static bool MergeMultiplicationChildWithNext(Tree* child,
                                             int* numberOfDependencies) {
  Tree* next = child->nextTree();
  Tree* merge = nullptr;
  if (child->isZero()) {
    if (next->isInf()) {
      // 0 * inf -> undef
      merge = KUndef->cloneTree();
    } else {
      if (Dimension::IsNonListScalar(next)) {
        if (approximationIsFinite(next)) {
          // 0 * x -> 0
          next->removeTree();
        } else {
          // 0 * x -> dep(0, {0 * x})
          SharedTreeStack->pushMult(2);
          child->cloneTree();
          next->detachTree();
          (*numberOfDependencies)++;
        }
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
    merge = (child->isMatrix()
                 ? Matrix::Multiplication
                 : Matrix::ScalarMultiplication)(child, next, false, nullptr);
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
                                            int* numberOfSiblings,
                                            int* numberOfDependencies) {
  assert(*numberOfSiblings > 0);
  bool changed = false;
  while (index < *numberOfSiblings) {
    if (!(index + 1 < *numberOfSiblings &&
          MergeMultiplicationChildWithNext(child, numberOfDependencies))) {
      // Child is neither 0, 1 and can't be merged with next child (or is last).
      return changed;
    }
    (*numberOfSiblings)--;
    assert(*numberOfSiblings > 0);
    changed = true;
  }
  return changed;
}

static bool ReduceMultiplicationChildRec(Tree* child, int index,
                                         int* numberOfSiblings,
                                         bool* multiplicationChanged,
                                         int* numberOfDependencies) {
  assert(*numberOfSiblings > 0);
  assert(index < *numberOfSiblings);
  // Merge child with right siblings as much as possible.
  bool childChanged = MergeMultiplicationChildrenFrom(
      child, index, numberOfSiblings, numberOfDependencies);
  // Simplify starting from next child.
  if (index + 1 < *numberOfSiblings &&
      ReduceMultiplicationChildRec(child->nextTree(), index + 1,
                                   numberOfSiblings, multiplicationChanged,
                                   numberOfDependencies)) {
    // Next child changed, child may now merge with it.
    childChanged = MergeMultiplicationChildrenFrom(
                       child, index, numberOfSiblings, numberOfDependencies) ||
                   childChanged;
  }
  *multiplicationChanged = *multiplicationChanged || childChanged;
  return childChanged;
}

static bool ReduceMultiplicationWithInf(Tree* e) {
  // TODO_PCJ: what about x complex? sign is not defined on complexes
  // x*inf -> sign(x)*inf
  // Except when x = -1,0,1 or sign (to avoid infinite loop)
  PatternMatching::Context ctx;
  if (PatternMatching::Match(e, KMult(KA, KInf), &ctx) ||
      PatternMatching::Match(e, KMult(KInf, KA), &ctx)) {
    const Tree* x = ctx.getTree(KA);
    assert(!x->isZero() && !x->isOne());
    if (x->isMinusOne() || x->isSign()) {
      return false;
    }
  }
  // Except when x is not scalar.
  if (Dimension::Get(e).isScalar() &&
      PatternMatching::MatchReplaceSimplify(
          e, KMult(KA_s, KInf, KB_s), KMult(KSign(KMult(KA_s, KB_s)), KInf))) {
    /* Warning: it works because sign(z)=undef if z is complex and we don't
     * handle i*inf.*/
    return true;
  }
  return false;
}

bool SystematicOperation::ReduceSortedMultiplication(Tree* e) {
  assert(e->isMult());
  int n = e->numberOfChildren();
  bool changed = false;
  assert(n > 1);
  /* Recursively merge children.
   * Keep track of n and changed status. */
  int numberOfDependencies = 0;
  TreeRef depList = SharedTreeStack->pushDepList(numberOfDependencies);
  ReduceMultiplicationChildRec(e->child(0), 0, &n, &changed,
                               &numberOfDependencies);
  assert(n > 0);
  NAry::SetNumberOfChildren(e, n);

  Tree* mult = e;
  if (numberOfDependencies > 0) {
    NAry::SetNumberOfChildren(depList, numberOfDependencies);
    // TODO: create moveTreeAfterNode
    e->nextTree()->moveTreeBeforeNode(depList);
    e->cloneNodeAtNode(KDep);
    mult = Dependency::Main(e);
  } else {
    depList->removeTree();
  }
  assert(mult->isMult());

  if (mult->child(0)->isZero()) {
    Dimension dim = Dimension::Get(mult);
    if (dim.isUnit()) {
      // 0 * 0 * 2 * (m + km) * m -> 0 * m^2
      // Use hash because change is too complex to track.
      uint32_t hash = changed ? 0 : mult->hash();
      if (dim.hasNonKelvinTemperatureUnit()) {
        /* Temperature exception : 0*_°C != 0*K : unit must be preserved.
         * Taking advantage of the fact only very simple expressions of such
         * temperatures are allowed. */
        assert(dim.unit.vector.supportSize() == 1 &&
               dim.unit.vector.temperature == 1);
        mult->moveTreeOverTree(Units::Unit::Push(dim.unit.representative));
        mult->moveTreeOverTree(
            PatternMatching::Create(KMult(0_e, KA), {.KA = mult}));
      } else {
        // Since all units are equivalent, use base SI.
        mult->moveTreeOverTree(Units::Unit::GetBaseUnits(dim.unit.vector));
        if (mult->isMult()) {
          NAry::Sort(mult, Order::OrderType::PreserveMatrices);
        } else {
          mult->cloneNodeAtNode(KMult.node<1>);
        }
        NAry::AddChildAtIndex(mult, (0_e)->cloneTree(), 0);
      }
      return changed || (hash != mult->hash());
    }
  }

  if (changed && NAry::SquashIfPossible(mult)) {
    return true;
  }

  if (ReduceMultiplicationWithInf(mult)) {
    if (mult->isDep()) {
      // e can also be a dep, we need to bubble up
      Dependency::ShallowBubbleUpDependencies(e);
    }
    return true;
  }
  assert(mult->isMult());

  if (!changed) {
    return false;
  }

  /* Merging children can un-sort the multiplication. It must then be simplified
   * again once sorted again. For example:
   * 3*a*i*i -> Simplify -> 3*a*-1 -> Sort -> -1*3*a -> Simplify -> -3*a */
  if (NAry::Sort(mult, Order::OrderType::PreserveMatrices)) {
    ReduceSortedMultiplication(mult);
  }
  return true;
}

}  // namespace Poincare::Internal
