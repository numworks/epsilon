#pragma once

#include <omg/global_box.h>
#include <omg/unreachable.h>
#include <poincare/preferences.h>

class DefaultPreferences : public Poincare::Preferences::Interface {
 public:
  static OMG::GlobalBox<DefaultPreferences> s_sharedPreferences;

  constexpr Poincare::Preferences::CombinatoricSymbols combinatoricSymbols()
      const override {
    return Poincare::Preferences::k_defaultCombinatoricSymbol;
  }
  constexpr bool mixedFractionsAreEnabled() const override {
    return static_cast<bool>(Poincare::Preferences::k_defaultMixedFraction);
  }
  constexpr Poincare::Preferences::LogarithmBasePosition logarithmBasePosition()
      const override {
    return Poincare::Preferences::k_defaultLogarithmBasePosition;
  }
  Poincare::Preferences::TranslateBuiltins translateBuiltins() const override {
    return Poincare::Preferences::TranslateBuiltins::No;
  }
  constexpr Poincare::Preferences::ParabolaParameter parabolaParameter()
      const override {
    return Poincare::Preferences::k_defaultParabolaParameter;
  }
  constexpr Poincare::Preferences::MethodForQuartiles methodForQuartiles()
      const override {
    return Poincare::Preferences::k_defaultMethodForQuartiles;
  }
};
