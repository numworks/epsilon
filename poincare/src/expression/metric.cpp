#include "metric.h"

#if USE_TREE_SIZE_METRIC

#else

#include <poincare/src/memory/pattern_matching.h>

#include "dependency.h"
#include "k_tree.h"

namespace Poincare::Internal {

int Metric::GetMetric(const Tree* e) {
  int result = GetMetric(e->type());
  PatternMatching::Context ctx;
  switch (e->type()) {
    case Type::Mult: {
      // Ignore (-1) in multiplications
      if (e->child(0)->isMinusOne()) {
        result -= GetMetric(Type::MinusOne);
        if (e->numberOfChildren() == 2) {
          result -= GetMetric(Type::Mult);
        }
      }
      // Beautification hyperbolic triginometry (cosh, sinh, asinh and atanh)
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
        Tree* exponent = PatternMatching::CreateSimplify(KMult(KA_s), ctx);
        if (!exponent->isHalf()) {
          result += GetMetric(exponent);
        }
        exponent->removeTree();
        return result + GetMetric(ctx.getTree(KB));
      }
      break;
    }
    case Type::Dep:
      return result + GetMetric(Dependency::Main(e));
    case Type::Trig:
    case Type::ATrig:
      // Ignore second child
      return result + GetMetric(e->child(0));
    default:
      break;
  }
  for (const Tree* child : e->children()) {
    result += GetMetric(child);
  }
  return result;
}

int Metric::GetMetric(Type type) {
  switch (type) {
    case Type::Zero:
    case Type::One:
    case Type::Two:
    case Type::MinusOne:
      return k_defaultMetric / 3;
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

}  // namespace Poincare::Internal

#endif
