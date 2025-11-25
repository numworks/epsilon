#include "logarithm.h"

#include <limits.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_helpers.h>

#include "arithmetic.h"
#include "k_tree.h"
#include "rational.h"
#include "sign.h"
#include "systematic_reduction.h"

namespace Poincare::Internal {

bool Logarithm::ReduceLn(Tree* e) {
  Tree* child = e->child(0);
  if (child->isExp()) {
    const Tree* x = child->child(0);
    if (GetComplexProperties(child->child(0)).isReal()) {
      // ln(exp(x)) -> x if x is real
      e->removeNode();
      e->removeNode();
      return true;
    }
    PatternMatching::Context ctx;
    if (PatternMatching::Match(x, KMult(KA, KLn(KB)), &ctx)) {
      const Tree* a = ctx.getTree(KA);
      assert(!a->isOne());
      if (a->isRational() && Rational::CompareAbs(a, 1_e) < 0) {
        /* ln(exp(A*ln(B))) -> A*ln(B) if A is real and -1 < A <= 1
         * This can be proven using B in exponential form, the formula
         * ln(exp(C)) = re(C) + i*arg(exp(i*im(C))), and the fact that
         * arg(exp(i*D))) = D if D real and -π < D <= π
         * This probably shortcuts several advanced reduction steps. */
        e->moveTreeOverTree(PatternMatching::Create(KMult(KA, KLn(KB)), ctx));
        return true;
      }
    }
  }
  if (child->isInf()) {
    // ln(inf) -> inf
    e->removeNode();
    return true;
  }
  if (child->isComplexI()) {
    // ln(i) -> i*π/2
    e->cloneTreeOverTree(KMult(1_e / 2_e, π_e, i_e));
    return true;
  }
  if (child->isMinusOne()) {
    // ln(-1) -> iπ - Necessary so that sqrt(-1)->i
    e->cloneTreeOverTree(KMult(π_e, i_e));
    return true;
  }
  if (child->isOne()) {
    e->cloneTreeOverTree(0_e);
    return true;
  }
  /* Because reduction goes bottom-up, ln(0) is not immediately reduced, to
   * allow exp(x*ln(0)) to be properly reduced */
  if (child->isZero()) {
    return false;
  }
#if 0
  // Seems like a good idea, but doesn't quite work for √(√(-8))
  // This bypass an advanced reduction step
  bool changed = false;
  if (child->isStrictlyNegativeRational()) {
    // ln(-p) -> ln(p) + iπ
    Rational::SetSign(child, NonStrictSign::Positive);
    e->moveTreeOverTree(
        PatternMatching::Create(KAdd(KA, KMult(π_e, i_e)), {.KA = e}));
    // Hack to avoid calling ReduceLn again to reduce the ln(positiveRational)
    e = e->child(0);
    child = e->child(0);
    changed = true;
  }
#endif
  if (child->isStrictlyPositiveRational() &&
      Rational::Numerator(child).isOne()) {
    // ln(1/n) = -ln(n)
    Tree* res = SharedTreeStack->pushMult(2);
    SharedTreeStack->pushMinusOne();
    SharedTreeStack->pushLn();
    Rational::Denominator(child).pushOnTreeStack();
    e->moveTreeOverTree(res);
    return true;
  }
  PatternMatching::Context ctx;
  if (PatternMatching::Match(child, KPow(KA, KB), &ctx) &&
      ctx.getTree(KA)->isPositiveRational() && ctx.getTree(KB)->isInteger()) {
    // ln(a^b) -> b*ln(a) if b is an integer and a is a positive rational
    assert(
        GetComplexProperties(ctx.getTree(KA)).imagSign().isNull() &&
        GetComplexProperties(ctx.getTree(KA)).realSign().isStrictlyPositive());
    e->moveTreeOverTree(PatternMatching::CreateReduce(KMult(KB, KLn(KA)), ctx));
    return true;
  }
  return false;
}

/* Using integers to represent bounds around multiples of π/2.
 *       -2     -1      0      1      2
 *  ------|------|------|------|------|------
 *       -π    -π/2     0     π/2     π
 * For both bounds, we store the integer and a boolean for inclusive/exclusive.
 * For example, ]-π, π/2] is ({-2, false},{1, true}) */
class PiInterval {
 public:
  static PiInterval Add(PiInterval a, PiInterval b) {
    return PiInterval(
        a.m_min + b.m_min, a.m_minIsInclusive && b.m_minIsInclusive,
        a.m_max + b.m_max, a.m_maxIsInclusive && b.m_maxIsInclusive);
  }
  static PiInterval Mult(PiInterval a, int b) {
    return b >= 0 ? PiInterval(a.m_min * b, a.m_minIsInclusive, a.m_max * b,
                               a.m_maxIsInclusive)
                  : PiInterval(a.m_max * b, a.m_maxIsInclusive, a.m_min * b,
                               a.m_minIsInclusive);
  }
  static PiInterval Arg(ComplexProperties properties) {
    PiInterval result;
    bool realCanBeNegative = properties.realSign().canBeStrictlyNegative();
    bool realCanBeNull = properties.realSign().canBeNull();
    bool realCanBePositive = properties.realSign().canBeStrictlyPositive();
    if (properties.imagSign().canBeStrictlyNegative()) {
      if (realCanBeNegative) {
        result.unionWith(PiInterval(-2, false, -1, false));
      }
      if (realCanBeNull) {
        result.unionWith(PiInterval(-1, true, -1, true));
      }
      if (realCanBePositive) {
        result.unionWith(PiInterval(-1, false, 0, false));
      }
    }
    if (properties.imagSign().canBeNull()) {
      if (realCanBeNegative) {
        result.unionWith(PiInterval(2, true, 2, true));
      }
      if (realCanBeNull) {
        // Ignore this case
      }
      if (realCanBePositive) {
        result.unionWith(PiInterval(0, true, 0, true));
      }
    }
    if (properties.imagSign().canBeStrictlyPositive()) {
      if (realCanBeNegative) {
        result.unionWith(PiInterval(1, false, 2, false));
      }
      if (realCanBeNull) {
        result.unionWith(PiInterval(1, true, 1, true));
      }
      if (realCanBePositive) {
        result.unionWith(PiInterval(0, false, 1, false));
      }
    }
    return result;
  }
  // Return k such that max bound is in ]-π + 2kπ, π + 2kπ]
  int maxK() const {
    // m_maxIsInclusive doesn't matter.
    return DivideRoundDown(m_max + 1, 4);
  }
  // Return k such that min bound is in ]-π + 2kπ, π + 2kπ]
  int minK() const {
    // ]-π, ...] {-2, false} is 0 and [-π, ...] {-2, true} is -1
    return DivideRoundDown(m_min + !m_minIsInclusive + 1, 4);
  }

