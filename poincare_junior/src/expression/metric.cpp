#if 0
#include "metric.h"

#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/memory/pattern_matching.h>

namespace PoincareJ {

int Metric::GetMetric(const Tree* u) {
  int result = GetMetric(u->type());
  switch (u->type()) {
    case Type::Multiplication: {
      // Ignore (-1) in multiplications
      PatternMatching::Context ctx;
      if (u->nextNode()->isMinusOne()) {
        result -= GetMetric(Type::MinusOne);
        if (u->numberOfChildren() == 2) {
          result -= GetMetric(Type::Multiplication);
        }
      }
      break;
    }
    case Type::Exponential: {
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
    case Type::Trig:
    case Type::ATrig:
      // Ignore second child
      return result + GetMetric(u->nextNode());
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
    case Type::PowerReal:
    case Type::Random:
    case Type::RandInt:
      return k_defaultMetric * 2;
    case Type::Sum:
    case Type::Variable:
      return k_defaultMetric * 3;
  }
}

}  // namespace PoincareJ
#endif
