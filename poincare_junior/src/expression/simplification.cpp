#include "simplification.h"

#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/n_ary.h>
#include <poincare_junior/src/probability/distribution_method.h>

#include "advanced_simplification.h"
#include "approximation.h"
#include "beautification.h"
#include "binary.h"
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
  bool modified = (u->isMult() || u->isAdd()) && NAry::Flatten(u);
  for (Tree* child : u->children()) {
    modified |= DeepSystematicReduce(child);
    assert(!child->isUndef());
    if (u->isDependency()) {
      // Skip systematic simplification of Dependencies.
      break;
    }
  }
#if ASSERTIONS
  TreeRef previousTree = u->clone();
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
  if (u->hasChildSatisfying([](const Tree* e) { return e->isFloat(); }) &&
      Approximation::ApproximateAndReplaceEveryScalar(u)) {
    *changed = true;
    if (u->isFloat()) {
      return true;
    }
  }
  return false;
}

bool Simplification::ShallowSystematicReduce(Tree* u) {
  // This assert is quite costly, should be an assert level 2 ?
  assert(Dimension::DeepCheckDimensions(u));
  if (!u->isNAry() && u->numberOfChildren() == 0) {
    // No childless tree have a reduction pattern.
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
    case Type::Abs:
      return SimplifyAbs(u);
    case Type::Add:
      return SimplifyAddition(u);
    case Type::ATanRad:
      return Trigonometry::SimplifyArcTangentRad(u);
    case Type::ATrig:
      return Trigonometry::SimplifyATrig(u);
    case Type::Binomial:
      return Arithmetic::SimplifyBinomial(u);
    case Type::Arg:
      return SimplifyComplexArgument(u);
    case Type::Diff:
    case Type::NthDiff:
      return Derivation::ShallowSimplify(u);
    case Type::Dim:
      return SimplifyDim(u);
    case Type::Distribution:
      return SimplifyDistribution(u);
    case Type::Exp:
      return SimplifyExp(u);
    case Type::Fact:
      return Arithmetic::SimplifyFactorial(u);
    case Type::Floor:
      return Arithmetic::SimplifyFloor(u);
    case Type::GCD:
      return Arithmetic::SimplifyGCD(u);
    case Type::Im:
    case Type::Re:
      return SimplifyComplexPart(u);
    case Type::LCM:
      return Arithmetic::SimplifyLCM(u);
    case Type::ListSort:
    case Type::Median:
      return List::ShallowApplyListOperators(u);
    case Type::Ln:
      return Logarithm::SimplifyLn(u);
    case Type::LnReal:
      return SimplifyLnReal(u);
    case Type::Mult:
      return SimplifyMultiplication(u);
    case Type::Permute:
      return Arithmetic::SimplifyPermute(u);
    case Type::Piecewise:
      return Binary::SimplifyPiecewise(u);
    case Type::Pow:
      return SimplifyPower(u);
    case Type::PowReal:
      return SimplifyPowerReal(u);
    case Type::Quo:
    case Type::Rem:
      return Arithmetic::SimplifyQuotientOrRemainder(u);
    case Type::Round:
      return Arithmetic::SimplifyRound(u);
    case Type::Sign:
      return SimplifySign(u);
    case Type::Sum:
    case Type::Product:
      return Parametric::SimplifySumOrProduct(u);
    case Type::Trig:
      return Trigonometry::SimplifyTrig(u);
    case Type::TrigDiff:
      return Trigonometry::SimplifyTrigDiff(u);
    default:
      if (u->type().isListToScalar()) {
        return List::ShallowApplyListOperators(u);
      }
      if (u->type().isLogicalOperator()) {
        return Binary::SimplifyBooleanOperator(u);
      }
      if (u->type().isComparison()) {
        return Binary::SimplifyComparison(u);
      }
      if (u->isAMatrixOrContainsMatricesAsChildren()) {
        return Matrix::SimplifySwitch(u);
      }
      return false;
  }
}