 private:
  // We want DivideRoundDown(-1, 4) to be -1
  inline static int DivideRoundDown(int num, int den) {
    int result = num / den;
    if (num < 0 && -num % den != 0) {
      // -1/4 is 0 but -4/4 is -1, we expect -1 for both.
      result -= 1;
    }
    return result;
  }
  PiInterval() : PiInterval(INT_MAX, true, INT_MIN, true) {}
  PiInterval(int min, bool minIsInclusive, int max, bool maxIsInclusive)
      : m_min(min),
        m_minIsInclusive(minIsInclusive),
        m_max(max),
        m_maxIsInclusive(maxIsInclusive) {}
  void unionWith(PiInterval other) {
    if (m_min > other.m_min) {
      m_min = other.m_min;
      m_minIsInclusive = other.m_minIsInclusive;
    } else if (m_min == other.m_min && other.m_minIsInclusive) {
      m_minIsInclusive = true;
    }
    if (m_max < other.m_max) {
      m_max = other.m_max;
      m_maxIsInclusive = other.m_maxIsInclusive;
    } else if (m_max == other.m_max && other.m_maxIsInclusive) {
      m_maxIsInclusive = true;
    }
  }
  int m_min;
  bool m_minIsInclusive;
  int m_max;
  bool m_maxIsInclusive;
};

// If possible, find k such that arg(A) + arg(B) + ... = arg(A*B*...) + 2iπk
bool CanGetArgSumModulo(const Tree* firstTree, int numberOfTrees, int* k) {
  assert(numberOfTrees > 0);
  // Find an interval for the sum of the arguments
  PiInterval interval = PiInterval::Arg(GetComplexProperties(1_e));
  const Tree* tree = firstTree;
  for (int i = 0; i < numberOfTrees; i++) {
    assert(!tree->isZero());
    interval =
        PiInterval::Add(interval, PiInterval::Arg(GetComplexProperties(tree)));
    tree = tree->nextTree();
  }
  assert(interval.maxK() <= numberOfTrees / 2 &&
         interval.minK() >= -numberOfTrees / 2);
  *k = interval.maxK();
  return *k == interval.minK();
}

// If possible, find k such that B*arg(A) = arg(A^B) + 2iπk
bool CanGetArgProdModulo(const Tree* a, const Tree* b, int* k) {
  assert(b->isInteger() && !a->isZero() && !b->isOne());
  if (!Integer::Handler(b).is<int>()) {
    return false;
  }
  int bValue = Integer::Handler(b).to<int>();
  PiInterval interval =
      PiInterval::Mult(PiInterval::Arg(GetComplexProperties(a)), bValue);
  *k = interval.maxK();
  return *k == interval.minK();
}

Tree* PushIK2Pi(int k) {
  // Push i*k*2π
  Tree* result = (KMult.node<4>)->cloneNode();
  (i_e)->cloneTree();
  (2_e)->cloneTree();
  Integer::Push(k);
  (π_e)->cloneTree();
  SystematicReduction::DeepReduce(result);
  return result;
}

Tree* PushProductCorrection(const Tree* a, const Tree* b) {
  // B*ln(A) - ln(A^B) = k*2π*i = (B*arg(A) - arg(A^B))*i
  int k;
  if (CanGetArgProdModulo(a, b, &k)) {
    return PushIK2Pi(k);
  }
  // Push B*arg(A) - arg(A^B)
  return PatternMatching::CreateReduce(
      KMult(KAdd(KMult(KB, KArg(KA)), KMult(-1_e, KArg(KPow(KA, KB)))), i_e),
      {.KA = a, .KB = b});
}

Tree* PushAdditionCorrection(const Tree* firstTree, int numberOfTrees) {
  /* Push correction by computing either:
   * - if possible, the π-interval for arg(A) + arg(B) + ...
   *   such that ln(A) + ln(B) + ... - ln(A*B*...) = k*2π*i
   * - otherwise, (arg(A) + arg(B) + ... - arg(A*B*...))*i */
  int k;
  if (CanGetArgSumModulo(firstTree, numberOfTrees, &k)) {
    return PushIK2Pi(k);
  }

  // Create arg(A) + arg(B) + ...
  TreeRef argSum = KAdd.node<0>->cloneNode();
  const Tree* tree = firstTree;
  for (int i = 0; i < numberOfTrees; i++) {
    Tree* arg = KArg->cloneNode();
    tree->cloneTree();
    SystematicReduction::ShallowReduce(arg);
    NAry::AddChild(argSum, arg);
    tree = tree->nextTree();
  }
  SystematicReduction::ShallowReduce(argSum);

  // Create arg(A*B*...)
  TreeRef multArg = KMult.node<0>->cloneNode();
  tree = firstTree;
  for (int i = 0; i < numberOfTrees; i++) {
    Tree* child = tree->cloneTree();
    NAry::AddChild(multArg, child);
    tree = tree->nextTree();
  }
  SystematicReduction::ShallowReduce(multArg);
  multArg->cloneNodeAtNode(KArg);
  SystematicReduction::ShallowReduce(multArg);

  // Create correction as (sum(arg) - arg(prod))*i
  TreeRef correction = PatternMatching::CreateReduce(
      KMult(KAdd(KA, KMult(-1_e, KB)), i_e), {.KA = argSum, .KB = multArg});
  argSum->removeTree();
  multArg->removeTree();
  return correction;
}

bool Logarithm::ContractLn(Tree* e) {
  PatternMatching::Context ctx;
  // (A/D)*ln(B) = (ln(B^A) + i*(A*arg(B) - arg(B^A)))/D with A, D integers.
  if (PatternMatching::Match(e, KMult(KA, KLn(KB)), &ctx) &&
      ctx.getTree(KA)->isRational() &&
      !Rational::Numerator(ctx.getTree(KA)).isOne() &&
      !Rational::Numerator(ctx.getTree(KA)).isMinusOne() &&
      !ctx.getTree(KB)->isZero()) {
    TreeRef a = Rational::Numerator(ctx.getTree(KA)).pushOnTreeStack();
    const Tree* b = ctx.getTree(KB);
    TreeRef c = PushProductCorrection(b, a);
    TreeRef d = Rational::Denominator(ctx.getTree(KA)).pushOnTreeStack();
    e->moveTreeOverTree(PatternMatching::CreateReduce(
        KMult(KPow(KD, -1_e), KAdd(KLn(KPow(KB, KA)), KC)),
        {.KA = a, .KB = b, .KC = c, .KD = d}));
    d->removeTree();
    c->removeTree();
    a->removeTree();
    return true;
  }

  /* ... + ln(A) + ... + ln(B) + ... - ln(C) + ... =
   * ... + ln(AB/C) + i*(arg(A) + arg(B) + arg(1/C) - arg(AB/C)) */
  ctx = PatternMatching::Context();
  if (e->isAdd() &&
      (PatternMatching::Match(e, KAdd(KA_s, KLn(KB), KC_s), &ctx) ||
       PatternMatching::Match(e, KAdd(KA_s, KMult(-1_e, KLn(KB)), KC_s),
                              &ctx))) {
    // Clone e to work at the end of the treestack
    TreeRef clone = e->cloneTree();
    int numberOfCloneChildren = clone->numberOfChildren();
    TreeRef lnChildren = KMult.node<0>->cloneNode();
    int numberOfLnChildren = 0;

    // Identify ln and -ln children
    TreeRef end = pushEndMarker(clone);
    assert(clone->numberOfChildren() > 0);
    Tree* child = clone->child(0);
    while (child != end) {
      if (child->isLn()) {
        // Extract A from ln(A)
        child->child(0)->detachTree();
        child->removeNode();
        NAry::SetNumberOfChildren(lnChildren, ++numberOfLnChildren);
        NAry::SetNumberOfChildren(clone, --numberOfCloneChildren);
      } else if (PatternMatching::MatchReplaceReduce(
                     child, KMult(-1_e, KLn(KA)), KPow(KA, -1_e))) {
        // Extract 1/A from -ln(A)
        child->detachTree();
        NAry::SetNumberOfChildren(lnChildren, ++numberOfLnChildren);
        NAry::SetNumberOfChildren(clone, --numberOfCloneChildren);
      } else {
        child = child->nextTree();
      }
    }
    removeMarker(end);

    assert(numberOfLnChildren > 0);
    if (numberOfLnChildren == 1) {
      // No possible contraction
      lnChildren->removeTree();
      clone->removeTree();
      return false;
    }

    PushAdditionCorrection(lnChildren->child(0),
                           lnChildren->numberOfChildren());
    SystematicReduction::ShallowReduce(lnChildren);
    lnChildren->cloneNodeAtNode(KLn);
    SystematicReduction::ShallowReduce(lnChildren);
    // Add ln product and correction
    NAry::SetNumberOfChildren(clone, numberOfCloneChildren + 2);
    SystematicReduction::ShallowReduce(clone);
    e->moveTreeOverTree(clone);
    return true;
  }
  return false;
}

bool Logarithm::ExpandLn(Tree* e) {
  if (!e->isLn()) {
    return false;
  }
  // ln(12/7) = 2*ln(2) + ln(3) - ln(7)
  if (ExpandLnOnRational(e)) {
    return true;
  }
  PatternMatching::Context ctx;
  // ln(A*B?) = ln(A) + ln(B) - i*(arg(A) + arg(B) - arg(AB))
  if (PatternMatching::Match(e, KLn(KMult(KA, KB_p)), &ctx)) {
    // Since KB_p can match multiple trees, we need them as a single tree.
    TreeRef a = ctx.getTree(KA)->cloneTree();
    TreeRef b = PatternMatching::CreateReduce(KMult(KB_p), ctx);
    TreeRef c = PushAdditionCorrection(a, 2);
    e->moveTreeOverTree(PatternMatching::CreateReduce(
        KAdd(KLn(KA), KLn(KB), KMult(-1_e, KC)), {.KA = a, .KB = b, .KC = c}));
    c->removeTree();
    b->removeTree();
    a->removeTree();
    return true;
  }
  // ln(A^B) = B*ln(A) - i*(B*arg(A) - arg(A^B))
  if (PatternMatching::Match(e, KLn(KPow(KA, KB)), &ctx)) {
    const Tree* a = ctx.getTree(KA);
    const Tree* b = ctx.getTree(KB);
    TreeRef c = PushProductCorrection(a, b);
    ctx.setNode(KC, c, 1, false);
    e->moveTreeOverTree(PatternMatching::CreateReduce(
        KAdd(KMult(KB, KLn(KA)), KMult(-1_e, KC)), ctx));
    c->removeTree();
    return true;
  }
  /* ln(exp(A))-> ln(exp(re(A)))+ln(exp(i*im(A)) -> re(A) + i*arg(exp(i*im(A)))
   * This essentially brings back im(A) within ]-π,π] */
  return PatternMatching::MatchReplaceReduce(
      e, KLn(KExp(KA)),
      KAdd(KRe(KA), KMult(i_e, KArg(KExp(KMult(i_e, KIm(KA)))))));
}

bool Logarithm::ExpandLnOnRational(Tree* e) {
  if (!e->isLn() || !e->child(0)->isRational()) {
    return false;
  }
  const Tree* child = e->child(0);
  Tree* denominator =
      child->isInteger()
          ? nullptr
          : ExpandLnOnInteger(Rational::Denominator(child), false);
  if (!denominator && !child->isInteger()) {
    // factorization of denominator failed.
    return false;
  }
  Tree* numerator =
      ExpandLnOnInteger(Rational::Numerator(child), denominator == nullptr);
  if (!numerator) {
    if (denominator) {
      // factorization of numerator failed.
      denominator->removeTree();
    }
    // ln(13) -> ln(13)
    return false;
  }
  Tree* result;
  if (denominator) {
    // ln(13/11) -> ln(13)-ln(11)
    PatternMatching::CreateReduce(KAdd(KA, KMult(-1_e, KB)),
                                  {.KA = numerator, .KB = denominator});
    numerator->removeTree();
    denominator->removeTree();
    // denominator is now KAdd(KA, KMult(-1_e, KB)
    result = denominator;
  } else {
    // ln(12) -> 2ln(2)+ln(3)
    result = numerator;
  }
  e->moveTreeOverTree(result);
  return true;
}

Tree* Logarithm::ExpandLnOnInteger(IntegerHandler m, bool escapeIfPrime) {
  bool isNegative = m.strictSign() == StrictSign::Negative;
  m.setSign(NonStrictSign::Positive);
  Arithmetic::FactorizedInteger factorization =
      Arithmetic::PrimeFactorization(m);
  if (factorization.numberOfFactors ==
      Arithmetic::FactorizedInteger::k_factorizationFailed) {
    return nullptr;
  }
  if (!isNegative && escapeIfPrime &&
      (factorization.numberOfFactors == 0 ||
       (factorization.numberOfFactors == 1 &&
        factorization.coefficients[0] == 1))) {
    return nullptr;
  }
  Tree* result = KAdd.node<0>->cloneNode();
  for (int i = 0; i < factorization.numberOfFactors; i++) {
    if (factorization.coefficients[i] > 1) {
      KMult.node<2>->cloneNode();
      Integer::Push(factorization.coefficients[i]);
    }
    KLn->cloneNode();
    Integer::Push(factorization.factors[i]);
  }
  if (isNegative) {
    // ln(-1) = iπ using the principal complex logarithm.
    KMult(π_e, i_e)->cloneTree();
  }
  NAry::SetNumberOfChildren(result, factorization.numberOfFactors + isNegative);
  NAry::SquashIfPossible(result);
  assert(!SystematicReduction::DeepReduce(result));
  return result;
}

}  // namespace Poincare::Internal
