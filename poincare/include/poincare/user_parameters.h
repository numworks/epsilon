#pragma once

#include "angle_unit.h"
#include "complex_format.h"
#include "preferences.h"
#include "symbol_context.h"

namespace Poincare {

// User parameters needed by reduction and approximation algorithms
struct UserParameters {
#if POINCARE_COMPLEX
  ComplexFormat complexFormat = ComplexFormat::None;
#else
  constexpr static ComplexFormat complexFormat = ComplexFormat::Real;
#endif
  AngleUnit angleUnit = AngleUnit::None;
  Preferences::UnitFormat unitFormat = Preferences::UnitFormat::Metric;

  const Poincare::SymbolContext& context = k_emptySymbolContext;
};

}  // namespace Poincare
