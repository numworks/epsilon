#include "metric.h"

#if USE_TREE_SIZE_METRIC

#else

#include <limits.h>
#include <poincare/src/memory/pattern_matching.h>

#include "dependency.h"
#include "k_tree.h"
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
}  // namespace

int Metric::GetTrueMetric(const Tree* e) {
  int result = GetMetric(e->type());
  /* Some functions must have the smallest children possible, so we increase the
   * cost of all children inside the parent expression with a coefficient. */
  int childrenCoeff = 1;
  PatternMatching::Context ctx;
  switch (e->type()) {
    case Type::RationalNegBig:
    case Type::RationalPosBig:
    case Type::IntegerNegBig:
    case Type::IntegerPosBig:
      return GetMetric(ShortTypeForBigType(e->type())) * e->nodeSize();
    case Type::Mult: {
      // Ignore cost of multiplication in (-A)
      if (e->child(0)->isMinusOne() && e->numberOfChildren() == 2) {
        result -= GetMetric(Type::Mult);
      }
      /* Trigonometry with complexes will be beautified into hyperbolic
       * trigonometry (cosh, sinh, asinh and atanh)*/
      // TODO: cost difference between trig and hyperbolic trig
      if (PatternMatching::Match(
              e, KMult(KA_s, KTrig(KMult(KB_s, i_e), 1_e), KC_s, i_e), &ctx) ||
          PatternMatching::Match(
              e, KMult(KA_s, KATrig(KMult(KB_s, i_e), 1_e), KC_s, i_e), &ctx) ||
          PatternMatching::Match(
              e, KMult(KA_s, KATanRad(KMult(KB_s, i_e)), KC_s, i_e), &ctx)) {
        result += GetMetric(Type::MinusOne) - GetMetric(Type::ComplexI) * 2;
        if (ctx.getNumberOfTrees(KB) == 1) {
          result -= GetMetric(Type::Mult);
        }
      } else if (PatternMatching::Match(e, KTrig(KMult(KA_s, i_e), 0_e),
                                        &ctx)) {
        result -= GetMetric(Type::ComplexI);
        if (ctx.getNumberOfTrees(KA) == 1) {
          result -= GetMetric(Type::Mult);
        }
      }
      break;
    }
    case Type::Exp: {
      // exp(A*ln(B)) -> Root(B,A) exception
      if (PatternMatching::Match(e, KExp(KMult(KA_s, KLn(KB))), &ctx)) {
        Tree* exponent = PatternMatching::Create(KMult(KA_s), ctx);
        if (!exponent->isHalf()) {
          // Ignore cost of exponent for squareroot
          result += GetTrueMetric(exponent);
        }
        exponent->removeTree();
        childrenCoeff = 2;
        const Tree* base = ctx.getTree(KB);
        ComplexSign baseSign = GetComplexSign(base);
        if ((baseSign.isReal() && baseSign.realSign().isStrictlyNegative()) ||
            (baseSign.isPureIm() && baseSign.imagSign().isStrictlyNegative())) {
          // Increase cost of negative children in roots
          childrenCoeff = 4;
        }
        return result + GetTrueMetric(base) * childrenCoeff;
      }
      break;
    }
    case Type::Dep:
      return result + GetTrueMetric(Dependency::Main(e));
    case Type::Trig:
    case Type::ATrig:
      childrenCoeff = 2;
      // Ignore second child
      return result + GetTrueMetric(e->child(0)) * childrenCoeff;
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
    case Type::Log:
    case Type::Ln:
    case Type::Sign:
      childrenCoeff = 2;
      break;
    default:
      break;
  }
  for (const Tree* child : e->children()) {
    result += GetTrueMetric(child) * childrenCoeff;
  }
  return result;
}

int Metric::GetMetric(const Tree* e) {
  if (CannotBeReducedFurther(e)) {
    return k_perfectMetric;
  }
  int metric = GetTrueMetric(e);
  assert(metric != k_perfectMetric);
  /* INT_MAX in case of overflow
   * NOTE this does not completely prevent overflows as we could circle back
   * above 0
   * TODO a long-term solution could be to use a float metric instead */
  return metric > 0 ? metric : INT_MAX;
}

int Metric::GetMetric(Type type) {
  switch (type) {
    case Type::Zero:
    case Type::One:
    case Type::Two:
    case Type::MinusOne:
      return k_defaultMetric / 3;
    case Type::IntegerNegShort:
    case Type::IntegerPosShort:
      return k_defaultMetric / 2;
    default:
      return k_defaultMetric;
    case Type::PowReal:
    case Type::Random:
    case Type::RandInt:
      return k_defaultMetric * 2;
    case Type::Sum:
    case Type::Var:
      return k_defaultMetric * 3;
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
  return HasCartesianForm(e);
}

}  // namespace Poincare::Internal

#endif
