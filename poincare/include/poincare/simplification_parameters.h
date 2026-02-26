#pragma once

#include "preferences.h"
#include "simplification_policies.h"
#include "user_settings.h"

namespace Poincare {

struct ProjectionParameters {
  UserSettings user = {};
  ProjectionPolicy policy = {};
};

struct SimplificationParameters {
  UserSettingsWithUnit user = {};
  ProjectionPolicy projectionPolicy = {};
  ReductionPolicy reductionPolicy = {};
  UnitDisplay unitPolicy = {};
};

struct BeautificationParameters {
  UserSettingsWithUnit user = {};
  ReductionPolicy reductionPolicy = {};
  UnitDisplay unitPolicy = {};
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
      .reductionPolicy = simplificationParameters.reductionPolicy,
      .unitPolicy = simplificationParameters.unitPolicy};
}

inline UnitParameters ToUnitParameters(
    const BeautificationParameters& beautificationParameters) {
  return UnitParameters{
      .unitFormat = beautificationParameters.user.unitFormat,
      .unitDisplay = beautificationParameters.unitPolicy,
      .angleUnit = beautificationParameters.user.angleUnit,
  };
}

}  // namespace Poincare
