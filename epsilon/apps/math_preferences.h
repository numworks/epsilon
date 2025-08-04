#ifndef APPS_MATH_PREFERENCES_H
#define APPS_MATH_PREFERENCES_H

#include <apps/exam_mode_manager.h>
#include <apps/global_preferences.h>
#include <poincare/preferences.h>

/* [MathPreferences] is just a preferences accessor that regroup all the
 * "math" related settings.
 * TODO: get rid of it by directly accessing relevent preferences object in apps
 */
class MathPreferences : public GlobalPreferences {
 public:
  inline static MathPreferences* SharedPreferences() {
    return static_cast<MathPreferences*>(
        GlobalPreferences::SharedGlobalPreferences());
  }

  using GlobalPreferences::angleUnit;
  using GlobalPreferences::calculationPreferences;
  using GlobalPreferences::combinatoricSymbols;
  using GlobalPreferences::complexFormat;
  using GlobalPreferences::displayMode;
  using GlobalPreferences::mathPreferencesCheckSum;
  using GlobalPreferences::mixedFractionsAreEnabled;
  using GlobalPreferences::numberOfSignificantDigits;
  using GlobalPreferences::setAngleUnit;
  using GlobalPreferences::setComplexFormat;
  using GlobalPreferences::setDisplayMode;
  using GlobalPreferences::setNumberOfSignificantDigits;
};

#endif
