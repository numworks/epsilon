#pragma once

#include "angle_unit.h"
#include "complex_format.h"
#include "preferences.h"
#include "symbol_context.h"

namespace Poincare {

// User settings needed by reduction and approximation algorithms
struct UserSettings {
#if POINCARE_COMPLEX
  ComplexFormat complexFormat = ComplexFormat::None;
#else
  constexpr static ComplexFormat complexFormat = ComplexFormat::Real;
#endif
  AngleUnit angleUnit = AngleUnit::None;
  const Poincare::SymbolContext& context = k_emptySymbolContext;
};

}  // namespace Poincare
