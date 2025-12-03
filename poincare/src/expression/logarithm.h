#pragma once

#include <poincare/src/memory/tree_ref.h>

#include "integer.h"

namespace Poincare::Internal {

class Logarithm final {
 public:
  static bool ReduceLn(Tree* e);
  static bool ContractLn(Tree* e);
  static bool ExpandLn(Tree* e);
  // ln(12/5)->2*ln(2)+ln(3)-ln(5)
  static bool ExpandLnOnRational(Tree* e);

 private:
  // ln(12)->2*ln(2)+ln(3), return nullptr if m is prime and escapeIfPrime true.
  static Tree* ExpandLnOnInteger(IntegerHandler m, bool escapeIfPrime);
};

/* Using integers to represent bounds around multiples of π/2.
 *       -2     -1      0      1      2
 *  ------|------|------|------|------|------
 *       -π    -π/2     0     π/2     π
 * For both bounds, we store the integer and a boolean for inclusive/exclusive.
 * For example, ]-π, π/2] is ({-2, false},{1, true}) */
class PiInterval {
 public:
  constexpr PiInterval(int min, bool minIsInclusive, int max,
                       bool maxIsInclusive)
      : m_min(min),
        m_minIsInclusive(minIsInclusive),
        m_max(max),
        m_maxIsInclusive(maxIsInclusive) {}

  static PiInterval Add(PiInterval a, PiInterval b);
  static PiInterval Mult(PiInterval a, int b);
  static PiInterval Arg(ComplexProperties properties);
  // Return k such that max bound is in ]-π + 2kπ, π + 2kπ]
  inline int maxK() const {
    // m_maxIsInclusive doesn't matter.
    return DivideRoundDown(m_max + 1, 4);
  }
  // Return k such that min bound is in ]-π + 2kπ, π + 2kπ]
  inline int minK() const {
    // ]-π, ...] {-2, false} is 0 and [-π, ...] {-2, true} is -1
    return DivideRoundDown(m_min + !m_minIsInclusive + 1, 4);
  }

  constexpr bool operator==(const PiInterval&) const = default;

 private:
  // We want DivideRoundDown(-1, 4) to be -1
  inline static int DivideRoundDown(int num, int den) {
    int result = num / den;
    if (num < 0 && -num % den != 0) {
      // -1/4 is 0 but -4/4 is -1, we expect -1 for both.
      result -= 1;
    }
    return result;
  }

  void unionWith(PiInterval other);

  int m_min;
  bool m_minIsInclusive;
  int m_max;
  bool m_maxIsInclusive;
};

static constexpr PiInterval k_neutralInterval =
    PiInterval(INT_MAX, true, INT_MIN, true);

static constexpr PiInterval k_defaultInterval =
    PiInterval(INT_MIN, true, INT_MAX, true);

}  // namespace Poincare::Internal
