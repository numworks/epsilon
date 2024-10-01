#include "trigonometry.h"

#include <omg/unreachable.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "degree.h"
#include "infinity.h"
#include "k_tree.h"
#include "number.h"
#include "order.h"
#include "polynomial.h"
#include "projection.h"
#include "rational.h"
#include "sign.h"
#include "systematic_reduction.h"
#include "trigonometry_exact_formulas.h"

namespace Poincare::Internal {

// Given n, return the exact expression of sin(n*π/120).
const Tree* getExactFormula(uint8_t n, bool isSin, bool* isOpposed) {
  // TODO_PCJ: add exact formula for inverse functions too
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
  Tree* reducedAngle = SharedTreeStack->pushMult(2);
  Rational::Push(n, 120);
  SharedTreeStack->pushPi();
  SystematicReduction::DeepReduce(reducedAngle);
  const Tree* result = ExactFormula::GetTrigOf(reducedAngle, isSin);
  reducedAngle->removeTree();
  return result;
}

const Tree* getExactFormula(const Tree* piFactor, bool isSin, bool* isOpposed) {
  /* We have exact values for 1/d with d ∈ {1,2,3,4,5,6,8,10,12}
   * We thus rule out piFactor that are not of of the form n/d, n∈ℕ
   * Which means piFactor*120 must be int (120 = lcm(1,2,3,4,5,6,8,10,12)) */
  Tree* multipleTree = Rational::Multiplication(120_e, piFactor);
  if (multipleTree->isInteger()) {
    // Trig is 2pi periodic, n can be retrieved as a uint8_t.
    multipleTree->moveTreeOverTree(IntegerHandler::Remainder(
        Integer::Handler(multipleTree), IntegerHandler(240)));
    assert(Integer::Is<uint8_t>(multipleTree));
    uint8_t n = Integer::Handler(multipleTree).to<uint8_t>();
    multipleTree->removeTree();
    return getExactFormula(n, isSin, isOpposed);
  }
  multipleTree->removeTree();
  return nullptr;
}

bool Trigonometry::ReduceTrigDiff(Tree* e) {
  assert(e->isTrigDiff());
  /* TrigDiff is used to factorize Trigonometric contraction. It determines the
   * first term of these equations :
   * 2*sin(x)*sin(y) = cos(x-y) - cos(x+y)  -> TrigDiff(1,1) = 0
   * 2*sin(x)*cos(y) = sin(x-y) + sin(x+y)  -> TrigDiff(1,0) = 1
   * 2*cos(x)*sin(y) =-sin(x-y) + sin(x+y)  -> TrigDiff(0,1) = 3
   * 2*cos(x)*cos(y) = cos(x-y) + cos(x+y)  -> TrigDiff(0,0) = 0
   */
  // Reduce children as trigonometry second elements.
  bool isOpposed = false;
  Tree* x = e->child(0);
  ReduceTrigSecondElement(x, &isOpposed);
  Tree* y = x->nextTree();
  ReduceTrigSecondElement(y, &isOpposed);
  // Find TrigDiff value depending on children types (sin or cos)
  bool isDifferent = x->type() != y->type();
  // Account for sign difference between TrigDiff(1,0) and TrigDiff(0,1)
  if (isDifferent && x->isZero()) {
    isOpposed = !isOpposed;
  }
  // Replace TrigDiff with result
  e->cloneTreeOverTree(isDifferent ? (isOpposed ? 3_e : 1_e)
                                   : (isOpposed ? 2_e : 0_e));
  return true;
}

// If e is of the form π*n, return n.
const Tree* getPiFactor(const Tree* e) {
  if (e->treeIsIdenticalTo(π_e)) {
    return 1_e;
  }
  if (e->isZero()) {
    return 0_e;
  }
  if (e->isMult() && e->numberOfChildren() == 2 && e->child(0)->isRational() &&
      e->child(1)->treeIsIdenticalTo(π_e)) {
    return e->child(0);
  }
  return nullptr;
}

static Tree* computeSimplifiedPiFactor(const Tree* piFactor) {
  assert(piFactor && piFactor->isRational());
  /* x = piFactor * π
   * Look for equivalent angle in [0,2π]
   * Compute k = ⌊piFactor⌋
   * if k is even, x = π*(piFactor-k)
   * if k is odd, x = π*(piFactor-k+1) */
  Tree* res = PatternMatching::CreateSimplify(KFloor(KA), {.KA = piFactor});
  assert(res->isInteger());
  bool kIsEven = Integer::Handler(res).isEven();
  res->moveTreeOverTree(PatternMatching::CreateSimplify(
      KAdd(KA, KMult(-1_e, KB)), {.KA = piFactor, .KB = res}));
  if (!kIsEven) {
    res->moveTreeOverTree(
        PatternMatching::CreateSimplify(KAdd(1_e, KA), {.KA = res}));
  }
  return res;
}

static Tree* computeSimplifiedPiFactorForType(const Tree* piFactor, Type type) {
  assert(TypeBlock::IsDirectTrigonometryFunction(type) &&
         !TypeBlock::IsTrig(type));
  assert(piFactor && piFactor->isRational());
  /* x = piFactor * π
   *
   * For cos: look for equivalent angle in [0,π] (since acos ∈ [0,π])
   * Compute k = ⌊piFactor⌋
   * if k is even, acos(cos(x)) = π*(piFactor-k)
   * if k is odd, acos(cos(x)) = acos(cos(-x)) = π*(k-piFactor+1)

   * For sin: look for equivalent angle in [-π/2,π/2] (since asin ∈ [-π/2,π/2])
   * Compute k = ⌊piFactor + 1/2⌋
   * if k is even, asin(sin(x)) = π*(piFactor-k)
   * if k is odd, asin(sin(x)) = asin(sin(π-x)) = π*(k-piFactor)

   * For tan: look for equivalent angle in [-π/2,π/2] (since atan ∈ [-π/2,π/2])
   * Compute k = ⌊piFactor + 1/2⌋
   * if k is even, atan(tan(x)) = π*(piFactor-k)
   * if k is odd, atan(tan(x)) = atan(tan(x+π)) = π*(piFactor-k) */
  Tree* res = PatternMatching::CreateSimplify(
      type == Type::Cos ? KFloor(KA) : KFloor(KAdd(KA, 1_e / 2_e)),
      {.KA = piFactor});
  assert(res->isInteger());
  bool kIsEven = Integer::Handler(res).isEven();
  res->moveTreeOverTree(PatternMatching::CreateSimplify(
      KAdd(KA, KMult(-1_e, KB)), {.KA = piFactor, .KB = res}));
  if (!kIsEven && type != Type::Tan) {
    res->moveTreeOverTree(
        PatternMatching::CreateSimplify(KMult(-1_e, KA), {.KA = res}));
    if (type != Type::Sin) {
      res->moveTreeOverTree(
          PatternMatching::CreateSimplify(KAdd(1_e, KA), {.KA = res}));
    }
  }
  return res;
}

bool Trigonometry::ReduceTrig(Tree* e) {
  assert(e->isTrig());
  // Trig(x,y) = {Cos(x) if y=0, Sin(x) if y=1, -Cos(x) if y=2, -Sin(x) if y=3}
  Tree* firstArgument = e->child(0);
  Tree* secondArgument = firstArgument->nextTree();
  bool isOpposed = false;
  bool changed = ReduceTrigSecondElement(secondArgument, &isOpposed);
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
    bool tempIsOpposed = isOpposed;
    const Tree* exact = getExactFormula(piFactor, isSin, &tempIsOpposed);
    if (exact) {
      e->cloneTreeOverTree(exact);
      isOpposed = tempIsOpposed;
      changed = true;
    } else {
      // Translate angle in [0,2π]
      TreeRef simplifiedPiFactor = piFactor->cloneTree();
      if (isOpposed) {
        // -cos(x) = cos(x+π) and -sin(x) = sin(x+π)
        MoveTreeOverTree(simplifiedPiFactor,
                         PatternMatching::CreateSimplify(
                             KAdd(KA, 1_e), {.KA = simplifiedPiFactor}));
        isOpposed = false;
      }
      MoveTreeOverTree(simplifiedPiFactor,
                       computeSimplifiedPiFactor(simplifiedPiFactor));
      if (!simplifiedPiFactor->treeIsIdenticalTo(piFactor)) {
        firstArgument->moveTreeOverTree(PatternMatching::CreateSimplify(
            KMult(π_e, KA), {.KA = simplifiedPiFactor}));
        changed = true;
      }
      simplifiedPiFactor->removeTree();
      return changed;
    }
  } else if (PatternMatching::MatchReplace(e, KTrig(KATrig(KA, KB), KB), KA) ||
             PatternMatching::MatchReplaceSimplify(
                 e, KTrig(KATrig(KA, KB), KC),
                 KPow(KAdd(1_e, KMult(-1_e, KPow(KA, 2_e))), 1_e / 2_e))) {
    // sin(asin(x))=cos(acos(x))=x, sin(acos(x))=cos(asin(x))=√(1-x^2)
    // TODO_PCJ: detect tan(atan(x)) (but tan is split up in sin/cos)
    /* TODO: what about asin(sin(acos(x)))? Maybe the simplification
     * asin(sin(...)) is more interesting than the simplification of
     * sin(acos(x)).
     * Same with atan(tan(asin)), atan(tan(acos)), acos(cos(asin)).
     * Maybe we should move this transformation (sin(asin(x)) and cos(acos(x)))
     * to advanced reduction.*/
    changed = true;
  } else if (Infinity::IsPlusOrMinusInfinity(firstArgument)) {
    // sin(±inf) = cos(±inf) = undef
    e->cloneTreeOverTree(KUndef);
    return true;
  }
  if (isOpposed && changed) {
    e->cloneTreeAtNode(-1_e);
    e->moveNodeAtNode(SharedTreeStack->pushMult(2));
    SystematicReduction::ShallowReduce(e);
  }
  // TODO_PCJ: cos(atan(x)) -> 1/sqrt(1+x^2) and sin(atan(x))-> x/sqrt(1+x^2)
  return changed;
}

