#include "advanced_operation.h"

#include <poincare/src/memory/pattern_matching.h>

#include "k_tree.h"
#include "sign.h"
#include "systematic_reduction.h"

namespace Poincare::Internal {

// This is redundant with im and re expansion for finite expressions
bool AdvancedOperation::ContractImRe(Tree* e) {
  // re(A)+im(A)*i = A
  return PatternMatching::MatchReplaceSimplify(
      e, KAdd(KRe(KA), KMult(KIm(KA), i_e)), KA);
}

bool ExpandImReIfNotInfinite(Tree* e) {
  PatternMatching::Context ctx;
  // A? + B?*im(C)*D? + E? = A - B*C*D*i + B*re(C)*D*i + E
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
          KAdd(KA_s, KMult(-1_e, KB_s, KC, KD_s, i_e),
               KMult(KB_s, KRe(KC), KD_s, i_e), KE_s),
          ctx));
      return true;
    }
  }
  // A? + B?*re(C)*D? + E? = A + B*C*D - B*im(C)*D*i + E
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
               KMult(-1_e, KB_s, KIm(KC), KD_s, i_e), KE_s),
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
  PatternMatching::Context ctx;
  if (PatternMatching::Match(
          e, KAbs(KAdd(KExp(KMult(KA_s, i_e)), KExp(KMult(KB_s, i_e)))),
          &ctx)) {
    Tree* a = PatternMatching::CreateSimplify(KMult(KA_s), ctx);
    Tree* b = PatternMatching::CreateSimplify(KMult(KB_s), ctx);
    bool childrenAreReal =
        GetComplexSign(a).isReal() && GetComplexSign(b).isReal();
    b->removeTree();
    a->removeTree();
    if (childrenAreReal) {
      // |exp(A*i) + exp(B*i)| = √(2+2*cos(A-B)) for A and B real
      e->moveTreeOverTree(PatternMatching::CreateSimplify(
          KExp(KMult(1_e / 2_e,
                     KLn(KAdd(2_e, KMult(2_e, KTrig(KAdd(KMult(KA_s),
                                                         KMult(-1_e, KB_s)),
                                                    0_e)))))),
          ctx));
      return true;
    }
  }
  // A?*|B|*|C|*D? = A*|BC|*D
  return PatternMatching::MatchReplaceSimplify(
      e, KMult(KA_s, KAbs(KB), KAbs(KC), KD_s),
      KMult(KA_s, KAbs(KMult(KB, KC)), KD_s));
}

bool AdvancedOperation::ExpandAbs(Tree* e) {
  PatternMatching::Context ctx;
  if (e->isAbs() && e->child(0)->isMult()) {
    // |A*B*C*...| = |A|*|B|*|C|*.. expand abs of mult deeply
    const Tree* mult = e->child(0);
    Tree* result = mult->cloneNode();
    for (const Tree* child : mult->children()) {
      PatternMatching::CreateSimplify(KAbs(KA), {.KA = child});
    }
    SystematicReduction::ShallowReduce(result);
    e->moveTreeOverTree(result);
    return true;
  }

  if (PatternMatching::MatchReplaceSimplify(
          e, KPow(KAbs(KA), 2_e),
          KAdd(KPow(KRe(KA), 2_e), KPow(KIm(KA), 2_e)))) {
    // |A|^2 = re(A)^2+im(A)^2
    return true;
  }
  if ((PatternMatching::Match(e, KAbs(KExp(KMult(KA, KLn(KB)))), &ctx) ||
       PatternMatching::Match(e, KAbs(KPow(KB, KA)), &ctx)) &&
      GetComplexSign(ctx.getTree(KA)).isReal()) {
    // |B^A| = |B|^A for A real
    e->moveTreeOverTree(
        e->child(0)->isExp()
            ? PatternMatching::CreateSimplify(KExp(KMult(KA, KLn(KAbs(KB)))),
                                              ctx)
            : PatternMatching::CreateSimplify(KPow(KAbs(KB), KA), ctx));
    return true;
  }
  if (PatternMatching::Match(e, KAbs(KA), &ctx) &&
      GetComplexSign(ctx.getTree(KA)).isReal()) {
    // |A| = A*sign(A) for A real
    e->moveTreeOverTree(
        PatternMatching::CreateSimplify(KMult(KA, KSign(KA)), ctx));
    return true;
  }
  // |A| = √(re(A)^2+im(A)^2)
  return PatternMatching::MatchReplaceSimplify(
      e, KAbs(KA),
      KExp(
          KMult(1_e / 2_e, KLn(KAdd(KPow(KRe(KA), 2_e), KPow(KIm(KA), 2_e))))));
}

