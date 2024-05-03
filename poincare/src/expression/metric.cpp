#if 0
#include "metric.h"

#include <poincare/src/memory/pattern_matching.h>

#include "dependency.h"
#include "k_tree.h"

namespace Poincare::Internal {

int Metric::GetMetric(const Tree* u) {
  int result = GetMetric(u->type());
  switch (u->type()) {
    case Type::Mult: {
      // Ignore (-1) in multiplications
      PatternMatching::Context ctx;
      if (u->child(0)->isMinusOne()) {
        result -= GetMetric(Type::MinusOne);
        if (u->numberOfChildren() == 2) {
          result -= GetMetric(Type::Mult);
        }
      }
      break;
    }
    case Type::Exp: {
      // exp(A*ln(B)) -> Root(B,A) exception
      PatternMatching::Context ctx;
      if (PatternMatching::Match(KExp(KMult(KA_s, KLn(KB))), u, &ctx)) {
        if (!ctx.getNode(KA)->isHalf()) {
          result += GetMetric(ctx.getNode(KA));
        }
        return result + GetMetric(ctx.getNode(KB));
      }
      break;
    }
    case Type::Dependency:
      return result + GetMetric(Dependency::Main(u));
    case Type::Trig:
    case Type::ATrig:
      // Ignore second child
      return result + GetMetric(u->child(0));
    default:
      break;
  }
  for (const Tree* child : u->children()) {
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