bool Trigonometry::ReduceTrigSecondElement(Tree* e, bool* isOpposed) {
  // Trig second element is always expected to be a reduced integer.
  assert(e->isInteger() && !SystematicReduction::DeepReduce(e));
  bool changed = false;
  IntegerHandler i = Integer::Handler(e);
  Tree* remainder = IntegerHandler::Remainder(i, IntegerHandler(4));
  if (Order::CompareSystem(remainder, 2_e) >= 0) {
    changed = true;
    *isOpposed = !*isOpposed;
    remainder->moveTreeOverTree(
        IntegerHandler::Remainder(i, IntegerHandler(2)));
    assert(!remainder->treeIsIdenticalTo(e));
  }
  changed |= !remainder->treeIsIdenticalTo(e);
  if (changed) {
    e->moveTreeOverTree(remainder);
  } else {
    remainder->removeTree();
  }
  // Simplified second element should have only two possible values.
  assert(e->isZero() || e->isOne());
  return changed;
}

// Transform atan(sin(a)/cos(b)) in atan(sin(x)/cos(x)) if possible
static void preprocessAtanOfTan(Tree* e) {
  PatternMatching::Context ctx;
  // Match atan(sin(a)/cos(b))
  if (!PatternMatching::Match(
          e, KATanRad(KMult(KPow(KTrig(KB, 0_e), -1_e), KTrig(KA, 1_e))),
          &ctx)) {
    return;
  }
  Tree* a = const_cast<Tree*>(ctx.getTree(KA));
  Tree* b = const_cast<Tree*>(ctx.getTree(KB));
  const Tree* aFactor = getPiFactor(a);
  const Tree* bFactor = getPiFactor(b);
  if (!aFactor || !bFactor) {
    return;
  }
  assert(aFactor->isRational() && bFactor->isRational());

  /* Transform atan(sin(a)/cos(b)) in atan(sin(x)/cos(x))
   * a = b      ==>  sin(a)/cos(b) = sin(a)/cos(a) = sin(b)/cos(b)
   * a = -b     ==>  sin(a)/cos(b) = sin(a)/cos(a)
   * a = π - b  ==>  sin(a)/cos(b) = sin(b)/cos(b)
   * a = π + b  ==>  sin(a)/cos(b) = sin(-a)/cos(-a) = sin(-b)/cos(-b) */

  Tree* sub = PatternMatching::CreateSimplify(KAdd(KA, KMult(-1_e, KB)),
                                              {.KA = aFactor, .KB = bFactor});
  sub->moveTreeOverTree(computeSimplifiedPiFactor(sub));
  assert(sub->isRational());
  if (sub->treeIsIdenticalTo(0_e)) {
    // a = b ==> sin(a)/cos(a)
    sub->removeTree();
    b->cloneTreeOverTree(a);
    return;
  } else if (sub->treeIsIdenticalTo(1_e)) {
    // a = π + b ==> sin(-a)/cos(-a)
    sub->removeTree();
    a->moveTreeOverTree(
        PatternMatching::CreateSimplify(KMult(-1_e, KA), {.KA = a}));
    b->cloneTreeOverTree(a);
    return;
  }
  sub->removeTree();

  Tree* add = PatternMatching::CreateSimplify(KAdd(KA, KB),
                                              {.KA = aFactor, .KB = bFactor});
  add->moveTreeOverTree(computeSimplifiedPiFactor(add));
  assert(add->isRational());
  if (add->treeIsIdenticalTo(0_e)) {
    // a = -b ==> sin(a)/cos(a)
    add->removeTree();
    b->cloneTreeOverTree(a);
    return;
  } else if (add->treeIsIdenticalTo(1_e)) {
    // a = π - b ==> sin(b)/cos(b)
    add->removeTree();
    a->cloneTreeOverTree(b);
    return;
  }
  add->removeTree();
}

