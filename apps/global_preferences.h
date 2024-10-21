#ifndef APPS_GLOBAL_PREFERENCES_H
#define APPS_GLOBAL_PREFERENCES_H

#include <apps/i18n.h>
#include <ion.h>
#include <kandinsky/font.h>
#include <omg/global_box.h>
#include <poincare/preferences.h>

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

/* GlobalPreferences live in the Storage, which does not enforce alignment. The
 * packed attribute ensures the compiler will not emit instructions that require
 * the data to be aligned. */
class __attribute__((packed)) GlobalPreferences {
  friend OMG::GlobalBox<GlobalPreferences>;
  friend Ion::Storage::FileSystem;

 public:
  constexpr static char k_recordName[] = "gp";

  static GlobalPreferences* SharedGlobalPreferences();

  I18n::Language language() const { return m_language; }
  void setLanguage(I18n::Language language) { m_language = language; }
  I18n::Country country() const { return m_country; }
  void setCountry(I18n::Country country, bool updateSnapshots = true) {
    m_country = country;
    countryHasChanged(updateSnapshots);
  }
  void countryHasChanged(bool updateSnapshots = true);

  CountryPreferences::AvailableExamModes availableExamModes() const {
    return preferences().availableExamModes();
  }
  CountryPreferences::MethodForQuartiles methodForQuartiles() const {
    return preferences().methodForQuartiles();
  }
  CountryPreferences::OutlierDefaultVisibility outliersStatus() const {
    return preferences().outliersStatus();
  }
  CountryPreferences::HistogramsOffset histogramOffset() const {
    return preferences().histogramOffset();
  }
  Poincare::Preferences::UnitFormat unitFormat() const {
    return preferences().unitFormat();
  }
  CountryPreferences::HomeAppsLayout homeAppsLayout() const {
    return preferences().homeAppsLayout();
  }
  const char* discriminantSymbol() const {
    return preferences().discriminantSymbol();
  }
  const char* yPredictedSymbol() const {
    return preferences().yPredictedSymbol();
  }
  CountryPreferences::StatsRowsLayout statsRowsLayout() const {
    return preferences().statsRowsLayout();
  }
  Poincare::Preferences::CombinatoricSymbols combinatoricsSymbols() const {
    return preferences().combinatoricSymbols();
  }
  CountryPreferences::ListsStatsOrderInToolbox listsStatsOrderInToolbox()
      const {
    return preferences().listsStatsOrderInToolbox();
  }
  Poincare::Preferences::MixedFractions mixedFractions() const {
    return preferences().mixedFractions();
  }
  CountryPreferences::RegressionApp regressionAppVariant() const {
    return preferences().regressionAppVariant();
  }
  CountryPreferences::GraphTemplatesLayout graphTemplatesLayout() const {
    return preferences().graphTemplatesLayout();
  }
  Poincare::Preferences::LogarithmBasePosition logarithmBasePosition() const {
    return preferences().logarithmBasePosition();
  }
  Poincare::Preferences::LogarithmKeyEvent logarithmKeyEvent() const {
    return preferences().logarithmKeyEvent();
  }
  Poincare::Preferences::ParabolaParameter parabolaParameter() const {
    return preferences().parabolaParameter();
  }
  int sequencesInitialRank() const;

  const char* openIntervalChar(bool left) const {
    /* This should be done by country instead of language. However, some
     * messages have intervals. The toolbox would have to contain buffers
     * instead of messages to apply the correct interval. We decided that for
     * now that it's a language preference only. */
    return I18n::translate(left ? I18n::Message::OpenLeftInterval
                                : I18n::Message::OpenRightInterval);
  }

  bool showPopUp() const { return m_showPopUp; }
  void setShowPopUp(bool showPopUp) { m_showPopUp = showPopUp; }

  constexpr static int NumberOfBrightnessStates = 12;
  int brightnessLevel() const { return m_brightnessLevel; }
  void setBrightnessLevel(int brightnessLevel);

  int dimmingTime() const { return m_dimmingTime; }
  void setDimmingTime(int dimmingTime) { m_dimmingTime = dimmingTime; }

  KDFont::Size font() const { return m_font; }
  void setFont(KDFont::Size font) { m_font = font; }

  constexpr static uint32_t k_defaultDimmingTime = 30000;

 private:
  constexpr static uint8_t k_version = 0;

  GlobalPreferences()
      : m_version(k_version),
        m_brightnessLevel(Ion::Backlight::MaxBrightness),
        m_showPopUp(true),
        m_font(KDFont::Size::Large),
        m_dimmingTime(k_defaultDimmingTime) {
    setLanguage(I18n::Language::EN);
    setCountry(I18n::Country::WW, false);
  }

  const CountryPreferences& preferences() const {
    return I18n::CountryPreferencesArray[static_cast<uint8_t>(m_country)];
  }

  /* There should already have been an error when processing an empty
   * EPSILON_I18N flag */
  static_assert(I18n::NumberOfLanguages > 0,
                "I18n::NumberOfLanguages is not superior to 0");
  /* There should already have been an error when processing an empty
   * EPSILON_COUNTRIES flag */
  static_assert(I18n::NumberOfCountries > 0,
                "I18n::NumberOfCountries is not superior to 0");

#if __EMSCRIPTEN__
  CODE_GUARD(global_preferences, 3643843895,           //
             uint8_t m_version;                        //
             emscripten_align1_int m_brightnessLevel;  //
             I18n::Language m_language;                //
             I18n::Country m_country;                  //
             bool m_showPopUp;                         //
             KDFont::Size m_font;)
#else
  CODE_GUARD(global_preferences, 1861289878,  //
             uint8_t m_version;               //
             int m_brightnessLevel;           //
             I18n::Language m_language;       //
             I18n::Country m_country;         //
             bool m_showPopUp;                //
             KDFont::Size m_font;)
#endif

  // TODO: move inside CODE_GUARD?
  uint32_t m_dimmingTime;
};

#if PLATFORM_DEVICE
static_assert(sizeof(GlobalPreferences) == 13,
              "Class GlobalPreferences changed size");
#endif

#if __EMSCRIPTEN
/* GlobalPreferences live in the Storage which does not enforce alignment, so
 * make sure Emscripten cannot attempt unaligned accesses. */
static_assert(std::alignment_of<GlobalPreferences>() == 1);
#endif

#endif
