#include "simplification.h"

#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/n_ary.h>

#include "advanced_simplification.h"
#include "approximation.h"
#include "beautification.h"
#include "comparison.h"
#include "dependency.h"
#include "derivation.h"
#include "dimension.h"
#include "k_tree.h"
#include "list.h"
#include "matrix.h"
#include "number.h"
#include "random.h"
#include "rational.h"
#include "unit.h"
#include "variables.h"
#include "vector.h"

namespace PoincareJ {

bool Simplification::DeepSystematicReduce(Tree* u) {
  /* Although they are also flattened in ShallowSystematicReduce, flattening
   * here could save multiple ShallowSystematicReduce and flatten calls. */
  bool modified =
      (u->isMultiplication() || u->isAddition()) && NAry::Flatten(u);
  for (Tree* child : u->children()) {
    modified |= DeepSystematicReduce(child);
    assert(!child->isUndefined());
    if (u->isDependency()) {
      // Skip systematic simplification of Dependencies.
      break;
    }
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

/* Approximate all children if one of them is already float. Return true if the
 * entire tree have been approximated. */
bool CanApproximateTree(Tree* u, bool* changed) {
  for (Tree* child : u->children()) {
    if (child->isFloat()) {
      if (Approximation::ApproximateAndReplaceEveryScalar(u)) {
        *changed = true;
        if (u->isFloat()) {
          return true;
        }
      }
      break;
    }
  }
  return false;
}

bool Simplification::ShallowSystematicReduce(Tree* u) {
  // This assert is quite costly, should be an assert level 2 ?
  assert(Dimension::DeepCheckDimensions(u));
  if (u->numberOfChildren() == 0) {
    // No childless trees have a reduction pattern.
    return false;
  }
  bool changed = false;
  /* During a PatternMatching replace KPow(KA, KB) -> KExp(KMult(KLn(KA), KB))
   * with KA a Float and KB a UserVariable. We need to
   * ApproximateAndReplaceEveryScalar again on ShallowSystematicReduce. */
  if (CanApproximateTree(u, &changed)) {
    return true;
  }
  changed |= SimplifySwitch(u);
  if (Dependency::ShallowBubbleUpDependencies(u)) {
    ShallowSystematicReduce(u->child(0));
    // f(dep(a, ...)) -> dep(f(a), ...) -> dep(dep(b, ...), ...) -> dep(b, ...)
    Dependency::ShallowBubbleUpDependencies(u);
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
    case BlockType::LnReal:
      return SimplifyLnReal(u);
    case BlockType::Abs:
      return SimplifyAbs(u);
    case BlockType::TrigDiff:
      return Trigonometry::SimplifyTrigDiff(u);
    case BlockType::Trig:
      return Trigonometry::SimplifyTrig(u);
    case BlockType::ATrig:
      return Trigonometry::SimplifyATrig(u);
    case BlockType::ArcTangentRad:
      return Trigonometry::SimplifyArcTangentRad(u);
    case BlockType::Derivative:
      return Derivation::ShallowSimplify(u);
    case BlockType::Ln:
      return Logarithm::SimplifyLn(u);
    case BlockType::Exponential:
      return SimplifyExp(u);
    case BlockType::ComplexArgument:
      return SimplifyComplexArgument(u);
    case BlockType::ImaginaryPart:
    case BlockType::RealPart:
      return SimplifyComplexPart(u);
    case BlockType::Sum:
    case BlockType::Product:
      return Parametric::SimplifySumOrProduct(u);
    case BlockType::GCD:
      return Arithmetic::SimplifyGCD(u);
    case BlockType::LCM:
      return Arithmetic::SimplifyLCM(u);
    case BlockType::Quotient:
    case BlockType::Remainder:
      return Arithmetic::SimplifyQuotientOrRemainder(u);
    case BlockType::Factorial:
      return Arithmetic::SimplifyFactorial(u);
    case BlockType::Binomial:
      return Arithmetic::SimplifyBinomial(u);
    case BlockType::Permute:
      return Arithmetic::SimplifyPermute(u);
    case BlockType::Sign:
      return SimplifySign(u);
    case BlockType::Floor:
      return Arithmetic::SimplifyFloor(u);
    case BlockType::Round:
      return Arithmetic::SimplifyRound(u);
    case BlockType::ListSort:
    case BlockType::Median:
      return List::ShallowApplyListOperators(u);
    case BlockType::Dim:
      if (Dimension::GetDimension(u->child(0)).isMatrix()) {
        return false;
      }
      return List::ShallowApplyListOperators(u);
    default:
      if (u->type().isListToScalar()) {
        return List::ShallowApplyListOperators(u);
      }
      return false;
  }
}

bool Simplification::SimplifyExp(Tree* u) {
  Tree* child = u->nextNode();
  if (child->isLn()) {
    /* TODO_PCJ: Add a ln(x) dependency on user-inputted ln only when x can be
     * null. */
    // exp(ln(x)) -> x
    u->removeNode();
    u->removeNode();
    return true;
  }
  if (child->isZero()) {
    // exp(0) = 1
    u->cloneTreeOverTree(1_e);
    return true;
  }
  PatternMatching::Context ctx;
  if (PatternMatching::Match(KExp(KMult(KA, KLn(KB))), u, &ctx) &&
      (ctx.getNode(KA)->isInteger() || ctx.getNode(KB)->isZero())) {
    /* To ensure there is only one way of representing x^n. Also handle 0^y with
     * Power logic. */
    // exp(n*ln(x)) -> x^n with n an integer or x null.
    u->moveTreeOverTree(PatternMatching::CreateAndSimplify(KPow(KB, KA), ctx));
    assert(!u->isExponential());
    return true;
  }
  return false;
}

bool Simplification::SimplifyAbs(Tree* u) {
  assert(u->isAbs());
  Tree* child = u->nextNode();
  if (child->isAbs()) {
    // ||x|| -> |x|
    child->removeNode();
    assert(!SimplifyAbs(u));
    return true;
  }
  ComplexSign complexSign = ComplexSign::Get(child);
  if (!complexSign.isPure()) {
    return false;
  }
  bool isReal = complexSign.isReal();
  Sign sign = isReal ? complexSign.realSign() : complexSign.imagSign();
  if (sign.canBeNegative() && sign.canBePositive()) {
    return false;
  }
  const Tree* minusOne = (isReal == sign.canBeNegative()) ? -1_e : 1_e;
  const Tree* complexI = isReal ? 1_e : i_e;
  // |3| = |-3| = |3i| = |-3i| = 3
  u->moveTreeOverTree(PatternMatching::CreateAndSimplify(
      KMult(KA, KB, KC), {.KA = minusOne, .KB = complexI, .KC = child}));
  return true;
}

bool Simplification::SimplifyPower(Tree* u) {
  assert(u->isPower());
  Tree* v = u->child(0);
  // 1^x -> 1
  if (v->isOne()) {
    u->cloneTreeOverTree(1_e);
    return true;
  }
  // v^n
  EditionReference n = v->nextTree();
  if (v->isZero()) {
    ComplexSign indexSign = ComplexSign::Get(n);
    if (indexSign.realSign().isStrictlyPositive()) {
      // 0^x is always defined.
      u->cloneTreeOverTree(0_e);
      return true;
    }
    if (!indexSign.realSign().canBePositive()) {
      // 0^x cannot be defined
      ExceptionCheckpoint::Raise(ExceptionType::Unhandled);
    }
    // Use a dependency as a fallback.
    return PatternMatching::MatchAndReplace(u, KA, KDep(0_e, KSet(KA)));
  }
  // After systematic reduction, a power can only have integer index.
  if (!n->isInteger()) {
    // v^n -> exp(n*ln(v))
    return PatternMatching::MatchReplaceAndSimplify(u, KPow(KA, KB),
                                                    KExp(KMult(KLn(KA), KB)));
  }
  if (v->isRational()) {
    u->moveTreeOverTree(Rational::IntegerPower(v, n));
    return true;
  }
  // v^0 -> 1
  if (n->isZero()) {
    if (ComplexSign::Get(v).canBeNull()) {
      return PatternMatching::MatchAndReplace(u, KA, KDep(1_e, KSet(KA)));
    }
    u->cloneTreeOverTree(1_e);
    return true;
  }
  // v^1 -> v
  if (n->isOne()) {
    u->moveTreeOverTree(v);
    return true;
  }
  if (v->isComplexI()) {
    // i^n -> ±1 or ±i
    Tree* remainder =
        IntegerHandler::Remainder(Integer::Handler(n), IntegerHandler(4));
    int rem = Integer::Handler(remainder).to<uint8_t>();
    remainder->removeTree();
    u->cloneTreeOverTree(
        rem == 0 ? 1_e
                 : (rem == 1 ? i_e : (rem == 2 ? -1_e : KMult(-1_e, i_e))));
    return true;
  }
  // (w^p)^n -> w^(p*n)
  if (v->isPower()) {
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
  if (v->isMultiplication()) {
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

const Tree* Base(const Tree* u) { return u->isPower() ? u->child(0) : u; }

const Tree* Exponent(const Tree* u) { return u->isPower() ? u->child(1) : 1_e; }

void Simplification::ConvertPowerRealToPower(Tree* u) {
  u->cloneNodeOverNode(KPow);
  SimplifyPower(u);
}

bool Simplification::SimplifyPowerReal(Tree* u) {
  assert(u->isPowerReal());
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
  Tree* y = x->nextTree();
  ComplexSign xSign = ComplexSign::Get(x);
  ComplexSign ySign = ComplexSign::Get(y);
  if (!ySign.canBeNonInteger() ||
      (xSign.isReal() && xSign.realSign().isPositive())) {
    ConvertPowerRealToPower(u);
    return true;
  }

  if (!y->isRational()) {
    // We don't know enough to simplify further.
    return false;
  }

  bool pIsEven = Rational::Numerator(y).isEven();
  bool qIsEven = Rational::Denominator(y).isEven();
  // y is simplified, both p and q can't be even
  assert(!qIsEven || !pIsEven);

  bool xNegative = xSign.realSign().isStrictlyNegative();

  if (!pIsEven && !xNegative) {
    // We don't know enough to simplify further.
    return false;
  }
  assert(xNegative || pIsEven);

  if (xNegative && qIsEven) {
    ExceptionCheckpoint::Raise(ExceptionType::Nonreal);
  }

  // We can fallback to |x|^y
  x->cloneNodeAtNode(KAbs);
  SimplifyAbs(x);
  ConvertPowerRealToPower(u);

  if (xNegative && !pIsEven) {
    // -|x|^y
    u->cloneTreeAtNode(KMult(-1_e));
    NAry::SetNumberOfChildren(u, 2);
    SimplifyMultiplication(u);
  }
  return true;
}

bool Simplification::SimplifyLnReal(Tree* u) {
  assert(u->isLnReal());
  // Under real mode, inputted ln(x) must return nonreal if x < 0
  ComplexSign childSign = ComplexSign::Get(u->child(0));
  if (childSign.realSign().isStrictlyNegative() ||
      !childSign.imagSign().canBeNull()) {
    // Child can't be real, positive or null
    ExceptionCheckpoint::Raise(ExceptionType::Nonreal);
  }
  if (childSign.realSign().canBeNegative() || !childSign.imagSign().isZero()) {
    // Child can be nonreal or negative, add a dependency in case.
    u->moveTreeOverTree(PatternMatching::Create(
        KDep(KLn(KA), KSet(KLnReal(KA))), {.KA = u->child(0)}));
    u = u->child(0);
  } else {
    // Safely fallback to complex logarithm.
    u->cloneNodeOverNode(KLn);
  }
  Logarithm::SimplifyLn(u);
  return true;
}

bool Simplification::MergeMultiplicationChildWithNext(Tree* child) {
  Tree* next = child->nextTree();
  Tree* merge = nullptr;
  if (child->isNumber() && next->isNumber() &&
      !((child->isConstant()) || next->isConstant())) {
    // Merge numbers
    merge = Number::Multiplication(child, next);
  } else if (Base(child)->treeIsIdenticalTo(Base(next))) {
    // t^m * t^n -> t^(m+n)
    merge = PatternMatching::CreateAndSimplify(
        KPow(KA, KAdd(KB, KC)),
        {.KA = Base(child), .KB = Exponent(child), .KC = Exponent(next)});
    assert(!merge->isMultiplication());
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
    if (child->isZero()) {
      *zero = true;
      return false;
    }
    if (child->isOne()) {
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
  if (!changed || NAry::SquashIfPossible(multiplication)) {
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
  assert(u->isMultiplication());
  bool changed = NAry::Flatten(u);
  if (changed && CanApproximateTree(u, &changed)) {
    /* In case of successful flatten, approximateAndReplaceEveryScalar must be
     * tried again to properly handle possible new float children. */
    return true;
  }
  if (NAry::SquashIfPossible(u)) {
    return true;
  }
  changed = NAry::Sort(u, Comparison::Order::PreserveMatrices) || changed;
  changed = SimplifySortedMultiplication(u) || changed;
  assert(!changed || !u->isMultiplication() || !SimplifyMultiplication(u));
  return changed;
}

bool TermsAreEqual(const Tree* u, const Tree* v) {
  if (!u->isMultiplication()) {
    if (!v->isMultiplication()) {
      return u->treeIsIdenticalTo(v);
    }
    return TermsAreEqual(v, u);
  }
  if (!v->isMultiplication()) {
    return u->numberOfChildren() == 2 && u->child(0)->isRational() &&
           u->child(1)->treeIsIdenticalTo(v);
  }
  bool uHasRational = u->child(0)->isRational();
  bool vHasRational = v->child(0)->isRational();
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
  if (u->isMultiplication() && u->child(0)->isRational()) {
    NAry::RemoveChildAtIndex(c, 0);
    NAry::SquashIfPossible(c);
  }
  return c;
}

// The constant of 2ab is 2
const Tree* Constant(const Tree* u) {
  if (u->isMultiplication() && u->child(0)->isRational()) {
    return u->child(0);
  }
  return 1_e;
}

bool Simplification::MergeAdditionChildWithNext(Tree* child, Tree* next) {
  assert(next == child->nextTree());
  Tree* merge = nullptr;
  if (child->isNumber() && next->isNumber() &&
      !((child->isConstant()) || next->isConstant())) {
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
  assert(u->isAddition());
  bool modified = NAry::Flatten(u);
  if (modified && CanApproximateTree(u, &modified)) {
    /* In case of successful flatten, approximateAndReplaceEveryScalar must be
     * tried again to properly handle possible new float children. */
    return true;
  }
  if (NAry::SquashIfPossible(u)) {
    return true;
  }
  modified = NAry::Sort(u) || modified;
  bool didSquashChildren = false;
  int n = u->numberOfChildren();
  int i = 0;
  Tree* child = u->nextNode();
  while (i < n) {
    if (child->isZero()) {
      child->removeTree();
      modified = true;
      n--;
      continue;
    }
    Tree* next = child->nextTree();
    if (i + 1 < n && MergeAdditionChildWithNext(child, next)) {
      // 1 + (a + b)/2 + (a + b)/2 -> 1 + a + b
      if (child->isAddition()) {
        n += child->numberOfChildren() - 1;
        child->removeNode();
        didSquashChildren = true;
      }
      modified = true;
      n--;
    } else {
      child = next;
      i++;
    }
  }
  if (n != u->numberOfChildren()) {
    assert(modified);
    NAry::SetNumberOfChildren(u, n);
    if (NAry::SquashIfPossible(u)) {
      return true;
    }
  }
  if (didSquashChildren) {
    /* Newly squashed children should be sorted again and they may allow new
     * simplifications. NOTE: Further simplification could theoretically be
     * unlocked, see following assertion. */
    NAry::Sort(u);
  }
  /* TODO: SimplifyAddition may encounter the same issues as the multiplication.
   * If this assert can't be preserved, SimplifyAddition must handle one or both
   * of this cases as handled in multiplication:
   * With a,b and c the sorted addition children (a < b < c), M(a,b) the result
   * of merging children a and b (with MergeAdditionChildWithNext) if it exists.
   * - M(a,b) > c or a > M(b,c) (Addition must be sorted again)
   * - M(a,b) doesn't exists, but M(a,M(b,c)) does (previous child should try
   * merging again when child merged with nextChild) */
  assert(!modified || !SimplifyAddition(u));
  return modified;
}

bool Simplification::SimplifyComplexArgument(Tree* tree) {
  assert(tree->isComplexArgument());
  const Tree* child = tree->child(0);
  ComplexSign childSign = ComplexSign::Get(child);
  // arg(x + iy) = atan2(y, x)
  Sign realSign = childSign.realSign();
  if (!realSign.isKnown()) {
    return false;
  }
  // TODO: Maybe move this in advanced reduction
  Sign imagSign = childSign.imagSign();
  if (realSign.isZero() && imagSign.isKnown()) {
    if (imagSign.isZero()) {
      // atan2(0, 0) = undef
      ExceptionCheckpoint::Raise(ExceptionType::Undefined);
    }
    // atan2(y, 0) = π/2 if y > 0, -π/2 if y < 0
    tree->cloneTreeOverTree(imagSign.isStrictlyPositive()
                                ? KMult(KHalf, π_e)
                                : KMult(-1_e / 2_e, π_e));
  } else if (realSign.isStrictlyPositive() || imagSign.isPositive() ||
             imagSign.isStrictlyNegative()) {
    /* atan2(y, x) = arctan(y/x)      if x > 0
     *               arctan(y/x) + π  if y >= 0 and x < 0
     *               arctan(y/x) - π  if y < 0  and x < 0 */
    tree->moveTreeOverTree(PatternMatching::CreateAndSimplify(
        KAdd(KATanRad(KMult(KIm(KA), KPow(KRe(KA), -1_e))), KMult(KB, π_e)),
        {.KA = child,
         .KB = realSign.isStrictlyPositive()
                   ? 0_e
                   : (imagSign.isPositive() ? 1_e : -1_e)}));
  } else {
    return false;
  }
  return true;
}

bool Simplification::SimplifyComplexPart(Tree* tree) {
  assert(tree->isRealPart() || tree->isImaginaryPart());
  Tree* child = tree->child(0);
  ComplexSign childSign = ComplexSign::Get(child);
  if (!childSign.isPure()) {
    // Rely on advanced reduction re(x+iy) -> re(x) + re(iy)
    return false;
  }
  if (tree->isRealPart() != childSign.isReal()) {
    // re(x) = 0 or im(x) = 0
    tree->cloneTreeOverTree(0_e);
  } else if (tree->isRealPart()) {
    // re(x) = x
    tree->removeNode();
  } else {
    // im(x) = -i*x
    tree->moveTreeOverTree(PatternMatching::CreateAndSimplify(
        KMult(-1_e, i_e, KA), {.KA = child}));
  }
  return true;
}

bool Simplification::SimplifySign(Tree* expr) {
  assert(expr->isSign());
  ComplexSign sign = ComplexSign::Get(expr->firstChild());
  const Tree* result;
  if (sign.isZero()) {
    result = 0_e;
  } else if (!sign.isReal()) {
    // Could use sign(z) = exp(i*arg(z)) but sign(z) is preferred. Advanced ?
    return false;
  } else if (sign.realSign().isStrictlyPositive()) {
    result = 1_e;
  } else if (sign.realSign().isStrictlyNegative()) {
    result = -1_e;
  } else {
    return false;
  }
  expr->cloneTreeOverTree(result);
  return true;
}

bool ShouldApproximateOnSimplify(Dimension dimension) {
  // Only angle units are expected not to be approximated.
  return (dimension.isUnit() && !dimension.isAngleUnit());
}

bool RelaxProjectionContext(void* context) {
  ProjectionContext* projectionContext =
      static_cast<ProjectionContext*>(context);
  if (projectionContext->m_strategy == Strategy::ApproximateToFloat) {
    // Nothing more can be done.
    return false;
  }
  projectionContext->m_strategy =
      (projectionContext->m_strategy == Strategy::NumbersToFloat)
          ? Strategy::ApproximateToFloat
          : Strategy::NumbersToFloat;
  return true;
}

bool Simplification::Simplify(Tree* ref, ProjectionContext projectionContext) {
  // Clone the tree, and use an adaptive strategy to handle pool overflow.
  SharedEditionPool->executeAndReplaceTree(
      [](void* context, const void* data) {
        SimplifyLastTree(static_cast<const Tree*>(data)->clone(),
                         *static_cast<ProjectionContext*>(context));
      },
      &projectionContext, ref, RelaxProjectionContext);
  /* TODO: Due to projection/beautification cycles, SimplifyLastTree will most
   *       likely return true everytime anyway. */
  return true;
}

bool Simplification::SimplifyLastTree(Tree* ref,
                                      ProjectionContext projectionContext) {
  assert(SharedEditionPool->lastBlock() == ref->nextTree()->block());
  ExceptionTryAfterBlock(ref->block()) {
    if (!Dimension::DeepCheckDimensions(ref) ||
        !Dimension::DeepCheckListLength(ref)) {
      // TODO: Raise appropriate exception in DeepCheckDimensions.
      ExceptionCheckpoint::Raise(ExceptionType::UnhandledDimension);
    }
    projectionContext.m_dimension = Dimension::GetDimension(ref);
    if (projectionContext.m_strategy != Strategy::ApproximateToFloat &&
        ShouldApproximateOnSimplify(projectionContext.m_dimension)) {
      ExceptionCheckpoint::Raise(ExceptionType::RelaxContext);
    }
    bool changed = false;
    // Seed random nodes before anything is merged/duplicated.
    changed = Random::SeedTreeNodes(ref) > 0;
    changed =
        Projection::DeepSystemProjection(ref, projectionContext) || changed;
    Tree* variables = Variables::GetUserSymbols(ref);
    SwapTrees(&ref, &variables);
    Variables::ProjectToId(
        ref, variables,
        projectionContext.m_complexFormat == ComplexFormat::Real
            ? ComplexSign::RealUnknown()
            : ComplexSign::Unknown());
    changed = DeepSystematicReduce(ref) || changed;
    changed = DeepApplyMatrixOperators(ref) || changed;
    assert(!DeepSystematicReduce(ref));
    assert(!DeepApplyMatrixOperators(ref));
    changed =
        List::BubbleUp(
            ref, [](Tree* e) -> bool { return ShallowSystematicReduce(e); }) ||
        changed;
    changed = AdvancedSimplification::AdvancedReduction(ref) || changed;

    if (projectionContext.m_strategy == Strategy::ApproximateToFloat) {
      // Approximate again in case exact numbers appeared during simplification.
      changed = Approximation::ApproximateAndReplaceEveryScalar(ref);
    }
    changed = Beautification::DeepBeautify(ref, projectionContext) || changed;
    Variables::BeautifyToName(ref, variables);
    variables->removeTree();
    return changed;
  }
  ExceptionCatch(type) {
    switch (type) {
      case ExceptionType::BadType:
      case ExceptionType::Nonreal:
      case ExceptionType::ZeroPowerZero:
      case ExceptionType::ZeroDivision:
      case ExceptionType::UnhandledDimension:
      case ExceptionType::Unhandled:
      case ExceptionType::Undefined:
        (type == ExceptionType::Nonreal ? KNonreal : KUndef)->clone();
        return true;
      default:
        ExceptionCheckpoint::Raise(type);
    }
  }
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
    // Apply operation anyway, as if it was in a squashed NAry
    // f(A,E) -> f'(A,E)
    return operation(ref);
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
    // f(0,E) ... +(,B,C) ... *(f'(A,E),,)
  }
  // f(0,E) ... +(,,) ... *(f'(A,E), f'(B,E), f'(C,E))
  children->removeNode();
  // f(0,E) ... *(f'(A,E), f'(B,E), f'(C,E))
  ref = ref->moveTreeOverTree(output);
  // *(f'(A,E), f'(B,E), f'(C,E)) ...
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

bool Simplification::TryOneOperation(Tree* e, const Operation* operations,
                                     int numberOfOperations) {
  assert(!DeepSystematicReduce(e));
  for (size_t i = 0; i < numberOfOperations; i++) {
    if (operations[i](e)) {
      assert(!DeepSystematicReduce(e));
      return true;
    }
  }
  return false;
}

bool Simplification::DeepContract(Tree* e) {
  bool changed = false;
  for (Tree* child : e->children()) {
    changed = DeepContract(child) || changed;
  }
  // TODO: Assert !DeepContract(e)
  return ShallowContract(e, true) || changed;
}

bool Simplification::DeepExpand(Tree* e) {
  if (Tree::ApplyShallowInDepth(
          e, [](Tree* e, void* context) { return ShallowExpand(e, true); })) {
    // Bottom-up systematic reduce is necessary.
    DeepSystematicReduce(e);
    // TODO_PCJ: Find a solution so we don't have to run this twice.
    bool temp = DeepExpand(e);
    assert(!temp || !DeepExpand(e));
    return true;
  }
  return false;
}

bool Simplification::ExpandImRe(Tree* ref) {
  return
      // im(A+B) = im(A) + im(B)
      PatternMatching::MatchReplaceAndSimplify(ref, KIm(KAdd(KA, KTB)),
                                               KAdd(KIm(KA), KIm(KAdd(KTB)))) ||
      // re(A+B) = re(A) + re(B)
      PatternMatching::MatchReplaceAndSimplify(ref, KRe(KAdd(KA, KTB)),
                                               KAdd(KRe(KA), KRe(KAdd(KTB)))) ||
      // im(A*B) = im(A)re(B) + re(A)im(B)
      PatternMatching::MatchReplaceAndSimplify(
          ref, KIm(KMult(KA, KTB)),
          KAdd(KMult(KIm(KA), KRe(KMult(KTB))),
               KMult(KRe(KA), KIm(KMult(KTB))))) ||
      // re(A*B) = re(A)*re(B) - im(A)*im(B)
      PatternMatching::MatchReplaceAndSimplify(
          ref, KRe(KMult(KA, KTB)),
          KAdd(KMult(KRe(KA), KRe(KMult(KTB))),
               KMult(-1_e, KIm(KA), KIm(KMult(KTB))))) ||
      // Replace im and re in additions only to prevent infinitely expanding
      // A? + B?*im(C)*D? + E? = A - i*B*C*D + i*B*re(C)*D + E
      PatternMatching::MatchReplaceAndSimplify(
          ref, KAdd(KTA, KMult(KTB, KIm(KC), KTD), KTE),
          KAdd(KTA, KMult(-1_e, i_e, KTB, KC, KTD),
               KMult(i_e, KTB, KRe(KC), KTD), KTE)) ||
      // A? + B?*re(C)*D? + E? = A + B*C*D - i*B*im(C)*D + E
      PatternMatching::MatchReplaceAndSimplify(
          ref, KAdd(KTA, KMult(KTB, KRe(KC), KTD), KTE),
          KAdd(KTA, KMult(KTB, KC, KTD), KMult(-1_e, i_e, KTB, KIm(KC), KTD),
               KTE)) ||
      // A? + im(B) + C? = A - i*B + i*re(B) + C
      PatternMatching::MatchReplaceAndSimplify(
          ref, KAdd(KTA, KIm(KB), KTC),
          KAdd(KTA, KMult(-1_e, i_e, KB), KMult(i_e, KRe(KB)), KTC)) ||
      // A? + re(B) + C? = A + B - i*im(B) + C
      PatternMatching::MatchReplaceAndSimplify(
          ref, KAdd(KTA, KRe(KB), KTC),
          KAdd(KTA, KB, KMult(-1_e, i_e, KIm(KB)), KTC));
}

bool Simplification::ContractAbs(Tree* ref) {
  // A?*|B|*|C|*D? = A*|BC|*D
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KMult(KTA, KAbs(KB), KAbs(KC), KTD),
      KMult(KTA, KAbs(KMult(KB, KC)), KTD));
}

bool Simplification::ExpandAbs(Tree* ref) {
  return
      // |A*B?| = |A|*|B|
      PatternMatching::MatchReplaceAndSimplify(
          ref, KAbs(KMult(KA, KTB)), KMult(KAbs(KA), KAbs(KMult(KTB)))) ||
      // |x| = √(re(x)^2+im(x)^2)
      PatternMatching::MatchReplaceAndSimplify(
          ref, KAbs(KA),
          KExp(
              KMult(KHalf, KLn(KAdd(KPow(KRe(KA), 2_e), KPow(KIm(KA), 2_e))))));
}

bool Simplification::ExpandExp(Tree* ref) {
  // TODO: Also handle exp(i)
  return
      // exp(A?*i*B?) = cos(A*B) + i*sin(A*B)
      PatternMatching::MatchReplaceAndSimplify(
          ref, KExp(KMult(KTA, i_e, KTB)),
          KAdd(KTrig(KMult(KTA, KTB), 0_e),
               KMult(i_e, KTrig(KMult(KTA, KTB), 1_e)))) ||
      // exp(A+B?) = exp(A) * exp(B)
      PatternMatching::MatchReplaceAndSimplify(
          ref, KExp(KAdd(KA, KTB)), KMult(KExp(KA), KExp(KAdd(KTB))));
}

bool Simplification::ContractExpMult(Tree* ref) {
  return
      // A? * exp(B) * exp(C) * D? = A * exp(B+C) * D
      PatternMatching::MatchReplaceAndSimplify(
          ref, KMult(KTA, KExp(KB), KExp(KC), KTD),
          KMult(KTA, KExp(KAdd(KB, KC)), KTD)) ||
      // A? + cos(B) + C? + i*sin(B) + D? = A + C + D + exp(i*B)
      PatternMatching::MatchReplaceAndSimplify(
          ref, KAdd(KTA, KTrig(KB, 0_e), KTC, KMult(i_e, KTrig(KB, 1_e)), KTD),
          KAdd(KTA, KTC, KTD, KExp(KMult(i_e, KB))));
}

bool Simplification::ExpandMult(Tree* ref) {
  // A?*(B+C?)*D? = A*B*D + A*C*D
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KMult(KTA, KAdd(KB, KTC), KTD),
      KAdd(KMult(KTA, KB, KTD), KMult(KTA, KAdd(KTC), KTD)));
}

bool Simplification::ContractMult(Tree* ref) {
  /* TODO: With  N and M positive, contract
   * A + B*A*C + A^N + D*A^M*E into A*(1 + B*C + A^(N-1) + D*A^(M-1)*E) */
  // A? + B?*C*D? + E? + F?*C*G? + H? = A + C*(B*D+F*G) + E + H
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KAdd(KTA, KMult(KTB, KC, KTD), KTE, KMult(KTF, KC, KTG), KTH),
      KAdd(KTA, KMult(KC, KAdd(KMult(KTB, KTD), KMult(KTF, KTG))), KTE, KTH));
}

bool Simplification::ExpandPower(Tree* ref) {
  // (A?*B)^C = A^C * B^C is currently in SystematicSimplification
  // (A? + B)^2 = (A^2 + 2*A*B + B^2)
  // TODO: Implement a more general (A + B)^C expand.
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KPow(KAdd(KTA, KB), 2_e),
      KAdd(KPow(KAdd(KTA), 2_e), KMult(2_e, KAdd(KTA), KB), KPow(KB, 2_e)));
}

// TODO: Properly integrate this within systematic simplification.
bool Simplification::ShallowApplyMatrixOperators(Tree* tree, void* context) {
  if (tree->numberOfChildren() < 1) {
    return false;
  }
  Tree* child = tree->child(0);
  if (tree->isIdentity()) {
    tree->moveTreeOverTree(Matrix::Identity(child));
    return true;
  }
  if (tree->isMultiplication()) {
    int numberOfMatrices = tree->numberOfChildren();
    // Find first matrix
    const Tree* firstMatrix = nullptr;
    for (const Tree* child : tree->children()) {
      if (child->isMatrix()) {
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
  if (!child->isMatrix()) {
    return false;
  }
  if (tree->isAddition()) {
    int n = tree->numberOfChildren() - 1;
    Tree* result = child->clone();
    while (n--) {
      child = child->nextTree();
      result->moveTreeOverTree(Matrix::Addition(result, child));
    }
    tree->moveTreeOverTree(result);
    return true;
  }
  if (tree->isPowerMatrix()) {
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
    if (!child2->isMatrix()) {
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
      assert(child->isMatrix());
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
