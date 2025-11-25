#if !USE_TREE_SIZE_METRIC

#include "metric.h"

#include <limits.h>
#include <poincare/src/memory/pattern_matching.h>

#include <cmath>

#include "dependency.h"
#include "k_tree.h"
#include "rational.h"
#include "sign.h"

namespace Poincare::Internal {

namespace {

constexpr static float k_defaultMetric = 1.f;

constexpr static float GetTypeMetric(Type type) {
  switch (type) {
    case Type::Add:
    case Type::Mult:
      /* Add and Mult metrics depend on their number of children. See
       * GetAddMultMetric. */
      return 0.f;
    case Type::Zero:
    case Type::One:
    case Type::Two:
    case Type::MinusOne:
      return k_defaultMetric / 3.f;
    case Type::IntegerNegShort:
    case Type::IntegerPosShort:
    case Type::IntegerNegBig:
    case Type::IntegerPosBig:
      return k_defaultMetric / 2.f;
    case Type::RationalNegShort:
    case Type::RationalNegBig:
      return k_defaultMetric * 2.f;
    default:
      return k_defaultMetric;
    case Type::PowReal:
    case Type::Random:
    case Type::RandInt:
      return k_defaultMetric * 2.f;
    case Type::Var:
    case Type::UserSymbol:
      return k_defaultMetric * 3.f;
    case Type::Sum:
      return k_defaultMetric * 4.f;
  }
}

/* Add/Mult Metric must depend on its number of children to ensure that if A is
 * better than B*C, A*D will also be better than B*C*D. */
constexpr static float GetAddMultMetric(int numberOfChildren) {
  static_assert(GetTypeMetric(Type::Mult) == 0.f);
  static_assert(GetTypeMetric(Type::Add) == 0.f);
  return k_defaultMetric * (numberOfChildren - 1);
}

constexpr static float GetAddMultMetric(const Tree* e) {
  assert(e->isAdd() || e->isMult());
  return GetAddMultMetric(e->numberOfChildren());
}

static float ChildrenCoeffLn(ComplexProperties properties) {
  if (properties.isReal() && properties.realSign().isStrictlyNegative()) {
    // Increase cost of real negative children in roots
    return 4.f;
  }
  if (!properties.isReal()) {
    // Increase cost of non-real children even more
    return 8.f;
  }
  return 2.f;
}

// Used in ln(A) and root(A,B) to give weight to A to favor smaller values.
static float ChildCoeffOffsetInLnOrRoot(const Tree* child,
                                        bool willBeBeautified, bool inRoot) {
  if (willBeBeautified && child->isRational() && !child->isZero()) {
    // Increase cost of rationals in ln according to their value
    IntegerHandler p = Rational::Numerator(child);
    IntegerHandler q = Rational::Denominator(child);
    p.setSign(NonStrictSign::Positive);
    float sum = p.to<float>() + q.to<float>();
    assert(sum != 1.f);
    if (inRoot) {
      // In roots, √(11)*√(2) should not be favored over √(22)
#if !POINCARE_NO_FLOAT_APPROXIMATION
      return 2.f * std::log(sum);
#else
      // Save firmware size by not using std::log float implementation
      return 2.f * std::log(static_cast<double>(sum));
#endif
    } else {
      return 4.f * (sum - 1.f);
    }
  }
  return 0.f;
}

// Return metric for a to-be beautified exp(KA*ln(KB))
static float GetBeautifiedPowerMetric(const PatternMatching::Context& ctx) {
  // exp(A*ln(B)) is beautified in many forms of B^A
  float result = 0.f;
  const Tree* base = ctx.getTree(KB);
  float childrenCoeff = ChildrenCoeffLn(GetComplexProperties(base));
  // Favor smaller bases
  childrenCoeff += ChildCoeffOffsetInLnOrRoot(base, true, true);

  if (ctx.getNumberOfTrees(KA) == 1 && ctx.getTree(KA)->isRational()) {
    const Tree* exponent = ctx.getTree(KA);
    if (exponent->isHalf() || Rational::IsMinusHalf(exponent)) {
      // exp(A*ln(B)) -> √(B)
      result += GetTypeMetric(Type::Sqrt);
      if (Rational::IsMinusHalf(exponent)) {
        // exp(A*ln(B)) -> 1/√(B)
        result += GetTypeMetric(Type::One) + GetTypeMetric(Type::Div);
      }
    } else if ((Rational::Numerator(exponent).isOne() ||
                Rational::Numerator(exponent).isMinusOne())) {
      // exp(A*ln(B)) -> Root(B,A)
      static_assert(GetTypeMetric(Type::IntegerPosShort) ==
                    GetTypeMetric(Type::IntegerPosBig));
      result += GetTypeMetric(Type::IntegerPosShort);
      result += GetTypeMetric(Type::Root);
      if (Rational::Numerator(exponent).isMinusOne()) {
        // exp(A*ln(B)) -> 1/Root(B,A)
        result += GetTypeMetric(Type::One) + GetTypeMetric(Type::Div);
      }
    } else {
      // exp(A*ln(B)) -> B^A
      result += GetTypeMetric(Type::Pow);
      result += Metric::GetTrueMetric(exponent, ReductionTarget::User);
      /* Favor root forms over power of rationals(e.g. Root(4,3) over
       * 2^(2/3))  */
      childrenCoeff += 3 * k_defaultMetric;
    }
  } else {
    Tree* exponent = PatternMatching::Create(KMult(KA_s), ctx);
    // exp(A*ln(B)) -> B^A
    result += GetTypeMetric(Type::Pow);
    result += Metric::GetTrueMetric(exponent, ReductionTarget::User);
    exponent->removeTree();
  }

  if (base->isPow()) {
    // Favor 1/Root(A,B) over Root(1/A,B)
    childrenCoeff += 3 * k_defaultMetric;
  }

  // Cost of ln, multiplication and sometimes of the exponent are ignored.
  float baseMetric = Metric::GetTrueMetric(base, ReductionTarget::User);
  return result + baseMetric * childrenCoeff;
}

}  // namespace

float Metric::GetTrueMetric(const Tree* e, ReductionTarget reductionTarget) {
  const bool willBeBeautified = reductionTarget == ReductionTarget::User;
  const bool shouldExpand =
      reductionTarget != ReductionTarget::SystemForApproximation;
  float result = GetTypeMetric(e->type());
  /* Some functions must have the smallest children possible, so we increase the
   * cost of all children inside the parent expression with a coefficient. */
  float childrenCoeff = 1;
  PatternMatching::Context ctx;
  switch (e->type()) {
    case Type::Mult: {
      result += GetAddMultMetric(e);
      if (willBeBeautified) {
        // Ignore cost of multiplication in (-A), unless it's (-1)*(A+B)
        if (e->child(0)->isMinusOne() && e->numberOfChildren() == 2 &&
            !e->child(1)->isAdd()) {
          assert(result == GetAddMultMetric(e));
          result = 0.f;
        }
        /* Trigonometry with complexes is beautified into hyperbolic
         * trigonometry (cosh, sinh, asinh and atanh)*/
        // TODO: cost difference between trig and hyperbolic trig
        if (PatternMatching::Match(
                e, KMult(KA_s, KTrig(KMult(KB_s, i_e), 1_e), KC_s, i_e),
                &ctx) ||
            PatternMatching::Match(
                e, KMult(KA_s, KATrig(KMult(KB_s, i_e), 1_e), KC_s, i_e),
                &ctx) ||
            PatternMatching::Match(
                e, KMult(KA_s, KATanRad(KMult(KB_s, i_e)), KC_s, i_e), &ctx)) {
          if (ctx.getNumberOfTrees(KB) == 1) {
            assert(result == GetAddMultMetric(e));
            result = 0.f;
          }
          result +=
              GetTypeMetric(Type::MinusOne) - GetTypeMetric(Type::ComplexI) * 2;
        }
        // Reset context
        ctx = PatternMatching::Context();
        // ln(A)/ln(B) is beautified into log(A,B)
        constexpr const Tree* invLn = KPow(KLn(KB), -1_e);
        bool hasLn = false;
        const Tree* lastInvLn = nullptr;
        for (const Tree* child : e->children()) {
          assert((lastInvLn == nullptr) == (ctx.getTree(KB) == nullptr));
          if (child->isLn()) {
            hasLn = true;
          } else if (!lastInvLn && PatternMatching::Match(child, invLn, &ctx)) {
            lastInvLn = child;
          }
          if (hasLn && ctx.getTree(KB)) {
            assert((lastInvLn == nullptr) == (ctx.getTree(KB) == nullptr));
            // Discard 1/ln(B) cost, but preserve B cost.
            if (e->numberOfChildren() == 2) {
              assert(result == GetAddMultMetric(e));
              result = 0.f;
            }
            result -= GetTrueMetric(lastInvLn, reductionTarget);
            result += GetTrueMetric(ctx.getTree(KB), reductionTarget);
            break;
          }
        }
      }
      break;
    }
    case Type::Add: {
      result += GetAddMultMetric(e);
      if (shouldExpand &&
          PatternMatching::Match(
              e, KAdd(KA_s, KMult(KB, KC), KD_s, KMult(KB, KE), KF_s), &ctx)) {
        /* Ignore cost of having developed B*(C+E) into B*C + B*E when B:
         * - is not minus one
         * - is not the inverse of an expression
         * - is small enough (<= k_defaultMetric)
         * To do so, we ensure M(B*(C+E)) ~= M(BC+BE))
         * This is equivalent to
         * M(*2)+M(B)+M(+2)+M(C)+M(E) ~= M(+2)+M(*2)+M(B)+M(C)+M(*2)+M(B)+M(E)
         * We now need to offset the added cost of M(*2) and M(B) .
         * We can bound M(B) with k_defaultMetric (otherwise contracted form
         * will be preferred). */
        const Tree* factor = ctx.getTree(KB);
        if (!(factor->isPow() && factor->child(1)->isMinusOne()) &&
            !factor->isMinusOne()) {
          result -= GetAddMultMetric(2);
          result -= k_defaultMetric;
        }
      }
      break;
    }
    case Type::Exp: {
      if (willBeBeautified &&
          PatternMatching::Match(e, KExp(KMult(KA_s, KLn(KB))), &ctx)) {
        assert(reductionTarget == ReductionTarget::User);
        return GetBeautifiedPowerMetric(ctx);
      }
      break;
    }
    case Type::Pow: {
      if (shouldExpand && e->child(0)->isAdd() && e->child(1)->isInteger() &&
          !e->child(1)->isMinusOne()) {
        // Increase cost of factorized expressions as integer power
        childrenCoeff = 4;
      }
      break;
    }
    case Type::Dep:
      /* Cost of dependency is ignored */
      return GetTrueMetric(Dependency::Main(e), reductionTarget);
    case Type::Trig:
    case Type::ATrig: {
      if (willBeBeautified &&
          PatternMatching::Match(e, KTrig(KMult(KA_s, i_e), 0_e), &ctx)) {
        // cos(A*i) is beautified into cosh(A)
        result -= GetTypeMetric(Type::ComplexI);
        if (ctx.getNumberOfTrees(KA) == 1) {
          result -= GetAddMultMetric(e->child(0));
        }
      }
      childrenCoeff = 2;
      // Ignore second child
      return result +
             GetTrueMetric(e->child(0), reductionTarget) * childrenCoeff;
    }
    case Type::Ln: {
      childrenCoeff = ChildrenCoeffLn(GetComplexProperties(e->child(0)));
      const Tree* firstChild =
          e->child(0)->isMult() ? e->child(0)->child(0) : e->child(0);
      childrenCoeff +=
          ChildCoeffOffsetInLnOrRoot(firstChild, willBeBeautified, false);
      break;
    }
    case Type::Abs:
    case Type::Arg:
    case Type::Im:
    case Type::Re:
    case Type::Conj:
    case Type::ATanRad:
    case Type::Frac:
    case Type::Ceil:
    case Type::Floor:
    case Type::Round:
    case Type::PowReal:
    case Type::Root:
    case Type::Sign:
      childrenCoeff = 2;
      break;
    default:
      break;
  }
  for (const Tree* child : e->children()) {
    result += GetTrueMetric(child, reductionTarget) * childrenCoeff;
  }
  return result;
}

float Metric::GetMetric(const Tree* e, ReductionTarget reductionTarget) {
  if (CannotBeReducedFurther(e)) {
    return k_perfectMetric;
  }
  float metric = GetTrueMetric(e, reductionTarget);
  assert(metric > k_perfectMetric);
  /* Adding a small factor based on size, from 0 to 1/8.
   * The goal is to prefer smaller tree when they have the same metric.
   * Dependencies must be ignored so that two trees with the same hash in
   * advanced reduction have the same metric.
   * This factor should be smaller than the minimal difference in metric
   * i.e.:
   * GetTypeMetric(Type::IntegerPosShort) - GetTypeMetric(Type::Zero)
   * = 1/2 - 1/3 = 1/6 */
  float sizeFactor =
      M_2_PI *
      std::atan(static_cast<float>(Dependency::SafeMain(e)->treeSize()) /
                100.f);
  metric += sizeFactor * k_defaultMetric / 8.f;
  return metric;
}

namespace {
bool IsMultOfNumbers(const Tree* e, bool withI) {
  if (e->isNumber() || (withI && e->isComplexI())) {
    return true;
  }
  if (!e->isMult()) {
    return false;
  }
  for (const Tree* child : e->children()) {
    if (!(child->isNumber() || (withI && child->isComplexI()))) {
      return false;
    }
  }
  return true;
}

bool HasCartesianForm(const Tree* e) {
  if (IsMultOfNumbers(e, true)) {
    return true;
  }
  return e->isAdd() && e->numberOfChildren() == 2 &&
         IsMultOfNumbers(e->child(0), false) &&
         IsMultOfNumbers(e->child(1), true);
}
}  // namespace

bool Metric::CannotBeReducedFurther(const Tree* e) {
  return HasCartesianForm(Dependency::SafeMain(e));
}

}  // namespace Poincare::Internal

#endif
