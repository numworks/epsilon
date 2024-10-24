#include "metric.h"

#if USE_TREE_SIZE_METRIC

#else

#include <poincare/src/memory/pattern_matching.h>

#include "dependency.h"
#include "k_tree.h"

namespace Poincare::Internal {

int Metric::GetMetric(const Tree* e) {
  int result = GetMetric(e->type());
  switch (e->type()) {
    case Type::Mult: {
      // Ignore (-1) in multiplications
      PatternMatching::Context ctx;
      if (e->child(0)->isMinusOne()) {
        result -= GetMetric(Type::MinusOne);
        if (e->numberOfChildren() == 2) {
          result -= GetMetric(Type::Mult);
        }
      }
      break;
    }
    case Type::Exp: {
      // exp(A*ln(B)) -> Root(B,A) exception
      PatternMatching::Context ctx;
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
