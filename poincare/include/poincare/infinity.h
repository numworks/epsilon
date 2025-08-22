#pragma once

namespace Poincare {

class Infinity {
 public:
  constexpr static const char* k_infinityName = "∞";
  constexpr static const char* k_minusInfinityName = "-∞";
  constexpr static const char* Name(bool negative) {
    return negative ? k_minusInfinityName : k_infinityName;
  }
};

}  // namespace Poincare