static bool simplifyATrigOfTrig(Tree* e) {
  TypeBlock type = Type::Undef;
  PatternMatching::Context ctx;
  preprocessAtanOfTan(e);
  if (PatternMatching::Match(e, KATrig(KTrig(KA, KB), KC), &ctx)) {
    // asin(sin) or asin(cos) or acos(cos) or acos(sin)
    type = ctx.getTree(KB)->isOne() ? Type::Sin : Type::Cos;
  } else if (PatternMatching::Match(
                 e, KATanRad(KMult(KPow(KTrig(KA, 0_e), -1_e), KTrig(KA, 1_e))),
                 &ctx)) {
    // atan(sin/cos)
    type = Type::Tan;
  } else {
    return false;
  }
  // x = π*y
  const Tree* y = getPiFactor(ctx.getTree(KA));
  if (!y) {
    return false;
  }
  // We can simplify asin(cos) or acos(sin) using acos(x) = π/2 - asin(x)
  bool swapATrig = type != Type::Tan &&
                   (!ctx.getTree(KB)->treeIsIdenticalTo(ctx.getTree(KC)));
  Tree* res = computeSimplifiedPiFactorForType(y, type);
  if (swapATrig) {
    res->moveTreeOverTree(PatternMatching::CreateSimplify(
        KAdd(1_e / 2_e, KMult(-1_e, KA)), {.KA = res}));
  }
  res->moveTreeOverTree(
      PatternMatching::CreateSimplify(KMult(π_e, KA), {.KA = res}));
  e->moveTreeOverTree(res);
  return true;
}

