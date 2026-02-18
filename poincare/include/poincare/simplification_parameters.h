#pragma once

#include "math_options.h"
#include "preferences.h"
#include "symbol_context.h"

namespace Poincare {

struct SimplificationParameters {
#if POINCARE_COMPLEX
  ComplexFormat complexFormat = ComplexFormat::None;
#else
  constexpr static ComplexFormat complexFormat = ComplexFormat::Real;
#endif
  AngleUnit angleUnit = AngleUnit::None;
  Strategy strategy = Strategy::Default;
  ReductionTarget reductionTarget = ReductionTarget::User;
  Preferences::UnitFormat unitFormat = Preferences::UnitFormat::Metric;
  SymbolicComputation symbolic = SymbolicComputation::KeepAllSymbols;
  const Poincare::SymbolContext& context = k_emptySymbolContext;
  UnitDisplay unitDisplay = UnitDisplay::MainOutput;
  // Optional simplification step
  bool advanceReduce = true;
};

}  // namespace Poincare
