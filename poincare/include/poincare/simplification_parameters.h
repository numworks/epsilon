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
  UserParameters user = {};
  ProjectionPolicy policy = {};
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
  UserParameters user = {};
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
  return ProjectionParameters{
      .user = simplificationParameters.user,
      .policy = simplificationParameters.projectionPolicy};
}

inline ProjectionParameters ToProjectionParameters(
    const BeautificationParameters& beautificationParameters) {
  return ProjectionParameters{
      .user = beautificationParameters.user,
      .policy = {.symbolic = SymbolicComputation::KeepAllSymbols,
                 .strategy = Strategy::Default}};
}

inline BeautificationParameters ToBeautificationParameters(
    const SimplificationParameters& simplificationParameters) {
  return BeautificationParameters{
      .user = simplificationParameters.user,
      .reduction = simplificationParameters.reduction};
}

inline UnitParameters ToUnitParameters(
    const SimplificationParameters& simplificationParameters) {
  return UnitParameters{
      .unitFormat = simplificationParameters.unitMode.unitFormat,
      .unitDisplay = simplificationParameters.unitMode.unitDisplay,
      .angleUnit = simplificationParameters.user.angleUnit};
}

}  // namespace Poincare