bool Trigonometry::ReduceATrig(Tree* e) {
  assert(e->isATrig());
  // atrig(trig(x))
  if (simplifyATrigOfTrig(e)) {
    return true;
  }
  Tree* arg = e->child(0);
  bool isAsin = arg->nextTree()->isOne();
  ComplexSign argSign = GetComplexSign(arg);
  if (!argSign.isReal()) {
    return false;
  }
  bool argIsOpposed = argSign.realSign().canBeStrictlyNegative() &&
                      !argSign.realSign().canBeStrictlyPositive();
  bool changed = argIsOpposed;
  if (argIsOpposed) {
    PatternMatching::MatchReplaceSimplify(arg, KA, KMult(-1_e, KA));
  }
  const Tree* angle = ExactFormula::GetAngleOf(arg, isAsin);
  if (angle) {
    e->cloneTreeOverTree(angle);
    changed = true;
  }
  if (argIsOpposed) {
    assert(changed);
    // asin(-x) = -asin(x) and acos(-x) = π - acos(x)
    PatternMatching::MatchReplaceSimplify(
        e, KA, isAsin ? KMult(-1_e, KA) : KAdd(π_e, KMult(-1_e, KA)));
  }
  return changed;
}

bool Trigonometry::ReduceArcTangentRad(Tree* e) {
  // atan(-x) = -atan(x)
  if (PatternMatching::MatchReplaceSimplify(
          e, KATanRad(KMult(KA_s, -1_e, KB_s)),
          KMult(-1_e, KATanRad(KMult(KA_s, KB_s))))) {
    return true;
  }
  // atan(tan(x)) = x
  if (simplifyATrigOfTrig(e)) {
    return true;
  }
  if (PatternMatching::MatchReplaceSimplify(e, KATanRad(KInf),
                                            KMult(1_e / 2_e, π_e))) {
    return true;
  }
  assert(e->isATanRad());
  const Tree* arg = e->child(0);
  if (arg->isZero()) {
    // atan(0) = 0
    e->cloneTreeOverTree(0_e);
    return true;
  }
  if (arg->isOne()) {
    // atan(1) = π/4
    e->cloneTreeOverTree(KMult(1_e / 4_e, π_e));
    return true;
  }
  PatternMatching::Context ctx;
  if (PatternMatching::Match(arg, KExp(KMult(1_e / 2_e, KLn(3_e))), &ctx)) {
    // atan(√3) = π/3
    e->cloneTreeOverTree(KMult(1_e / 3_e, π_e));
    return true;
  }
  assert(!PatternMatching::Match(arg, KExp(KMult(-1_e / 2_e, KLn(3_e))), &ctx));
  if (PatternMatching::Match(
          arg, KMult(1_e / 3_e, KExp(KMult(1_e / 2_e, KLn(3_e)))), &ctx)) {
    // atan(1/√3) = π/6
    e->cloneTreeOverTree(KMult(1_e / 6_e, π_e));
    return true;
  }
  // TODO_PCJ: Reduce atan(1/x) in dep(sign(x)*π/2-atan(x),{1/x})
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
  return
      // A?+cos(B)^2+C?+sin(B)^2+D? = 1 + A + C + D
      PatternMatching::MatchReplaceSimplify(
          e,
          KAdd(KA_s, KPow(KTrig(KB, 0_e), 2_e), KC_s, KPow(KTrig(KB, 1_e), 2_e),
               KD_s),
          KDep(KAdd(1_e, KA_s, KC_s, KD_s), KDepList(KB))) ||
      // A?*Trig(B, C)*D?*Trig(E, F)*G? =
      // 0.5*A*D*(Trig(B-E, TrigDiff(C,F)) + Trig(B+E, C+F))*G
      PatternMatching::MatchReplaceSimplify(
          e, KMult(KA_s, KTrig(KB, KC), KD_s, KTrig(KE, KF), KG_s),
          KMult(1_e / 2_e, KA_s, KD_s,
                KAdd(KTrig(KAdd(KB, KMult(-1_e, KE)), KTrigDiff(KC, KF)),
                     KTrig(KAdd(KB, KE), KAdd(KF, KC))),
                KG_s));
}

Type Trigonometry::GetInverseType(Type type) {
  switch (type) {
    case Type::Cos:
      return Type::ACos;
    case Type::Sin:
      return Type::ASin;
    case Type::Tan:
      return Type::ATan;
    default:
      OMG::unreachable();
  }
}

/* TODO: Maybe expand arccos(x) = π/2 - arcsin(x).
 * Beware of infinite expansion. */

}  // namespace Poincare::Internal
