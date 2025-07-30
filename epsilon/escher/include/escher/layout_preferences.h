#ifndef ESCHER_PREFERENCES_H
#define ESCHER_PREFERENCES_H

#include <poincare/preferences.h>

namespace Escher {

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

  static void Init(const Interface* prefs) {
    assert(prefs);
    s_preferences = prefs;
  }

  const Interface* operator->() const {
    assert(s_preferences);
    return s_preferences;
  }

 private:
  LayoutPreferences operator=(LayoutPreferences) = delete;
  static const Interface* s_preferences;
};

extern const LayoutPreferences SharedPreferences;

}  // namespace Escher
#endif
