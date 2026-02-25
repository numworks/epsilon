#pragma once

#include "preferences.h"
#include "simplification_policies.h"
#include "user_settings.h"

namespace Poincare {

struct ProjectionParameters {
  UserSettings user = {};
  ProjectionPolicy policy = {};
};

struct UnitModeParameters {
  Preferences::UnitFormat unitFormat = Preferences::UnitFormat::Metric;
  UnitDisplay unitDisplay = UnitDisplay::MainOutput;
};

struct SimplificationParameters {
  UserSettings user = {};
  ProjectionPolicy projectionPolicy = {};
  ReductionPolicy reduction = {};
  UnitModeParameters unitMode = {};
};

struct BeautificationParameters {
  UserSettings user = {};
  ReductionPolicy reduction = {};
  UnitModeParameters unitMode = {};
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
      .reduction = simplificationParameters.reduction,
      .unitMode = simplificationParameters.unitMode};
}

inline UnitParameters ToUnitParameters(
    const BeautificationParameters& beautificationParameters) {
  return UnitParameters{
      .unitFormat = beautificationParameters.unitMode.unitFormat,
      .unitDisplay = beautificationParameters.unitMode.unitDisplay,
      .angleUnit = beautificationParameters.user.angleUnit,
  };
}

}  // namespace Poincare
