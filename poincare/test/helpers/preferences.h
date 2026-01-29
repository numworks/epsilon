#pragma once

#include <omg/global_box.h>
#include <poincare/preferences.h>

namespace Poincare {

struct MutablePreferences final : public Preferences::Interface {
  static OMG::GlobalBox<MutablePreferences> s_instance;

  Preferences::CombinatoricSymbols m_combinatoricSymbols =
      Preferences::k_defaultCombinatoricSymbol;
  Preferences::MixedFractions m_mixedFractions =
      Preferences::k_defaultMixedFraction;
  Preferences::LogarithmBasePosition m_logarithmBasePosition =
      Preferences::k_defaultLogarithmBasePosition;
  Preferences::TranslateBuiltins m_translateBuiltins =
      Preferences::TranslateBuiltins::No;
  Preferences::ParabolaParameter m_parabolaParameter =
      Preferences::k_defaultParabolaParameter;
  Preferences::MethodForQuartiles m_methodForQuartiles =
      Preferences::k_defaultMethodForQuartiles;

  Preferences::CombinatoricSymbols combinatoricSymbols() const override {
    return m_combinatoricSymbols;
  }
  bool mixedFractionsAreEnabled() const override {
    return m_mixedFractions == Preferences::MixedFractions::Enabled;
  }
  Preferences::LogarithmBasePosition logarithmBasePosition() const override {
    return m_logarithmBasePosition;
  }
  Preferences::TranslateBuiltins translateBuiltins() const override {
    return m_translateBuiltins;
  }
  Preferences::ParabolaParameter parabolaParameter() const override {
    return m_parabolaParameter;
  }
  Preferences::MethodForQuartiles methodForQuartiles() const override {
    return m_methodForQuartiles;
  }
};

}  // namespace Poincare
