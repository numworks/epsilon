#ifndef POINCARE_EXPRESSION_INFINITY_H
#define POINCARE_EXPRESSION_INFINITY_H

#include <stdint.h>

namespace Poincare::Internal {

class Tree;

class Infinity {
 public:
  constexpr static const char* k_infinityName = "∞";
  constexpr static const char* k_minusInfinityName = "-∞";
  constexpr static const char* Name(bool negative) {
    return negative ? k_minusInfinityName : k_infinityName;
  }

  static bool IsPlusOrMinusInfinity(const Tree* e);
  static bool IsMinusInfinity(const Tree* e);
};

}  // namespace Poincare::Internal

#endif
