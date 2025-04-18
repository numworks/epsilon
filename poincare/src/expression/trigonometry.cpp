#include "trigonometry.h"

#include <omg/unreachable.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "advanced_operation.h"
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
  const Tree* result =
      ExactFormula::GetTrigOf(reducedAngle, isSin ? Type::Sin : Type::Cos);
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
   * Look for equivalent angle in [0,2π[
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
  assert((TypeBlock::IsDirectTrigonometryFunction(type) ||
          TypeBlock::IsArg(type)) &&
         !TypeBlock::IsTrig(type));
  assert(piFactor);
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

   * For tan: look for equivalent angle in [-π/2,π/2]
   * (since atan ∈ ]-π/2,π/2[ and we ignore undefined values for x=n*π/2)
   * Compute k = ⌊piFactor + 1/2⌋
   * if k is even, atan(tan(x)) = π*(piFactor-k)
   * if k is odd, atan(tan(x)) = atan(tan(x+π)) = π*(piFactor-k)
   *
   * For arg: look for equivalent angle in ]-π,π]
   * (since principal argument ∈ ]-π,π])
   * Compute k = ⌈piFactor⌉
   * if k is even, Arg(exp(i*x)) = π*(piFactor-k)
   * if k is odd, Arg(exp(i*x)) = π*(piFactor-k+1) */
  Tree* res = PatternMatching::CreateSimplify(
      type == Type::Cos   ? KFloor(KA)
      : type == Type::Arg ? KMult(-1_e, KFloor(KMult(-1_e, KA)))
                          : KFloor(KAdd(KA, 1_e / 2_e)),
      {.KA = piFactor});
  if (!res->isInteger()) {
    /* If it was not possible to reduce ⌊piFactor + 1/2⌋ or ⌈piFactor⌉, then it
     * is impossible to know the parity of k, and the reduction cannot be
     * processed further. */
    res->removeTree();
    return nullptr;
  }
  bool kIsEven = Integer::Handler(res).isEven();
  res->moveTreeOverTree(PatternMatching::CreateSimplify(
      KAdd(KA, KMult(-1_e, KB)), {.KA = piFactor, .KB = res}));
  if (!kIsEven && type != Type::Tan) {
    if (type != Type::Arg) {
      res->moveTreeOverTree(
          PatternMatching::CreateSimplify(KMult(-1_e, KA), {.KA = res}));
    }
    if (type != Type::Sin) {
      res->moveTreeOverTree(
          PatternMatching::CreateSimplify(KAdd(1_e, KA), {.KA = res}));
    }
  }
  return res;
}

/* Reduce to principal argument in ]-π,π] if the argument is of the form
 * r*π with r rational */
