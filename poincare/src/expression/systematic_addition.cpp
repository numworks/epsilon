#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "infinity.h"
#include "matrix.h"
#include "number.h"
#include "systematic_operation.h"
#include "systematic_reduction.h"

namespace Poincare::Internal {

bool TermsAreEqual(const Tree* u, const Tree* v) {
  if (!u->isMult()) {
    if (!v->isMult()) {
      return u->treeIsIdenticalTo(v);
    }
    return TermsAreEqual(v, u);
  }
  if (!v->isMult()) {
    return u->numberOfChildren() == 2 && u->child(0)->isRational() &&
           u->child(1)->treeIsIdenticalTo(v);
  }
  bool uHasRational = u->child(0)->isRational();
  bool vHasRational = v->child(0)->isRational();
  int n = u->numberOfChildren() - uHasRational;
  if (n != v->numberOfChildren() - vHasRational) {
    return false;
  }
  const Tree* childU = u->child(uHasRational);
  const Tree* childV = v->child(vHasRational);
  for (int i = 0; i < n; i++) {
    if (!childU->treeIsIdenticalTo(childV)) {
      return false;
    }
    childU = childU->nextTree();
    childV = childV->nextTree();
  }
  return true;
}

// The term of 2ab is ab
Tree* PushTerm(const Tree* u) {
  Tree* c = u->cloneTree();
  if (u->isMult() && u->child(0)->isRational()) {
    NAry::RemoveChildAtIndex(c, 0);
    NAry::SquashIfPossible(c);
  }
  return c;
}

// The constant of 2ab is 2
const Tree* Constant(const Tree* u) {
  if (u->isMult() && u->child(0)->isRational()) {
    return u->child(0);
  }
  return 1_e;
}

static bool MergeAdditionChildWithNext(Tree* child, Tree* next) {
  assert(next == child->nextTree());
  Tree* merge = nullptr;
  if (child->isRationalOrFloat() && next->isRationalOrFloat()) {
    // Merge numbers
    merge = Number::Addition(child, next);
  } else if (Infinity::IsPlusOrMinusInfinity(next) && child->isNumber()) {
    // number ± inf -> ± inf
    child->removeTree();
    return true;
  } else if (TermsAreEqual(child, next)) {
    // k1 * a + k2 * a -> (k1+k2) * a
    /* inf-inf, inf+inf and -inf-inf will be handled here */
    Tree* term = PushTerm(child);
    merge = PatternMatching::CreateSimplify(
        KMult(KAdd(KA, KB), KC),
        {.KA = Constant(child), .KB = Constant(next), .KC = term});
    term->removeTree();
    merge = term;
  } else if (child->isMatrix() && next->isMatrix()) {
    merge = Matrix::Addition(child, next);
  }
  if (!merge) {
    return false;
  }
  // Replace both child and next with merge
  next->moveTreeOverTree(merge);
  child->removeTree();
  return true;
}

bool SystematicOperation::ReduceAddition(Tree* u) {
  assert(u->isAdd());
  bool modified = NAry::Flatten(u);
  if (modified && CanApproximateTree(u, &modified)) {
    /* In case of successful flatten, approximateAndReplaceEveryScalar must be
     * tried again to properly handle possible new float children. */
    return true;
  }
  if (NAry::SquashIfPossible(u)) {
    return true;
  }
  modified = NAry::Sort(u) || modified;
  bool didSquashChildren = false;
  int n = u->numberOfChildren();
  int i = 0;
  Tree* child = u->child(0);
  while (i < n) {
    if (child->isZero()) {
      child->removeTree();
      modified = true;
      n--;
      continue;
    }
    Tree* next = child->nextTree();
    if (i + 1 < n && MergeAdditionChildWithNext(child, next)) {
      // 1 + (a + b)/2 + (a + b)/2 -> 1 + a + b
      if (child->isAdd()) {
        n += child->numberOfChildren() - 1;
        child->removeNode();
        didSquashChildren = true;
      }
      modified = true;
      n--;
    } else {
      child = next;
      i++;
    }
  }
  if (n != u->numberOfChildren()) {
    assert(modified);
    NAry::SetNumberOfChildren(u, n);
    if (NAry::SquashIfPossible(u)) {
      return true;
    }
  }
  if (didSquashChildren) {
    /* Newly squashed children should be sorted again and they may allow new
     * simplifications. NOTE: Further simplification could theoretically be
     * unlocked, see following assertion. */
    NAry::Sort(u);
  }
  /* TODO: ReduceAddition may encounter the same issues as the multiplication.
   * If this assert can't be preserved, ReduceAddition must handle one or both
   * of this cases as handled in multiplication:
   * With a,b and c the sorted addition children (a < b < c), M(a,b) the result
   * of merging children a and b (with MergeAdditionChildWithNext) if it exists.
   * - M(a,b) > c or a > M(b,c) (Addition must be sorted again)
   * - M(a,b) doesn't exists, but M(a,M(b,c)) does (previous child should try
   * merging again when child merged with nextChild) */
  if (modified && u->isAdd()) {
    // Bubble-up may be unlocked after merging equal terms.
    SystematicReduction::BubbleUpFromChildren(u);
    assert(!SystematicReduction::ShallowReduce(u));
  }
  return modified;
}

}  // namespace Poincare::Internal