bool Simplification::SimplifyDim(Tree* u) {
  Dimension dim = Dimension::GetDimension(u->child(0));
  if (dim.isMatrix()) {
    Tree* result = SharedTreeStack->push<Type::Matrix>(1, 2);
    Integer::Push(dim.matrix.rows);
    Integer::Push(dim.matrix.cols);
    u->moveTreeOverTree(result);
    return true;
  }
  return List::ShallowApplyListOperators(u);
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
    u->moveTreeOverTree(PatternMatching::CreateSimplify(KPow(KB, KA), ctx));
    assert(!u->isExp());
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
  u->moveTreeOverTree(PatternMatching::CreateSimplify(
      KMult(KA, KB, KC), {.KA = minusOne, .KB = complexI, .KC = child}));
  return true;
}

bool Simplification::SimplifyPower(Tree* u) {
  assert(u->isPow());
  Tree* v = u->child(0);
  // 1^x -> 1
  if (v->isOne()) {
    u->cloneTreeOverTree(1_e);
    return true;
  }
  // v^n
  TreeRef n = v->nextTree();
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
    return PatternMatching::MatchReplace(u, KA, KDep(0_e, KSet(KA)));
  }
  // After systematic reduction, a power can only have integer index.
  if (!n->isInteger()) {
    // v^n -> exp(n*ln(v))
    return PatternMatching::MatchReplaceSimplify(u, KPow(KA, KB),
                                                 KExp(KMult(KLn(KA), KB)));
  }
  if (v->isRational()) {
    u->moveTreeOverTree(Rational::IntegerPower(v, n));
    return true;
  }
  // v^0 -> 1
  if (n->isZero()) {
    if (ComplexSign::Get(v).canBeNull()) {
      return PatternMatching::MatchReplace(u, KA, KDep(1_e, KSet(KA)));
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
  if (v->isPow()) {
    TreeRef p = v->child(1);
    assert(p->nextTree() == static_cast<Tree*>(n));
    // PowU PowV w p n
    v->removeNode();
    MoveNodeAtNode(p, SharedTreeStack->push<Type::Mult>(2));
    // PowU w Mult<2> p n
    SimplifyMultiplication(p);
    SimplifyPower(u);
    return true;
  }
  // (w1*...*wk)^n -> w1^n * ... * wk^n
  if (v->isMult()) {
    for (Tree* w : v->children()) {
      TreeRef m = SharedTreeStack->push(Type::Pow);
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
  return PatternMatching::MatchReplaceSimplify(u, KPow(KExp(KA), KB),
                                               KExp(KMult(KA, KB)));
}

const Tree* Base(const Tree* u) { return u->isPow() ? u->child(0) : u; }

const Tree* Exponent(const Tree* u) { return u->isPow() ? u->child(1) : 1_e; }

void Simplification::ConvertPowerRealToPower(Tree* u) {
  u->cloneNodeOverNode(KPow);
  SimplifyPower(u);
}

bool Simplification::SimplifyPowerReal(Tree* u) {
  assert(u->isPowReal());
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
      !((child->isMathematicalConstant()) || next->isMathematicalConstant())) {
    // Merge numbers
    merge = Number::Multiplication(child, next);
  } else if (Base(child)->treeIsIdenticalTo(Base(next))) {
    // t^m * t^n -> t^(m+n)
    merge = PatternMatching::CreateSimplify(
        KPow(KA, KAdd(KB, KC)),
        {.KA = Base(child), .KB = Exponent(child), .KC = Exponent(next)});
    assert(!merge->isMult());
  } else if (next->isMatrix()) {
    // TODO: Maybe this should go in advanced reduction.
    merge =
        (child->isMatrix() ? Matrix::Multiplication
                           : Matrix::ScalarMultiplication)(child, next, false);
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
    // 0 * {1, 2, 4} -> {0, 0, 0}. Same for matrices.
    Tree* zeroTree;
    Dimension dim = Dimension::GetDimension(multiplication);
    if (dim.isMatrix()) {
      zeroTree = Matrix::Zero(dim.matrix);
    } else {
      int length = Dimension::GetListLength(multiplication);
      if (length >= 0) {
        // Push ListSequence of 0s instead of a list to delay its expansion.
        zeroTree = Integer::Push(length);
        zeroTree->moveTreeOverTree(PatternMatching::Create(
            KListSequence(KVarK, KA, 0_e), {.KA = zeroTree}));
      } else {
        zeroTree = (0_e)->clone();
      }
    }
    multiplication->moveTreeOverTree(zeroTree);
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
  assert(u->isMult());
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
  assert(!changed || !u->isMult() || !SimplifyMultiplication(u));
  return changed;
}

bool TermsAreEqual(const Tree* u, const Tree* v) {
  if (!u->isMult()) {
    if (!v->isMult()) {
      return u->treeIsIdenticalTo(v);
    }
    return TermsAreEqual(v, u);
  }
  if (!v->isMult()) {
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
  if (u->isMult() && u->child(0)->isRational()) {
    NAry::RemoveChildAtIndex(c, 0);
    NAry::SquashIfPossible(c);
  }
  return c;
}

// The constant of 2ab is 2
const Tree* Constant(const Tree* u) {
  if (u->isMult() && u->child(0)->isRational()) {
    return u->child(0);
  }
  return 1_e;
}

bool Simplification::MergeAdditionChildWithNext(Tree* child, Tree* next) {
  assert(next == child->nextTree());
  Tree* merge = nullptr;
  if (child->isNumber() && next->isNumber() &&
      !((child->isMathematicalConstant()) || next->isMathematicalConstant())) {
    // Merge numbers
    merge = Number::Addition(child, next);
  } else if (TermsAreEqual(child, next)) {
    // k1 * a + k2 * a -> (k1+k2) * a
    Tree* term = PushTerm(child);
    merge = PatternMatching::CreateSimplify(
        KMult(KAdd(KA, KB), KC),
        {.KA = Constant(child), .KB = Constant(next), .KC = term});
    term->removeTree();
    merge = term;
  } else if (child->isMatrix() && next->isMatrix()) {
    merge = Matrix::Addition(child, next);
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
  assert(u->isAdd());
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
      if (child->isAdd()) {
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
  assert(tree->isArg());
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
    tree->moveTreeOverTree(PatternMatching::CreateSimplify(
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
  assert(tree->isRe() || tree->isIm());
  Tree* child = tree->child(0);
  ComplexSign childSign = ComplexSign::Get(child);
  if (!childSign.isPure()) {
    // Rely on advanced reduction re(x+iy) -> re(x) + re(iy)
    return false;
  }
  if (tree->isRe() != childSign.isReal()) {
    // re(x) = 0 or im(x) = 0
    tree->cloneTreeOverTree(0_e);
  } else if (tree->isRe()) {
    // re(x) = x
    tree->removeNode();
  } else {
    // im(x) = -i*x
    tree->moveTreeOverTree(
        PatternMatching::CreateSimplify(KMult(-1_e, i_e, KA), {.KA = child}));
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

bool Simplification::SimplifyDistribution(Tree* expr) {
  const Tree* child = expr->child(0);
  const Tree* abscissae[DistributionMethod::k_maxNumberOfParameters];
  DistributionMethod::Type methodType = DistributionMethod::Get(expr);
  for (int i = 0; i < DistributionMethod::numberOfParameters(methodType); i++) {
    abscissae[i] = child;
    child = child->nextTree();
  }
  Distribution::Type distributionType = Distribution::Get(expr);
  const Tree* parameters[Distribution::k_maxNumberOfParameters];
  for (int i = 0; i < Distribution::numberOfParameters(distributionType); i++) {
    parameters[i] = child;
    child = child->nextTree();
  }
  const DistributionMethod* method = DistributionMethod::Get(methodType);
  const Distribution* distribution = Distribution::Get(distributionType);
  bool parametersAreOk;
  bool couldCheckParameters = distribution->expressionParametersAreOK(
      &parametersAreOk, parameters, nullptr);
  if (!couldCheckParameters) {
    return false;
  }
  if (!parametersAreOk) {
    expr->cloneTreeOverTree(KUndef);
    return true;
  }
  return method->shallowReduce(abscissae, distribution, parameters, expr);
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
  projectionContext->m_strategy = Strategy::ApproximateToFloat;
  return true;
}

bool Simplification::Simplify(Tree* e, ProjectionContext* projectionContext) {
  if (e->isStore()) {
    // Store is an expression only for convenience
    return Simplify(e->child(0), projectionContext);
  }
  if (e->isUnitConversion()) {
    if (!Dimension::DeepCheckDimensions(e)) {
      // TODO: Raise appropriate exception in DeepCheckDimensions.
      ExceptionCheckpoint::Raise(ExceptionType::UnhandledDimension);
    }
    Simplify(e->child(0), projectionContext);
    e->moveTreeOverTree(e->child(0));
    // TODO PCJ actually select the required unit
    return e;
  }
  assert(projectionContext);
  // Clone the tree, and use an adaptive strategy to handle pool overflow.
  SharedTreeStack->executeAndReplaceTree(
      [](void* context, const void* data) {
        SimplifyLastTree(static_cast<const Tree*>(data)->clone(),
                         *static_cast<ProjectionContext*>(context));
      },
      projectionContext, e, RelaxProjectionContext);
  /* TODO: Due to projection/beautification cycles, SimplifyLastTree will most
   *       likely return true every time anyway. */
  return true;
}

/* TODO: The operation order could be rearranged :
 * - UserNamed nodes could be replaced from the start (to avoid simplifying
 *   twice). To properly account for random seeding, UserFunctions could be
 *   replaced before with a parametric structure.
 * - GetUserSymbols and ProjectToId steps could be factorized.
 * - Steps could be better grouped under well constructed steps. */
bool Simplification::SimplifyLastTree(Tree* e,
                                      ProjectionContext projectionContext) {
  assert(SharedTreeStack->lastBlock() == e->nextTree()->block());
  ExceptionTryAfterBlock(e->block()) {
    bool changed = false;
    // Seeded random nodes may remain between Successive iterations.
    int maxRandomSeed = 0;
    do {
      if (!Dimension::DeepCheckDimensions(e) ||
          !Dimension::DeepCheckListLength(e)) {
        // TODO: Raise appropriate exception in DeepCheckDimensions.
        ExceptionCheckpoint::Raise(ExceptionType::UnhandledDimension);
      }
      projectionContext.m_dimension = Dimension::GetDimension(e);
      if (projectionContext.m_strategy != Strategy::ApproximateToFloat &&
          ShouldApproximateOnSimplify(projectionContext.m_dimension)) {
        ExceptionCheckpoint::Raise(ExceptionType::RelaxContext);
      }
      // Seed random nodes before anything is merged/duplicated.
      maxRandomSeed = Random::SeedPoolObjects(e, maxRandomSeed);
      changed = maxRandomSeed > 0;
      changed = Projection::DeepSystemProject(e, projectionContext) || changed;
      /* TODO: GetUserSymbols and ProjectToId could be factorized. We split them
       * because of the ordered structure of the set. When projecting y+x,
       * variables will be {x, y} and we must have found all user symbols to
       * properly project y to 1. */
      Tree* variables = Variables::GetUserSymbols(e);
      SwapTreesPointers(&e, &variables);
      Variables::ProjectToId(
          e, variables,
          projectionContext.m_complexFormat == ComplexFormat::Real
              ? ComplexSign::RealUnknown()
              : ComplexSign::Unknown());
      changed = DeepSystematicReduce(e) || changed;
      assert(!DeepSystematicReduce(e));
      changed = List::BubbleUp(e, ShallowSystematicReduce) || changed;
      changed = AdvancedSimplification::AdvancedReduce(e) || changed;
      changed = Dependency::DeepRemoveUselessDependencies(e) || changed;

      if (projectionContext.m_strategy == Strategy::ApproximateToFloat) {
        /* Approximate again in case exact numbers appeared during
         * simplification. */
        changed = Approximation::ApproximateAndReplaceEveryScalar(e);
      }
      changed = Beautification::DeepBeautify(e, projectionContext) || changed;
      Variables::BeautifyToName(e, variables);
      variables->removeTree();
      e = variables;
      if (!Projection::DeepReplaceUserNamed(e, projectionContext)) {
        return changed;
      }
      changed = true;
      /* If functions and variables can be replaced with their definitions,
       * iterate again to simplify the expression further. */
      assert(projectionContext.m_symbolic !=
             SymbolicComputation::DoNotReplaceAnySymbol);
      projectionContext.m_symbolic = SymbolicComputation::DoNotReplaceAnySymbol;
    } while (true);
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
        /* TODO PCJ: We need to catch undefs when reducing children of lists and
         * points since (undef,0) and {undef,0} should be allowed. */
        (type == ExceptionType::Nonreal ? KNonReal : KUndef)->clone();
        return true;
      default:
        ExceptionCheckpoint::Raise(type);
    }
  }
  // Silence warning
  return false;
}

}  // namespace PoincareJ
