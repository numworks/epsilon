#include "trigonometry.h"

#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/number.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/pattern_matching.h>

namespace PoincareJ {

bool Trigonometry::IsDirect(const Tree* node) {
  switch (node->type()) {
    case Type::Cosine:
    case Type::Sine:
    case Type::Tangent:
      return true;
    default:
      return false;
  }
}

bool Trigonometry::IsInverse(const Tree* node) {
  switch (node->type()) {
    case Type::ArcCosine:
    case Type::ArcSine:
    case Type::ArcTangent:
      return true;
    default:
      return false;
  }
}

const Tree* Trigonometry::ExactFormula(uint8_t n, bool isSin, bool* isOpposed) {
  // Sin and cos are 2pi periodic. With sin(n*π/120), n goes from 0 to 239.
  n = n % 240;
  // Formula is opposed depending on the quadrant
  if ((isSin && n >= 120) || (!isSin && n >= 60 && n < 180)) {
    *isOpposed = !*isOpposed;
  }
  // Last two quadrant are now equivalent to the first two ones.
  n = n % 120;
  /* In second half of first quadrant and in first half of second quadrant,
   * we can simply swap Sin/Cos formulas. */
  if (n > 30 && n <= 90) {
    isSin = !isSin;
  }
  // Second quadrant is now equivalent to the first one.
  n = n % 60;
  // Second half of the first quadrant is the first half mirrored.
  if (n > 30) {
    n = 60 - n;
  }
  /* Only 7 exact formulas are left to handle. */
  assert(n >= 0 && n <= 30);
  switch (n) {
    case 0:  // 0
      return isSin ? 0_e : 1_e;
    case 10:  // π/12
      return isSin
                 ? KMult(KAdd(KExp(KMult(KHalf, KLn(3_e))), -1_e),
                         KPow(KMult(KExp(KMult(KHalf, KLn(2_e))), 2_e), -1_e))
                 : KMult(KAdd(KExp(KMult(KHalf, KLn(3_e))), 1_e),
                         KPow(KMult(KExp(KMult(KHalf, KLn(2_e))), 2_e), -1_e));
    case 12:  // π/10
      return isSin ? KMult(1_e / 4_e, KAdd(-1_e, KExp(KMult(KHalf, KLn(5_e)))))
                   : KExp(KMult(
                         KHalf,
                         KLn(KMult(1_e / 8_e,
                                   KAdd(5_e, KExp(KMult(KHalf, KLn(5_e))))))));
    case 15:  // π/8
      return isSin
                 ? KMult(
                       KHalf,
                       KExp(KMult(
                           KHalf,
                           KLn(KAdd(2_e, KMult(-1_e, KExp(KMult(KHalf,
                                                                KLn(2_e)))))))))
                 : KMult(KHalf,
                         KExp(KMult(
                             KHalf,
                             KLn(KAdd(2_e, KExp(KMult(KHalf, KLn(2_e))))))));
    case 20:  // π/6
      return isSin ? KHalf : KMult(KExp(KMult(KHalf, KLn(3_e))), KHalf);
    case 24:  // π/5
      return isSin ? KExp(KMult(
                         KHalf,
                         KLn(KMult(
                             1_e / 8_e,
                             KAdd(5_e,
                                  KMult(-1_e, KExp(KMult(KHalf, KLn(5_e)))))))))
                   : KMult(1_e / 4_e, KAdd(1_e, KExp(KMult(KHalf, KLn(5_e)))));
    case 30:  // π/4
      return KExp(KMult(-1_e, KHalf, KLn(2_e)));
    default:
      return nullptr;
  }
}

bool Trigonometry::SimplifyTrigDiff(Tree* u) {
  assert(u->isTrigDiff());
  /* TrigDiff is used to factorize Trigonometric contraction. It determines the
   * first term of these equations :
   * 2*sin(x)*sin(y) = cos(x-y) - cos(x+y)  -> TrigDiff(1,1) = 0
   * 2*sin(x)*cos(y) = sin(x-y) + sin(x+y)  -> TrigDiff(1,0) = 1
   * 2*cos(x)*sin(y) =-sin(x-y) + sin(x+y)  -> TrigDiff(0,1) = 3
   * 2*cos(x)*cos(y) = cos(x-y) + cos(x+y)  -> TrigDiff(0,0) = 0
   */
  // Simplify children as trigonometry second elements.
  bool isOpposed = false;
  Tree* x = u->child(0);
  SimplifyTrigSecondElement(x, &isOpposed);
  Tree* y = x->nextTree();
  SimplifyTrigSecondElement(y, &isOpposed);
  // Find TrigDiff value depending on children types (sin or cos)
  bool isDifferent = x->type() != y->type();
  // Account for sign difference between TrigDiff(1,0) and TrigDiff(0,1)
  if (isDifferent && x->isZero()) {
    isOpposed = !isOpposed;
  }
  // Replace TrigDiff with result
  u->cloneTreeOverTree(isDifferent ? (isOpposed ? 3_e : 1_e)
                                   : (isOpposed ? 2_e : 0_e));
  return true;
}

// If u is of the form π*n, return n.
const Tree* getPiFactor(const Tree* u) {
  if (u->treeIsIdenticalTo(π_e)) {
    return 1_e;
  }
  if (u->isZero()) {
    return 0_e;
  }
  if (u->isMultiplication() && u->numberOfChildren() == 2 &&
      u->child(0)->isRational() && u->child(1)->treeIsIdenticalTo(π_e)) {
    return u->child(0);
  }
  return nullptr;
}

bool Trigonometry::SimplifyTrig(Tree* u) {
  assert(u->isTrig());
  // Trig(x,y) = {Cos(x) if y=0, Sin(x) if y=1, -Cos(x) if y=2, -Sin(x) if y=3}
  Tree* firstArgument = u->child(0);
  Tree* secondArgument = firstArgument->nextTree();
  bool isOpposed = false;
  bool changed = SimplifyTrigSecondElement(secondArgument, &isOpposed);
  assert(secondArgument->isZero() || secondArgument->isOne());
  bool isSin = secondArgument->isOne();
  // cos(-x) = cos(x) and sin(-x) = -sin(x)
  // TODO: Maybe factorize even/odd functions logic
  if (PatternMatching::MatchReplaceAndSimplify(
          firstArgument, KMult(KA_s, -1_e, KB_s), KMult(KA_s, KB_s))) {
    changed = true;
    if (isSin) {
      isOpposed = !isOpposed;
    }
  }
  const Tree* piFactor = getPiFactor(firstArgument);
  /* TODO: Maybe the exact trigonometric values should be replaced in advanced
   *        reduction. */
  if (piFactor) {
    // Find n to match Trig((n/120)*π, ...) with exact value.
    Tree* multipleTree = Rational::Multiplication(120_e, piFactor);
    if (multipleTree->isInteger()) {
      // Trig is 2pi periodic, n can be retrieved as a uint8_t.
      multipleTree->moveTreeOverTree(IntegerHandler::Remainder(
          Integer::Handler(multipleTree), IntegerHandler(240)));
      assert(Integer::Is<uint8_t>(multipleTree));
      uint8_t n = Integer::Handler(multipleTree).to<uint8_t>();
      multipleTree->removeTree();
      const Tree* exactFormula = ExactFormula(n, isSin, &isOpposed);
      if (exactFormula) {
        u->cloneTreeOverTree(exactFormula);
        Simplification::DeepSystematicReduce(u);
        changed = true;
      }
    } else {
      multipleTree->removeTree();
    }
  } else if (PatternMatching::MatchAndReplace(u, KTrig(KATrig(KA, KB), KB),
                                              KA) ||
             PatternMatching::MatchReplaceAndSimplify(
                 u, KTrig(KATrig(KA, KB), KC),
                 KPow(KAdd(1_e, KMult(-1_e, KPow(KA, 2_e))), KHalf))) {
    // sin(asin(x))=cos(acos(x))=x, sin(acos(x))=cos(asin(x))=sqrt(1-x^2)
    changed = true;
  }
  if (isOpposed && changed) {
    u->cloneTreeAtNode(-1_e);
    u->moveNodeAtNode(SharedEditionPool->push<Type::Multiplication>(2));
    Simplification::SimplifyMultiplication(u);
  }
  return changed;
}

bool Trigonometry::SimplifyTrigSecondElement(Tree* u, bool* isOpposed) {
  // Trig second element is always expected to be a reduced integer.
  assert(u->isInteger() && !Simplification::DeepSystematicReduce(u));
  bool changed = false;
  IntegerHandler i = Integer::Handler(u);
  Tree* remainder = IntegerHandler::Remainder(i, IntegerHandler(4));
  if (Comparison::Compare(remainder, 2_e) >= 0) {
    changed = true;
    *isOpposed = !*isOpposed;
    remainder->moveTreeOverTree(
        IntegerHandler::Remainder(i, IntegerHandler(2)));
    assert(!remainder->treeIsIdenticalTo(u));
  }
  changed |= !remainder->treeIsIdenticalTo(u);
  if (changed) {
    u->moveTreeOverTree(remainder);
  } else {
    remainder->removeTree();
  }
  // Simplified second element should have only two possible values.
  assert(u->isZero() || u->isOne());
  return changed;
}

bool Trigonometry::SimplifyATrig(Tree* u) {
  assert(u->isATrig());
  PatternMatching::Context ctx;
  if (PatternMatching::Match(KATrig(KTrig(KA, KB), KC), u, &ctx)) {
    const Tree* piFactor = getPiFactor(ctx.getNode(KA));
    if (!piFactor) {
      return false;
    }
    // We can simplify but functions do not match. Use acos(x) = π/2 - asin(x)
    bool swapATrig = (!ctx.getNode(KB)->treeIsIdenticalTo(ctx.getNode(KC)));
    // atrig(trig(π*y, i), i)
    bool isSin = ctx.getNode(KB)->isOne();
    // Compute k = ⌊y⌋ for acos, ⌊y + π/2⌋ for asin.
    // acos(cos(π*y)) = π*(y-k) if k even, π*(k-y+1) otherwise.
    // asin(sin(π*y)) = π*(y-k) if k even, π*(k-y) otherwise.
    Tree* res = PatternMatching::CreateAndSimplify(
        isSin ? KFloor(KAdd(KA, KHalf)) : KFloor(KA), {.KA = piFactor});
    assert(res->isInteger());
    bool kIsEven = Integer::Handler(res).isEven();
    res->moveTreeOverTree(PatternMatching::CreateAndSimplify(
        KAdd(KA, KMult(-1_e, KB)), {.KA = piFactor, .KB = res}));
    if (!kIsEven) {
      res->moveTreeOverTree(
          PatternMatching::CreateAndSimplify(KMult(-1_e, KA), {.KA = res}));
      if (!isSin) {
        res->moveTreeOverTree(
            PatternMatching::CreateAndSimplify(KAdd(1_e, KA), {.KA = res}));
      }
    }
    if (swapATrig) {
      res->moveTreeOverTree(PatternMatching::CreateAndSimplify(
          KAdd(KHalf, KMult(-1_e, KA)), {.KA = res}));
    }
    res->moveTreeOverTree(
        PatternMatching::CreateAndSimplify(KMult(π_e, KA), {.KA = res}));
    u->moveTreeOverTree(res);
    return true;
  }
  const Tree* arg = u->child(0);
  bool isAsin = arg->nextTree()->isOne();
  ComplexSign argSign = ComplexSign::Get(arg);
  if (argSign.isZero()) {
    u->cloneTreeOverTree(isAsin ? 0_e : KMult(KHalf, π_e));
    return true;
  }
  if (!argSign.isReal()) {
    return false;
  }
  bool argIsOpposed = argSign.realSign().isNegative();
  bool changed = argIsOpposed;
  if (argIsOpposed) {
    u->child(0)->moveTreeOverTree(
        PatternMatching::CreateAndSimplify(KMult(-1_e, KA), {.KA = arg}));
  }
  if (arg->isOne()) {
    u->cloneTreeOverTree(isAsin ? KMult(KHalf, π_e) : 0_e);
    changed = true;
  } else if (arg->isHalf()) {
    u->moveTreeOverTree(PatternMatching::CreateAndSimplify(
        KMult(π_e, KPow(KA, -1_e)), {.KA = isAsin ? 6_e : 3_e}));
    changed = true;
  } else if (arg->isMultiplication()) {
    /* TODO: Handle the same angles as ExactFormula (π/12, π/10, π/8 and π/5 are
     * missing) and find a better implementation for these special cases. */
    changed =
        // acos(√2/2) = asin(√2/2) = π/4
        PatternMatching::MatchReplaceAndSimplify(
            u, KATrig(KMult(KHalf, KExp(KMult(KHalf, KLn(2_e)))), KA),
            KMult(π_e, KPow(4_e, -1_e))) ||
        // acos(√3/2) = π/6
        PatternMatching::MatchReplaceAndSimplify(
            u, KATrig(KMult(KHalf, KExp(KMult(KHalf, KLn(3_e)))), 0_e),
            KMult(π_e, KPow(6_e, -1_e))) ||
        // asin(√3/2) = π/3
        PatternMatching::MatchReplaceAndSimplify(
            u, KATrig(KMult(KHalf, KExp(KMult(KHalf, KLn(3_e)))), 1_e),
            KMult(π_e, KPow(3_e, -1_e))) ||
        changed;
  }
  if (argIsOpposed) {
    assert(changed);
    // asin(-x) = -asin(x) and acos(-x) = π - acos(x)
    PatternMatching::MatchReplaceAndSimplify(
        u, KA, isAsin ? KMult(-1_e, KA) : KAdd(π_e, KMult(-1_e, KA)));
  }
  return changed;
}

bool Trigonometry::SimplifyArcTangentRad(Tree* u) {
  // TODO_PCJ: Add more exact values (√3, 1/√3, ...)
  switch (u->child(0)->type()) {
    case Type::Zero:
      u->cloneTreeOverTree(0_e);
      return true;
    case Type::One:
      u->cloneTreeOverTree(KMult(1_e / 4_e, π_e));
      return true;
    case Type::MinusOne:
      u->cloneTreeOverTree(KMult(-1_e / 4_e, π_e));
      return true;
    default:
      return false;
  }
}

/* TODO: Find an easier solution for nested expand/contract smart shallow
 * simplification. */

bool Trigonometry::ExpandTrigonometric(Tree* ref) {
  // Trig(A?+B, C) = Trig(A, 0)*Trig(B, C) + Trig(A, 1)*Trig(B, C-1)
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KTrig(KAdd(KA, KB_p), KD),
      KAdd(KMult(KTrig(KAdd(KA), 0_e), KTrig(KAdd(KB_p), KD)),
           KMult(KTrig(KAdd(KA), 1_e), KTrig(KAdd(KB_p), KAdd(KD, -1_e)))));
}

bool Trigonometry::ContractTrigonometric(Tree* ref) {
  return
      // A?+cos(B)^2+C?+sin(D)^2+E? = 1 + A + C + E
      PatternMatching::MatchReplaceAndSimplify(
          ref,
          KAdd(KA_s, KPow(KTrig(KB, 0_e), 2_e), KC_s, KPow(KTrig(KD, 1_e), 2_e),
               KE_s),
          KAdd(1_e, KA_s, KC_s, KE_s)) ||
      // A?*Trig(B, C)*D?*Trig(E, F)*G? =
      // 0.5*A*D*(Trig(B-E, TrigDiff(C,F)) + Trig(B+E, C+F))*G
      PatternMatching::MatchReplaceAndSimplify(
          ref, KMult(KA_s, KTrig(KB, KC), KD_s, KTrig(KE, KF), KG_s),
          KMult(KHalf, KA_s, KD_s,
                KAdd(KTrig(KAdd(KB, KMult(-1_e, KE)), KTrigDiff(KC, KF)),
                     KTrig(KAdd(KB, KE), KAdd(KF, KC))),
                KG_s));
}

/* TODO: Maybe expand arccos(x) = π/2 - arcsin(x).
 * Beware of infinite expansion. */

}  // namespace PoincareJ
