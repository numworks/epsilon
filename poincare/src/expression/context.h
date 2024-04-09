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

}  // namespace Poincare::Internal

#endif
