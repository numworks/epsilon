#include "infinity.h"

#include <poincare/src/memory/pattern_matching.h>

#include "approximation.h"
#include "k_tree.h"

namespace Poincare::Internal {

bool Infinity::ShallowBubbleUpInfinity(Tree* u) {
  /* TODO_PCJ: recode every exact simplification for nodes we know (ex: power
   * with inf) to avoid having Float nodes. We would be able for example to
   * reduce atan(e^inf) to π/2 or atan(-inf) in -π/2. */
  if (PatternMatching::MatchReplaceSimplify(u, KATanRad(KInf),
                                            KMult(1_e / 2_e, π_e))) {
    return true;
  }

  if (!u->isMult()) {
    return Approximation::ApproximateAndReplaceEveryScalar(u);
  }

  // inf*inf -> inf
  bool changed = false;
  int n = u->numberOfChildren();
  while (PatternMatching::MatchReplaceSimplify(
      u, KMult(KA_s, KInf, KB_s, KInf, KC_s), KMult(KA_s, KB_s, KC_s, KInf))) {
    assert(u->numberOfChildren() < n);
    n = u->numberOfChildren();
    changed = true;
  }

  // x*inf -> sign(x)*inf
  PatternMatching::Context ctx;
  if (u->numberOfChildren() == 2) {
    if (!u->child(0)->isInf() && !u->child(1)->isInf()) {
      return false;
    }
    Tree* otherChild = u->child(0)->isInf() ? u->child(1) : u->child(0);
    // Handle cases -1,0,1,sign to avoid infinite loop
    if (otherChild->isZero()) {
      // 0*inf -> undef
      u->cloneTreeOverTree(KUndef);
      return true;
    } else if (otherChild->isOne() || otherChild->isMinusOne() ||
               otherChild->isSign()) {
      // SimplifyMultiplication will handle 1*inf -> inf
      // Do not reduce -1 -> sign(-1) or sign() -> sign(sign)
      return changed;
    }
  }
  if (PatternMatching::MatchReplaceSimplify(
          u, KMult(KA_s, KInf, KB_s), KMult(KSign(KMult(KA_s, KB_s)), KInf))) {
    return true;
  }

  return changed;
}

bool Infinity::TreeIsPlusOrMinusInfinity(const Tree* u) {
  return u->isInf() || TreeIsMinusInfinity(u);
}

bool Infinity::TreeIsMinusInfinity(const Tree* u) {
  PatternMatching::Context ctx;
  return PatternMatching::Match(KMult(-1_e, KInf), u, &ctx);
}

}  // namespace Poincare::Internal
