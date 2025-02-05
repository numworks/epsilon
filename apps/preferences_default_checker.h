#ifndef APPS_PREFERENCES_DEFAULT_CHECKER_H
#define APPS_PREFERENCES_DEFAULT_CHECKER_H

/* Ensure consistency between default values of Poincare::Preferences and
 * CountryPreferences of the default country.
 * Poincare::Preferences are independent from the Epsilon preferences
 * (GlobalPreferences). However, the default CountryPreferences (a subset of the
 * GlobalPreferences), which are the CountryPreferences of the default
 * "I18n::Country::WW" country, should be consistent with the default values of
 * country-dependent Poincare::Preferences. This ensures consistent results when
 * testing Poincare in isolation with default Poincare settings, and when
 * testing an Epsilon app with default country settings. */

#include <poincare/preferences.h>

#include "global_preferences.h"

static_assert(
    GlobalPreferences::defaultCountryPreferences().combinatoricSymbols() ==
    Poincare::Preferences::CombinatoricSymbols::Default);

static_assert(GlobalPreferences::defaultCountryPreferences().mixedFractions() ==
              Poincare::Preferences::k_defaultMixedFraction);

static_assert(
    GlobalPreferences::defaultCountryPreferences().logarithmBasePosition() ==
    Poincare::Preferences::k_defaultLogarithmBasePosition);

static_assert(
    GlobalPreferences::defaultCountryPreferences().logarithmKeyEvent() ==
    Poincare::Preferences::LogarithmKeyEvent::Default);

static_assert(
    GlobalPreferences::defaultCountryPreferences().parabolaParameter() ==
    Poincare::Preferences::ParabolaParameter::Default);

#endif