bool AdvancedOperation::ExpandExp(Tree* e) {
  return
      // exp(A?*i) = cos(A) + sin(A)*i
      PatternMatching::MatchReplaceSimplify(
          e, KExp(KMult(KA_s, i_e)),
          KAdd(KTrig(KMult(KA_s), 0_e), KMult(KTrig(KMult(KA_s), 1_e), i_e))) ||
      // exp(A+B?) = exp(A) * exp(B)
      PatternMatching::MatchReplaceSimplify(e, KExp(KAdd(KA, KB_p)),
                                            KMult(KExp(KA), KExp(KAdd(KB_p))));
}

bool AdvancedOperation::ContractExp(Tree* e) {
  // A? * exp(B * Ln(C)) * D? * exp(B * Ln(E)) * F? =>
  // A * exp(B * Ln(C * E)) * D * F with B C and E real
  // This steps shortcuts 3 contract steps
  PatternMatching::Context ctx;
  if (PatternMatching::Match(e,
                             KMult(KA_s, KExp(KMult(KB_p, KLn(KC))), KD_s,
                                   KExp(KMult(KB_p, KLn(KE))), KF_s),
                             &ctx) &&
      GetComplexSign(ctx.getTree(KC)).isReal() &&
      GetComplexSign(ctx.getTree(KE)).isReal()) {
    Tree* contracted = PatternMatching::CreateSimplify(
        KMult(KA_s, KExp(KMult(KB_p, KLn(KMult(KC, KE)))), KD_s, KF_s), ctx);
    e->moveTreeOverTree(contracted);
    return true;
  }
  return
      // A? * exp(B) * exp(C) * D? = A * exp(B+C) * D
      PatternMatching::MatchReplaceSimplify(
          e, KMult(KA_s, KExp(KB), KExp(KC), KD_s),
          KMult(KA_s, KExp(KAdd(KB, KC)), KD_s)) ||
      // A? + cos(B) + C? + sin(B)*i + D? = A + C + D + exp(B*i)
      PatternMatching::MatchReplaceSimplify(
          e, KAdd(KA_s, KTrig(KB, 0_e), KC_s, KMult(KTrig(KB, 1_e), i_e), KD_s),
          KAdd(KA_s, KC_s, KD_s, KExp(KMult(KB, i_e)))) ||
      // A? + cos(B) + C? - sin(B)*i + D? = A + C + D + exp(-B*i)
      PatternMatching::MatchReplaceSimplify(
          e,
          KAdd(KA_s, KTrig(KB, 0_e), KC_s, KMult(-1_e, KTrig(KB, 1_e), i_e),
               KD_s),
          KAdd(KA_s, KC_s, KD_s, KExp(KMult(-1_e, KB, i_e))));
}

// A*(B+C+..)*(D+E+...) = A*B*D + A*B*E+...+A*C*D+...
bool AdvancedOperation::ExpandMult(Tree* e) {
  /* As opposed to most advanced operation steps, we expand mult in depth
   * because too many advanced reduction steps were needed to simplify
   * multiplications of additions. We therefore rely on ContractMult for atomic
   * contractions that improve the metric. */
  PatternMatching::Context ctx;
  if (PatternMatching::Match(e, KMult(KA_s, KAdd(KB, KC_p), KD_s), &ctx) &&
      (ctx.getNumberOfTrees(KA) != 0 || ctx.getNumberOfTrees(KD) != 0)) {
    int numberOfTerms = 1 + ctx.getNumberOfTrees(KC);
    Tree* result = SharedTreeStack->pushAdd(numberOfTerms);
    const Tree* term = ctx.getTree(KB);
    for (int i = 0; i < numberOfTerms; i++) {
      ctx.setNode(KB, term, 1, false);
      // TODO: Maybe limit the number of recursive calls.
      ExpandMult(PatternMatching::CreateSimplify(KMult(KA_s, KB, KD_s), ctx));
      term = term->nextTree();
    }
    e->moveTreeOverTree(result);
    SystematicReduction::ShallowReduce(e);
    return true;
  }
  return false;
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
  // 1/(A+iB) -> (A-B*i) / (A^2+B^2)
  if (PatternMatching::Match(e, KPow(KA, -1_e), &ctx)) {
    ComplexSign s = GetComplexSign(ctx.getTree(KA));
    // Filter out infinite and pure expressions for useful and accurate results
    if (!s.isPure() && !s.canBeInfinite()) {
      e->moveTreeOverTree(PatternMatching::CreateSimplify(
          KMult(KAdd(KRe(KA), KMult(-1_e, KIm(KA), i_e)),
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

bool AdvancedOperation::ExpandCeilFloor(Tree* e) {
  return
      // ceil(A)  -> -floor(-A)
      PatternMatching::MatchReplaceSimplify(
          e, KCeil(KA), KMult(-1_e, KFloor(KMult(-1_e, KA))));
}

bool AdvancedOperation::ContractCeilFloor(Tree* e) {
  return
      // floor(A) -> -ceil(-A)
      PatternMatching::MatchReplaceSimplify(
          e, KFloor(KA), KMult(-1_e, KCeil(KMult(-1_e, KA))));
}

}  // namespace Poincare::Internal
