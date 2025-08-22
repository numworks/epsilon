#pragma once

#include <poincare/preferences.h>

namespace Escher {

/* [LayoutPreferences] is a singleton accessible via the [SharedPreferences]
 * instance.
 * The object gives access to an implementation of [Interface], which provides
 * settings/preferences for Escher layouting and whatnot. */
class LayoutPreferences {
 public:
  enum class LogarithmKeyEvent : char { Default, WithBaseTen };

  class Interface {
   public:
    virtual bool linearMode() const = 0;
    virtual LogarithmKeyEvent logarithmKeyEvent() const = 0;
    virtual Poincare::Preferences::PrintFloatMode displayMode() const = 0;
    bool operator==(const Interface&) const = default;
  };

  static void Init(const Interface* preferences) {
    assert(preferences);
    s_preferences = preferences;
  }

  const Interface* operator->() const {
    assert(s_preferences);
    return s_preferences;
  }

  static const LayoutPreferences PreferencesInstance;

 private:
  LayoutPreferences() = default;
  static const Interface* s_preferences;
};
static_assert(sizeof(LayoutPreferences) == 1,
              "LayoutPreferences should not contain anything");

inline constexpr const LayoutPreferences& SharedPreferences =
    LayoutPreferences::PreferencesInstance;

}  // namespace Escher
