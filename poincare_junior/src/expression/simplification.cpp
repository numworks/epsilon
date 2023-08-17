#include "simplification.h"

#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/expression/decimal.h>
#include <poincare_junior/src/expression/dimension.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/metric.h>
#include <poincare_junior/src/expression/p_pusher.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/trigonometry.h>
#include <poincare_junior/src/expression/vector.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/placeholder.h>
#include <poincare_junior/src/n_ary.h>

#include "derivation.h"
#include "number.h"

namespace PoincareJ {

using namespace Placeholders;

bool IsInteger(const Tree* u) { return u->block()->isInteger(); }
bool IsNumber(const Tree* u) { return u->block()->isNumber(); }
bool IsRational(const Tree* u) { return u->block()->isRational(); }
bool IsConstant(const Tree* u) { return IsNumber(u); }
bool IsUndef(const Tree* u) { return u->type() == BlockType::Undefined; }

const Tree* RealPart(const Tree* tree) {
  return tree->type() == BlockType::Complex ? tree->nextNode() : tree;
}
const Tree* ImagPart(const Tree* tree) {
  return tree->type() == BlockType::Complex ? tree->childAtIndex(1) : 0_e;
}

bool Simplification::DeepSystematicReduce(Tree* u) {
  /* Although they are also flattened in ShallowSystematicReduce, flattening
   * here could save multiple ShallowSystematicReduce and flatten calls. */
  bool modified = (u->type() == BlockType::Multiplication ||
                   u->type() == BlockType::Addition) &&
                  NAry::Flatten(u);
  int numberOfChildren = u->numberOfChildren();
  Tree* child = u->nextNode();
  while (numberOfChildren > 0) {
    modified |= DeepSystematicReduce(child);
    if (IsUndef(child)) {
      u->cloneTreeOverTree(KUndef);
      return true;
    }
    child = child->nextTree();
    numberOfChildren--;
  }
  return ShallowSystematicReduce(u) || modified;
}

bool Simplification::ShallowSystematicReduce(Tree* u) {
  // This assert is quite costly, should be an assert level 2 ?
  assert(!Dimension::ComputeDimension(u).isInvalid());
  if (u->numberOfChildren() == 0) {
    // Strict rationals are the only childless trees that can be reduced.
    return Rational::MakeIrreducible(u);
  }

  switch (u->type()) {
    case BlockType::Power:
      return SimplifyPower(u);
    case BlockType::Addition:
      return SimplifyAddition(u);
    case BlockType::Multiplication:
      return SimplifyMultiplication(u);
    case BlockType::PowerReal:
      return SimplifyPowerReal(u);
    case BlockType::Abs:
      return SimplifyAbs(u);
    case BlockType::TrigDiff:
      return SimplifyTrigDiff(u);
    case BlockType::Trig:
      return SimplifyTrig(u);
    case BlockType::Derivative:
      return Derivation::ShallowSimplify(u);
    case BlockType::Ln:
      return SimplifyLn(u);
    case BlockType::Exponential:
      return SimplifyExp(u);
    case BlockType::Complex:
      return SimplifyComplex(u);
    default:
      return false;
  }
}

bool Simplification::SimplifyComplex(Tree* u) {
  Tree* real = u->nextNode();
  Tree* imag = real->nextTree();
  if (Number::IsZero(imag)) {
    // (A+0*i) -> A
    imag->removeTree();
    u->removeNode();
    return true;
  }
  if (real->type() != BlockType::Complex &&
      imag->type() != BlockType::Complex) {
    return false;
  }
  // (A+B*i + (C+D*i)*i) -> (A+B*i) + (0 + 1*i)*(C+D*i)
  imag->cloneTreeAtNode(KComplex(0_e, 1_e));
  imag->moveNodeAtNode(SharedEditionPool->push<BlockType::Multiplication>(2));
  SimplifyMultiplication(imag);
  u->moveNodeOverNode(SharedEditionPool->push<BlockType::Addition>(2));
  SimplifyAddition(u);
  return true;
}

bool Simplification::SimplifyExp(Tree* u) {
  Tree* child = u->nextNode();
  if (child->type() == BlockType::Ln) {
    // exp(ln(x)) -> x
    u->removeNode();
    u->removeNode();
    return true;
  }
  if (Number::IsZero(child)) {
    // exp(0) = 1
    u->cloneTreeOverTree(1_e);
    return true;
  }
  PatternMatching::Context ctx;
  if (PatternMatching::Match(
          KExp(KMult(KPlaceholder<A>(), KLn(KPlaceholder<B>()))), u, &ctx) &&
      IsInteger(ctx.getNode(A))) {
    // exp(n*ln(x)) -> x^n with n an integer
    u->moveTreeOverTree(PatternMatching::Create(
        KPow(KPlaceholder<B>(), KPlaceholder<A>()), ctx, true));
    return true;
  }
  return false;
}

bool Simplification::SimplifyLn(Tree* u) {
  Tree* child = u->nextNode();
  if (child->type() == BlockType::Exponential) {
    // ln(exp(x)) -> x
    u->removeNode();
    u->removeNode();
    return true;
  }
  if (!IsInteger(child)) {
    return false;
  }
  if (Number::IsMinusOne(child)) {
    // ln(-1) -> iπ - Necessary so that sqrt(-1)->i
    u->cloneTreeOverTree(KComplex(0_e, π_e));
    return true;
  } else if (Number::IsOne(child)) {
    u->cloneTreeOverTree(0_e);
    return true;
  } else if (Number::IsZero(child)) {
    u->cloneTreeOverTree(KUndef);
    return true;
  }
  return false;
}

bool Simplification::SimplifyAbs(Tree* u) {
  assert(u->type() == BlockType::Abs);
  Tree* child = u->nextNode();
  bool changed = false;
  if (child->type() == BlockType::Abs) {
    // ||x|| -> |x|
    child->removeNode();
    changed = true;
  }
  if (!IsNumber(child)) {
    return changed;
  }
  if (Number::Sign(child) == NonStrictSign::Positive) {
    // |3| -> 3
    u->removeNode();
  } else {
    // |-3| -> (-1)*(-3)
    u->cloneTreeOverNode(KMult(-1_e));
    NAry::SetNumberOfChildren(u, 2);
    SimplifyMultiplication(u);
  }
  return true;
}

bool Simplification::SimplifyTrigSecondElement(Tree* u, bool* isOpposed) {
  // Trig second element is always expected to be a reduced integer.
  assert(IsInteger(u) && !DeepSystematicReduce(u));
  IntegerHandler i = Integer::Handler(u);
  Tree* remainder = IntegerHandler::Remainder(i, IntegerHandler(4));
  if (Comparison::Compare(remainder, 2_e) >= 0) {
    *isOpposed = !*isOpposed;
    remainder->moveTreeOverTree(
        IntegerHandler::Remainder(i, IntegerHandler(2)));
  }
  bool changed = Comparison::Compare(remainder, u) != 0;
  u->moveTreeOverTree(remainder);
  // Simplified second element should have only two possible values.
  assert(Number::IsZero(u) || Number::IsOne(u));
  return changed;
}

bool Simplification::SimplifyTrigDiff(Tree* u) {
  assert(u->type() == BlockType::TrigDiff);
  /* TrigDiff is used to factorize Trigonometric contraction. It determines the
   * first term of these equations :
   * 2*sin(x)*sin(y) = cos(x-y) - cos(x+y)  -> TrigDiff(1,1) = 0
   * 2*sin(x)*cos(y) = sin(x-y) + sin(x+y)  -> TrigDiff(1,0) = 1
   * 2*cos(x)*sin(y) =-sin(x-y) + sin(x+y)  -> TrigDiff(0,1) = 3
   * 2*cos(x)*cos(y) = cos(x-y) + cos(x+y)  -> TrigDiff(0,0) = 0
   */
  // Simplify children as trigonometry second elements.
  bool isOpposed = false;
  Tree* x = u->childAtIndex(0);
  SimplifyTrigSecondElement(x, &isOpposed);
  Tree* y = x->nextTree();
  SimplifyTrigSecondElement(y, &isOpposed);
  // Find TrigDiff value depending on children types (sin or cos)
  bool isDifferent = x->type() != y->type();
  // Account for sign difference between TrigDiff(1,0) and TrigDiff(0,1)
  if (isDifferent && Number::IsZero(x)) {
    isOpposed = !isOpposed;
  }
  // Replace TrigDiff with result
  u->cloneTreeOverTree(isDifferent ? (isOpposed ? 3_e : 1_e)
                                   : (isOpposed ? 2_e : 0_e));
  return true;
}

bool Simplification::SimplifyTrig(Tree* u) {
  assert(u->type() == BlockType::Trig);
  // Trig(x,y) = {Cos(x) if y=0, Sin(x) if y=1, -Cos(x) if y=2, -Sin(x) if y=3}
  Tree* secondArgument = u->childAtIndex(1);
  bool isOpposed = false;
  bool changed = SimplifyTrigSecondElement(secondArgument, &isOpposed);
  assert(Number::IsZero(secondArgument) || Number::IsOne(secondArgument));
  bool isSin = Number::IsOne(secondArgument);
  // cos(-x) = cos(x) and sin(-x) = -sin(x)
  Tree* firstArgument = u->nextNode();
  if (PatternMatching::MatchAndReplace(
          firstArgument,
          KMult(KAnyTreesPlaceholder<A>(), -1_e, KAnyTreesPlaceholder<B>()),
          KMult(KAnyTreesPlaceholder<A>(), KAnyTreesPlaceholder<B>()))) {
    changed = true;
    if (isSin) {
      isOpposed = !isOpposed;
    }
    // Multiplication could have been squashed.
    if (firstArgument->type() == BlockType::Multiplication) {
      SimplifyMultiplication(firstArgument);
    }
  }

  // Replace Trig((n/12)*pi,*) with exact value.
  if (firstArgument->treeIsIdenticalTo(π_e) ||
      (firstArgument->type() == BlockType::Multiplication &&
       firstArgument->numberOfChildren() == 2 &&
       IsRational(firstArgument->nextNode()) &&
       firstArgument->childAtIndex(1)->treeIsIdenticalTo(π_e))) {
    const Tree* piFactor = firstArgument->type() == BlockType::Multiplication
                               ? firstArgument->nextNode()
                               : 1_e;
    // Compute n such that firstArgument = (n/12)*pi
    Tree* multipleTree = Rational::Multiplication(12_e, piFactor);
    Rational::MakeIrreducible(multipleTree);
    if (IsInteger(multipleTree)) {
      // Trig is 2pi periodic, n can be retrieved as a uint8_t.
      multipleTree->moveTreeOverTree(IntegerHandler::Remainder(
          Integer::Handler(multipleTree), IntegerHandler(24)));
      uint8_t n = Integer::Uint8(multipleTree);
      multipleTree->removeTree();
      u->cloneTreeOverTree(Trigonometry::ExactFormula(n, isSin, &isOpposed));
      DeepSystematicReduce(u);
      changed = true;
    } else {
      multipleTree->removeTree();
    }
  }

  if (isOpposed) {
    u->moveTreeAtNode(SharedEditionPool->push<BlockType::MinusOne>());
    u->moveNodeAtNode(SharedEditionPool->push<BlockType::Multiplication>(2));
    SimplifyMultiplication(u);
    changed = true;
  }
  return changed;
}

bool Simplification::SimplifyPower(Tree* u) {
  assert(u->type() == BlockType::Power);
  Tree* v = u->childAtIndex(0);
  EditionReference n = u->childAtIndex(1);
  // 0^n -> 0
  if (Number::IsZero(v)) {
    if (!Number::IsZero(n) && Rational::StrictSign(n) == StrictSign::Positive) {
      u->cloneTreeOverTree(0_e);
      return true;
    }
    u->cloneTreeOverTree(KUndef);
    return true;
  }
  // 1^n -> 1
  if (Number::IsOne(v)) {
    u->cloneTreeOverTree(1_e);
    return true;
  }
  if (IsRational(v)) {
    u->moveTreeOverTree(Rational::IntegerPower(v, n));
    Rational::MakeIrreducible(u);
    return true;
  }
  assert(IsInteger(n));
  // v^0 -> 1
  if (Number::IsZero(n)) {
    u->cloneTreeOverTree(1_e);
    return true;
  }
  // v^1 -> v
  if (Number::IsOne(n)) {
    u->moveTreeOverTree(v);
    return true;
  }
  if (v->type() == BlockType::Complex && Number::IsZero(v->nextNode())) {
    // (0 + A*i)^n -> ±(A^n) or (0±(A^n)*i)
    Tree* remainder =
        IntegerHandler::Remainder(Integer::Handler(n), IntegerHandler(4));
    int rem = Integer::Uint8(remainder);
    remainder->removeTree();
    v->nextNode()->removeTree();
    v->removeNode();
    // A^n
    SimplifyPower(u);
    // u could be any tree from this point forward
    if (rem > 1) {
      // -u
      u->moveTreeAtNode(SharedEditionPool->push<BlockType::MinusOne>());
      u->moveNodeAtNode(SharedEditionPool->push<BlockType::Multiplication>(2));
      SimplifyMultiplication(u);
    }
    if (rem % 2 == 1) {
      // u is a pure imaginary
      u->moveTreeAtNode(SharedEditionPool->push<BlockType::Zero>());
      u->moveNodeAtNode(SharedEditionPool->push<BlockType::Complex>());
      assert(!SimplifyComplex(u));
    }
    return true;
  }
  // (w^p)^n -> w^(p*n)
  if (v->type() == BlockType::Power) {
    EditionReference p = v->childAtIndex(1);
    assert(p->nextTree() == static_cast<Tree*>(n));
    // PowU PowV w p n
    v->removeNode();
    MoveNodeAtNode(p, SharedEditionPool->push<BlockType::Multiplication>(2));
    // PowU w Mult<2> p n
    SimplifyMultiplication(p);
    return SimplifyPower(u);
  }
  // (w1*...*wk)^n -> w1^n * ... * wk^n
  if (v->type() == BlockType::Multiplication) {
    for (auto [w, index] : NodeIterator::Children<Editable>(v)) {
      EditionReference m = SharedEditionPool->push<BlockType::Power>();
      w->clone();
      n->clone();
      w->moveTreeOverTree(m);
      SimplifyPower(m);
    }
    n->removeTree();
    u->removeNode();
    return SimplifyMultiplication(u);
  }
  // exp(a)^b -> exp(a*b)
  return PatternMatching::MatchReplaceAndSimplify(
      u, KPow(KExp(KPlaceholder<A>()), KPlaceholder<B>()),
      KExp(KMult(KPlaceholder<A>(), KPlaceholder<B>())));
}

bool BasesAreEqual(const Tree* u1, const Tree* u2) {
  const Tree* b1 = u1->type() == BlockType::Power ? u1->childAtIndex(0) : u1;
  const Tree* b2 = u2->type() == BlockType::Power ? u2->childAtIndex(0) : u2;
  return b1->treeIsIdenticalTo(b2);
}

Tree* PushBase(const Tree* u) {
  if (u->type() == BlockType::Power) {
    return u->childAtIndex(0)->clone();
  }
  return u->clone();
}

Tree* PushExponent(const Tree* u) {
  if (u->type() == BlockType::Power) {
    return u->childAtIndex(1)->clone();
  }
  return P_ONE();
}

void Simplification::ConvertPowerRealToPower(Tree* u) {
  // x^y -> exp(ln(x)*y)
  PatternMatching::MatchAndReplace(
      u, KPowReal(KPlaceholder<A>(), KPlaceholder<B>()),
      KExp(KMult(KLn(KPlaceholder<A>()), KPlaceholder<B>())));
  // Ln - Add if there is a systematic shallow simplification
  assert(!ShallowSystematicReduce(u->nextNode()->nextNode()));
  // Mult
  SimplifyMultiplication(u->nextNode());
  // Exp - Add if there is a systematic shallow simplification
  assert(!ShallowSystematicReduce(u));
}

bool Simplification::SimplifyPowerReal(Tree* u) {
  assert(u->type() == BlockType::PowerReal);
  /* Return :
   * - x^y if x is complex or positive
   * - PowerReal(x,y) y is not a rational
   * - Looking at y's reduced rational form p/q :
   *   * PowerReal(x,y) if x is of unknown sign and p odd
   *   * Unreal if q is even and x negative
   *   * |x|^y if p is even
   *   * -|x|^y if p is odd
   */
  Tree* x = u->childAtIndex(0);
  bool xIsNumber = IsNumber(x);
  bool xIsPositiveNumber =
      xIsNumber && Number::Sign(x) == NonStrictSign::Positive;
  bool xIsNegativeNumber = xIsNumber && !xIsPositiveNumber;
  if (xIsPositiveNumber) {
    // TODO : Same if x is complex
    ConvertPowerRealToPower(u);
    return true;
  }
  Tree* y = u->childAtIndex(1);
  if (!IsRational(y)) {
    // We don't know enough to simplify further.
    return false;
  }

  bool pIsEven = Rational::Numerator(y).isEven();
  bool qIsEven = Rational::Denominator(y).isEven();
  // y is simplified, both p and q can't be even
  assert(!qIsEven || !pIsEven);

  if (!pIsEven && !xIsNumber) {
    // We don't know enough to simplify further.
    return false;
  }
  assert(xIsNegativeNumber || pIsEven);

  if (xIsNegativeNumber && qIsEven) {
    // TODO: Implement and return NonReal
    u->cloneTreeOverTree(KUndef);
    return true;
  }

  // We can fallback to |x|^y
  x->cloneNodeAtNode(KAbs);
  SimplifyAbs(x);
  ConvertPowerRealToPower(u);

  if (xIsNegativeNumber && !pIsEven) {
    // -|x|^y
    u->cloneTreeAtNode(KMult(-1_e));
    NAry::SetNumberOfChildren(u, 2);
    SimplifyMultiplication(u);
  }
  return true;
}

bool Simplification::MergeMultiplicationChildWithNext(Tree* child) {
  Tree* next = child->nextTree();
  Tree* merge = nullptr;
  if (IsRational(child) && IsRational(next)) {
    // Merge constants
    merge = Rational::Multiplication(child, next);
    Rational::MakeIrreducible(merge);
  } else if (BasesAreEqual(child, next)) {
    // t^m * t^n -> t^(m+n)
    merge =
        P_POW(PushBase(child), P_ADD(PushExponent(child), PushExponent(next)));
    SimplifyAddition(merge->childAtIndex(1));
    SimplifyPower(merge);
    assert(merge->type() != BlockType::Multiplication);
  } else if (child->type() == BlockType::Complex ||
             next->type() == BlockType::Complex) {
    // (A+B*i)*(C+D*i) -> ((AC-BD)+(AD+BC)*i)
    merge = SharedEditionPool->push<BlockType::Complex>();
    Tree* addition = SharedEditionPool->push<BlockType::Addition>(2);
    Tree* multiplication =
        SharedEditionPool->push<BlockType::Multiplication>(2);
    RealPart(child)->clone();
    RealPart(next)->clone();
    SimplifyMultiplication(multiplication);
    multiplication = SharedEditionPool->push<BlockType::Multiplication>(3);
    SharedEditionPool->push<BlockType::MinusOne>();
    ImagPart(child)->clone();
    ImagPart(next)->clone();
    SimplifyMultiplication(multiplication);
    SimplifyAddition(addition);
    addition = SharedEditionPool->push<BlockType::Addition>(2);
    multiplication = SharedEditionPool->push<BlockType::Multiplication>(2);
    RealPart(child)->clone();
    ImagPart(next)->clone();
    SimplifyMultiplication(multiplication);
    multiplication = SharedEditionPool->push<BlockType::Multiplication>(2);
    ImagPart(child)->clone();
    RealPart(next)->clone();
    SimplifyMultiplication(multiplication);
    SimplifyAddition(addition);
    SimplifyComplex(merge);
  } else if (next->type() == BlockType::Matrix) {
    if (child->type() == BlockType::Matrix) {
      merge = Matrix::Multiplication(child, next);
    } else {
      merge = Matrix::ScalarMultiplication(child, next);
    }
  }
  if (!merge) {
    return false;
  }
  // Replace both child and next with merge
  next->moveTreeOverTree(merge);
  child->removeTree();
  return true;
}

bool Simplification::MergeMultiplicationChildrenFrom(Tree* child, int index,
                                                     int* numberOfSiblings,
                                                     bool* zero) {
  bool changed = false;
  while (index < *numberOfSiblings) {
    if (Number::IsZero(child)) {
      *zero = true;
      return false;
    }
    if (Number::IsOne(child)) {
      child->removeTree();
    } else if (!(index + 1 < *numberOfSiblings &&
                 MergeMultiplicationChildWithNext(child))) {
      // Child is neither 0, 1 and can't be merged with next child (or is last).
      return changed;
    }
    (*numberOfSiblings)--;
    changed = true;
  }
  return changed;
}

bool Simplification::SimplifyMultiplicationChildRec(Tree* child, int index,
                                                    int* numberOfSiblings,
                                                    bool* multiplicationChanged,
                                                    bool* zero) {
  assert(index < *numberOfSiblings);
  // Merge child with right siblings as much as possible.
  bool childChanged =
      MergeMultiplicationChildrenFrom(child, index, numberOfSiblings, zero);
  // Simplify starting from next child.
  if (!*zero && index + 1 < *numberOfSiblings &&
      SimplifyMultiplicationChildRec(child->nextTree(), index + 1,
                                     numberOfSiblings, multiplicationChanged,
                                     zero)) {
    // Next child changed, child may now merge with it.
    assert(!*zero);
    childChanged =
        MergeMultiplicationChildrenFrom(child, index, numberOfSiblings, zero) ||
        childChanged;
  }
  if (*zero) {
    return false;
  }
  *multiplicationChanged = *multiplicationChanged || childChanged;
  return childChanged;
}

bool Simplification::SimplifySortedMultiplication(Tree* multiplication) {
  int n = multiplication->numberOfChildren();
  bool changed = false;
  bool zero = false;
  /* Recursively merge children.
   * Keep track of n, changed status and presence of zero child. */
  SimplifyMultiplicationChildRec(multiplication->nextNode(), 0, &n, &changed,
                                 &zero);
  NAry::SetNumberOfChildren(multiplication, n);
  if (zero) {
    multiplication->cloneTreeOverTree(0_e);
    return true;
  }
  if (!changed || NAry::SquashIfUnary(multiplication) ||
      NAry::SquashIfEmpty(multiplication)) {
    return changed;
  }
  /* Merging children can un-sort the multiplication. It must then be simplified
   * again once sorted again. For example:
   * 3*a*i*i -> Simplify -> 3*a*-1 -> Sort -> -1*3*a -> Simplify -> -3*a */
  if (NAry::Sort(multiplication)) {
    SimplifySortedMultiplication(multiplication);
  }
  return true;
}

bool Simplification::SimplifyMultiplication(Tree* u) {
  assert(u->type() == BlockType::Multiplication);
  bool changed = NAry::Flatten(u);
  if (NAry::SquashIfUnary(u) || NAry::SquashIfEmpty(u)) {
    return true;
  }
  changed = NAry::Sort(u) || changed;
  changed = SimplifySortedMultiplication(u) || changed;
  assert(!changed || u->type() != BlockType::Multiplication ||
         !SimplifyMultiplication(u));
  return changed;
}

bool TermsAreEqual(const Tree* u, const Tree* v) {
  if (u->type() != BlockType::Multiplication) {
    if (v->type() != BlockType::Multiplication) {
      return u->treeIsIdenticalTo(v);
    }
    return TermsAreEqual(v, u);
  }
  if (v->type() != BlockType::Multiplication) {
    return u->numberOfChildren() == 2 && IsConstant(u->childAtIndex(0)) &&
           u->childAtIndex(1)->treeIsIdenticalTo(v);
  }
  bool hasConstU = IsConstant(u->childAtIndex(0));
  bool hasConstV = IsConstant(v->childAtIndex(0));
  int n = u->numberOfChildren() - hasConstU;
  if (n != v->numberOfChildren() - hasConstV) {
    return false;
  }
  const Tree* childU = u->childAtIndex(hasConstU);
  const Tree* childV = v->childAtIndex(hasConstV);
  for (int i = 0; i < n; i++) {
    if (!childU->treeIsIdenticalTo(childV)) {
      return false;
    }
    childU = childU->nextTree();
    childV = childV->nextTree();
  }
  return true;
}

// The term of 2ab is ab
Tree* PushTerm(const Tree* u) {
  Tree* c = u->clone();
  if (u->type() == BlockType::Multiplication &&
      IsConstant(u->childAtIndex(0))) {
    NAry::RemoveChildAtIndex(c, 0);
    NAry::SquashIfUnary(c);
  }
  return c;
}

// The constant of 2ab is 2
Tree* PushConstant(const Tree* u) {
  if (u->type() == BlockType::Multiplication &&
      IsConstant(u->childAtIndex(0))) {
    return u->childAtIndex(0)->clone();
  }
  return P_ONE();
}

bool Simplification::MergeAdditionChildWithNext(Tree* child, Tree* next) {
  assert(next == child->nextTree());
  Tree* merge = nullptr;
  if (IsRational(child) && IsRational(next)) {
    // Merge constants
    merge = Rational::Addition(child, next);
    Rational::MakeIrreducible(merge);
  } else if (TermsAreEqual(child, next)) {
    // k1 * a + k2 * a -> (k1+k2) * a
    merge =
        P_MULT(P_ADD(PushConstant(child), PushConstant(next)), PushTerm(child));
    SimplifyAddition(merge->childAtIndex(0));
    SimplifyMultiplication(merge);
    assert(merge->type() != BlockType::Addition);
  } else if (child->type() == BlockType::Complex ||
             next->type() == BlockType::Complex) {
    // (A+B*i)+(C+D*i) -> ((A+C)+(B+D)*i)
    merge = SharedEditionPool->push<BlockType::Complex>();
    Tree* addition = SharedEditionPool->push<BlockType::Addition>(2);
    RealPart(child)->clone();
    RealPart(next)->clone();
    SimplifyAddition(addition);
    addition = SharedEditionPool->push<BlockType::Addition>(2);
    ImagPart(child)->clone();
    ImagPart(next)->clone();
    SimplifyAddition(addition);
    SimplifyComplex(merge);
  } else if (next->type() == BlockType::Matrix) {
    if (child->type() == BlockType::Matrix) {
      merge = Matrix::Addition(child, next);
    } else {
      merge = KUndef->clone();
    }
  }
  if (!merge) {
    return false;
  }
  // Replace both child and next with merge
  next->moveTreeOverTree(merge);
  child->removeTree();
  return true;
}

bool Simplification::SimplifyAddition(Tree* u) {
  assert(u->type() == BlockType::Addition);
  bool modified = NAry::Flatten(u);
  if (NAry::SquashIfUnary(u)) {
    return true;
  }
  modified = NAry::Sort(u) || modified;
  int n = u->numberOfChildren();
  int i = 0;
  Tree* child = u->nextNode();
  while (i < n) {
    if (Number::IsZero(child)) {
      child->removeTree();
      n--;
      continue;
    }
    Tree* next = child->nextTree();
    if (i + 1 < n && MergeAdditionChildWithNext(child, next)) {
      assert(child->type() != BlockType::Addition);
      n--;
    } else {
      child = next;
      i++;
    }
  }
  if (n == u->numberOfChildren()) {
    return modified;
  }
  NAry::SetNumberOfChildren(u, n);
  if (NAry::SquashIfUnary(u) || NAry::SquashIfEmpty(u)) {
    return true;
  }
  /* TODO: SimplifyAddition may encounter the same issues as the multiplication.
   * If this assert can't be preserved, SimplifyAddition must handle one or both
   * of this cases as handled in multiplication:
   * With a,b and c the sorted addition children (a < b < c), M(a,b) the result
   * of merging children a and b (with MergeAdditionChildWithNext) if it exists.
   * - M(a,b) > c or a > M(b,c) (Addition must be sorted again)
   * - M(a,b) doesn't exists, but M(a,M(b,c)) does (previous child should try
   *   merging again when child merged with nextCHild) */
  assert(!SimplifyAddition(u));
  return true;
}

bool Simplification::Simplify(Tree* ref, ProjectionContext projectionContext) {
  if (Dimension::ComputeDimension(ref).isInvalid()) {
    ref->cloneTreeOverTree(KUndef);
    return true;
  }
  bool changed = false;
  /* TODO: If simplification fails, come back to this step with a simpler
   * projection context. */
  changed = DeepSystemProjection(ref, projectionContext) || changed;
  changed = DeepSystematicReduce(ref) || changed;
  changed = DeepApplyMatrixOperators(ref) || changed;
  // TODO: Bubble up Matrices, complexes, units, lists and dependencies.
  changed = AdvancedReduction(ref, ref) || changed;
  assert(!DeepSystematicReduce(ref));
  changed = DeepBeautify(ref) || changed;
  return changed;
}

bool Simplification::AdvancedReduction(Tree* ref, const Tree* root) {
  assert(!DeepSystematicReduce(ref));
  bool changed = false;
  for (std::pair<EditionReference, int> indexedNode :
       NodeIterator::Children<Editable>(ref)) {
    changed =
        AdvancedReduction(std::get<EditionReference>(indexedNode), root) ||
        changed;
  }
  if (changed) {
    ShallowSystematicReduce(ref);
  }
  return ShallowAdvancedReduction(ref, root, changed) || changed;
}

bool Simplification::ShallowAdvancedReduction(Tree* ref, const Tree* root,
                                              bool changed) {
  assert(!DeepSystematicReduce(ref));
  return (ref->block()->isAlgebraic()
              ? AdvanceReduceOnAlgebraic(ref, root, changed)
              : AdvanceReduceOnTranscendental(ref, root, changed));
}

// Reverse most system projections to display better expressions
bool Simplification::ShallowBeautify(Tree* ref, void* context) {
  ProjectionContext* projectionContext =
      static_cast<ProjectionContext*>(context);
  if (ref->type() == BlockType::Trig) {
    const Tree* k_angles[3] = {
        KPlaceholder<A>(), KMult(KPlaceholder<A>(), 180_e, KPow(π_e, -1_e)),
        KMult(KPlaceholder<A>(), 200_e, KPow(π_e, -1_e))};
    Tree* child = ref->childAtIndex(0);
    PatternMatching::MatchAndReplace(
        child, KPlaceholder<A>(),
        k_angles[static_cast<uint8_t>(projectionContext->m_angleUnit)]);
    DeepSystematicReduce(child);
  }

  // RealPow(A,B) -> A^B
  // exp(A? * ln(B) * C?) -> B^(A*C)
  if (PatternMatching::MatchAndReplace(
          ref, KPowReal(KPlaceholder<A>(), KPlaceholder<B>()),
          KPow(KPlaceholder<A>(), KPlaceholder<B>())) ||
      PatternMatching::MatchAndReplace(
          ref,
          KExp(KMult(KAnyTreesPlaceholder<A>(), KLn(KPlaceholder<B>()),
                     KAnyTreesPlaceholder<C>())),
          KPow(KPlaceholder<B>(),
               KMult(KAnyTreesPlaceholder<A>(), KAnyTreesPlaceholder<C>())))) {
    // A^0.5 -> Sqrt(A)
    PatternMatching::MatchAndReplace(ref, KPow(KPlaceholder<A>(), KHalf),
                                     KSqrt(KPlaceholder<A>()));
    return true;
  }
  bool changed = false;
  // A + B? + (-1)*C + D?-> ((A + B) - C) + D
  // Applied as much as necessary while preserving the order.
  while (PatternMatching::MatchAndReplace(
      ref,
      KAdd(KPlaceholder<A>(), KAnyTreesPlaceholder<B>(),
           KMult(-1_e, KAnyTreesPlaceholder<C>()), KAnyTreesPlaceholder<D>()),
      KAdd(KSub(KAdd(KPlaceholder<A>(), KAnyTreesPlaceholder<B>()),
                KMult(KAnyTreesPlaceholder<C>())),
           KAnyTreesPlaceholder<D>()))) {
    changed = true;
  }
  return changed ||
         // Complex(0,1) -> i
         PatternMatching::MatchAndReplace(ref, KComplex(0_e, 1_e), i_e) ||
         // Complex(0,A) -> A*i
         PatternMatching::MatchAndReplace(ref, KComplex(0_e, KPlaceholder<A>()),
                                          KMult(KPlaceholder<A>(), i_e)) ||
         // Complex(A,B) -> A+B*i
         PatternMatching::MatchAndReplace(
             ref, KComplex(KPlaceholder<A>(), KPlaceholder<B>()),
             KAdd(KPlaceholder<A>(), KMult(KPlaceholder<B>(), i_e))) ||
         // trig(A, 0) -> cos(A)
         PatternMatching::MatchAndReplace(ref, KTrig(KPlaceholder<A>(), 0_e),
                                          KCos(KPlaceholder<A>())) ||
         // trig(A, 1) -> sin(A)
         PatternMatching::MatchAndReplace(ref, KTrig(KPlaceholder<A>(), 1_e),
                                          KSin(KPlaceholder<A>())) ||
         // exp(A) -> e^A
         PatternMatching::MatchAndReplace(ref, KExp(KPlaceholder<A>()),
                                          KPow(e_e, KPlaceholder<A>())) ||
         // ln(A) * ln(B)^(-1) -> log(A, B)
         PatternMatching::MatchAndReplace(
             ref,
             KMult(KLn(KPlaceholder<A>()), KPow(KLn(KPlaceholder<B>()), -1_e)),
             KLogarithm(KPlaceholder<A>(), KPlaceholder<B>()));
}

bool Simplification::DeepSystemProjection(Tree* ref,
                                          ProjectionContext projectionContext) {
  bool changed =
      (projectionContext.m_strategy == Strategy::ApproximateToFloat) &&
      Approximation::ApproximateAndReplaceEveryScalar(ref);
  return ApplyShallowInDepth(ref, ShallowSystemProjection,
                             static_cast<void*>(&projectionContext)) ||
         changed;
}

/* The order of nodes in NAry is not a concern here. They will be sorted before
 * SystemReduction. */
bool Simplification::ShallowSystemProjection(Tree* ref, void* context) {
  /* TODO: Most of the projections could be optimized by simply replacing and
   * inserting nodes. This optimization could be applied in matchAndReplace. See
   * comment in matchAndReplace. */
  ProjectionContext* projectionContext =
      static_cast<ProjectionContext*>(context);
  if (projectionContext->m_strategy == Strategy::NumbersToFloat &&
      ref->block()->isInteger()) {
    return Approximation::ApproximateAndReplaceEveryScalar(ref);
  }

  if (ref->type() == BlockType::Decimal) {
    Decimal::Project(ref);
    return true;
  }

  if (ref->block()->isOfType(
          {BlockType::Sine, BlockType::Cosine, BlockType::Tangent})) {
    const Tree* k_angles[3] = {
        KPlaceholder<A>(), KMult(KPlaceholder<A>(), π_e, KPow(180_e, -1_e)),
        KMult(KPlaceholder<A>(), π_e, KPow(200_e, -1_e))};
    PatternMatching::MatchAndReplace(
        ref->childAtIndex(0), KPlaceholder<A>(),
        k_angles[static_cast<uint8_t>(projectionContext->m_angleUnit)]);
  }
  // These types should only be available after projection
  assert(!ref->block()->isOfType({BlockType::Exponential, BlockType::Trig,
                                  BlockType::TrigDiff, BlockType::Polynomial,
                                  BlockType::PowerReal}));
  // Sqrt(A) -> A^0.5
  PatternMatching::MatchAndReplace(ref, KSqrt(KPlaceholder<A>()),
                                   KPow(KPlaceholder<A>(), KHalf));
  if (ref->type() == BlockType::Power) {
    const Tree* index = ref->nextNode()->nextTree();
    if (!index->block()->isInteger()) {
      // e^A -> exp(A)
      if (!PatternMatching::MatchAndReplace(ref, KPow(e_e, KPlaceholder<A>()),
                                            KExp(KPlaceholder<A>()))) {
        if (projectionContext->m_complexFormat != ComplexFormat::Real) {
          // A^B -> exp(ln(A)*B)
          PatternMatching::MatchAndReplace(
              ref, KPow(KPlaceholder<A>(), KPlaceholder<B>()),
              KExp(KMult(KLn(KPlaceholder<A>()), KPlaceholder<B>())));
        } else {
          // A^B -> RealPow(A,B)
          PatternMatching::MatchAndReplace(
              ref, KPow(KPlaceholder<A>(), KPlaceholder<B>()),
              KPowReal(KPlaceholder<A>(), KPlaceholder<B>()));
        }
      }
      return true;
    }
  }

  /* All replaced structure do not not need further shallow projection.
   * Operator || only is used. */
  return
      // i -> Complex(0,1)
      PatternMatching::MatchAndReplace(ref, i_e, KComplex(0_e, 1_e)) ||
      // A - B -> A + (-1)*B
      PatternMatching::MatchAndReplace(
          ref, KSub(KPlaceholder<A>(), KPlaceholder<B>()),
          KAdd(KPlaceholder<A>(), KMult(-1_e, KPlaceholder<B>()))) ||
      // A / B -> A * B^-1
      PatternMatching::MatchAndReplace(
          ref, KDiv(KPlaceholder<A>(), KPlaceholder<B>()),
          KMult(KPlaceholder<A>(), KPow(KPlaceholder<B>(), -1_e))) ||
      // cos(A) -> trig(A, 0)
      PatternMatching::MatchAndReplace(ref, KCos(KPlaceholder<A>()),
                                       KTrig(KPlaceholder<A>(), 0_e)) ||
      // sin(A) -> trig(A, 1)
      PatternMatching::MatchAndReplace(ref, KSin(KPlaceholder<A>()),
                                       KTrig(KPlaceholder<A>(), 1_e)) ||
      // tan(A) -> sin(A) * cos(A)^(-1)
      /* TODO: Tangent will duplicate its yet to be projected children,
       * replacing it after everything else may be an optimization.
       * Sin and cos terms will be replaced afterwards. */
      PatternMatching::MatchAndReplace(
          ref, KTan(KPlaceholder<A>()),
          KMult(KSin(KPlaceholder<A>()),
                KPow(KCos(KPlaceholder<A>()), -1_e))) ||
      // log(A, e) -> ln(e)
      PatternMatching::MatchAndReplace(ref, KLogarithm(KPlaceholder<A>(), e_e),
                                       KLn(KPlaceholder<A>())) ||
      // log(A) -> ln(A) * ln(10)^(-1)
      // TODO: Maybe log(A) -> log(A, 10) and rely on next matchAndReplace
      PatternMatching::MatchAndReplace(
          ref, KLog(KPlaceholder<A>()),
          KMult(KLn(KPlaceholder<A>()), KPow(KLn(10_e), -1_e))) ||
      // log(A, B) -> ln(A) * ln(B)^(-1)
      PatternMatching::MatchAndReplace(
          ref, KLogarithm(KPlaceholder<A>(), KPlaceholder<B>()),
          KMult(KLn(KPlaceholder<A>()), KPow(KLn(KPlaceholder<B>()), -1_e)));
}

bool Simplification::ApplyShallowInDepth(Tree* ref,
                                         ShallowOperation shallowOperation,
                                         void* context) {
  bool changed = shallowOperation(ref, context);
  int treesToProject = ref->numberOfChildren();
  Tree* node = ref->nextNode();
  while (treesToProject > 0) {
    treesToProject--;
    changed = shallowOperation(node, context) || changed;
    treesToProject += node->numberOfChildren();
    node = node->nextNode();
  }
  return changed;
}

bool Simplification::AdvanceReduceOnTranscendental(Tree* ref, const Tree* root,
                                                   bool changed) {
  if (ReduceInverseFunction(ref) || changed) {
    return true;
  }
  const Metric metric(ref, root);
  EditionReference clone(ref->clone());
  if (ShallowExpand(ref)) {
    if (metric.hasImproved()) {
      /* AdvanceReduce further the expression only if it is algebraic.
       * Transcendental tree can expand but stay transcendental:
       * |(-1)*x| -> |(-1)|*|x| -> |x| */
      bool reducedAlgebraic = ref->block()->isAlgebraic() &&
                              AdvanceReduceOnAlgebraic(ref, root, true);
      // If algebraic got advanced reduced, metric must have been improved.
      assert(!reducedAlgebraic || metric.hasImproved());
      clone->removeTree();
      return true;
    }
    // Restore ref
    ref->moveTreeOverTree(clone);
  } else {
    clone->removeTree();
  }
  return false;
}

bool Simplification::AdvanceReduceOnAlgebraic(Tree* ref, const Tree* root,
                                              bool changed) {
  assert(!DeepSystematicReduce(ref));
  const Metric metric(ref, root);
  EditionReference clone(ref->clone());
  if (ShallowContract(ref)) {
    if (metric.hasImproved()) {
      clone->removeTree();
      return true;
    }
    // Restore ref
    ref->cloneTreeOverTree(clone);
  }
  if (PolynomialInterpretation(ref)) {
    if (metric.hasImproved()) {
      clone->removeTree();
      return true;
    }
    // Restore ref
    ref->moveTreeOverTree(clone);
  } else {
    clone->removeTree();
  }
  return false;
}

bool Simplification::ReduceInverseFunction(Tree* e) {
  /* TODO :Transformations such as exp(ln) were expected to be performed here
   * but have been moved to systematicReduce because ln(2x) (in exp(ln(2x))
   * advance reduce to ln(2)+ln(x) before ReduceInverseFunction is called on the
   * Exponential tree. Possible solutions :
   * - Keep inverse function reduction in systematicReduce
   * - ln(2x) doesn't advance reduce to ln(2)+ln(x)
   * - Call ReduceInverseFunction before (and after ?) advanceReduce on children
   */
  return false;
}

bool Simplification::ExpandTranscendentalOnRational(Tree* e) {
  // ln(18/5) = 3ln(3)+ln(2)-ln(5)
  // TODO : Implement
  return false;
}

bool Simplification::PolynomialInterpretation(Tree* e) {
  assert(!DeepSystematicReduce(e));
  // Prepare the expression for Polynomial interpretation:
  bool changed = ExpandTranscendentalOnRational(e);
  changed = ShallowAlgebraicExpand(e) || changed;
  // TODO : Implement PolynomialInterpretation
  return changed;
}

bool Simplification::DistributeOverNAry(Tree* ref, BlockType target,
                                        BlockType naryTarget,
                                        BlockType naryOutput, int childIndex) {
  assert(naryTarget == BlockType::Addition ||
         naryTarget == BlockType::Multiplication);
  assert(naryOutput == BlockType::Addition ||
         naryOutput == BlockType::Multiplication);
  if (ref->type() != target) {
    return false;
  }
  int numberOfChildren = ref->numberOfChildren();
  assert(childIndex < numberOfChildren);
  EditionReference children = ref->childAtIndex(childIndex);
  if (children->type() != naryTarget) {
    return false;
  }
  int numberOfGrandChildren = children->numberOfChildren();
  size_t childIndexOffset = children->block() - ref->block();
  // f(+(A,B,C),E)
  children->cloneTreeBeforeNode(0_e);
  children = children->detachTree();
  // f(0,E) ... +(A,B,C)
  Tree* grandChild = children->nextNode();
  EditionReference output =
      naryOutput == BlockType::Addition
          ? SharedEditionPool->push<BlockType::Addition>(numberOfGrandChildren)
          : SharedEditionPool->push<BlockType::Multiplication>(
                numberOfGrandChildren);
  // f(0,E) ... +(A,B,C) ... *(,,)
  for (int i = 0; i < numberOfGrandChildren; i++) {
    EditionReference clone = ref->clone();
    // f(0,E) ... +(A,B,C) ... *(f(0,E),,)
    /* Since it is constant, use a childIndexOffset to avoid childAtIndex calls:
     * clone.childAtIndex(childIndex)=Tree(clone.block()+childIndexOffset) */
    EditionReference(clone->block() + childIndexOffset)
        ->moveTreeOverTree(grandChild);
    // f(0,E) ... +(,B,C) ... *(f(A,E),,)
    /* TODO: clone is always of the same type, the exact simplify method could
     * be passed as an argument. */
    ShallowSystematicReduce(clone);
  }
  // f(0,E) ... +(,,) ... *(f(A,E), f(B,E), f(C,E))
  children->removeNode();
  // f(0,E) ... *(f(A,E), f(B,E), f(C,E))
  ref = ref->moveTreeOverTree(output);
  // *(f(A,E), f(B,E), f(C,E))
  // TODO: SimplifyAddition or SimplifyMultiplication
  ShallowSystematicReduce(ref);
  return true;
}

bool Simplification::TryAllOperations(Tree* e, const Operation* operations,
                                      int numberOfOperations) {
  /* For example :
   * Most contraction operations are very shallow.
   * exp(A)*exp(B)*exp(C)*|D|*|E| = exp(A+B)*exp(C)*|D|*|E|
   *                              = exp(A+B)*exp(C)*|D*E|
   *                              = exp(A+B+C)*|D*E|
   * Most expansion operations have to handle themselves smartly.
   * exp(A+B+C) = exp(A)*exp(B)*exp(C) */
  int failures = 0;
  int i = 0;
  assert(!DeepSystematicReduce(e));
  while (failures < numberOfOperations) {
    failures = operations[i % numberOfOperations](e) ? 0 : failures + 1;
    // EveryOperation should preserve e's reduced status
    assert(!DeepSystematicReduce(e));
    i++;
  }
  return i > numberOfOperations;
}

bool Simplification::ContractAbs(Tree* ref) {
  // A*|B|*|C|*D = A*|BC|*D
  return PatternMatching::MatchReplaceAndSimplify(
      ref,
      KMult(KAnyTreesPlaceholder<A>(), KAbs(KPlaceholder<B>()),
            KAbs(KPlaceholder<C>()), KAnyTreesPlaceholder<D>()),
      KMult(KAnyTreesPlaceholder<A>(),
            KAbs(KMult(KPlaceholder<B>(), KPlaceholder<C>())),
            KAnyTreesPlaceholder<D>()));
}

bool Simplification::ExpandAbs(Tree* ref) {
  // |A*B*...| = |A|*|B|*...
  return DistributeOverNAry(ref, BlockType::Abs, BlockType::Multiplication,
                            BlockType::Multiplication);
}

bool Simplification::ContractLn(Tree* ref) {
  // A? + Ln(B) + Ln(C) + D? = A + ln(BC) + D
  return PatternMatching::MatchReplaceAndSimplify(
      ref,
      KAdd(KAnyTreesPlaceholder<A>(), KLn(KPlaceholder<B>()),
           KLn(KPlaceholder<C>()), KAnyTreesPlaceholder<D>()),
      KAdd(KAnyTreesPlaceholder<A>(),
           KLn(KMult(KPlaceholder<B>(), KPlaceholder<C>())),
           KAnyTreesPlaceholder<D>()));
}

bool Simplification::ExpandLn(Tree* ref) {
  // ln(A*B*...) = ln(A) + ln(B) + ...
  return DistributeOverNAry(ref, BlockType::Ln, BlockType::Multiplication,
                            BlockType::Addition);
}

bool Simplification::ExpandExp(Tree* ref) {
  return
      // exp(A+iB) = exp(A)*(cos(B) + i*sin(B))
      PatternMatching::MatchReplaceAndSimplify(
          ref, KExp(KComplex(KPlaceholder<A>(), KPlaceholder<B>())),
          KMult(KExp(KPlaceholder<A>()),
                KComplex(KTrig(KPlaceholder<B>(), 0_e),
                         KTrig(KPlaceholder<B>(), 1_e)))) ||
      // exp(A+B+...) = exp(A) * exp(B) * ...
      DistributeOverNAry(ref, BlockType::Exponential, BlockType::Addition,
                         BlockType::Multiplication);
}

bool Simplification::ContractExpMult(Tree* ref) {
  // A? * exp(B) * exp(C) * D? = A * exp(B+C) * D
  return PatternMatching::MatchReplaceAndSimplify(
      ref,
      KMult(KAnyTreesPlaceholder<A>(), KExp(KPlaceholder<B>()),
            KExp(KPlaceholder<C>()), KAnyTreesPlaceholder<D>()),
      KMult(KAnyTreesPlaceholder<A>(),
            KExp(KAdd(KPlaceholder<B>(), KPlaceholder<C>())),
            KAnyTreesPlaceholder<D>()));
}

/* TODO : Find an easier solution for nested expand/contract smart shallow
 * simplification. */

bool Simplification::ExpandTrigonometric(Tree* ref) {
  /* Trig(A?+B, C) = Trig(A, 0)*Trig(B, C) + Trig(A, 1)*Trig(B, C-1)
   * ExpandTrigonometric is more complex than other expansions and cannot be
   * factorized with DistributeOverNAry. */
  // MatchReplaceAndSimplify's cannot be used because of nested expansion.
  if (!PatternMatching::MatchAndReplace(
          ref,
          KTrig(KAdd(KAnyTreesPlaceholder<A>(), KPlaceholder<B>()),
                KPlaceholder<C>()),
          KAdd(KMult(KTrig(KAdd(KAnyTreesPlaceholder<A>()), 0_e),
                     KTrig(KPlaceholder<B>(), KPlaceholder<C>())),
               KMult(
                   KTrig(KAdd(KAnyTreesPlaceholder<A>()), 1_e),
                   KTrig(KPlaceholder<B>(), KAdd(KPlaceholder<C>(), -1_e)))))) {
    return false;
  }
  EditionReference newMult1(ref->nextNode());
  EditionReference newTrig1(newMult1->nextNode());
  EditionReference newTrig2(newTrig1->nextTree());
  EditionReference newMult2(newMult1->nextTree());
  EditionReference newTrig3(newMult2->nextNode());
  EditionReference newTrig4(newTrig3->nextTree());
  // Addition is expected to have been squashed if unary.
  assert(newTrig1->nextNode()->type() != BlockType::Addition ||
         newTrig1->nextNode()->numberOfChildren() > 1);
  // Trig(A, 0) and Trig(A, 1) may be expanded again, do it recursively
  if (ExpandTrigonometric(newTrig1)) {
    if (!ExpandTrigonometric(newTrig3)) {
      assert(false);
    }
  } else {
    SimplifyTrig(newTrig1);
    SimplifyTrig(newTrig3);
  }
  /* Shallow reduce new trees. This step must be performed after sub-expansions
   * since SimplifyProduct may invalidate newTrig1 and newTrig3. */
  SimplifyAddition(newTrig4->childAtIndex(1));
  SimplifyTrig(newTrig2);
  SimplifyTrig(newTrig4);
  SimplifyMultiplication(newMult1);
  SimplifyMultiplication(newMult2);
  SimplifyAddition(ref);
  return true;
}

bool Simplification::ContractTrigonometric(Tree* ref) {
  // A?+cos(B)^2+C?+sin(D)^2+E? = 1 + A + C + E
  if (PatternMatching::MatchReplaceAndSimplify(
          ref,
          KAdd(KAnyTreesPlaceholder<A>(),
               KPow(KTrig(KPlaceholder<B>(), 0_e), 2_e),
               KAnyTreesPlaceholder<C>(),
               KPow(KTrig(KPlaceholder<D>(), 1_e), 2_e),
               KAnyTreesPlaceholder<E>()),
          KAdd(1_e, KAnyTreesPlaceholder<A>(), KAnyTreesPlaceholder<C>(),
               KAnyTreesPlaceholder<E>()))) {
    return true;
  }
  /* A?*Trig(B, C)*Trig(D, E)*F?
   * = (Trig(B-D, TrigDiff(C,E))*F + Trig(B+D, E+C))*F)*A*0.5
   * F is duplicated in case it contains other Trig trees that could be
   * contracted as well. ContractTrigonometric is therefore more complex than
   * other contractions. It handles nested trees itself. */
  // MatchReplaceAndSimplify's cannot be used because of nested contraction.
  if (!PatternMatching::MatchAndReplace(
          ref,
          KMult(KAnyTreesPlaceholder<A>(),
                KTrig(KPlaceholder<B>(), KPlaceholder<C>()),
                KTrig(KPlaceholder<D>(), KPlaceholder<E>()),
                KAnyTreesPlaceholder<F>()),
          KMult(
              KAdd(KMult(KTrig(KAdd(KMult(-1_e, KPlaceholder<D>()),
                                    KPlaceholder<B>()),
                               KTrigDiff(KPlaceholder<C>(), KPlaceholder<E>())),
                         KAnyTreesPlaceholder<F>()),
                   KMult(KTrig(KAdd(KPlaceholder<B>(), KPlaceholder<D>()),
                               KAdd(KPlaceholder<E>(), KPlaceholder<C>())),
                         KAnyTreesPlaceholder<F>())),
              KAnyTreesPlaceholder<A>(), KHalf))) {
    return false;
  }
  // TODO : Find the replaced nodes and ShallowSystematicReduce smartly
  EditionReference newAdd(ref->nextNode());
  EditionReference newMult1(newAdd->nextNode());
  EditionReference newMult2(newMult1->nextTree());
  // If F is empty, Multiplications have been squashed
  bool fIsEmpty = newMult1->type() != BlockType::Multiplication;
  EditionReference newTrig1 =
      fIsEmpty ? newMult1 : EditionReference(newMult1->nextNode());
  EditionReference newTrig2 =
      fIsEmpty ? newMult2 : EditionReference(newMult2->nextNode());

  // Shallow reduce new trees
  EditionReference newTrig1Add = newTrig1->nextNode();
  EditionReference newTrig1AddMult = newTrig1Add->nextNode();
  SimplifyMultiplication(newTrig1AddMult);
  SimplifyAddition(newTrig1Add);
  SimplifyTrigDiff(newTrig1->childAtIndex(1));
  SimplifyTrig(newTrig1);
  SimplifyAddition(newTrig2->childAtIndex(0));
  SimplifyAddition(newTrig2->childAtIndex(1));
  SimplifyTrig(newTrig2);

  if (!fIsEmpty) {
    SimplifyMultiplication(newMult1);
    SimplifyMultiplication(newMult2);
    // Contract newly created multiplications :
    // - Trig(B-D, TrigDiff(C,E))*F
    if (ContractTrigonometric(newMult1)) {
      // - Trig(B+D, E+C))*F
      if (!ContractTrigonometric(newMult2)) {
        assert(false);
      }
    }
  }
  SimplifyAddition(newAdd);
  SimplifyMultiplication(ref);
  return true;
}

bool Simplification::ExpandMult(Tree* ref) {
  // A?*(B?+C)*D? = A*B*D + A*C*D
  if (ref->type() != BlockType::Multiplication) {
    return false;
  }
  Tree* child = ref->nextNode();
  int numberOfChildren = ref->numberOfChildren();
  // Find the NAry in children
  int childIndex = 0;
  while (childIndex < numberOfChildren &&
         child->type() != BlockType::Addition) {
    childIndex++;
    child = child->nextTree();
  }
  if (childIndex >= numberOfChildren) {
    return false;
  }
  if (!DistributeOverNAry(ref, BlockType::Multiplication, BlockType::Addition,
                          BlockType::Addition, childIndex)) {
    return false;
  }
  // Recursively expand multiplication children.
  assert(ref->type() == BlockType::Addition);
  bool changedAgain = false;
  child = ref->nextNode();
  for (size_t i = 0; i < ref->numberOfChildren(); i++) {
    if (ExpandMult(child)) {
      changedAgain = true;
    }
    child = child->nextTree();
  }
  if (changedAgain) {
    SimplifyAddition(ref);
  }
  return true;
}

bool Simplification::ExpandPowerComplex(Tree* ref) {
  // (A + B*i)^2 = (A^2 -2*B^2 + 2*A*B*i)
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KPow(KComplex(KPlaceholder<A>(), KPlaceholder<B>()), 2_e),
      KComplex(KAdd(KPow(KPlaceholder<A>(), 2_e),
                    KMult(-1_e, KPow(KPlaceholder<B>(), 2_e))),
               KMult(2_e, KPlaceholder<A>(), KPlaceholder<B>())));
}

