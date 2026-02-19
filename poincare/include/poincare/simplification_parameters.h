#pragma once

#include "math_options.h"
#include "preferences.h"
#include "symbol_context.h"

namespace Poincare {

struct ReductionParameters {
  ReductionTarget target = ReductionTarget::User;
  bool advanced = true;
};

struct SimplificationParameters {
#if POINCARE_COMPLEX
  ComplexFormat complexFormat = ComplexFormat::None;
#else
  constexpr static ComplexFormat complexFormat = ComplexFormat::Real;
#endif
  AngleUnit angleUnit = AngleUnit::None;
  Strategy strategy = Strategy::Default;
  ReductionParameters reduction = {};
  Preferences::UnitFormat unitFormat = Preferences::UnitFormat::Metric;
  SymbolicComputation symbolic = SymbolicComputation::KeepAllSymbols;
  const Poincare::SymbolContext& context = k_emptySymbolContext;
  UnitDisplay unitDisplay = UnitDisplay::MainOutput;
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

struct BeautificationParameters {
#if POINCARE_COMPLEX
  ComplexFormat complexFormat = ComplexFormat::None;
#else
  constexpr static ComplexFormat complexFormat = ComplexFormat::Real;
#endif
  AngleUnit angleUnit = AngleUnit::None;
  const Poincare::SymbolContext& context = k_emptySymbolContext;
  ReductionParameters reduction = {};
};

inline BeautificationParameters ToBeautificationParameters(
    const SimplificationParameters& simplificationParameters) {
#if POINCARE_COMPLEX
  return BeautificationParameters{
      .complexFormat = simplificationParameters.complexFormat,
      .angleUnit = simplificationParameters.angleUnit,
      .context = simplificationParameters.context,
      .reduction = simplificationParameters.reduction};
#else
  return BeautificationParameters{
      .angleUnit = simplificationParameters.angleUnit,
      .context = simplificationParameters.context,
      .reduction = simplificationParameters.reduction};
#endif
}

// Strategy will be default and symbolic will be KeepAllSymbols
inline ProjectionParameters ToProjectionParameters(
    const BeautificationParameters& beautificationParameters) {
#if POINCARE_COMPLEX
  return ProjectionParameters{
      .complexFormat = beautificationParameters.complexFormat,
      .angleUnit = beautificationParameters.angleUnit,
      .symbolic = SymbolicComputation::KeepAllSymbols,
      .context = beautificationParameters.context,
      .strategy = Strategy::Default};
#else
  return ProjectionParameters{.angleUnit = beautificationParameters.angleUnit,
                              .symbolic = SymbolicComputation::KeepAllSymbols,
                              .context = beautificationParameters.context,
                              .strategy = Strategy::Default};
#endif
}

struct UnitParameters {
  Preferences::UnitFormat unitFormat = Preferences::UnitFormat::Metric;
  UnitDisplay unitDisplay = UnitDisplay::MainOutput;
  AngleUnit angleUnit = AngleUnit::None;
};

inline UnitParameters ToUnitParameters(
    const SimplificationParameters& simplificationParameters) {
  return UnitParameters{.unitFormat = simplificationParameters.unitFormat,
                        .unitDisplay = simplificationParameters.unitDisplay,
                        .angleUnit = simplificationParameters.angleUnit};
}

}  // namespace Poincare
