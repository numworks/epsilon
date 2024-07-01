#ifndef POINCARE_EXPRESSION_CONTEXT_H
#define POINCARE_EXPRESSION_CONTEXT_H

#include <stdint.h>

namespace Poincare::Internal {

enum class AngleUnit : uint8_t {
  Radian = 0,
  Degree,
  Gradian,
  LastAngleUnit = Gradian,
};

enum class ComplexFormat : uint8_t {
  Real = 0,
  Cartesian,
  Polar,
  LastComplexFormat = Polar,
};

enum class Strategy { Default, ApproximateToFloat };

enum class UnitFormat : bool { Metric = 0, Imperial = 1 };

enum class SymbolicComputation {
  ReplaceAllSymbolsWithDefinitionsOrUndefined = 0,
  ReplaceAllDefinedSymbolsWithDefinition = 1,
  ReplaceDefinedFunctionsWithDefinitions = 2,
  ReplaceAllSymbolsWithUndefined = 3,  // Used in UnitConvert::shallowReduce
  DoNotReplaceAnySymbol = 4
};

enum class UnitDisplay : uint8_t {
  // Display for main output in Calculation
  MainOutput,
  // Best prefix and best metric representative
  AutomaticMetric,
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

#endif
