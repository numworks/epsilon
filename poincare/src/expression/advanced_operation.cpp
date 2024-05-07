#include "advanced_operation.h"

#include <poincare/src/memory/pattern_matching.h>

#include "k_tree.h"

namespace Poincare::Internal {

bool AdvancedOperation::ExpandImRe(Tree* e) {
  return
      // im(A+B?) = im(A) + im(B)
      PatternMatching::MatchReplaceSimplify(e, KIm(KAdd(KA, KB_p)),
                                            KAdd(KIm(KA), KIm(KAdd(KB_p)))) ||
      // re(A+B?) = re(A) + re(B)
      PatternMatching::MatchReplaceSimplify(e, KRe(KAdd(KA, KB_p)),
                                            KAdd(KRe(KA), KRe(KAdd(KB_p)))) ||
      // im(A*B?) = im(A)re(B) + re(A)im(B)
      PatternMatching::MatchReplaceSimplify(
          e, KIm(KMult(KA, KB_p)),
          KAdd(KMult(KIm(KA), KRe(KMult(KB_p))),
               KMult(KRe(KA), KIm(KMult(KB_p))))) ||
      // re(A*B?) = re(A)*re(B) - im(A)*im(B)
      PatternMatching::MatchReplaceSimplify(
          e, KRe(KMult(KA, KB_p)),
          KAdd(KMult(KRe(KA), KRe(KMult(KB_p))),
               KMult(-1_e, KIm(KA), KIm(KMult(KB_p))))) ||
      // Replace im and re in additions only to prevent infinitely expanding
      // A? + B?*im(C)*D? + E? = A - i*B*C*D + i*B*re(C)*D + E
      PatternMatching::MatchReplaceSimplify(
          e, KAdd(KA_s, KMult(KB_s, KIm(KC), KD_s), KE_p),
          KAdd(KA_s, KMult(-1_e, i_e, KB_s, KC, KD_s),
               KMult(i_e, KB_s, KRe(KC), KD_s), KE_p)) ||
      // A? + B?*re(C)*D? + E? = A + B*C*D - i*B*im(C)*D + E
      PatternMatching::MatchReplaceSimplify(
          e, KAdd(KA_s, KMult(KB_s, KRe(KC), KD_s), KE_p),
          KAdd(KA_s, KMult(KB_s, KC, KD_s),
               KMult(-1_e, i_e, KB_s, KIm(KC), KD_s), KE_p)) ||
      // A? + B?*im(C)*D? = A - i*B*C*D + i*B*re(C)*D
      PatternMatching::MatchReplaceSimplify(
          e, KAdd(KA_p, KMult(KB_s, KIm(KC), KD_s)),
          KAdd(KA_p, KMult(-1_e, i_e, KB_s, KC, KD_s),
               KMult(i_e, KB_s, KRe(KC), KD_s))) ||
      // A? + B?*re(C)*D? = A + B*C*D - i*B*im(C)*D
      PatternMatching::MatchReplaceSimplify(
          e, KAdd(KA_p, KMult(KB_s, KRe(KC), KD_s)),
          KAdd(KA_p, KMult(KB_s, KC, KD_s),
               KMult(-1_e, i_e, KB_s, KIm(KC), KD_s)));
}

bool AdvancedOperation::ContractAbs(Tree* e) {
  // A?*|B|*|C|*D? = A*|BC|*D
  return PatternMatching::MatchReplaceSimplify(
      e, KMult(KA_s, KAbs(KB), KAbs(KC), KD_s),
      KMult(KA_s, KAbs(KMult(KB, KC)), KD_s));
}

bool AdvancedOperation::ExpandAbs(Tree* e) {
  return
      // |A*B?| = |A|*|B|
      PatternMatching::MatchReplaceSimplify(
          e, KAbs(KMult(KA, KB_p)), KMult(KAbs(KA), KAbs(KMult(KB_p)))) ||
      // |x| = √(re(x)^2+im(x)^2)
      PatternMatching::MatchReplaceSimplify(
          e, KAbs(KA),
          KExp(KMult(1_e / 2_e,
                     KLn(KAdd(KPow(KRe(KA), 2_e), KPow(KIm(KA), 2_e))))));
}

bool AdvancedOperation::ExpandExp(Tree* e) {
  return
      // exp(A?*i*B?) = cos(A*B) + i*sin(A*B)
      PatternMatching::MatchReplaceSimplify(
          e, KExp(KMult(KA_s, i_e, KB_s)),
          KAdd(KTrig(KMult(KA_s, KB_s), 0_e),
               KMult(i_e, KTrig(KMult(KA_s, KB_s), 1_e)))) ||
      // exp(A+B?) = exp(A) * exp(B)
      PatternMatching::MatchReplaceSimplify(e, KExp(KAdd(KA, KB_p)),
                                            KMult(KExp(KA), KExp(KAdd(KB_p))));
}

bool AdvancedOperation::ContractExpMult(Tree* e) {
  return
      // A? * exp(B) * exp(C) * D? = A * exp(B+C) * D
      PatternMatching::MatchReplaceSimplify(
          e, KMult(KA_s, KExp(KB), KExp(KC), KD_s),
          KMult(KA_s, KExp(KAdd(KB, KC)), KD_s)) ||
      // A? + cos(B) + C? + i*sin(B) + D? = A + C + D + exp(i*B)
      PatternMatching::MatchReplaceSimplify(
          e, KAdd(KA_s, KTrig(KB, 0_e), KC_s, KMult(i_e, KTrig(KB, 1_e)), KD_s),
          KAdd(KA_s, KC_s, KD_s, KExp(KMult(i_e, KB))));
}

bool AdvancedOperation::ExpandMult(Tree* e) {
  // We need at least one factor before or after addition.
  return
      // A?*(B+C?)*D? = A*B*D + A*C*D
      PatternMatching::MatchReplaceSimplify(
          e, KMult(KA_p, KAdd(KB, KC_p), KD_s),
          KAdd(KMult(KA_p, KB, KD_s), KMult(KA_p, KAdd(KC_p), KD_s))) ||
      // (A+B?)*C? = A*C + B*C
      PatternMatching::MatchReplaceSimplify(
          e, KMult(KAdd(KA, KB_p), KC_p),
          KAdd(KMult(KA, KC_p), KMult(KAdd(KB_p), KC_p)));
}

bool AdvancedOperation::ContractMult(Tree* e) {
  /* TODO: With  N and M positive, contract
   * A + B*A*C + A^N + D*A^M*E into A*(1 + B*C + A^(N-1) + D*A^(M-1)*E) */
  // A? + B?*C*D? + E? + F?*C*G? + H? = A + C*(B*D+F*G) + E + H
  return PatternMatching::MatchReplaceSimplify(
      e, KAdd(KA_s, KMult(KB_s, KC, KD_s), KE_s, KMult(KF_s, KC, KG_s), KH_s),
      KAdd(KA_s, KMult(KC, KAdd(KMult(KB_s, KD_s), KMult(KF_s, KG_s))), KE_s,
           KH_s));
}

bool AdvancedOperation::ExpandPower(Tree* e) {
  // (A?*B)^C = A^C * B^C is currently in SystematicSimplification
  // (A + B?)^2 = (A^2 + 2*A*B + B^2)
  // TODO: Implement a more general (A + B)^C expand.
  return PatternMatching::MatchReplaceSimplify(
      e, KPow(KAdd(KA, KB_p), 2_e),
      KAdd(KPow(KA, 2_e), KMult(2_e, KA, KAdd(KB_p)), KPow(KAdd(KB_p), 2_e)));
}

}  // namespace Poincare::Internal
