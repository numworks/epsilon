#include "trigonometry.h"

#include <poincare/src/memory/pattern_matching.h>

#include "comparison.h"
#include "infinity.h"
#include "k_tree.h"
#include "number.h"
#include "rational.h"
#include "simplification.h"

namespace Poincare::Internal {

bool Trigonometry::IsDirect(const Tree* node) {
  switch (node->type()) {
    case Type::Cos:
    case Type::Sin:
    case Type::Tan:
      return true;
    default:
      return false;
  }
}

bool Trigonometry::IsInverse(const Tree* node) {
  switch (node->type()) {
    case Type::ACos:
    case Type::ASin:
    case Type::ATan:
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
      return isSin ? KMult(KAdd(KExp(KMult(1_e / 2_e, KLn(3_e))), -1_e),
                           KPow(KMult(KExp(KMult(1_e / 2_e, KLn(2_e))), 2_e),
                                -1_e))
                   : KMult(KAdd(KExp(KMult(1_e / 2_e, KLn(3_e))), 1_e),
                           KPow(KMult(KExp(KMult(1_e / 2_e, KLn(2_e))), 2_e),
                                -1_e));
    case 12:  // π/10
      return isSin ? KMult(1_e / 4_e,
                           KAdd(-1_e, KExp(KMult(1_e / 2_e, KLn(5_e)))))
                   : KExp(KMult(1_e / 2_e,
                                KLn(KMult(1_e / 8_e,
                                          KAdd(5_e, KExp(KMult(1_e / 2_e,
                                                               KLn(5_e))))))));
    case 15:  // π/8
      return isSin
                 ? KMult(
                       1_e / 2_e,
                       KExp(KMult(
                           1_e / 2_e,
                           KLn(KAdd(2_e, KMult(-1_e, KExp(KMult(1_e / 2_e,
                                                                KLn(2_e)))))))))
                 : KMult(
                       1_e / 2_e,
                       KExp(KMult(
                           1_e / 2_e,
                           KLn(KAdd(2_e, KExp(KMult(1_e / 2_e, KLn(2_e))))))));
    case 20:  // π/6
      return isSin ? 1_e / 2_e
                   : KMult(KExp(KMult(1_e / 2_e, KLn(3_e))), 1_e / 2_e);
    case 24:  // π/5
      return isSin ? KExp(KMult(
                         1_e / 2_e,
                         KLn(KMult(
                             1_e / 8_e,
                             KAdd(5_e, KMult(-1_e, KExp(KMult(1_e / 2_e,
                                                              KLn(5_e)))))))))
                   : KMult(1_e / 4_e,
                           KAdd(1_e, KExp(KMult(1_e / 2_e, KLn(5_e)))));
    case 30:  // π/4
      return KExp(KMult(-1_e, 1_e / 2_e, KLn(2_e)));
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
  if (u->isMult() && u->numberOfChildren() == 2 && u->child(0)->isRational() &&
      u->child(1)->treeIsIdenticalTo(π_e)) {
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
  if (PatternMatching::MatchReplaceSimplify(
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
  } else if (PatternMatching::MatchReplace(u, KTrig(KATrig(KA, KB), KB), KA) ||
             PatternMatching::MatchReplaceSimplify(
                 u, KTrig(KATrig(KA, KB), KC),
                 KPow(KAdd(1_e, KMult(-1_e, KPow(KA, 2_e))), 1_e / 2_e))) {
    // sin(asin(x))=cos(acos(x))=x, sin(acos(x))=cos(asin(x))=√(1-x^2)
    /* TODO: what about asin(sin(acos(x)))? Maybe the simplification
     * asin(sin(...)) is more interesting than the simplification of
     * sin(acos(x)).
     * Same with atan(tan(asin)), atan(tan(acos)), acos(cos(asin)).
     * Maybe we should move this transformation (sin(asin(x)) and cos(acos(x)))
     * to advanced reduction.*/
    changed = true;
  } else if (Infinity::IsPlusOrMinusInfinity(firstArgument)) {
    // sin(±inf) = cos(±inf) = undef
    u->cloneTreeOverTree(KUndef);
    changed = true;
  }
  if (isOpposed && changed) {
    u->cloneTreeAtNode(-1_e);
    u->moveNodeAtNode(SharedTreeStack->push<Type::Mult>(2));
    Simplification::ShallowSystematicReduce(u);
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

static bool simplifyATrigOfTrig(Tree* u) {
  TypeBlock type = Type::Undef;
  PatternMatching::Context ctx;
  if (PatternMatching::Match(KATrig(KTrig(KA, KB), KC), u, &ctx)) {
    // asin(sin) or asin(cos) or acos(cos) or acos(sin)
    type = ctx.getNode(KB)->isOne() ? Type::Sin : Type::Cos;
  } else if (PatternMatching::Match(
                 KATanRad(KMult(KPow(KTrig(KA, 0_e), -1_e), KTrig(KA, 1_e))), u,
                 &ctx)) {
    // atan(sin/cos)
    type = Type::Tan;
  } else {
    return false;
  }
  // x = π*y
  const Tree* y = getPiFactor(ctx.getNode(KA));
  if (!y) {
    return false;
  }
  // We can simplify asin(cos) or acos(sin) using acos(x) = π/2 - asin(x)
  bool swapATrig = type != Type::Tan &&
                   (!ctx.getNode(KB)->treeIsIdenticalTo(ctx.getNode(KC)));
  /* For acos ∈ [0,π]:
   * Compute k = ⌊y⌋
   * if k is even, acos(cos(x)) = π*(y-k)
   * if k is odd, acos(cos(x)) = acos(cos(-x)) = π*(k-y+1)

   * For asin ∈ [-π/2,π/2]:
   * Compute k = ⌊y + 1/2⌋
   * if k is even, asin(sin(x)) = π*(y-k)
   * if k is odd, asin(sin(x)) = asin(sin(π-x)) = π*(k-y)

   * For atan ∈ [-π/2,π/2]:
   * Compute k = ⌊y + 1/2⌋
   * if k is even, atan(tan(x)) = π*(y-k)
   * if k is odd, atan(tan(x)) = atan(tan(x+π)) = π*(y-k) */
  Tree* res = PatternMatching::CreateSimplify(
      type == Type::Cos ? KFloor(KA) : KFloor(KAdd(KA, 1_e / 2_e)), {.KA = y});
  assert(res->isInteger());
  bool kIsEven = Integer::Handler(res).isEven();
  res->moveTreeOverTree(PatternMatching::CreateSimplify(
      KAdd(KA, KMult(-1_e, KB)), {.KA = y, .KB = res}));
  if (!kIsEven && type != Type::Tan) {
    res->moveTreeOverTree(
        PatternMatching::CreateSimplify(KMult(-1_e, KA), {.KA = res}));
    if (type != Type::Sin) {
      res->moveTreeOverTree(
          PatternMatching::CreateSimplify(KAdd(1_e, KA), {.KA = res}));
    }
  }
  if (swapATrig) {
    res->moveTreeOverTree(PatternMatching::CreateSimplify(
        KAdd(1_e / 2_e, KMult(-1_e, KA)), {.KA = res}));
  }
  res->moveTreeOverTree(
      PatternMatching::CreateSimplify(KMult(π_e, KA), {.KA = res}));
  u->moveTreeOverTree(res);
  return true;
}

bool Trigonometry::SimplifyATrig(Tree* u) {
  assert(u->isATrig());
  // atrig(trig(x))
  if (simplifyATrigOfTrig(u)) {
    return true;
  }
  const Tree* arg = u->child(0);
  bool isAsin = arg->nextTree()->isOne();
  ComplexSign argSign = ComplexSign::Get(arg);
  if (argSign.isZero()) {
    u->cloneTreeOverTree(isAsin ? 0_e : KMult(1_e / 2_e, π_e));
    return true;
  }
  if (!argSign.isReal()) {
    return false;
  }
  bool argIsOpposed = argSign.realSign().isNegative();
  bool changed = argIsOpposed;
  if (argIsOpposed) {
    u->child(0)->moveTreeOverTree(
        PatternMatching::CreateSimplify(KMult(-1_e, KA), {.KA = arg}));
  }
  if (arg->isOne()) {
    // asin(1) = π/2 and acos(1) = 0
    u->cloneTreeOverTree(isAsin ? KMult(1_e / 2_e, π_e) : 0_e);
    changed = true;
  } else if (arg->isHalf()) {
    // asin(1/2) = π/6 and acos(1/2) = π/3
    u->moveTreeOverTree(PatternMatching::CreateSimplify(
        KMult(π_e, KPow(KA, -1_e)), {.KA = isAsin ? 6_e : 3_e}));
    changed = true;
  } else if (arg->treeIsIdenticalTo(KExp(KMult(-1_e / 2_e, KLn(2_e)))) ||
             arg->treeIsIdenticalTo(
                 KMult(1_e / 2_e, KExp(KMult(1_e / 2_e, KLn(2_e)))))) {
    // TODO_PCJ: we shouldn't have to test both x=1/√2 and x=√2/2
    // acos(1/√2) = asin(1/√2) = π/4
    u->moveTreeOverTree(PatternMatching::CreateSimplify(KMult(1_e / 4_e, π_e)));
    changed = true;
  } else if (arg->isMult()) {
    /* TODO: Handle the same angles as ExactFormula (π/12, π/10, π/8 and π/5 are
     * missing) and find a better implementation for these special cases. */
    changed =
        // acos(√3/2) = π/6
        PatternMatching::MatchReplaceSimplify(
            u, KATrig(KMult(1_e / 2_e, KExp(KMult(1_e / 2_e, KLn(3_e)))), 0_e),
            KMult(1_e / 6_e, π_e)) ||
        // asin(√3/2) = π/3
        PatternMatching::MatchReplaceSimplify(
            u, KATrig(KMult(1_e / 2_e, KExp(KMult(1_e / 2_e, KLn(3_e)))), 1_e),
            KMult(1_e / 3_e, π_e)) ||
        changed;
  }
  if (argIsOpposed) {
    assert(changed);
    // asin(-x) = -asin(x) and acos(-x) = π - acos(x)
    PatternMatching::MatchReplaceSimplify(
        u, KA, isAsin ? KMult(-1_e, KA) : KAdd(π_e, KMult(-1_e, KA)));
  }
  return changed;
}

bool Trigonometry::SimplifyArcTangentRad(Tree* u) {
  // atan(-x) = -atan(x)
  if (PatternMatching::MatchReplaceSimplify(
          u, KATanRad(KMult(KA_s, -1_e, KB_s)),
          KMult(-1_e, KATanRad(KMult(KA_s, KB_s))))) {
    return true;
  }
  // atan(tan(x)) = x
  if (simplifyATrigOfTrig(u)) {
    return true;
  }
  if (PatternMatching::MatchReplaceSimplify(u, KATanRad(KInf),
                                            KMult(1_e / 2_e, π_e))) {
    return true;
  }
  assert(u->isATanRad());
  const Tree* arg = u->child(0);
  if (arg->isZero()) {
    // atan(0) = 0
    u->cloneTreeOverTree(0_e);
    return true;
  }
  if (arg->isOne()) {
    // atan(1) = π/4
    u->cloneTreeOverTree(KMult(1_e / 4_e, π_e));
    return true;
  }
  PatternMatching::Context ctx;
  if (PatternMatching::Match(KExp(KMult(1_e / 2_e, KLn(3_e))), arg, &ctx)) {
    // atan(√3) = π/3
    u->cloneTreeOverTree(KMult(1_e / 3_e, π_e));
    return true;
  }
  if (PatternMatching::Match(KExp(KMult(-1_e / 2_e, KLn(3_e))), arg, &ctx) ||
      PatternMatching::Match(KMult(1_e / 3_e, KExp(KMult(1_e / 2_e, KLn(3_e)))),
                             arg, &ctx)) {
    // TODO_PCJ: we shouldn't have to test both x=1/√3 and x=√3/3
    // atan(1/√3) = π/6
    u->cloneTreeOverTree(KMult(1_e / 6_e, π_e));
    return true;
  }
  return false;
}

/* TODO: Find an easier solution for nested expand/contract smart shallow
 * simplification. */

bool Trigonometry::ExpandTrigonometric(Tree* e) {
  // Trig(A?+B, C) = Trig(A, 0)*Trig(B, C) + Trig(A, 1)*Trig(B, C-1)
  return PatternMatching::MatchReplaceSimplify(
      e, KTrig(KAdd(KA, KB_p), KD),
      KAdd(KMult(KTrig(KAdd(KA), 0_e), KTrig(KAdd(KB_p), KD)),
           KMult(KTrig(KAdd(KA), 1_e), KTrig(KAdd(KB_p), KAdd(KD, -1_e)))));
}

bool Trigonometry::ContractTrigonometric(Tree* e) {
  /* TODO: Does not catch cos(B)^2+2*sin(B)^2, one solution could be changing
   * cos(B)^2 to 1-sin(B)^2, but we would also need it the other way, and having
   * both way would lead to infinite possible contractions. */
  return  // A?+cos(B)^2+C?+sin(B)^2+D? = 1 + A + C + D
      PatternMatching::MatchReplaceSimplify(
          e,
          KAdd(KA_s, KPow(KTrig(KB, 0_e), 2_e), KC_s, KPow(KTrig(KB, 1_e), 2_e),
               KD_s),
          KAdd(1_e, KA_s, KC_s, KD_s)) ||
      // A?*Trig(B, C)*D?*Trig(E, F)*G? =
      // 0.5*A*D*(Trig(B-E, TrigDiff(C,F)) + Trig(B+E, C+F))*G
      PatternMatching::MatchReplaceSimplify(
          e, KMult(KA_s, KTrig(KB, KC), KD_s, KTrig(KE, KF), KG_s),
          KMult(1_e / 2_e, KA_s, KD_s,
                KAdd(KTrig(KAdd(KB, KMult(-1_e, KE)), KTrigDiff(KC, KF)),
                     KTrig(KAdd(KB, KE), KAdd(KF, KC))),
                KG_s));
}

/* TODO: Maybe expand arccos(x) = π/2 - arcsin(x).
 * Beware of infinite expansion. */

}  // namespace Poincare::Internal