bool Simplification::ExpandPower(Tree* ref) {
  // (A?*B)^C = A^C * B^C is currently in SystematicSimplification
  // (A? + B)^2 = (A^2 + 2*A*B + B^2)
  // TODO: Implement a more general (A + B)^C expand.
  /* This isn't factorized with DistributeOverNAry because of the necessary
   * second term expansion. */
  // MatchReplaceAndSimplify's cannot be used because of nested expansion.
  if (!PatternMatching::MatchAndReplace(
          ref, KPow(KAdd(KAnyTreesPlaceholder<A>(), KPlaceholder<B>()), 2_e),
          KAdd(KPow(KAdd(KAnyTreesPlaceholder<A>()), 2_e),
               KMult(2_e, KAdd(KAnyTreesPlaceholder<A>()), KPlaceholder<B>()),
               KPow(KPlaceholder<B>(), 2_e)))) {
    return false;
  }
  // TODO : Find the replaced nodes and ShallowSystematicReduce smartly
  // A^2 and 2*A*B may be expanded again, do it recursively
  EditionReference newPow1(ref->nextNode());
  EditionReference newMult(newPow1->nextTree());
  EditionReference newPow2(newMult->nextTree());
  // Addition is expected to have been squashed if unary.
  assert(newPow1->nextNode()->type() != BlockType::Addition ||
         newPow1->nextNode()->numberOfChildren() > 1);
  if (ExpandPower(newPow1)) {
    ExpandMult(newPow1->nextTree());
  } else {
    SimplifyPower(newPow1);
    SimplifyMultiplication(newMult);
  }
  SimplifyPower(newPow2);
  SimplifyAddition(ref);
  return true;
}

