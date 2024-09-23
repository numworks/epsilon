#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "infinity.h"
#include "k_tree.h"
#include "matrix.h"
#include "number.h"
#include "systematic_operation.h"
#include "systematic_reduction.h"

namespace Poincare::Internal {

bool TermsAreEqual(const Tree* e1, const Tree* e2) {
  if (!e1->isMult()) {
    if (!e2->isMult()) {
      return e1->treeIsIdenticalTo(e2);
    }
    return TermsAreEqual(e2, e1);
  }
  if (!e2->isMult()) {
    return e1->numberOfChildren() == 2 && e1->child(0)->isRational() &&
           e1->child(1)->treeIsIdenticalTo(e2);
  }
  bool e1HasRational = e1->child(0)->isRational();
  bool e2HasRational = e2->child(0)->isRational();
  int n = e1->numberOfChildren() - e1HasRational;
  if (n != e2->numberOfChildren() - e2HasRational) {
    return false;
  }
  const Tree* childE1 = e1->child(e1HasRational);
  const Tree* childE2 = e2->child(e2HasRational);
  for (int i = 0; i < n; i++) {
    if (!childE1->treeIsIdenticalTo(childE2)) {
      return false;
    }
    childE1 = childE1->nextTree();
    childE2 = childE2->nextTree();
  }
  return true;
}

// The term of 2ab is ab
Tree* PushTerm(const Tree* e) {
  Tree* c = e->cloneTree();
  if (e->isMult() && e->child(0)->isRational()) {
    NAry::RemoveChildAtIndex(c, 0);
    NAry::SquashIfPossible(c);
  }
  return c;
}

// The constant of 2ab is 2
const Tree* Constant(const Tree* e) {
  if (e->isMult() && e->child(0)->isRational()) {
    return e->child(0);
  }
  return 1_e;
}

static bool MergeAdditionChildWithNext(Tree* child, Tree* next) {
  assert(next == child->nextTree());
  Tree* merge = nullptr;
  if (child->isRationalOrFloat() && next->isRationalOrFloat()) {
    // Merge numbers
    merge = Number::Addition(child, next);
  } else if (Infinity::IsPlusOrMinusInfinity(next) &&
             GetComplexSign(child).isFinite()) {
    // finite expression ± inf -> ± inf
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

bool SystematicOperation::ReduceAddition(Tree* e) {
  assert(e->isAdd());
  bool modified = NAry::Flatten(e);
  if (modified && CanApproximateTree(e, &modified)) {
    /* In case of successful flatten, approximateAndReplaceEveryScalar must be
     * tried again to properly handle possible new float children. */
    return true;
  }
  if (NAry::SquashIfPossible(e)) {
    return true;
  }
  modified = NAry::Sort(e) || modified;
  bool didSquashChildren = false;
  int n = e->numberOfChildren();
  int i = 0;
  Tree* child = e->child(0);
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
  if (n != e->numberOfChildren()) {
    assert(modified);
    NAry::SetNumberOfChildren(e, n);
    if (NAry::SquashIfPossible(e)) {
      return true;
    }
  }
  if (didSquashChildren) {
    /* Newly squashed children should be sorted again and they may allow new
     * simplifications. NOTE: Further simplification could theoretically be
     * unlocked, see following assertion. */
    NAry::Sort(e);
  }
  /* TODO: ReduceAddition may encounter the same issues as the multiplication.
   * If this assert can't be preserved, ReduceAddition must handle one or both
   * of this cases as handled in multiplication:
   * With a,b and c the sorted addition children (a < b < c), M(a,b) the result
   * of merging children a and b (with MergeAdditionChildWithNext) if it exists.
   * - M(a,b) > c or a > M(b,c) (Addition must be sorted again)
   * - M(a,b) doesn't exists, but M(a,M(b,c)) does (previous child should try
   * merging again when child merged with nextChild) */
  if (modified && e->isAdd()) {
    // Bubble-up may be unlocked after merging equal terms.
    SystematicReduction::BubbleUpFromChildren(e);
    assert(!SystematicReduction::ShallowReduce(e));
  }
  return modified;
}

}  // namespace Poincare::Internal
