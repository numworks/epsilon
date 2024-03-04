#if 0
#include "metric.h"

#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/memory/pattern_matching.h>

namespace PoincareJ {

int Metric::GetMetric(const Tree* u) {
  int result = GetMetric(u->type());
  switch (u->type()) {
    case BlockType::Multiplication: {
      // Ignore (-1) in multiplications
      PatternMatching::Context ctx;
      if (u->nextNode()->isMinusOne()) {
        result -= GetMetric(BlockType::MinusOne);
        if (u->numberOfChildren() == 2) {
          result -= GetMetric(BlockType::Multiplication);
        }
      }
      break;
    }
    case BlockType::Exponential: {
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
    case BlockType::Dependency:
    case BlockType::Trig:
    case BlockType::ATrig:
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

int Metric::GetMetric(BlockType type) {
  switch (type) {
    case BlockType::Zero:
    case BlockType::One:
    case BlockType::Two:
    case BlockType::MinusOne:
      return k_defaultMetric / 3;
    default:
      return k_defaultMetric;
    case BlockType::PowerReal:
    case BlockType::Random:
    case BlockType::RandInt:
      return k_defaultMetric * 2;
    case BlockType::Sum:
    case BlockType::Variable:
      return k_defaultMetric * 3;
  }
}

}  // namespace PoincareJ
#endif
