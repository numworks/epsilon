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
Type ShortTypeForBigType(Type t) {
  switch (t) {
    case Type::RationalNegBig:
      return Type::RationalNegShort;
    case Type::RationalPosBig:
      return Type::RationalPosShort;
    case Type::IntegerNegBig:
      return Type::IntegerNegShort;
    case Type::IntegerPosBig:
      return Type::IntegerPosShort;
    default:
      OMG::unreachable();
  }
}

float ChildrenCoeffLn(ComplexSign sign) {
  float childrenCoeff = 2;
  if (sign.isReal() && sign.realSign().isStrictlyNegative()) {
    // Increase cost of real negative children in roots
    childrenCoeff = 4;
  } else if (!sign.isReal()) {
    // Increase cost of non-real children even more
    childrenCoeff = 8;
  }
  return childrenCoeff;
}

}  // namespace

float Metric::GetTrueMetric(const Tree* e, ReductionTarget reductionTarget) {
  const bool willBeBeautified = reductionTarget == ReductionTarget::User;
  const bool shouldExpand =
      reductionTarget != ReductionTarget::SystemForApproximation;
  float result = GetMetric(e->type());
  /* Some functions must have the smallest children possible, so we increase the
   * cost of all children inside the parent expression with a coefficient. */
  float childrenCoeff = 1;
  PatternMatching::Context ctx;
  switch (e->type()) {
    case Type::RationalNegBig:
    case Type::RationalPosBig:
    case Type::IntegerNegBig:
    case Type::IntegerPosBig:
      return willBeBeautified
                 ? GetMetric(ShortTypeForBigType(e->type())) * e->nodeSize()
                 : GetMetric(e->type());
    case Type::Mult: {
      if (willBeBeautified) {
        // Ignore cost of multiplication in (-A)
        if (e->child(0)->isMinusOne() && e->numberOfChildren() == 2) {
          result -= GetMetric(Type::Mult);
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
          result += GetMetric(Type::MinusOne) - GetMetric(Type::ComplexI) * 2;
          if (ctx.getNumberOfTrees(KB) == 1) {
            result -= GetMetric(Type::Mult);
          }
        }
        // Reset context
        ctx = PatternMatching::Context();
        // ln(A)/ln(B) is beautified into log(A,B)
        constexpr const Tree* invLn = KPow(KLn(KB), -1_e);
        bool hasLn = false;
        const Tree* lastInvLn = nullptr;
        for (const Tree* child : e->children()) {
          assert(!lastInvLn == !ctx.getTree(KB));
          if (child->isLn()) {
            hasLn = true;
          } else if (!lastInvLn && PatternMatching::Match(child, invLn, &ctx)) {
            lastInvLn = child;
          }
          if (hasLn && ctx.getTree(KB)) {
            assert(!lastInvLn == !ctx.getTree(KB));
            // Discard 1/ln(B) cost, bu t preserve B cost.
            result -= GetTrueMetric(lastInvLn, reductionTarget);
            result += GetTrueMetric(ctx.getTree(KB), reductionTarget);
            if (e->numberOfChildren() == 2) {
              result -= GetMetric(Type::Mult);
            }
            break;
          }
        }
      }
      break;
    }
    case Type::Add: {
      if (shouldExpand &&
          PatternMatching::Match(
              e, KAdd(KA_s, KMult(KB, KC), KD_s, KMult(KB, KE), KF_s), &ctx)) {
        /* Ignore cost of developing B*(C+E) when B:
         * - is not minus one
         * - is not the inverse of an expression
         * - is small enough (<= k_defaultMetric) */
        const Tree* factor = ctx.getTree(KB);
        if (!(factor->isPow() && factor->child(1)->isMinusOne()) &&
            !factor->isMinusOne()) {
          result -= GetMetric(Type::Mult);
          result -= k_defaultMetric;
        }
      }
      break;
    }
    case Type::Exp: {
      if (willBeBeautified &&
          PatternMatching::Match(e, KExp(KMult(KA_s, KLn(KB))), &ctx)) {
        // exp(A*ln(B)) -> Root(B,A) exception
        Tree* exponent = PatternMatching::Create(KMult(KA_s), ctx);
        if (!exponent->isHalf()) {
          // Ignore cost of exponent for squareroot
          result += GetTrueMetric(exponent, reductionTarget);
        }
        exponent->removeTree();
        const Tree* base = ctx.getTree(KB);
        childrenCoeff = ChildrenCoeffLn(GetComplexSign(base));
        return result + GetTrueMetric(base, reductionTarget) * childrenCoeff;
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
        result -= GetMetric(Type::ComplexI);
        if (ctx.getNumberOfTrees(KA) == 1) {
          result -= GetMetric(Type::Mult);
        }
      }
      childrenCoeff = 2;
      // Ignore second child
      return result +
             GetTrueMetric(e->child(0), reductionTarget) * childrenCoeff;
    }
    case Type::Ln: {
      childrenCoeff = ChildrenCoeffLn(GetComplexSign(e->child(0)));
      const Tree* firstChild =
          e->child(0)->isMult() ? e->child(0)->child(0) : e->child(0);
      if (willBeBeautified && firstChild->isRational() &&
          !firstChild->isZero()) {
        // Increase cost of rationals in ln according to their value
        IntegerHandler p = Rational::Numerator(firstChild);
        IntegerHandler q = Rational::Denominator(firstChild);
        p.setSign(NonStrictSign::Positive);
        assert(p.to<float>() + q.to<float>() != 1.f);
        childrenCoeff += 4.f * (p.to<float>() + q.to<float>() - 1.f);
      }
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
   * GetMetric(Type::IntegerPosShort) - GetMetric(Type::Zero)
   * = 1/2 - 1/3 = 1/6 */
  float sizeFactor =
      M_2_PI *
      std::atan(static_cast<float>(Dependency::SafeMain(e)->treeSize()) /
                100.f);
  metric += sizeFactor * k_defaultMetric / 8.f;
  return metric;
}

float Metric::GetMetric(Type type) {
  switch (type) {
    case Type::Zero:
    case Type::One:
    case Type::Two:
    case Type::MinusOne:
      return k_defaultMetric / 3.f;
    case Type::IntegerNegShort:
    case Type::IntegerPosShort:
      return k_defaultMetric / 2.f;
    default:
      return k_defaultMetric;
    case Type::PowReal:
    case Type::Random:
    case Type::RandInt:
      return k_defaultMetric * 2.f;
    case Type::Sum:
    case Type::Var:
    case Type::UserSymbol:
      return k_defaultMetric * 3.f;
  }
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
