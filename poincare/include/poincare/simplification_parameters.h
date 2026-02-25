#pragma once

#include "math_options.h"
#include "preferences.h"
#include "user_parameters.h"

namespace Poincare {

struct ProjectionPolicy {
  SymbolicComputation symbolic = SymbolicComputation::KeepAllSymbols;
  Strategy strategy = Strategy::Default;
};

struct ProjectionParameters {
  UserParameters user = {};
  ProjectionPolicy policy = {};
};

struct ReductionPolicy {
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
  ReductionPolicy reduction = {};
  UnitModeParameters unitMode = {};
};

struct BeautificationParameters {
  UserParameters user = {};
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
