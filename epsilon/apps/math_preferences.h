#ifndef APPS_MATH_PREFERENCES_H
#define APPS_MATH_PREFERENCES_H

#include <apps/global_preferences.h>
#include <poincare/preferences.h>

/**
 * This is the Apps pendant of Escher::LayoutPreferences.
 * The sole purpose of this class is to provide a way to access some protected
 * methods of Poincare::Preferences.
 * These methods are not meant to be used inside Poincare and thus are
 * protected, but the current implementation of the user preferences in Apps
 * relies on them.
 * Ideally, we would refactor entirely the Poincare::Preferences class to split
 * it between the true "preferences" part (that currently can be accessed in
 * Poincare) and the "parameters" part (like ComplexFormat or AngleUnit, that
 * are constantly changing during computations and are meant to be passed to the
 * methods instead of being stored in a global state).
 * This class is a temporary solution to allow the current implementation to
 * work while protecting the Poincare::Preferences class."
 * It's named MathPreferences to avoid name conflicts with Poincare::Preferences
 * See comment in poincare/preferences.h.
 */
class MathPreferences : public GlobalPreferences {
 public:
  inline static MathPreferences* SharedPreferences() {
    return static_cast<MathPreferences*>(
        GlobalPreferences::SharedGlobalPreferences());
  }

  Poincare::ExamMode examMode() const {
    return Poincare::Preferences::SharedPreferences()->examMode();
  };
  void setExamMode(Poincare::ExamMode examMode) {
    Poincare::Preferences::SharedPreferences()->setExamMode(examMode);
  };
  bool forceExamModeReload() const {
    return Poincare::Preferences::SharedPreferences()->forceExamModeReload();
  }
  bool mixedFractionsAreEnabled() const {
    return Poincare::Preferences::SharedPreferences()
        ->mixedFractionsAreEnabled();
  }
  void enableMixedFractions(Poincare::Preferences::MixedFractions enable) {
    Poincare::Preferences::SharedPreferences()->enableMixedFractions(enable);
  }
  Poincare::Preferences::CombinatoricSymbols combinatoricSymbols() const {
    return Poincare::Preferences::SharedPreferences()->combinatoricSymbols();
  }
  void setCombinatoricSymbols(
      Poincare::Preferences::CombinatoricSymbols combinatoricSymbols) {
    Poincare::Preferences::SharedPreferences()->setCombinatoricSymbols(
        combinatoricSymbols);
  }

  void setLogarithmBasePosition(
      Poincare::Preferences::LogarithmBasePosition position) {
    Poincare::Preferences::SharedPreferences()->setLogarithmBasePosition(
        position);
  }
  void setParabolaParameter(
      Poincare::Preferences::ParabolaParameter parameter) {
    Poincare::Preferences::SharedPreferences()->setParabolaParameter(parameter);
  }

  using GlobalPreferences::angleUnit;
  using GlobalPreferences::calculationPreferences;
  using GlobalPreferences::complexFormat;
  using GlobalPreferences::displayMode;
  using GlobalPreferences::mathPreferencesCheckSum;
  using GlobalPreferences::numberOfSignificantDigits;
  using GlobalPreferences::setAngleUnit;
  using GlobalPreferences::setComplexFormat;
  using GlobalPreferences::setDisplayMode;
  using GlobalPreferences::setNumberOfSignificantDigits;
};

static_assert(sizeof(MathPreferences) == sizeof(GlobalPreferences));

#endif
