#ifndef ESCHER_MATH_PREFERENCES_H
#define ESCHER_MATH_PREFERENCES_H

#include <poincare/preferences.h>

namespace Escher {

/**
 * TODO: outdated comment, update
 * This is the Escher pendant of Apps::MathPreferences.
 * The sole purpose of this class is to provide a way to access some protected
 * methods of Poincare::Preferences.
 * These methods are not meant to be used inside Poincare and thus are
 * protected, but the current implementation of the user preferences in Escher
 * relies on them.
 * Ideally, we would refactor entirely the Poincare::Preferences class to split
 * it between the true "preferences" part (that currently can be accessed in
 * Poincare) and the "parameters" part (like ComplexFormat or AngleUnit, that
 * are constantly changing during computations and are meant to be passed to the
 * methods instead of being stored in a global state).
 * This class is a temporary solution to allow the current implementation to
 * work while protecting the Poincare::Preferences class."
 * It's named LayoutPreferences to avoid name conflicts with
 * Poincare::Preferences See comment in poincare/preferences.h.
 */

enum class LogarithmKeyEvent : char { Default, WithBaseTen };

class LayoutPreferencesInterface {
 public:
  virtual bool linearMode() const = 0;
  virtual Escher::LogarithmKeyEvent logarithmKeyEvent() const = 0;
  virtual Poincare::Preferences::PrintFloatMode displayMode() const = 0;
  bool operator==(const LayoutPreferencesInterface&) const = default;
};

class LayoutPreferences {
 public:
  inline static LayoutPreferencesInterface* SharedPreferences() {
    assert(s_preferences);
    return s_preferences;
  }

  static void Init(LayoutPreferencesInterface* preferences) {
    s_preferences = preferences;
  }

 private:
  static LayoutPreferencesInterface* s_preferences;
};

}  // namespace Escher
#endif
