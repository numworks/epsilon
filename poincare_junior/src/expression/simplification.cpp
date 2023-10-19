#include "simplification.h"

#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/comparison.h>
#include <poincare_junior/src/expression/complex.h>
#include <poincare_junior/src/expression/decimal.h>
#include <poincare_junior/src/expression/dimension.h>
#include <poincare_junior/src/expression/float.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/metric.h>
#include <poincare_junior/src/expression/parametric.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/trigonometry.h>
#include <poincare_junior/src/expression/unit.h>
#include <poincare_junior/src/expression/vector.h>
#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/placeholder.h>
#include <poincare_junior/src/n_ary.h>

#include "arithmetic.h"
#include "derivation.h"
#include "number.h"
#include "poincare_junior/src/expression/dependency.h"
#include "poincare_junior/src/expression/variables.h"
#include "poincare_junior/src/memory/type_block.h"

namespace PoincareJ {

bool IsInteger(const Tree* u) { return u->type().isInteger(); }
bool IsNumber(const Tree* u) { return u->type().isNumber(); }
bool IsRational(const Tree* u) { return u->type().isRational(); }
bool IsConstant(const Tree* u) { return IsNumber(u); }
bool IsUndef(const Tree* u) { return u->type() == BlockType::Undefined; }

bool Simplification::DeepSystematicReduce(Tree* u) {
  /* Although they are also flattened in ShallowSystematicReduce, flattening
   * here could save multiple ShallowSystematicReduce and flatten calls. */
  bool modified = (u->type() == BlockType::Multiplication ||
                   u->type() == BlockType::Addition) &&
                  NAry::Flatten(u);
  for (Tree* child : u->children()) {
    modified |= DeepSystematicReduce(child);
    assert(!IsUndef(child));
  }
#if ASSERTIONS
  EditionReference previousTree = u->clone();
#endif
  bool shallowModified = ShallowSystematicReduce(u);
#if ASSERTIONS
  assert(shallowModified != u->treeIsIdenticalTo(previousTree));
  previousTree->removeTree();
#endif
  return shallowModified || modified;
}

bool Simplification::ShallowSystematicReduce(Tree* u) {
  // This assert is quite costly, should be an assert level 2 ?
  assert(Dimension::DeepCheckDimensions(u));
  if (u->numberOfChildren() == 0) {
    // Strict rationals are the only childless trees that can be reduced.
    return Rational::MakeIrreducible(u);
  }
  bool changed = false;
  /* During a PatternMatching replace KPow(KA, KB) -> KExp(KMult(KLn(KA), KB))
   * with KA a Float and KB a UserVariable. We need to
   * ApproximateAndReplaceEveryScalar again on ShallowSystematicReduce. */
  for (Tree* child : u->children()) {
    if (child->type().isFloat()) {
      changed = Approximation::ApproximateAndReplaceEveryScalar(u);
      if (changed && u->type().isFloat()) {
        return true;
      }
      break;
    }
  }
  changed |= SimplifySwitch(u);
  if (Dependency::ShallowBubbleUpDependencies(u)) {
    ShallowSystematicReduce(u->nextNode());
    changed = true;
  }
  return changed;
}

bool Simplification::SimplifySwitch(Tree* u) {
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
    case BlockType::ComplexArgument:
      return SimplifyComplexArgument(u);
    case BlockType::ImaginaryPart:
      return SimplifyImaginaryPart(u);
    case BlockType::RealPart:
      return SimplifyRealPart(u);
    case BlockType::Sum:
    case BlockType::Product:
      return Parametric::SimplifySumOrProduct(u);
    case BlockType::GCD:
      return Arithmetic::SimplifyGCD(u);
    case BlockType::LCM:
      return Arithmetic::SimplifyLCM(u);
    case BlockType::Quotient:
      return Arithmetic::SimplifyQuotient(u);
    case BlockType::Remainder:
      return Arithmetic::SimplifyRemainder(u);
    default:
      return false;
  }
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
  if (PatternMatching::Match(KExp(KMult(KA, KLn(KB))), u, &ctx) &&
      IsInteger(ctx.getNode(KA))) {
    // exp(n*ln(x)) -> x^n with n an integer
    u->moveTreeOverTree(PatternMatching::CreateAndSimplify(KPow(KB, KA), ctx));
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
    ExceptionCheckpoint::Raise(ExceptionType::Nonreal);
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
  if (child->type() == BlockType::Complex) {
    assert(Complex::IsSanitized(child));
    // |x+iy| = √(x^2+y^2) if x and y are reals
    return PatternMatching::MatchReplaceAndSimplify(
               u, KAbs(KComplex(KA, KB)),
               KExp(KMult(KHalf, KLn(KAdd(KPow(KA, 2_e), KPow(KB, 2_e)))))) ||
           changed;
  }
  if (!IsNumber(child)) {
    return changed;
  }
  if (Number::Sign(child).isPositive()) {
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
  Tree* x = u->child(0);
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
  Tree* secondArgument = u->child(1);
  bool isOpposed = false;
  bool changed = SimplifyTrigSecondElement(secondArgument, &isOpposed);
  assert(Number::IsZero(secondArgument) || Number::IsOne(secondArgument));
  bool isSin = Number::IsOne(secondArgument);
  // cos(-x) = cos(x) and sin(-x) = -sin(x)
  Tree* firstArgument = u->nextNode();
  if (PatternMatching::MatchAndReplace(firstArgument, KMult(KTA, -1_e, KTB),
                                       KMult(KTA, KTB))) {
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
  if (firstArgument->treeIsIdenticalTo(π_e) || Number::IsZero(firstArgument) ||
      (firstArgument->type() == BlockType::Multiplication &&
       firstArgument->numberOfChildren() == 2 &&
       IsRational(firstArgument->nextNode()) &&
       firstArgument->child(1)->treeIsIdenticalTo(π_e))) {
    const Tree* piFactor = firstArgument->type() == BlockType::Multiplication
                               ? firstArgument->nextNode()
                               : (Number::IsZero(firstArgument) ? 0_e : 1_e);
    // Compute n such that firstArgument = (n/12)*pi
    Tree* multipleTree = Rational::Multiplication(12_e, piFactor);
    Rational::MakeIrreducible(multipleTree);
    if (IsInteger(multipleTree)) {
      // Trig is 2pi periodic, n can be retrieved as a uint8_t.
      multipleTree->moveTreeOverTree(IntegerHandler::Remainder(
          Integer::Handler(multipleTree), IntegerHandler(24)));
      assert(Integer::Is<uint8_t>(multipleTree));
      uint8_t n = Integer::Handler(multipleTree).to<uint8_t>();
      multipleTree->removeTree();
      u->cloneTreeOverTree(Trigonometry::ExactFormula(n, isSin, &isOpposed));
      DeepSystematicReduce(u);
      changed = true;
    } else {
      multipleTree->removeTree();
    }
  }

  if (isOpposed) {
    u->moveTreeAtNode((-1_e)->clone());
    u->moveNodeAtNode(SharedEditionPool->push<BlockType::Multiplication>(2));
    SimplifyMultiplication(u);
    changed = true;
  }
  return changed;
}

bool Simplification::SimplifyPower(Tree* u) {
  assert(u->type() == BlockType::Power);
  Tree* v = u->child(0);
  // 1^x -> 1
  if (Number::IsOne(v)) {
    u->cloneTreeOverTree(1_e);
    return true;
  }
  // u^n
  EditionReference n = u->child(1);
  // After systematic reduction, a power can only have integer index.
  if (!n->type().isInteger()) {
    // TODO: Handle 0^x with x > 0 before to avoid ln(0)
    return PatternMatching::MatchReplaceAndSimplify(u, KPow(KA, KB),
                                                    KExp(KMult(KLn(KA), KB)));
  }
  // 0^n -> 0
  if (Number::IsZero(v)) {
    if (!Number::IsZero(n) && Rational::Sign(n).isStrictlyPositive()) {
      u->cloneTreeOverTree(0_e);
      return true;
    }
    ExceptionCheckpoint::Raise(ExceptionType::ZeroPowerZero);
  }
  if (IsRational(v)) {
    u->moveTreeOverTree(Rational::IntegerPower(v, n));
    Rational::MakeIrreducible(u);
    return true;
  }
  assert(IsInteger(n));
  // v^0 -> 1
  if (Number::IsZero(n)) {
    if (Variables::HasVariables(v)) {
      return PatternMatching::MatchAndReplace(u, KPow(KA, 0_e),
                                              KDep(1_e, KSet(KPow(KA, -1_e))));
    } else {
      // TODO use sign to check if it may be null
      u->cloneTreeOverTree(1_e);
      return true;
    }
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
    int rem = Integer::Handler(remainder).to<uint8_t>();
    remainder->removeTree();
    v->nextNode()->removeTree();
    v->removeNode();
    // A^n
    SimplifyPower(u);
    // u could be any tree from this point forward
    if (rem > 1) {
      // -u
      u->moveTreeAtNode((-1_e)->clone());
      u->moveNodeAtNode(SharedEditionPool->push<BlockType::Multiplication>(2));
      SimplifyMultiplication(u);
    }
    if (rem % 2 == 1) {
      // u is a pure imaginary
      u->moveTreeAtNode((0_e)->clone());
      u->moveNodeAtNode(SharedEditionPool->push(BlockType::Complex));
      assert(!SimplifyComplex(u));
    }
    return true;
  }
  // (w^p)^n -> w^(p*n)
  if (v->type() == BlockType::Power) {
    EditionReference p = v->child(1);
    assert(p->nextTree() == static_cast<Tree*>(n));
    // PowU PowV w p n
    v->removeNode();
    MoveNodeAtNode(p, SharedEditionPool->push<BlockType::Multiplication>(2));
    // PowU w Mult<2> p n
    SimplifyMultiplication(p);
    SimplifyPower(u);
    return true;
  }
  // (w1*...*wk)^n -> w1^n * ... * wk^n
  if (v->type() == BlockType::Multiplication) {
    for (Tree* w : v->children()) {
      EditionReference m = SharedEditionPool->push(BlockType::Power);
      w->clone();
      n->clone();
      w->moveTreeOverTree(m);
      SimplifyPower(m);
    }
    n->removeTree();
    u->removeNode();
    SimplifyMultiplication(u);
    return true;
  }
  // exp(a)^b -> exp(a*b)
  return PatternMatching::MatchReplaceAndSimplify(u, KPow(KExp(KA), KB),
                                                  KExp(KMult(KA, KB)));
}

const Tree* Base(const Tree* u) {
  return u->type() == BlockType::Power ? u->child(0) : u;
}

const Tree* Exponent(const Tree* u) {
  return u->type() == BlockType::Power ? u->child(1) : 1_e;
}

void Simplification::ConvertPowerRealToPower(Tree* u) {
  u->cloneNodeOverNode(KPow);
  SimplifyPower(u);
}

bool Simplification::SimplifyPowerReal(Tree* u) {
  assert(u->type() == BlockType::PowerReal);
  /* Return :
   * - x^y if x is complex or positive or y is integer
   * - PowerReal(x,y) if y is not a rational
   * - Looking at y's reduced rational form p/q :
   *   * PowerReal(x,y) if x is of unknown sign and p odd
   *   * Nonreal if q is even and x negative
   *   * |x|^y if p is even
   *   * -|x|^y if p is odd
   */
  Tree* x = u->child(0);
  Tree* y = u->child(1);
  bool xIsNumber = IsNumber(x);
  bool xIsPositiveNumber = xIsNumber && Number::Sign(x).isPositive();
  bool xIsNegativeNumber = xIsNumber && !xIsPositiveNumber;
  if (xIsPositiveNumber || x->type() == BlockType::Complex || IsInteger(y)) {
    // TODO : Handle sign and complex status not only on numbers
    ConvertPowerRealToPower(u);
    return true;
  }

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
    ExceptionCheckpoint::Raise(ExceptionType::Nonreal);
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
  if (IsNumber(child) && IsNumber(next) &&
      !((child->type() == BlockType::Constant) ||
        next->type() == BlockType::Constant)) {
    // Merge numbers
    merge = Number::Multiplication(child, next);
  } else if (Base(child)->treeIsIdenticalTo(Base(next))) {
    // t^m * t^n -> t^(m+n)
    merge = PatternMatching::CreateAndSimplify(
        KPow(KA, KAdd(KB, KC)),
        {.KA = Base(child), .KB = Exponent(child), .KC = Exponent(next)});
    assert(merge->type() != BlockType::Multiplication);
  } else if (child->type() == BlockType::Complex ||
             next->type() == BlockType::Complex) {
    // (A+B*i)*(C+D*i) -> ((AC-BD)+(AD+BC)*i)
    merge = PatternMatching::CreateAndSimplify(
        KComplex(KAdd(KMult(KA, KC), KMult(-1_e, KB, KD)),
                 KAdd(KMult(KA, KD), KMult(KB, KC))),
        {.KA = Complex::UnSanitizedRealPart(child),
         .KB = Complex::UnSanitizedImagPart(child),
         .KC = Complex::UnSanitizedRealPart(next),
         .KD = Complex::UnSanitizedImagPart(next)});
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
  if (NAry::Sort(multiplication, Comparison::Order::PreserveMatrices)) {
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
  changed = NAry::Sort(u, Comparison::Order::PreserveMatrices) || changed;
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
    return u->numberOfChildren() == 2 && IsRational(u->child(0)) &&
           u->child(1)->treeIsIdenticalTo(v);
  }
  bool uHasRational = IsRational(u->child(0));
  bool vHasRational = IsRational(v->child(0));
  int n = u->numberOfChildren() - uHasRational;
  if (n != v->numberOfChildren() - vHasRational) {
    return false;
  }
  const Tree* childU = u->child(uHasRational);
  const Tree* childV = v->child(vHasRational);
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
  if (u->type() == BlockType::Multiplication && IsRational(u->child(0))) {
    NAry::RemoveChildAtIndex(c, 0);
    NAry::SquashIfUnary(c);
  }
  return c;
}

// The constant of 2ab is 2
const Tree* Constant(const Tree* u) {
  if (u->type() == BlockType::Multiplication && IsRational(u->child(0))) {
    return u->child(0);
  }
  return 1_e;
}

bool Simplification::MergeAdditionChildWithNext(Tree* child, Tree* next) {
  assert(next == child->nextTree());
  Tree* merge = nullptr;
  if (IsNumber(child) && IsNumber(next) &&
      !((child->type() == BlockType::Constant) ||
        next->type() == BlockType::Constant)) {
    // Merge numbers
    merge = Number::Addition(child, next);
  } else if (TermsAreEqual(child, next)) {
    // k1 * a + k2 * a -> (k1+k2) * a
    Tree* term = PushTerm(child);
    merge = PatternMatching::CreateAndSimplify(
        KMult(KAdd(KA, KB), KC),
        {.KA = Constant(child), .KB = Constant(next), .KC = term});
    term->removeTree();
    merge = term;
    assert(merge->type() != BlockType::Addition);
  } else if (child->type() == BlockType::Complex ||
             next->type() == BlockType::Complex) {
    // (A+B*i)+(C+D*i) -> ((A+C)+(B+D)*i)
    merge = PatternMatching::CreateAndSimplify(
        KComplex(KAdd(KA, KC), KAdd(KB, KD)),
        {.KA = Complex::UnSanitizedRealPart(child),
         .KB = Complex::UnSanitizedImagPart(child),
         .KC = Complex::UnSanitizedRealPart(next),
         .KD = Complex::UnSanitizedImagPart(next)});
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

bool Simplification::SimplifyComplex(Tree* tree) {
  assert(tree->type() == BlockType::Complex);
  Tree* imag = tree->child(1);
  if (Number::IsZero(imag)) {
    // (A+0*i) -> A
    imag->removeTree();
    tree->removeNode();
    return true;
  }
  if (PatternMatching::MatchAndReplace(tree, KComplex(KRe(KA), KIm(KA)), KA)) {
    // re(x)+i*im(x) = x
    return true;
  }
  if (Complex::IsSanitized(tree)) {
    return false;
  }
  // x+iy = (re(x)-im(y)) + i*(im(x)+re(y))
  bool result = PatternMatching::MatchReplaceAndSimplify(
      tree, KComplex(KA, KB),
      KComplex(KAdd(KRe(KA), KMult(-1_e, KIm(KB))), KAdd(KIm(KA), KRe(KB))));
  assert(result && Complex::IsSanitized(tree));
  return result;
}

bool Simplification::SimplifyComplexArgument(Tree* tree) {
  assert(tree->type() == BlockType::ComplexArgument);
  Tree* child = tree->child(0);
  if (child->type().isNumber()) {
    Sign::Sign sign = Number::Sign(child);
    if (sign.isZero()) {
      ExceptionCheckpoint::Raise(ExceptionType::Unhandled);
    }
    tree->cloneTreeOverTree(sign.isStrictlyPositive() ? 0_e : π_e);
    return true;
  }
  // TODO: Implement for complexes
  return false;
}

bool Simplification::SimplifyRealPart(Tree* tree) {
  assert(tree->type() == BlockType::RealPart);
  Tree* child = tree->child(0);
  if (child->type() == BlockType::Complex || Complex::IsReal(child)) {
    assert(Complex::IsSanitized(child));
    // re(x+i*y) = x if x and y are reals
    tree->cloneTreeOverTree(Complex::UnSanitizedRealPart(child));
    return true;
  }
  // re(x+y) = re(x)+re(z)
  return (child->type() == BlockType::Addition) &&
         Simplification::DistributeOverNAry(
             tree, BlockType::RealPart, BlockType::Addition,
             BlockType::Addition, SimplifyRealPart);
}

bool Simplification::SimplifyImaginaryPart(Tree* tree) {
  assert(tree->type() == BlockType::ImaginaryPart);
  Tree* child = tree->child(0);
  if (child->type() == BlockType::Complex || Complex::IsReal(child)) {
    assert(Complex::IsSanitized(child));
    // im(x+i*y) = y if x and y are reals
    tree->cloneTreeOverTree(Complex::UnSanitizedImagPart(child));
    return true;
  }
  // im(x+y) = im(x)+im(z)
  return (child->type() == BlockType::Addition) &&
         Simplification::DistributeOverNAry(
             tree, BlockType::ImaginaryPart, BlockType::Addition,
             BlockType::Addition, SimplifyImaginaryPart);
}

bool ShouldApproximateOnSimplify(Dimension dimension) {
  // Only angle units are expected not to be approximated.
  return (dimension.isUnit() && !dimension.isAngleUnit());
}

bool Simplification::Simplify(Tree* ref, ProjectionContext projectionContext) {
  /* TODO: If following assert cannot be satisfied, copy ref at the end, call
   * SimplifyLastTree on copy and replace results. */
  assert(SharedEditionPool->lastBlock() == ref->nextTree()->block());
  return SimplifyLastTree(ref, projectionContext);
}

bool Simplification::SimplifyLastTree(Tree* ref,
                                      ProjectionContext projectionContext) {
  ExceptionTryAboveBlock(ref->block()) {
    if (!Dimension::DeepCheckDimensions(ref)) {
      // TODO: Raise appropriate exception in DeepCheckDimensions.
      ExceptionCheckpoint::Raise(ExceptionType::UnhandledDimension);
    }
    projectionContext.m_dimension = Dimension::GetDimension(ref);
    if (ShouldApproximateOnSimplify(projectionContext.m_dimension)) {
      projectionContext.m_strategy = Strategy::ApproximateToFloat;
    }
    bool changed = false;
    changed =
        Projection::DeepSystemProjection(ref, projectionContext) || changed;
    Tree* variables = Variables::GetUserSymbols(ref);
    SwapTrees(&ref, &variables);
    Variables::ProjectToId(ref, variables);
    changed = DeepSystematicReduce(ref) || changed;
    changed = DeepApplyMatrixOperators(ref) || changed;
    // TODO: Bubble up Matrices, complexes, units, lists.
    changed = AdvancedReduction(ref, ref) || changed;
    assert(!DeepSystematicReduce(ref));
    assert(!DeepApplyMatrixOperators(ref));
    changed = Beautification::DeepBeautify(ref, projectionContext) || changed;
    Variables::BeautifyToName(ref, variables);
    variables->removeTree();
    return changed;
  }
  ExceptionCatch(type) {
    switch (type) {
      case ExceptionType::Nonreal:
      case ExceptionType::ZeroPowerZero:
      case ExceptionType::ZeroDivision:
      case ExceptionType::UnhandledDimension:
      case ExceptionType::Unhandled:
        (type == ExceptionType::Nonreal ? KNonreal : KUndef)->clone();
        return true;
      case ExceptionType::PoolIsFull:
        /* TODO: If simplification fails, try with a simpler projection
         * context. */
      default:
        ExceptionCheckpoint::Raise(type);
    }
  }
}

bool Simplification::AdvancedReduction(Tree* ref, const Tree* root) {
  assert(!DeepSystematicReduce(ref));
  if (ref->type() == BlockType::Matrix) {
    // Escape matrices because no nested advanced reduction is expected.
    return false;
  }
  bool changed = false;
  for (Tree* child : ref->children()) {
    changed = AdvancedReduction(child, root) || changed;
  }
  if (changed) {
    ShallowSystematicReduce(ref);
  }
  return ShallowAdvancedReduction(ref, root, changed) || changed;
}

bool Simplification::ShallowAdvancedReduction(Tree* ref, const Tree* root,
                                              bool changed) {
  assert(!DeepSystematicReduce(ref));
  return (ref->type().isAlgebraic()
              ? AdvanceReduceOnAlgebraic(ref, root, changed)
              : AdvanceReduceOnTranscendental(ref, root, changed));
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
      bool reducedAlgebraic = ref->type().isAlgebraic() &&
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
                                        BlockType naryOutput,
                                        Operation operation, int childIndex) {
  assert(naryTarget == BlockType::Addition ||
         naryTarget == BlockType::Multiplication);
  assert(naryOutput == BlockType::Addition ||
         naryOutput == BlockType::Multiplication);
  if (ref->type() != target) {
    return false;
  }
  int numberOfChildren = ref->numberOfChildren();
  assert(childIndex < numberOfChildren);
  EditionReference children = ref->child(childIndex);
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
    /* Since it is constant, use a childIndexOffset to avoid child calls:
     * clone.child(childIndex)=Tree(clone.block()+childIndexOffset) */
    EditionReference(clone->block() + childIndexOffset)
        ->moveTreeOverTree(grandChild);
    // f(0,E) ... +(,B,C) ... *(f(A,E),,)
    operation(clone);
  }
  // f(0,E) ... +(,,) ... *(f(A,E), f(B,E), f(C,E))
  children->removeNode();
  // f(0,E) ... *(f(A,E), f(B,E), f(C,E))
  ref = ref->moveTreeOverTree(output);
  // *(f(A,E), f(B,E), f(C,E))
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
      ref, KMult(KTA, KAbs(KB), KAbs(KC), KTD),
      KMult(KTA, KAbs(KMult(KB, KC)), KTD));
}

bool Simplification::ExpandAbs(Tree* ref) {
  // |A*B*...| = |A|*|B|*...
  return DistributeOverNAry(ref, BlockType::Abs, BlockType::Multiplication,
                            BlockType::Multiplication, SimplifyAbs);
}

/* TODO:
 * - ContractLn and ExpandLn doesn't handle powers properly
 * - Many Contract methods could be factorized similarly to DistributeOverNAry
 */

bool Simplification::ContractLn(Tree* ref) {
  // A? + ln(B) + ln(C) + D? = A + ln(BC) + D
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KAdd(KTA, KLn(KB), KLn(KC), KTD),
      KAdd(KTA, KLn(KMult(KB, KC)), KTD));
}

bool Simplification::ExpandLn(Tree* ref) {
  // ln(A*B*...) = ln(A) + ln(B) + ...
  return DistributeOverNAry(ref, BlockType::Ln, BlockType::Multiplication,
                            BlockType::Addition, SimplifyLn);
}

bool Simplification::ExpandExp(Tree* ref) {
  return
      // exp(A+iB) = exp(A)*(cos(B) + i*sin(B))
      PatternMatching::MatchReplaceAndSimplify(
          ref, KExp(KComplex(KA, KB)),
          KMult(KExp(KA), KComplex(KTrig(KB, 0_e), KTrig(KB, 1_e)))) ||
      // exp(A+B+...) = exp(A) * exp(B) * ...
      DistributeOverNAry(ref, BlockType::Exponential, BlockType::Addition,
                         BlockType::Multiplication, SimplifyExp);
}

bool Simplification::ContractExpMult(Tree* ref) {
  // A? * exp(B) * exp(C) * D? = A * exp(B+C) * D
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KMult(KTA, KExp(KB), KExp(KC), KTD),
      KMult(KTA, KExp(KAdd(KB, KC)), KTD));
}

/* TODO : Find an easier solution for nested expand/contract smart shallow
 * simplification. */

bool Simplification::ExpandTrigonometric(Tree* ref) {
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
      // If newTrig1 expanded, newTrig3 should expand too
      assert(false);
    }
  } else {
    SimplifyTrig(newTrig1);
    SimplifyTrig(newTrig3);
  }
  /* Shallow reduce new trees. This step must be performed after sub-expansions
   * since SimplifyMultiplication may invalidate newTrig1 and newTrig3. */
  SimplifyAddition(newTrig4->child(1));
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
  SimplifyTrigDiff(newTrig1->child(1));
  SimplifyTrig(newTrig1);
  SimplifyAddition(newTrig2->child(0));
  SimplifyAddition(newTrig2->child(1));
  SimplifyTrig(newTrig2);

