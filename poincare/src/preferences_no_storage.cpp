#include <assert.h>
#include <poincare/preferences.h>

namespace Poincare {

class DefaultPreferences : public Preferences::Interface {
 public:
  constexpr Preferences::CombinatoricSymbols combinatoricSymbols()
      const override {
    return Preferences::k_defaultCombinatoricSymbol;
  }
  constexpr bool mixedFractionsAreEnabled() const override {
    return static_cast<bool>(Preferences::k_defaultMixedFraction);
  }
  constexpr Preferences::LogarithmBasePosition logarithmBasePosition()
      const override {
    return Preferences::k_defaultLogarithmBasePosition;
  }
  Preferences::TranslateBuiltins translateBuiltins() const override {
    return Preferences::TranslateBuiltins::No;
  }
  void setTranslateBuiltins(Preferences::TranslateBuiltins translate) override {
  }
  constexpr Preferences::ParabolaParameter parabolaParameter() const override {
    return Preferences::k_defaultParabolaParameter;
  }
};

#ifdef POINCARE_TRANSLATE_BUILTINS
class DefaultPreferencesWithTranslateBuiltins : public DefaultPreferences {
  Preferences::TranslateBuiltins translateBuiltins() const override {
    return m_translatedBuiltins;
  }
  void setTranslateBuiltins(Preferences::TranslateBuiltins translate) override {
    m_translatedBuiltins = translate;
  }

 private:
  Preferences::TranslateBuiltins m_translatedBuiltins;
};
OMG::GlobalBox<DefaultPreferencesWithTranslateBuiltins> s_sharedPreferences;
#else
OMG::GlobalBox<DefaultPreferences> s_sharedPreferences;
#endif

void Preferences::Init(Preferences::Interface* preferences) {
  assert(preferences == nullptr);
  s_sharedPreferences.init();
  s_preferences = s_sharedPreferences.get();
}

}  // namespace Poincare
