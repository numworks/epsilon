#pragma once

#include <stdint.h>

namespace Poincare {

enum class AngleUnit : uint8_t {
  Radian = 0,
  Degree,
  Gradian,
  None,
  NUnits = None
};

enum class ComplexFormat : uint8_t {
  Real = 0,
  Cartesian,
  Polar,
  None,
  NFormats = None
};

enum class ReductionTarget {
  /* Optimized for approximation : (1+x)^3 will not be developed. */
  SystemForApproximation = 0,
  /* Optimized for analysis : (1+x)^3 will be developed to identify polynomial
   * coefficients */
  SystemForAnalysis,
  /* Optimized for user display. */
  User
};

enum class SymbolicComputation {
  ReplaceAllSymbols = 0,  // If a symbol is not defined, it is replaced by undef
  ReplaceDefinedSymbols = 1,
  ReplaceDefinedFunctions = 2,
  ReplaceAllSymbolsWithUndefined = 3,  // Used in UnitConvert::shallowReduce
  KeepAllSymbols = 4
};

enum class Strategy { Default, ApproximateToFloat, DeepExpandAlgebraic };

}  // namespace Poincare
