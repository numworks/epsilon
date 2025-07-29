#ifndef ESCHER_MATH_PREFERENCES_H
#define ESCHER_MATH_PREFERENCES_H

#include <apps/global_preferences.h>
#include <poincare/preferences.h>

#include "layout_preferences_enum.h"

namespace Escher {

/**
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
/* TODO: remove dependency to apps. One solution could be call Escher::Init with
 * a pointer to an abstart class that declares the necessary methods */
class LayoutPreferences : public GlobalPreferences {
 public:
  inline static LayoutPreferences* SharedPreferences() {
    return static_cast<LayoutPreferences*>(
        GlobalPreferences::SharedGlobalPreferences());
  }

  Poincare::Preferences::PrintFloatMode displayMode() {
    return GlobalPreferences::SharedGlobalPreferences()->displayMode();
  }

  Escher::LogarithmKeyEvent logarithmKeyEvent() {
    return GlobalPreferences::SharedGlobalPreferences()->logarithmKeyEvent();
  }

  static bool LinearMode() {
    return GlobalPreferences::SharedGlobalPreferences()->editionMode() ==
           GlobalPreferences::EditionMode::Edition1D;
  }
};

static_assert(sizeof(LayoutPreferences) == sizeof(GlobalPreferences));

}  // namespace Escher
#endif