bool Simplification::ShallowApplyMatrixOperators(Tree* tree, void* context) {
  if (tree->numberOfChildren() < 1) {
    return false;
  }
  Tree* child = tree->childAtIndex(0);
  if (tree->type() == BlockType::Identity) {
    if (IsNumber(child)) {
      tree->moveTreeOverTree(Matrix::Identity(child));
    } else {
      tree->cloneTreeOverTree(KUndef);
    }
    return true;
  }
  // assert(child->type() == BlockType::Matrix);
  switch (tree->type()) {
    case BlockType::Inverse:
      tree->moveTreeOverTree(Matrix::Inverse(child));
      return true;
    case BlockType::Ref:
      Matrix::RowCanonize(child, false);
      tree->removeNode();
      return true;
    case BlockType::Rref:
      Matrix::RowCanonize(child, true);
      tree->removeNode();
      return true;
    case BlockType::Trace:
      tree->moveTreeOverTree(Matrix::Trace(child));
      return true;
    case BlockType::Transpose:
      tree->moveTreeOverTree(Matrix::Transpose(child));
      return true;
    case BlockType::Dim: {
      Tree* dim = SharedEditionPool->push<BlockType::Matrix>(1, 2);
      IntegerHandler(Matrix::NumberOfRows(child)).pushOnEditionPool();
      IntegerHandler(Matrix::NumberOfColumns(child)).pushOnEditionPool();
      tree->moveTreeOverTree(dim);
      return true;
    }
    case BlockType::Det: {
      Tree* determinant;
      Matrix::RowCanonize(child, true, &determinant);
      tree->moveTreeOverTree(determinant);
      return true;
    }
    case BlockType::Cross:
      tree->moveTreeOverTree(Vector::Cross(child, child->nextTree()));
      return true;
    case BlockType::Dot:
      tree->moveTreeOverTree(Vector::Dot(child, child->nextTree()));
      return true;
    case BlockType::Norm:
      tree->moveTreeOverTree(Vector::Norm(child));
      return true;
    default:
      return false;
  }
}

bool Simplification::DeepApplyMatrixOperators(Tree* tree) {
  int n = tree->numberOfChildren();
  Tree* child = tree->nextNode();
  bool changed = false;
  while (n--) {
    changed |= DeepApplyMatrixOperators(child);
    child = child->nextTree();
  }
  changed |= ShallowApplyMatrixOperators(tree);
  return changed;
}

}  // namespace PoincareJ
