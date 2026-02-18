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

struct ProjectionParameters {
#if POINCARE_COMPLEX
  ComplexFormat complexFormat = ComplexFormat::None;
#else
  constexpr static ComplexFormat complexFormat = ComplexFormat::Real;
#endif
  AngleUnit angleUnit = AngleUnit::None;
  SymbolicComputation symbolic = SymbolicComputation::KeepAllSymbols;
  const Poincare::SymbolContext& context = k_emptySymbolContext;
  Strategy strategy = Strategy::Default;
};

inline ProjectionParameters ToProjectionParameters(
    const SimplificationParameters& simplificationParameters) {
#if POINCARE_COMPLEX
  return ProjectionParameters{
      .complexFormat = simplificationParameters.complexFormat,
      .angleUnit = simplificationParameters.angleUnit,
      .symbolic = simplificationParameters.symbolic,
      .context = simplificationParameters.context,
      .strategy = simplificationParameters.strategy};
#else
  return ProjectionParameters{.angleUnit = simplificationParameters.angleUnit,
                              .symbolic = simplificationParameters.symbolic,
                              .context = simplificationParameters.context,
                              .strategy = simplificationParameters.strategy};
#endif
}

}  // namespace Poincare
