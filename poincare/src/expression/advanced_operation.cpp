#include "advanced_operation.h"

#include <poincare/src/memory/pattern_matching.h>

#include "k_tree.h"
#include "sign.h"

namespace Poincare::Internal {

// This is redundant with im and re expansion for finite expressions
bool AdvancedOperation::ContractImRe(Tree* e) {
  // re(A)+im(A)*i = A
  return PatternMatching::MatchReplaceSimplify(
      e, KAdd(KRe(KA), KMult(KIm(KA), i_e)), KA);
}

bool ExpandImReIfNotInfinite(Tree* e) {
  PatternMatching::Context ctx;
  // A? + B?*im(C)*D? + E? = A - i*B*C*D + i*B*re(C)*D + E
  if (PatternMatching::Match(e, KAdd(KA_s, KMult(KB_s, KIm(KC), KD_s), KE_s),
                             &ctx)) {
    const Tree* kc = ctx.getTree(KC);
    const Sign realSign = GetComplexSign(kc).realSign();
    /* - Pattern is only true if re(C) is finite
     * - At least one member of the addition must be non-empty to prevent
     * infinitely expanding */
    if (realSign.isFinite() &&
        (ctx.getNumberOfTrees(KA) != 0 || ctx.getNumberOfTrees(KE) != 0)) {
      e->moveTreeOverTree(PatternMatching::CreateSimplify(
          KAdd(KA_s, KMult(-1_e, i_e, KB_s, KC, KD_s),
               KMult(i_e, KB_s, KRe(KC), KD_s), KE_s),
          ctx));
      return true;
    }
  }
  // A? + B?*re(C)*D? + E? = A + B*C*D - i*B*im(C)*D + E
  if (PatternMatching::Match(e, KAdd(KA_s, KMult(KB_s, KRe(KC), KD_s), KE_s),
                             &ctx)) {
    const Tree* kc = ctx.getTree(KC);
    const Sign imagSign = GetComplexSign(kc).imagSign();
    /* - Pattern is only true if im(C) is finite
     * - At least one member of the addition must be non-empty to prevent
     * infinitely expanding */
    if (imagSign.isFinite() &&
        (ctx.getNumberOfTrees(KA) != 0 || ctx.getNumberOfTrees(KE) != 0)) {
      e->moveTreeOverTree(PatternMatching::CreateSimplify(
          KAdd(KA_s, KMult(KB_s, KC, KD_s),
               KMult(-1_e, i_e, KB_s, KIm(KC), KD_s), KE_s),
          ctx));
      return true;
    }
  }
  return false;
}

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
      ExpandImReIfNotInfinite(e);
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
      PatternMatching::MatchReplaceSimplify(
          e, KExp(KAdd(KA, KB_p)), KMult(KExp(KA), KExp(KAdd(KB_p)))) ||
      // exp(-(1/2)*A) = exp(1/2*A) * exp(-A)
      /* Used in 1/√(x) -> √(x)/x, a more general solution may be needed. */
      PatternMatching::MatchReplaceSimplify(
          e, KExp(KMult(-1_e / 2_e, KA_p)),
          KMult(KExp(KMult(1_e / 2_e, KA_p)), KExp(KMult(-1_e, KA_p))));
}

bool AdvancedOperation::ContractExp(Tree* e) {
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
  PatternMatching::Context ctx;
  // 1/(A+iB) -> (A-iB) / (A^2+B^2)
  if (PatternMatching::Match(e, KPow(KA, -1_e), &ctx)) {
    ComplexSign s = GetComplexSign(ctx.getTree(KA));
    // Filter out infinite and pure expressions for useful and accurate results
    if (!s.isPure() && !s.canBeInfinite()) {
      e->moveTreeOverTree(PatternMatching::CreateSimplify(
          KMult(KAdd(KRe(KA), KMult(-1_e, i_e, KIm(KA))),
                KPow(KAdd(KPow(KRe(KA), 2_e), KPow(KIm(KA), 2_e)), -1_e)),
          ctx));
      return true;
    }
  }

  // (A + B?)^2 = (A^2 + 2*A*B + B^2)
  if (PatternMatching::MatchReplaceSimplify(
          e, KPow(KAdd(KA, KB_p), 2_e),
          KAdd(KPow(KA, 2_e), KMult(2_e, KA, KAdd(KB_p)),
               KPow(KAdd(KB_p), 2_e)))) {
    return true;
  }

  // Binomial theorem
  // (A + B?)^n = sum(binomial(n, k) * A^k * B^(n-k), k, 0, n)
  if (PatternMatching::Match(e, KPow(KAdd(KA, KB_p), KC), &ctx) &&
      ctx.getTree(KC)->isInteger() && !ctx.getTree(KC)->isMinusOne()) {
    // a^n and b^n are out of the sum to avoid dependencies in a^0 and b^0
    bool inverse = ctx.getTree(KC)->isNegativeInteger();
    e->moveTreeOverTree(PatternMatching::CreateSimplify(
        KAdd(KPow(KA, KAbs(KC)),
             KSum("k"_e, 1_e, KAdd(KAbs(KC), -1_e),
                  KMult(KBinomial(KAbs(KC), KVarK), KPow(KA, KVarK),
                        KPow(KAdd(KB_p), KAdd(KAbs(KC), KMult(-1_e, KVarK))))),
             KPow(KAdd(KB_p), KAbs(KC))),
        ctx));
    Parametric::Explicit(e);
    if (inverse) {
      PatternMatching::MatchReplaceSimplify(e, KA, KPow(KA, -1_e));
    }
    return true;
  }

  return false;
}

}  // namespace Poincare::Internal
