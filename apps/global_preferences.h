#ifndef APPS_GLOBAL_PREFERENCES_H
#define APPS_GLOBAL_PREFERENCES_H

#include <apps/i18n.h>
#include <kandinsky/font.h>
#include <ion.h>
#include <poincare/preferences.h>

class GlobalPreferences {
public:
  static GlobalPreferences * sharedGlobalPreferences();
  I18n::Language language() const { return m_language; }
  void setLanguage(I18n::Language language) { m_language = language; }
  I18n::Country country() const { return m_country; }
  void setCountry(I18n::Country country);
  CountryPreferences::AvailableExamModes availableExamModes() const { return I18n::CountryPreferencesArray[static_cast<uint8_t>(m_country)].availableExamModes(); }
  CountryPreferences::MethodForQuartiles methodForQuartiles() const { return I18n::CountryPreferencesArray[static_cast<uint8_t>(m_country)].methodForQuartiles(); }
  CountryPreferences::OutlierDefaultVisibility outliersStatus() const { return I18n::CountryPreferencesArray[static_cast<uint8_t>(m_country)].outliersStatus(); }
  CountryPreferences::HistogramsOffset histogramOffset() const { return I18n::CountryPreferencesArray[static_cast<uint8_t>(m_country)].histogramOffset(); }
  Poincare::Preferences::UnitFormat unitFormat() const { return I18n::CountryPreferencesArray[static_cast<uint8_t>(m_country)].unitFormat(); }
  CountryPreferences::HomeAppsLayout homeAppsLayout() const { return I18n::CountryPreferencesArray[static_cast<uint8_t>(m_country)].homeAppsLayout(); }
  const char * discriminantSymbol() const { return I18n::CountryPreferencesArray[static_cast<uint8_t>(m_country)].discriminantSymbol(); }
  const char * yPredictedSymbol() const { return I18n::CountryPreferencesArray[static_cast<uint8_t>(m_country)].yPredictedSymbol(); }
  CountryPreferences::StatsRowsLayout statsRowsLayout() { return I18n::CountryPreferencesArray[static_cast<uint8_t>(m_country)].statsRowsLayout(); }
  Poincare::Preferences::CombinatoricSymbols combinatoricsSymbols() const { return I18n::CountryPreferencesArray[static_cast<uint8_t>(m_country)].combinatoricSymbols(); }
  CountryPreferences::ListsStatsOrderInToolbox listsStatsOrderInToolbox() const { return I18n::CountryPreferencesArray[static_cast<uint8_t>(m_country)].listsStatsOrderInToolbox(); }
  const char * openIntervalChar(bool left) const {
    /* This should be done by country instead of language. However, some
     * messages have intervals. The toolbox would have to contain buffers
     * instead of messages to apply the correct interval. We decided that for
     * now that it's a language preference only. */
    return I18n::translate(left ? I18n::Message::OpenLeftInterval : I18n::Message::OpenRightInterval);
  }

  bool showPopUp() const { return m_showPopUp; }
  void setShowPopUp(bool showPopUp) { m_showPopUp = showPopUp; }

  constexpr static int NumberOfBrightnessStates = 12;
  int brightnessLevel() const { return m_brightnessLevel; }
  void setBrightnessLevel(int brightnessLevel);

  const KDFont * font() const { return m_font; }
  void setFont(const KDFont * font) { m_font = font; }

private:
  static_assert(I18n::NumberOfLanguages > 0, "I18n::NumberOfLanguages is not superior to 0"); // There should already have been an error when processing an empty EPSILON_I18N flag
  static_assert(I18n::NumberOfCountries > 0, "I18n::NumberOfCountries is not superior to 0"); // There should already have been an error when processing an empty EPSILON_COUNTRIES flag
  GlobalPreferences() :
    m_language((I18n::Language)0),
    m_country((I18n::Country)0),
    m_showPopUp(true),
    m_brightnessLevel(Ion::Backlight::MaxBrightness),
    m_font(KDFont::LargeFont) {}

  I18n::Language m_language;
  I18n::Country m_country;
  bool m_showPopUp;
  int m_brightnessLevel;
  const KDFont * m_font;
};

#endif
