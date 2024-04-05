#include "logarithm.h"

#include <limits.h>
#include <poincare_junior/src/expression/arithmetic.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/number.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

bool Logarithm::SimplifyLn(Tree* u) {
  Tree* child = u->nextNode();
  if (child->isExponential()) {
    // ln(exp(x)) -> x
    u->removeNode();
    u->removeNode();
    return true;
  }
  if (!child->isInteger()) {
    return false;
  }
  if (child->isMinusOne()) {
    // ln(-1) -> iπ - Necessary so that sqrt(-1)->i
    u->cloneTreeOverTree(KMult(π_e, i_e));
    return true;
  } else if (child->isOne()) {
    u->cloneTreeOverTree(0_e);
    return true;
  }
  // TODO_PCJ: Raise Unhandled if child is zero and ln is user-inputted.
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
  static PiInterval Arg(ComplexSign sign) {
    PiInterval result;
    bool realCanBeNegative = sign.realSign().canBeNegative();
    bool realCanBeNull = sign.realSign().canBeNull();
    bool realCanBePositive = sign.realSign().canBePositive();
    if (sign.imagSign().canBeNegative()) {
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
    if (sign.imagSign().canBeNull()) {
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
    if (sign.imagSign().canBePositive()) {
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

// If possible, find k such that arg(A) + arg(B) = arg(AB) + 2iπk
bool CanGetArgSumModulo(const Tree* a, const Tree* b, int* k) {
  // a and b are not always known, find an interval for the sum of their arg.
  assert(!a->isZero() && !b->isZero());
  PiInterval interval = PiInterval::Add(PiInterval::Arg(ComplexSign::Get(a)),
                                        PiInterval::Arg(ComplexSign::Get(b)));
  assert(interval.maxK() <= 1 && interval.minK() >= -1);
  *k = interval.maxK();
  return *k == interval.minK();
}

// If possible, find k such that B*arg(A) = arg(A^B) + 2iπk
bool CanGetArgProdModulo(const Tree* a, const Tree* b, int* k) {
  assert(b->isInteger() && !a->isZero() && !b->isOne());
  int bValue = Integer::Handler(b).to<int>();
  PiInterval interval =
      PiInterval::Mult(PiInterval::Arg(ComplexSign::Get(a)), bValue);
  *k = interval.maxK();
  return *k == interval.minK();
}

Tree* PushIK2Pi(int k) {
  // Push i*k*2π
  Tree* result = (KMult.node<4>)->cloneNode();
  (i_e)->clone();
  (2_e)->clone();
  Integer::Push(k);
  (π_e)->clone();
  Simplification::DeepSystematicReduce(result);
  return result;
}

Tree* PushProductCorrection(const Tree* a, const Tree* b) {
  // B*ln(A) - ln(A^B) = i*k*2π = i*(B*arg(A) - arg(A^B))
  int k;
  if (CanGetArgProdModulo(a, b, &k)) {
    return PushIK2Pi(k);
  }
  // Push B*arg(A) - arg(A^B)
  return PatternMatching::CreateAndSimplify(
      KMult(i_e, KAdd(KMult(KB, KArg(KA)), KMult(-1_e, KArg(KPow(KA, KB))))),
      {.KA = a, .KB = b});
}

Tree* PushAdditionCorrection(const Tree* a, const Tree* b) {
  // ln(A) + ln(B) - ln(A*B) = i*k*2π = i*(arg(A) + arg(B) - arg(A*B))
  int k;
  if (CanGetArgSumModulo(a, b, &k)) {
    return PushIK2Pi(k);
  }
  // Push arg(A) + arg(B) - arg(AB)
  return PatternMatching::CreateAndSimplify(
      KMult(i_e, KAdd(KArg(KA), KArg(KB), KMult(-1_e, KArg(KMult(KA, KB))))),
      {.KA = a, .KB = b});
}

bool Logarithm::ContractLn(Tree* ref) {
  PatternMatching::Context ctx;
  // A*ln(B) = ln(B^A) + i*(A*arg(B) - arg(B^A)) if A is an integer.
  if (PatternMatching::Match(KMult(KA, KLn(KB)), ref, &ctx) &&
      ctx.getNode(KA)->isInteger()) {
    const Tree* a = ctx.getNode(KB);
    const Tree* b = ctx.getNode(KA);
    TreeRef c = PushProductCorrection(a, b);
    ctx.setNode(KC, c, 1, false);
    ref->moveTreeOverTree(
        PatternMatching::CreateAndSimplify(KAdd(KLn(KPow(KB, KA)), KC), ctx));
    c->removeTree();
    return true;
  }
  // A?+ ln(B) +C?+ ln(D) +E? = A+C+ ln(BD) +E+ i*(arg(B) + arg(D) - arg(BD))
  if (PatternMatching::Match(KAdd(KA_s, KLn(KB), KC_s, KLn(KD), KE_s), ref,
                             &ctx)) {
    const Tree* a = ctx.getNode(KB);
    const Tree* b = ctx.getNode(KD);
    TreeRef c = PushAdditionCorrection(a, b);
    ctx.setNode(KF, c, 1, false);
    ref->moveTreeOverTree(PatternMatching::CreateAndSimplify(
        KAdd(KA_s, KC_s, KLn(KMult(KB, KD)), KE_s, KF), ctx));
    c->removeTree();
    return true;
  }
  return false;
}

bool Logarithm::ExpandLn(Tree* ref) {
  // ln(12/7) = 2*ln(2) + ln(3) - ln(7)
  if (ExpandLnOnRational(ref)) {
    return true;
  }
  PatternMatching::Context ctx;
  // ln(A*B?) = ln(A) + ln(B) - i*(arg(A) + arg(B) - arg(AB))
  if (PatternMatching::Match(KLn(KMult(KA, KB_p)), ref, &ctx)) {
    // Since KB_p can match multiple trees, we need them as a single tree.
    const Tree* a = ctx.getNode(KA);
    TreeRef b = PatternMatching::CreateAndSimplify(KMult(KB_p), ctx);
    TreeRef c = PushAdditionCorrection(a, b);
    ref->moveTreeOverTree(PatternMatching::CreateAndSimplify(
        KAdd(KLn(KA), KLn(KB), KMult(-1_e, KC)), {.KA = a, .KB = b, .KC = c}));
    c->removeTree();
    b->removeTree();
    return true;
  }
  // ln(A^B) = B*ln(A) - i*( B*arg(A) - arg(A^B))
  if (PatternMatching::Match(KLn(KPow(KA, KB)), ref, &ctx)) {
    const Tree* a = ctx.getNode(KA);
    const Tree* b = ctx.getNode(KB);
    TreeRef c = PushProductCorrection(a, b);
    ctx.setNode(KC, c, 1, false);
    ref->moveTreeOverTree(PatternMatching::CreateAndSimplify(
        KAdd(KMult(KB, KLn(KA)), KMult(-1_e, KC)), ctx));
    c->removeTree();
    return true;
  }
  return false;
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
  Tree* numerator =
      ExpandLnOnInteger(Rational::Numerator(child), denominator == nullptr);
  if (!numerator) {
    assert(!denominator);
    // ln(13) -> ln(13)
    return false;
  }
  Tree* result;
  if (denominator) {
    // ln(13/11) -> ln(13)-ln(11)
    PatternMatching::CreateAndSimplify(KAdd(KA, KMult(-1_e, KB)),
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
  Arithmetic::FactorizedInteger factorization =
      Arithmetic::PrimeFactorization(m);
  if (escapeIfPrime && (factorization.numberOfFactors == 0 ||
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
    KMult(π_e, i_e)->clone();
  }
  NAry::SetNumberOfChildren(result, factorization.numberOfFactors + isNegative);
  NAry::SquashIfPossible(result);
  assert(!Simplification::DeepSystematicReduce(result));
  return result;
}

}  // namespace PoincareJ
