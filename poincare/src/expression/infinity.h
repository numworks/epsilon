#pragma once

#include <stdint.h>

namespace Poincare::Internal {

class Tree;

class Infinity {
 public:
  static bool IsPlusOrMinusInfinity(const Tree* e);
  static bool IsMinusInfinity(const Tree* e);
};

}  // namespace Poincare::Internal
