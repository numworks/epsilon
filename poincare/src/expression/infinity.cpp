#include "infinity.h"

#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/probability/distribution_method.h>

#include "k_tree.h"

namespace Poincare::Internal {

static bool shallowBubbleUpInfinityInDistribution(Tree* u) {
  /* - normcdf(inf,a,b) = 1
   * - normcdf(-inf,a,b) = 0
   * - tcdfrange(-inf,inf,b) = 1
   * - tcdfrange(-inf,-inf,b) = 0 */
  assert(u->isDistribution());
  DistributionMethod::Type methodType = DistributionMethod::Get(u);
  if (methodType != DistributionMethod::Type::CDF &&
      methodType != DistributionMethod::Type::CDFRange) {
    return false;
  }
  Tree* child = u->firstChild();
  PatternMatching::Context ctx;
  if (methodType == DistributionMethod::Type::CDFRange) {
    if (!PatternMatching::Match(KMult(-1_e, KInf), child, &ctx)) {
      return false;
    }
    child = child->nextTree();
  }
  if (child->isInf()) {
    u->cloneTreeOverTree(1_e);
    return true;
  }
  if (PatternMatching::Match(KMult(-1_e, KInf), child, &ctx)) {
    u->cloneTreeOverTree(0_e);
    return true;
  }
  /* TODO: return CDF of the same distributions with the same parameters
   * tcdfrange(-inf, 4, 5) => tcdf(4, 5) */
  return false;
}

bool Infinity::ShallowBubbleUpInfinity(Tree* u) {
  if (
      // ln(inf) = inf
      PatternMatching::MatchReplaceSimplify(u, KLn(KInf), KInf)
      // cos(inf) = sin(inf) = undef
      || PatternMatching::MatchReplaceSimplify(u, KTrig(KInf, KB), KUndef)
      // atan(inf) = π/2
      || PatternMatching::MatchReplaceSimplify(u, KATanRad(KInf),
                                               KMult(1_e / 2_e, π_e))
      // 1^inf -> undef
      || PatternMatching::MatchReplaceSimplify(u, KPowReal(1_e, KInf), KUndef)
      // 1^-inf -> undef
      || PatternMatching::MatchReplaceSimplify(
             u, KPowReal(1_e, KMult(-1_e, KInf)), KUndef)
      // inf^0 -> undef
      || PatternMatching::MatchReplaceSimplify(u, KPowReal(KInf, 0_e), KUndef)
      // inf^(-1) -> 0
      || PatternMatching::MatchReplaceSimplify(u, KPowReal(KInf, -1_e), 0_e)
      // inf - inf = undef
      || PatternMatching::MatchReplaceSimplify(
             u, KAdd(KA_s, KInf, KB_s, KMult(-1_e, KInf), KC_s), KUndef)
      // -inf + inf = undef
      || PatternMatching::MatchReplaceSimplify(
             u, KAdd(KA_s, KMult(-1_e, KInf), KB_s, KInf, KC_s), KUndef)) {
    return true;
  }

  if (u->isDistribution()) {
    return shallowBubbleUpInfinityInDistribution(u);
  }

  if (!u->isMult()) {
    return false;
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

}  // namespace Poincare::Internal