  if (!fIsEmpty) {
    SimplifyMultiplication(newMult1);
    SimplifyMultiplication(newMult2);
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
  SimplifyAddition(newAdd);
  SimplifyMultiplication(ref);
  return true;
}

bool Simplification::ExpandMult(Tree* ref) {
  // A?*(B?+C)*D? = A*B*D + A*C*D
  if (ref->type() != BlockType::Multiplication) {
    return false;
  }
  // Find the NAry in children
  int childIndex = 0;
  for (Tree* child : ref->children()) {
    if (child->type() == BlockType::Addition) {
      return DistributeOverNAry(ref, BlockType::Multiplication,
                                BlockType::Addition, BlockType::Addition,
                                ExpandMultSubOperation, childIndex);
    }
    childIndex++;
  }
  return false;
}

bool Simplification::ExpandPowerComplex(Tree* ref) {
  // (A + B*i)^2 = (A^2 -2*B^2 + 2*A*B*i)
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KPow(KComplex(KA, KB), 2_e),
      KComplex(KAdd(KPow(KA, 2_e), KMult(-1_e, KPow(KB, 2_e))),
               KMult(2_e, KA, KB)));
}

bool Simplification::ExpandPower(Tree* ref) {
  // (A?*B)^C = A^C * B^C is currently in SystematicSimplification
  // (A? + B)^2 = (A^2 + 2*A*B + B^2)
  // TODO: Implement a more general (A + B)^C expand.
  /* This isn't factorized with DistributeOverNAry because of the necessary
   * second term expansion. */
  // MatchReplaceAndSimplify's cannot be used because of nested expansion.
  if (!PatternMatching::MatchAndReplace(
          ref, KPow(KAdd(KTA, KB), 2_e),
          KAdd(KPow(KAdd(KTA), 2_e), KMult(2_e, KAdd(KTA), KB),
               KPow(KB, 2_e)))) {
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
  Tree* child = tree->child(0);
  if (tree->type() == BlockType::Identity) {
    tree->moveTreeOverTree(Matrix::Identity(child));
    return true;
  }
  if (tree->type() == BlockType::Multiplication) {
    int numberOfMatrices = tree->numberOfChildren();
    // Find first matrix
    const Tree* firstMatrix = nullptr;
    for (const Tree* child : tree->children()) {
      if (child->type() == BlockType::Matrix) {
        firstMatrix = child;
        break;
      }
      numberOfMatrices--;
    }
    if (!firstMatrix) {
      return false;
    }
    Tree* result = firstMatrix->clone();
    Tree* child = tree->nextNode();
    // Merge matrices
    while (child < firstMatrix) {
      result->moveTreeOverTree(Matrix::ScalarMultiplication(child, result));
      child = child->nextTree();
    }
    while (--numberOfMatrices) {
      child = child->nextTree();
      result->moveTreeOverTree(Matrix::Multiplication(result, child));
    }
    tree->moveTreeOverTree(result);
    return true;
  }
  if (child->type() != BlockType::Matrix) {
    return false;
  }
  if (tree->type() == BlockType::Addition) {
    int n = tree->numberOfChildren() - 1;
    Tree* result = child->clone();
    while (n--) {
      child = child->nextTree();
      result->moveTreeOverTree(Matrix::Addition(result, child));
    }
    tree->moveTreeOverTree(result);
    return true;
  }
  if (tree->type() == BlockType::PowerMatrix) {
    Tree* index = child->nextTree();
    if (!Integer::Is<int>(index)) {
      // TODO: Raise to rely on approximation.
      return false;
    }
    tree->moveTreeOverTree(
        Matrix::Power(child, Integer::Handler(index).to<int>()));
    return true;
  }
  if (tree->numberOfChildren() == 2) {
    Tree* child2 = child->nextTree();
    if (child2->type() != BlockType::Matrix) {
      return false;
    }
    switch (tree->type()) {
      case BlockType::Cross:
        tree->moveTreeOverTree(Vector::Cross(child, child2));
        return true;
      case BlockType::Dot:
        tree->moveTreeOverTree(Vector::Dot(child, child2));
        return true;
      default:
        return false;
    }
  }
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
      Integer::Push(Matrix::NumberOfRows(child));
      Integer::Push(Matrix::NumberOfColumns(child));
      tree->moveTreeOverTree(dim);
      return true;
    }
    case BlockType::Det: {
      Tree* determinant;
      Matrix::RowCanonize(child, true, &determinant);
      tree->moveTreeOverTree(determinant);
      return true;
    }
    case BlockType::Norm:
      tree->moveTreeOverTree(Vector::Norm(child));
      return true;
    default:
      return false;
  }
}

bool Simplification::DeepApplyMatrixOperators(Tree* tree) {
  bool changed = false;
  for (Tree* child : tree->children()) {
    changed |= DeepApplyMatrixOperators(child);
  }
  changed |= ShallowApplyMatrixOperators(tree);
  return changed;
}

}  // namespace PoincareJ