bool Trigonometry::ReduceArgumentToPrincipal(Tree* e) {
  assert(GetComplexSign(e).isReal());
  const Tree* piFactor = getPiFactor(e);
  if (piFactor) {
    TreeRef simplifiedPiFactor =
        computeSimplifiedPiFactorForType(piFactor, Type::Arg);
    if (simplifiedPiFactor) {
      e->moveTreeOverTree(PatternMatching::CreateSimplify(
          KMult(KA, π_e), {.KA = simplifiedPiFactor}));
      simplifiedPiFactor->removeTree();
      return true;
    }
  }
  // arg and atan are already in ]-π,π]
  return e->isArg() || e->isATanRad() ||
         (e->isRational() &&
          Rational::AbsSmallerThanPi(e) == OMG::Troolean::True);
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
#if POINCARE_TRIGONOMETRY_HYPERBOLIC
             PatternMatching::MatchReplace(
                 e, KTrig(KMult(KArCosH(KA), i_e), 0_e), KA) ||
#endif
             PatternMatching::MatchReplaceSimplify(
                 e, KTrig(KATrig(KA, KB), KC),
                 KPow(KAdd(1_e, KMult(-1_e, KPow(KA, 2_e))), 1_e / 2_e))) {
    /* sin(asin(x))=cos(acos(x))=cos(arcosh(x)*i)=x,
     * sin(acos(x))=cos(asin(x))=√(1-x^2) */
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

static Tree* SimplifyATrigOfTrig(const Tree* arg, Type type, bool swapATrig) {
  assert(type != Type::Undef);
  /* asin(sin(i*x)) = i*x, acos(cos(i*x)) = i*abs(i*x) and atan(tan(i*x)) = i*x
   * for x real */
  Tree* result = nullptr;
  if (GetComplexSign(arg).isPureIm()) {
    if (type == Type::Sin || type == Type::Tan) {
      result = arg->cloneTree();
    } else {
      assert(type == Type::Cos);
      result =
          PatternMatching::CreateSimplify(KMult(i_e, KAbs(KA)), {.KA = arg});
    }
  } else {
    // x = π*y
    const Tree* rationalPiFactor = getPiFactor(arg);
    TreeRef reducedPiFactor;
    if (rationalPiFactor) {
      reducedPiFactor =
          computeSimplifiedPiFactorForType(rationalPiFactor, type);
    } else {
      TreeRef genericPiFactor = PatternMatching::CreateSimplify(
          KMult(KA, KPow(π_e, -1_e)), {.KA = arg});
      reducedPiFactor = computeSimplifiedPiFactorForType(genericPiFactor, type);
      genericPiFactor->removeTree();
    }
    if (!reducedPiFactor) {
      return nullptr;
    }
    result = reducedPiFactor;
    PatternMatching::MatchReplaceSimplify(result, KA, KMult(π_e, KA));
  }
  assert(result);

  // We can simplify asin(cos) or acos(sin) using acos(x) = π/2 - asin(x)
  if (swapATrig) {
    PatternMatching::MatchReplaceSimplify(
        result, KA, KAdd(KMult(π_e, 1_e / 2_e), KMult(-1_e, KA)));
  }
  return result;
}

bool Trigonometry::ReduceATrig(Tree* e) {
  assert(e->isATrig());
  // atrig(trig(x))
  PatternMatching::Context ctx;
  if (PatternMatching::Match(e, KATrig(KTrig(KA, KB), KC), &ctx)) {
    // asin(sin) or asin(cos) or acos(cos) or acos(sin)
    Type type = ctx.getTree(KB)->isOne() ? Type::Sin : Type::Cos;
    bool swapATrig = type != (ctx.getTree(KC)->isOne() ? Type::Sin : Type::Cos);
    Tree* result = SimplifyATrigOfTrig(ctx.getTree(KA), type, swapATrig);
    if (result) {
      e->moveTreeOverTree(result);
      return true;
    }
  }
  Tree* arg = e->child(0);
  bool isAsin = arg->nextTree()->isOne();
  ComplexSign argSign = GetComplexSign(arg);
  if (!argSign.isReal()) {
    return false;
  }
  bool changed = false;
  bool argIsOpposed = !argSign.isNull() && argSign.realSign().isNegative();
  if (argIsOpposed) {
    changed = true;
    PatternMatching::MatchReplaceSimplify(arg, KA, KMult(-1_e, KA));
  }
  const Tree* angle =
      ExactFormula::GetAngleOf(arg, isAsin ? Type::Sin : Type::Cos);
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

// Return x such that atan(sin(a)/cos(b)) = atan(tan(x))
static Tree* GetAtanTanArg(const Tree* e) {
  // Match atan(sin(a)/cos(b))
  PatternMatching::Context ctx;
  if (!PatternMatching::Match(
          e, KATanRad(KMult(KPow(KTrig(KB, 0_e), -1_e), KTrig(KA, 1_e))),
          &ctx)) {
    return nullptr;
  }
  const Tree* a = ctx.getTree(KA);
  const Tree* b = ctx.getTree(KB);
  if (a->treeIsIdenticalTo(b)) {
    /* No need to check -a == b because cos(-1*a) is systematically reduced to
     * cos(a), so b can't be a mult with -1 factor. */
    return a->cloneTree();
  }
  const Tree* aFactor = getPiFactor(a);
  // Skip getPiFactor if aFactor is nullptr
  const Tree* bFactor = aFactor ? getPiFactor(b) : nullptr;
  if (!aFactor || !bFactor) {
    return nullptr;
  }
  assert(aFactor->isRational() && bFactor->isRational());

  /* Handle sin(a)/cos(b) like tan(x) if :
   * a = b      ==>  x = a = b     (sin(a) = sin(b))
   * a = -b     ==>  x = a         (cos(b) = cos(a))
   * a = π - b  ==>  x = b         (sin(a) = sin(π - b) = sin(b)  )
   * a = π + b  ==>  x = -a = -b   (cos(b) = cos(a - π) = -cos(-a)) */

  Tree* sub = PatternMatching::CreateSimplify(KAdd(KA, KMult(-1_e, KB)),
                                              {.KA = aFactor, .KB = bFactor});
  sub->moveTreeOverTree(computeSimplifiedPiFactor(sub));
  assert(sub->isRational());
  if (sub->isZero()) {
    sub->removeTree();
    // a = b, return a
    return a->cloneTree();
  } else if (sub->isOne()) {
    sub->removeTree();
    // a = π + b, return -a
    return PatternMatching::CreateSimplify(KMult(-1_e, KA), {.KA = a});
  }
  sub->removeTree();

  Tree* add = PatternMatching::CreateSimplify(KAdd(KA, KB),
                                              {.KA = aFactor, .KB = bFactor});
  add->moveTreeOverTree(computeSimplifiedPiFactor(add));
  assert(add->isRational());
  if (add->isZero()) {
    add->removeTree();
    // a = -b, return a
    return a->cloneTree();
  } else if (add->isOne()) {
    add->removeTree();
    // a = π - b, return b
    return b->cloneTree();
  }
  add->removeTree();
  return nullptr;
}

bool Trigonometry::ReduceArcTangentRad(Tree* e) {
  assert(e->isATanRad());
  Tree* arg = e->child(0);
  ComplexSign argSign = GetComplexSign(arg);
  Sign argRealSign = argSign.realSign();
  /* Oppose the argument if arg is negative, or if sign is unknown and there is
   * a -1 factor. Last case allows solving atan(-tan(x)). */
  PatternMatching::Context ctx;
  bool argIsOpposed =
      !argRealSign.isNull() &&
      (argRealSign.isNegative() ||
       (argRealSign.canBeStrictlyNegative() &&
        PatternMatching::Match(arg, KMult(KA_s, -1_e, KB_s), &ctx)));
  if (argIsOpposed) {
    // atan(-x) = -atan(x)
    return PatternMatching::MatchReplaceSimplify(
        e, KATanRad(KA), KMult(-1_e, KATanRad(KMult(-1_e, KA))));
  }
  Tree* atanTanArg = GetAtanTanArg(e);
  if (atanTanArg) {
    // Handle atan(tan(x))
    TreeRef result = SimplifyATrigOfTrig(atanTanArg, Type::Tan, false);
    atanTanArg->removeTree();
    if (result) {
      e->moveTreeOverTree(result);
      return true;
    }
  }
  if (!argSign.isReal()) {
    return false;
  }
  if (PatternMatching::Match(arg, KMult(-1_e, KAdd(KA_s)), &ctx)) {
    /* Expand the -1 to directly catch exact values such as (-1)*(-2+√3).
     * Advanced reduction will factorize it later if needed.
     * This shortcuts an advanced reduction step. */
    AdvancedOperation::ExpandMult(arg);
  }
  if (arg->isInf()) {
    e->cloneTreeOverTree(KMult(1_e / 2_e, π_e));
    return true;
  }
  const Tree* angle = ExactFormula::GetAngleOf(arg, Type::Tan);
  if (angle) {
    e->cloneTreeOverTree(angle);
    return true;
  }

  // TODO_PCJ: Reduce atan(1/x) in dep(sign(x)*π/2-atan(x),{1/x})
  return false;
}

bool Trigonometry::ReduceArCosH(Tree* e) {
#if POINCARE_TRIGONOMETRY_HYPERBOLIC
  PatternMatching::Context ctx;
  if (PatternMatching::Match(e, KArCosH(KTrig(KA, 0_e)), &ctx) &&
      GetComplexSign(ctx.getTree(KA)).isPureIm()) {
    // acosh(cos(x)) = abs(x) for x pure imaginary
    e->moveTreeOverTree(PatternMatching::CreateSimplify(KAbs(KA), ctx));
    return true;
  }
#endif
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
      // A?*tan(atan(B))*C? = A*B*C
      PatternMatching::MatchReplaceSimplify(
          e,
          KMult(KA_s, KPow(KTrig(KATanRad(KB), 0_e), -1_e),
                KTrig(KATanRad(KB), 1_e), KC_s),
          KMult(KA_s, KB, KC_s)) ||
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
