#pragma once

#include <stdint.h>

namespace Poincare::Internal {

enum class UnitDisplay : uint8_t {
  // Display for main output in Calculation
  MainOutput,
  // Best prefix and best metric representative
  AutomaticMetric,
  // Best metric representative, no prefix
  AutomaticPrefixFreeMetric,
  // Best prefix and best imperial representative
  AutomaticImperial,
  // If multiple choice, best of the input representative and prefix
  AutomaticInput,
  // Decompose time, angle, and imperial volume, area and length (1h 15min)
  Decomposition,
  // Best prefix with an equivalent representative (L <-> m^3, acre <-> ft^2)
  Equivalent,
  // No prefix, basic SI units only (m, s, mole, A, K, cd, kg)
  BasicSI,
  // Units are unchanged
  None,
  // Undef if input has non-angle unit dimension
  Forbidden,
};

}  // namespace Poincare::Internal
