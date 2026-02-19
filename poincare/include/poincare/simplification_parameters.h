#pragma once

#include "math_options.h"
#include "preferences.h"
#include "symbol_context.h"

namespace Poincare {

struct UserParameters {
#if POINCARE_COMPLEX
  ComplexFormat complexFormat = ComplexFormat::None;
#else
  constexpr static ComplexFormat complexFormat = ComplexFormat::Real;
#endif
  AngleUnit angleUnit = AngleUnit::None;
  const Poincare::SymbolContext& context = k_emptySymbolContext;
};

struct ProjectionPolicy {
  SymbolicComputation symbolic = SymbolicComputation::KeepAllSymbols;
  Strategy strategy = Strategy::Default;
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

struct ReductionParameters {
  ReductionTarget target = ReductionTarget::User;
  bool advanced = true;
};

struct UnitModeParameters {
  Preferences::UnitFormat unitFormat = Preferences::UnitFormat::Metric;
  UnitDisplay unitDisplay = UnitDisplay::MainOutput;
};

struct SimplificationParameters {
  UserParameters user = {};
  ProjectionPolicy projectionPolicy = {};
  ReductionParameters reduction = {};
  UnitModeParameters unitMode = {};
};

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

struct UnitParameters {
  Preferences::UnitFormat unitFormat = Preferences::UnitFormat::Metric;
  UnitDisplay unitDisplay = UnitDisplay::MainOutput;
  AngleUnit angleUnit = AngleUnit::None;
};

/* Conversions */

inline ProjectionParameters ToProjectionParameters(
    const SimplificationParameters& simplificationParameters) {
#if POINCARE_COMPLEX
  return ProjectionParameters{
      .complexFormat = simplificationParameters.user.complexFormat,
      .angleUnit = simplificationParameters.user.angleUnit,
      .symbolic = simplificationParameters.projectionPolicy.symbolic,
      .context = simplificationParameters.user.context,
      .strategy = simplificationParameters.projectionPolicy.strategy};
#else
  return ProjectionParameters{
      .angleUnit = simplificationParameters.user.angleUnit,
      .symbolic = simplificationParameters.projectionPolicy.symbolic,
      .context = simplificationParameters.user.context,
      .strategy = simplificationParameters.projectionPolicy.strategy};
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

inline BeautificationParameters ToBeautificationParameters(
    const SimplificationParameters& simplificationParameters) {
#if POINCARE_COMPLEX
  return BeautificationParameters{
      .complexFormat = simplificationParameters.user.complexFormat,
      .angleUnit = simplificationParameters.user.angleUnit,
      .context = simplificationParameters.user.context,
      .reduction = simplificationParameters.reduction};
#else
  return BeautificationParameters{
      .angleUnit = simplificationParameters.user.angleUnit,
      .context = simplificationParameters.user.context,
      .reduction = simplificationParameters.reduction};
#endif
}

inline UnitParameters ToUnitParameters(
    const SimplificationParameters& simplificationParameters) {
  return UnitParameters{
      .unitFormat = simplificationParameters.unitMode.unitFormat,
      .unitDisplay = simplificationParameters.unitMode.unitDisplay,
      .angleUnit = simplificationParameters.user.angleUnit};
}

}  // namespace Poincare
