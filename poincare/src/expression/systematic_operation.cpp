#include "systematic_operation.h"

#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/probability/distribution_method.h>

#include "infinity.h"
#include "list.h"
#include "rational.h"
#include "systematic_reduction.h"

namespace Poincare::Internal {

bool SystematicOperation::ReducePower(Tree* e) {
  assert(e->isPow());
  // base^n
  Tree* base = e->child(0);
  TreeRef n = base->nextTree();
  if (Infinity::IsPlusOrMinusInfinity(n) &&
      (base->isOne() || base->isMinusOne() ||
       ComplexSign::Get(base).isNonReal())) {
    // (±1)^(±inf) -> undef
    // complex^(±inf) -> undef
    e->cloneTreeOverTree(KUndef);
    return true;
  }
  if (base->isOne()) {
    // 1^x -> 1
    e->cloneTreeOverTree(1_e);
    return true;
  }
  if (Infinity::IsPlusOrMinusInfinity(base)) {
    if ((Infinity::IsMinusInfinity(base) && n->isInf()) || n->isZero()) {
      // (-inf)^inf -> undef
      // (±inf)^0 -> undef
      e->cloneTreeOverTree(KUndef);
      return true;
    }
    ComplexSign nSign = ComplexSign::Get(n);
    if (nSign.isNonReal()) {
      // (±inf)^i -> undef
      e->cloneTreeOverTree(KUndef);
      return true;
    }
    if (n->isInteger() || Infinity::IsPlusOrMinusInfinity(n)) {
      assert(nSign.isReal());
      if (nSign.realSign().isNegative()) {
        // (±inf)^neg -> 0
        e->cloneTreeOverTree(0_e);
        return true;
      }
      if (nSign.realSign().isPositive()) {
        if (base->isInf()) {
          // inf^pos -> inf
          e->cloneTreeOverTree(KInf);
          return true;
        }
        assert(Infinity::IsMinusInfinity(base));
        // (-inf)^pos -> ±inf
        e->moveTreeOverTree(PatternMatching::CreateSimplify(
            KMult(KPow(-1_e, KA), KInf), {.KA = n}));
        return true;
      }
    }
  }
  if (base->isZero()) {
    ComplexSign indexSign = ComplexSign::Get(n);
    if (indexSign.realSign().isStrictlyPositive()) {
      // 0^x is always defined.
      e->cloneTreeOverTree(0_e);
      return true;
    }
    if (!indexSign.realSign().canBeStrictlyPositive()) {
      // 0^x cannot be defined
      e->cloneTreeOverTree(KOutOfDefinition);
      return true;
    }
    // Use a dependency as a fallback.
    return PatternMatching::MatchReplace(e, KA, KDep(0_e, KSet(KA)));
  }
  // After systematic reduction, a power can only have integer index.
  if (!n->isInteger()) {
    // base^n -> exp(n*ln(base))
    return PatternMatching::MatchReplaceSimplify(e, KPow(KA, KB),
                                                 KExp(KMult(KLn(KA), KB)));
  }
  if (base->isRational()) {
    e->moveTreeOverTree(Rational::IntegerPower(base, n));
    return true;
  }
  // base^0 -> 1
  if (n->isZero()) {
    if (ComplexSign::Get(base).canBeNull()) {
      return PatternMatching::MatchReplace(e, KA, KDep(1_e, KSet(KA)));
    }
    e->cloneTreeOverTree(1_e);
    return true;
  }
  // base^1 -> base
  if (n->isOne()) {
    e->moveTreeOverTree(base);
    return true;
  }
  if (base->isComplexI()) {
    // i^n -> ±1 or ±i
    Tree* remainder =
        IntegerHandler::Remainder(Integer::Handler(n), IntegerHandler(4));
    int rem = Integer::Handler(remainder).to<uint8_t>();
    remainder->removeTree();
    e->cloneTreeOverTree(
        rem == 0 ? 1_e
                 : (rem == 1 ? i_e : (rem == 2 ? -1_e : KMult(-1_e, i_e))));
    return true;
  }
  // (w^p)^n -> w^(p*n)
  if (base->isPow()) {
    TreeRef p = base->child(1);
    assert(p->nextTree() == static_cast<Tree*>(n));
    ComplexSign nSign = ComplexSign::Get(n);
    ComplexSign pSign = ComplexSign::Get(p);
    assert(nSign.isReal() && pSign.isReal());
    if (nSign.realSign().canBeStrictlyNegative() &&
        pSign.realSign().canBeStrictlyNegative()) {
      // Add a dependency in case p*n becomes positive (ex: 1/(1/x))
      return PatternMatching::MatchReplaceSimplify(
          e, KPow(KPow(KA, KB), KC),
          KDep(KPow(KA, KMult(KB, KC)), KSet(KPow(KA, KB))));
    }
    return PatternMatching::MatchReplaceSimplify(e, KPow(KPow(KA, KB), KC),
                                                 KPow(KA, KMult(KB, KC)));
  }
  // (w1*...*wk)^n -> w1^n * ... * wk^n
  if (base->isMult()) {
    for (Tree* w : base->children()) {
      TreeRef m = SharedTreeStack->pushPow();
      w->cloneTree();
      n->cloneTree();
      w->moveTreeOverTree(m);
      SystematicReduction::ShallowReduce(m);
    }
    n->removeTree();
    e->removeNode();
    SystematicReduction::ShallowReduce(e);
    return true;
  }
  // exp(a)^b -> exp(a*b)
  return PatternMatching::MatchReplaceSimplify(e, KPow(KExp(KA), KB),
                                               KExp(KMult(KA, KB)));
}

void SystematicOperation::ConvertPowerRealToPower(Tree* e) {
  e->cloneNodeOverNode(KPow);
  SystematicReduction::ShallowReduce(e);
}

bool SystematicOperation::ReducePowerReal(Tree* e) {
  assert(e->isPowReal());
  /* Return :
   * - x^y if x is complex or positive or y is integer
   * - PowerReal(x,y) if y is not a rational
   * - Looking at y's reduced rational form p/q :
   *   * PowerReal(x,y) if x is of unknown sign and p odd
   *   * Nonreal if q is even and x negative
   *   * |x|^y if p is even
   *   * -|x|^y if p is odd
   */
  Tree* x = e->child(0);
  Tree* y = x->nextTree();
  ComplexSign xSign = ComplexSign::Get(x);
  ComplexSign ySign = ComplexSign::Get(y);
  if (Infinity::IsPlusOrMinusInfinity(x) ||
      Infinity::IsPlusOrMinusInfinity(y) || ySign.isInteger() ||
      (xSign.isReal() && xSign.realSign().isPositive())) {
    ConvertPowerRealToPower(e);
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
    e->cloneTreeOverTree(KNonReal);
    return true;
  }

  // We can fallback to |x|^y
  x->cloneNodeAtNode(KAbs);
  SystematicReduction::ShallowReduce(x);
  ConvertPowerRealToPower(e);

  if (xNegative && !pIsEven) {
    // -|x|^y
    e->cloneTreeAtNode(KMult(-1_e));
    NAry::SetNumberOfChildren(e, 2);
    SystematicReduction::ShallowReduce(e);
  }
  return true;
}

bool SystematicOperation::ReduceLnReal(Tree* e) {
  assert(e->isLnReal());
  // Under real mode, input ln(x) must return nonreal if x < 0
  ComplexSign childSign = ComplexSign::Get(e->child(0));
  if (childSign.realSign().isStrictlyNegative() || childSign.isNonReal()) {
    // Child can't be real, positive or null
    e->cloneTreeOverTree(KNonReal);
    return true;
  }
  if (childSign.realSign().canBeStrictlyNegative() ||
      childSign.canBeNonReal()) {
    // Child can be nonreal or negative, add a dependency in case.
    e->moveTreeOverTree(PatternMatching::Create(
        KDep(KLn(KA), KSet(KLnReal(KA))), {.KA = e->child(0)}));
    e = e->child(0);
  } else {
    // Safely fallback to complex logarithm.
    e->cloneNodeOverNode(KLn);
  }
  SystematicReduction::ShallowReduce(e);
  return true;
}

bool SystematicOperation::ReduceComplexArgument(Tree* e) {
  assert(e->isArg());
  const Tree* child = e->child(0);
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
      e->cloneTreeOverTree(KOutOfDefinition);
      return true;
    }
    // atan2(y, 0) = π/2 if y > 0, -π/2 if y < 0
    e->cloneTreeOverTree(imagSign.isStrictlyPositive()
                             ? KMult(1_e / 2_e, π_e)
                             : KMult(-1_e / 2_e, π_e));
  } else if (realSign.isStrictlyPositive() || imagSign.isPositive() ||
             imagSign.isStrictlyNegative()) {
    /* atan2(y, x) = arctan(y/x)      if x > 0
     *               arctan(y/x) + π  if y >= 0 and x < 0
     *               arctan(y/x) - π  if y < 0  and x < 0 */
    e->moveTreeOverTree(PatternMatching::CreateSimplify(
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

bool SystematicOperation::ReduceComplexPart(Tree* e) {
  assert(e->isRe() || e->isIm());
  bool isRe = e->isRe();
  Tree* child = e->child(0);
  ComplexSign childSign = ComplexSign::Get(child);
  if (childSign.isPure()) {
    if (isRe != childSign.isReal()) {
      // re(x) = 0 or im(x) = 0
      e->cloneTreeOverTree(0_e);
    } else if (isRe) {
      // re(x) = x
      e->removeNode();
    } else {
      // im(x) = -i*x
      e->moveTreeOverTree(
          PatternMatching::CreateSimplify(KMult(-1_e, i_e, KA), {.KA = child}));
    }
    return true;
  }
  if (!child->isAdd()) {
    return false;
  }
  // Extract pure real or pure imaginary in addition
  TreeRef a(SharedTreeStack->pushAdd(0));
  const int nbChildren = child->numberOfChildren();
  int nbChildrenRemoved = 0;
  int nbChildrenOut = 0;
  Tree* elem = child->child(0);
  for (int i = 0; i < nbChildren; i++) {
    ComplexSign elemSign = ComplexSign::Get(elem);
    if (elemSign.isPure()) {
      if (isRe != elemSign.isReal()) {
        // re(x) = 0 or im(x) = 0
        elem->removeTree();
      } else if (isRe) {
        // re(x) = x
        elem->detachTree();
        nbChildrenOut++;
      } else {
        // im(x) = -i*x
        TreeRef t(SharedTreeStack->pushMult(3));
        (-1_e)->cloneTree();
        (i_e)->cloneTree();
        elem->detachTree();
        SystematicReduction::ShallowReduce(t);
        nbChildrenOut++;
      }
      nbChildrenRemoved++;
    } else {
      elem = elem->nextTree();
    }
  }
  NAry::SetNumberOfChildren(child, nbChildren - nbChildrenRemoved);
  NAry::SetNumberOfChildren(a, nbChildrenOut);
  if (nbChildrenOut == 0) {
    a->removeTree();
    return nbChildrenRemoved > 0;
  }
  bool includeOriginalTree = true;
  if (child->numberOfChildren() == 0) {
    // Remove re/im(add) e
    e->removeTree();
    includeOriginalTree = false;
  } else if (child->numberOfChildren() == 1) {
    // Shallow reduce to remove the Add node
    SystematicReduction::ShallowReduce(child);
  }
  e->moveTreeBeforeNode(a);
  // Increase the number of children of a to include the original re/im
  NAry::SetNumberOfChildren(e, nbChildrenOut + includeOriginalTree);
  // Shallow reduce new tree
  SystematicReduction::ShallowReduce(e);
  return true;
}

bool SystematicOperation::ReduceSign(Tree* e) {
  assert(e->isSign());
  const Tree* child = e->child(0);
  ComplexSign sign = ComplexSign::Get(child);
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
  } else if (child->isLn() && child->child(0)->isRational()) {
    // sign(ln(x)) = 1 if x > 1, = 0 if x = 1, = -1 if 0 < x < 1
    child = child->child(0);
    assert(Rational::Sign(child).isPositive());  // otherwise !sign.isReal()
    result =
        child->isOne() ? 0_e : (Rational::IsGreaterThanOne(child) ? 1_e : -1_e);
  } else {
    return false;
  }
  e->cloneTreeOverTree(result);
  return true;
}

bool SystematicOperation::ReduceDistribution(Tree* e) {
  const Tree* child = e->child(0);
  const Tree* abscissae[DistributionMethod::k_maxNumberOfParameters];
  DistributionMethod::Type methodType = DistributionMethod::Get(e);
  for (int i = 0; i < DistributionMethod::numberOfParameters(methodType); i++) {
    abscissae[i] = child;
    child = child->nextTree();
  }
  Distribution::Type distributionType = Distribution::Get(e);
  const Tree* parameters[Distribution::k_maxNumberOfParameters];
  for (int i = 0; i < Distribution::numberOfParameters(distributionType); i++) {
    parameters[i] = child;
    child = child->nextTree();
  }
  const DistributionMethod* method = DistributionMethod::Get(methodType);
  const Distribution* distribution = Distribution::Get(distributionType);
  bool parametersAreOk;
  bool couldCheckParameters =
      distribution->expressionParametersAreOK(&parametersAreOk, parameters);
  if (!couldCheckParameters) {
    return false;
  }
  if (!parametersAreOk) {
    e->cloneTreeOverTree(KOutOfDefinition);
    return true;
  }
  return method->shallowReduce(abscissae, distribution, parameters, e);
}

bool SystematicOperation::ReduceDim(Tree* e) {
  Dimension dim = Dimension::Get(e->child(0));
  if (dim.isMatrix()) {
    Tree* result = SharedTreeStack->pushMatrix(1, 2);
    Integer::Push(dim.matrix.rows);
    Integer::Push(dim.matrix.cols);
    e->moveTreeOverTree(result);
    return true;
  }
  return List::ShallowApplyListOperators(e);
}

bool SystematicOperation::ReduceExp(Tree* e) {
  Tree* child = e->child(0);
  if (child->isLn()) {
    /* TODO_PCJ: Add a ln(x) dependency on user-input ln only when x can be
     * null. */
    // exp(ln(x)) -> x
    e->removeNode();
    e->removeNode();
    return true;
  }
  if (child->isInf()) {
    // exp(inf) -> inf
    e->removeNode();
    return true;
  }
  if (Infinity::IsMinusInfinity(child)) {
    // exp(-inf) = 0
    e->cloneTreeOverTree(0_e);
    return true;
  }
  if (child->isZero()) {
    // exp(0) = 1
    e->cloneTreeOverTree(1_e);
    return true;
  }
  PatternMatching::Context ctx;
  if (PatternMatching::Match(e, KExp(KMult(KA, KLn(KB))), &ctx) &&
      (ctx.getTree(KA)->isInteger() || ctx.getTree(KB)->isZero())) {
    /* To ensure there is only one way of representing x^n. Also handle 0^y with
     * Power logic. */
    // exp(n*ln(x)) -> x^n with n an integer or x null.
    e->moveTreeOverTree(PatternMatching::CreateSimplify(KPow(KB, KA), ctx));
    assert(!e->isExp());
    return true;
  }
  return false;
}

bool SystematicOperation::ReduceAbs(Tree* e) {
  assert(e->isAbs());
  Tree* child = e->child(0);
  if (child->isAbs()) {
    // ||x|| -> |x|
    child->removeNode();
    assert(!ReduceAbs(e));
    return true;
  }
  ComplexSign complexSign = ComplexSign::Get(child);
  if (!complexSign.isPure()) {
    return false;
  }
  bool isReal = complexSign.isReal();
  Sign sign = isReal ? complexSign.realSign() : complexSign.imagSign();
  if (sign.canBeStrictlyNegative() && sign.canBeStrictlyPositive()) {
    return false;
  }
  const Tree* minusOne = (isReal == sign.canBeStrictlyNegative()) ? -1_e : 1_e;
  const Tree* complexI = isReal ? 1_e : i_e;
  // |3| = |-3| = |3i| = |-3i| = 3
  e->moveTreeOverTree(PatternMatching::CreateSimplify(
      KMult(KA, KB, KC), {.KA = minusOne, .KB = complexI, .KC = child}));
  return true;
}

/* Approximate all children if one of them is already float. Return true if the
 * entire tree have been approximated. */
bool SystematicOperation::CanApproximateTree(Tree* e, bool* changed) {
  if (e->hasChildSatisfying([](const Tree* e) { return e->isFloat(); }) &&
      Approximation::ApproximateAndReplaceEveryScalar(e)) {
    *changed = true;
    if (e->isFloat()) {
      return true;
    }
  }
  return false;
}

}  // namespace Poincare::Internal
