#include "trigonometry.h"

#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/number.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/pattern_matching.h>

namespace PoincareJ {

// TODO: tests

bool Trigonometry::IsDirect(const Tree* node) {
  switch (node->type()) {
    case BlockType::Cosine:
    case BlockType::Sine:
    case BlockType::Tangent:
      return true;
    default:
      return false;
  }
}

bool Trigonometry::IsInverse(const Tree* node) {
  switch (node->type()) {
    case BlockType::ArcCosine:
    case BlockType::ArcSine:
    case BlockType::ArcTangent:
      return true;
    default:
      return false;
  }
}

const Tree* Trigonometry::ExactFormula(uint8_t n, bool isSin, bool* isOpposed) {
  // Sin and cos are 2pi periodic. With sin(n*π/12), n goes from 0 to 23.
  n = n % 24;
  // Formula is opposed depending on the quadrant
  if ((isSin && n >= 12) || (!isSin && n >= 6 && n < 18)) {
    *isOpposed = !*isOpposed;
  }
  // Last two quadrant are now equivalent to the first two ones.
  n = n % 12;
  /* In second half of first quadrant and in first half of second quadrant,
   * we can simply swap Sin/Cos formulas. */
  if (n > 3 && n <= 9) {
    isSin = !isSin;
  }
  // Second quadrant is now equivalent to the first one.
  n = n % 6;
  // Second half of the first quadrant is the first half mirrored.
  if (n > 3) {
    n = 6 - n;
  }
  // Only 4 formulas are left for angles 0, π/12, π/6 and π/4.
  assert(n >= 0 && n < 4);
  switch (n) {
    case 0:  // sin(0) / cos(0)
      return isSin ? 0_e : 1_e;
    case 1:  // sin(π/12) / cos(π/12)
      return isSin
                 ? KMult(KAdd(KExp(KMult(KHalf, KLn(3_e))), -1_e),
                         KPow(KMult(KExp(KMult(KHalf, KLn(2_e))), 2_e), -1_e))
                 : KMult(KAdd(KExp(KMult(KHalf, KLn(3_e))), 1_e),
                         KPow(KMult(KExp(KMult(KHalf, KLn(2_e))), 2_e), -1_e));
    case 2:  // sin(π/6) / cos(π/6)
      return isSin ? KHalf : KMult(KExp(KMult(KHalf, KLn(3_e))), KHalf);
    default:  // sin(π/4) = cos(π/4)
      return KExp(KMult(-1_e, KHalf, KLn(2_e)));
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
  Tree* secondArgument = u->child(1);
  bool isOpposed = false;
  bool changed = SimplifyTrigSecondElement(secondArgument, &isOpposed);
  assert(secondArgument->isZero() || secondArgument->isOne());
  bool isSin = secondArgument->isOne();
  // cos(-x) = cos(x) and sin(-x) = -sin(x)
  Tree* firstArgument = u->child(0);
  if (PatternMatching::MatchReplaceAndSimplify(
          firstArgument, KMult(KTA, -1_e, KTB), KMult(KTA, KTB))) {
    changed = true;
    if (isSin) {
      isOpposed = !isOpposed;
    }
  }
  const Tree* piFactor = getPiFactor(firstArgument);
  if (piFactor) {
    // TODO: Also handle (n/10)*π and (n/8)*π
    // Find n to match Trig((n/12)*π, ...) with exact value.
    Tree* multipleTree = Rational::Multiplication(12_e, piFactor);
    if (multipleTree->isInteger()) {
      // Trig is 2pi periodic, n can be retrieved as a uint8_t.
      multipleTree->moveTreeOverTree(IntegerHandler::Remainder(
          Integer::Handler(multipleTree), IntegerHandler(24)));
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
    u->moveNodeAtNode(SharedEditionPool->push<BlockType::Multiplication>(2));
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
  if (PatternMatching::Match(KATrig(KTrig(KA, KB), KB), u, &ctx)) {
    const Tree* piFactor = getPiFactor(ctx.getNode(KA));
    if (!piFactor) {
      return false;
    }
    // atrig(trig(π*piFactor, i), i)
    bool isSin = ctx.getNode(KB)->isOne();
    // Compute k = ⌊piFactor⌋ for acos, ⌊piFactor + π/2⌋ for asin.
    // acos(cos(π*r)) = π*(y-k) if k even, π*(k-y+1) otherwise.
    // asin(sin(π*r)) = π*(y-k) if k even, π*(k-y) otherwise.
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
    res->moveTreeOverTree(
        PatternMatching::CreateAndSimplify(KMult(π_e, KA), {.KA = res}));
    u->moveTreeOverTree(res);
    return true;
  }
  bool isAsin = u->child(1)->isOne();
  const Tree* arg = u->child(0);
  if (arg->isZero()) {
    u->cloneTreeOverTree(isAsin ? 0_e : KMult(KHalf, π_e));
    return true;
  }
  bool argIsOpposed = Sign::GetSign(arg).isStrictlyNegative();
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
    // TODO: Also handle (n/10)*π and (n/8)*π
    // TODO : Find a better implementation for these special cases.
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
        // TODO: Add (√6-√2)/4, (√6+√2)/4 and their opposites.
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

/* TODO : Find an easier solution for nested expand/contract smart shallow
 * simplification. */

bool Trigonometry::ExpandTrigonometric(Tree* ref) {
  /* Trig(A?+B, C) = Trig(A, 0)*Trig(B, C) + Trig(A, 1)*Trig(B, C-1)
   * ExpandTrigonometric is more complex than other expansions and cannot be
   * factorized with DistributeOverNAry. */
  // MatchReplaceAndSimplify's cannot be used because of nested expansion.
  if (!PatternMatching::MatchAndReplace(
          ref, KTrig(KAdd(KTA, KB), KC),
          KAdd(KMult(KTrig(KAdd(KTA), 0_e), KTrig(KB, KC)),
               KMult(KTrig(KAdd(KTA), 1_e), KTrig(KB, KAdd(KC, -1_e)))))) {
    return false;
  }
  EditionReference newMult1(ref->child(0));
  EditionReference newTrig1(newMult1->child(0));
  EditionReference newTrig2(newTrig1->nextTree());
  EditionReference newMult2(newMult1->nextTree());
  EditionReference newTrig3(newMult2->child(0));
  EditionReference newTrig4(newTrig3->nextTree());
  // Addition is expected to have been squashed if unary.
  assert(!newTrig1->child(0)->isAddition() ||
         newTrig1->child(0)->numberOfChildren() > 1);
  // Trig(A, 0) and Trig(A, 1) may be expanded again, do it recursively
  if (ExpandTrigonometric(newTrig1)) {
    if (!ExpandTrigonometric(newTrig3)) {
      // If newTrig1 expanded, newTrig3 should expand too
      assert(false);
    }
  } else {
    SimplifyTrig(newTrig1);
    SimplifyTrig(newTrig3);
  }
  /* Shallow reduce new trees. This step must be performed after sub-expansions
   * since SimplifyMultiplication may invalidate newTrig1 and newTrig3. */
  Simplification::SimplifyAddition(newTrig4->child(1));
  SimplifyTrig(newTrig2);
  SimplifyTrig(newTrig4);
  Simplification::SimplifyMultiplication(newMult1);
  Simplification::SimplifyMultiplication(newMult2);
  Simplification::SimplifyAddition(ref);
  return true;
}

bool Trigonometry::ContractTrigonometric(Tree* ref) {
  // A?+cos(B)^2+C?+sin(D)^2+E? = 1 + A + C + E
  if (PatternMatching::MatchReplaceAndSimplify(
          ref,
          KAdd(KTA, KPow(KTrig(KB, 0_e), 2_e), KTC, KPow(KTrig(KD, 1_e), 2_e),
               KTE),
          KAdd(1_e, KTA, KTC, KTE))) {
    return true;
  }
  /* A?*Trig(B, C)*Trig(D, E)*F?
   * = (Trig(B-D, TrigDiff(C,E))*F + Trig(B+D, E+C))*F)*A*0.5
   * F is duplicated in case it contains other Trig trees that could be
   * contracted as well. ContractTrigonometric is therefore more complex than
   * other contractions. It handles nested trees itself. */
  // MatchReplaceAndSimplify's cannot be used because of nested contraction.
  if (!PatternMatching::MatchAndReplace(
          ref, KMult(KTA, KTrig(KB, KC), KTrig(KD, KE), KTF),
          KMult(KAdd(KMult(KTrig(KAdd(KMult(-1_e, KD), KB), KTrigDiff(KC, KE)),
                           KTF),
                     KMult(KTrig(KAdd(KB, KD), KAdd(KE, KC)), KTF)),
                KTA, KHalf))) {
    return false;
  }
  // TODO : Find the replaced nodes and ShallowSystematicReduce smartly
  EditionReference newAdd(ref->child(0));
  EditionReference newMult1(newAdd->child(0));
  EditionReference newMult2(newMult1->nextTree());
  // If F is empty, Multiplications have been squashed
  bool fIsEmpty = !newMult1->isMultiplication();
  EditionReference newTrig1 =
      fIsEmpty ? newMult1 : EditionReference(newMult1->child(0));
  EditionReference newTrig2 =
      fIsEmpty ? newMult2 : EditionReference(newMult2->child(0));

  // Shallow reduce new trees
  EditionReference newTrig1Add = newTrig1->child(0);
  EditionReference newTrig1AddMult = newTrig1Add->child(0);
  Simplification::SimplifyMultiplication(newTrig1AddMult);
  Simplification::SimplifyAddition(newTrig1Add);
  SimplifyTrigDiff(newTrig1->child(1));
  SimplifyTrig(newTrig1);
  Simplification::SimplifyAddition(newTrig2->child(0));
  Simplification::SimplifyAddition(newTrig2->child(1));
  SimplifyTrig(newTrig2);

  if (!fIsEmpty) {
    Simplification::SimplifyMultiplication(newMult1);
    Simplification::SimplifyMultiplication(newMult2);
    // Contract newly created multiplications :
    // - Trig(B-D, TrigDiff(C,E))*F
    if (ContractTrigonometric(newMult1)) {
      // - Trig(B+D, E+C))*F
      if (!ContractTrigonometric(newMult2)) {
        // If newMult1 contracted, newMult2 should contract too
        assert(false);
      }
    }
  }
  Simplification::SimplifyAddition(newAdd);
  Simplification::SimplifyMultiplication(ref);
  return true;
}

bool Trigonometry::ExpandATrigonometric(Tree* ref) {
  // atrig(x,i) = π/2 - atrig(x,1-i)
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KATrig(KA, KB),
      KAdd(KMult(π_e, KHalf),
           KMult(-1_e, KATrig(KA, KAdd(1_e, KMult(-1_e, KB))))));
}

}  // namespace PoincareJ
