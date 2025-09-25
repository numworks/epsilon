#pragma once

#include <apps/i18n.h>
#include <escher/layout_preferences.h>
#include <ion.h>
#include <kandinsky/font.h>
#include <omg/global_box.h>
#include <omg/unreachable.h>

#include "apps_container_helper.h"

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

/* [GlobalPreferences] is a singleton, its unique instance is accessible via
 * [SharedGlobalPreferences()].
 * The preferences are stored in Storage via a struct [GlobalPreferencesData].
 * See comment before CODE_GUARD for more context. */
class GlobalPreferences : public Escher::LayoutPreferences::Interface,
                          public Poincare::Preferences::Interface {
  /* Friend class for test purposes that can access the GlobalPreferences
   * private constructor. */
  friend class GlobalPreferencesTestBuilder;

 public:
  constexpr static I18n::Country k_defaultCountry = I18n::Country::WW;

  constexpr static const CountryPreferences& defaultCountryPreferences() {
    return I18n::CountryPreferencesArray[static_cast<uint8_t>(
        k_defaultCountry)];
  }

  static GlobalPreferences* SharedGlobalPreferences();
  static void Init();

  I18n::Language language() const { return s_data->m_language; }
  void setLanguage(I18n::Language language) { s_data->m_language = language; }
  I18n::Country country() const { return s_data->m_country; }
  void setCountry(I18n::Country country, bool updateSnapshots = true) {
    s_data->m_country = country;
    AppsContainerHelper::notifyCountryChangeToSnapshots();
  }

  CountryPreferences::AvailableExamModes availableExamModes() const {
    return countryPreferences().availableExamModes();
  }
  Poincare::Preferences::MethodForQuartiles methodForQuartiles()
      const override {
    return countryPreferences().methodForQuartiles();
  }
  CountryPreferences::OutlierDefaultVisibility outliersStatus() const {
    return countryPreferences().outliersStatus();
  }
  CountryPreferences::HistogramsOffset histogramOffset() const {
    return countryPreferences().histogramOffset();
  }
  Poincare::Preferences::UnitFormat unitFormat() const {
    return countryPreferences().unitFormat();
  }
  CountryPreferences::HomeAppsLayout homeAppsLayout() const {
    return countryPreferences().homeAppsLayout();
  }
  const char* discriminantSymbol() const {
    return countryPreferences().discriminantSymbol();
  }
  const char* yPredictedSymbol() const {
    return countryPreferences().yPredictedSymbol();
  }
  CountryPreferences::StatsRowsLayout statsRowsLayout() const {
    return countryPreferences().statsRowsLayout();
  }
  Poincare::Preferences::CombinatoricSymbols combinatoricSymbols()
      const override {
    return countryPreferences().combinatoricSymbols();
  }
  CountryPreferences::ListsStatsOrderInToolbox listsStatsOrderInToolbox()
      const {
    return countryPreferences().listsStatsOrderInToolbox();
  }
  Poincare::Preferences::MixedFractions mixedFractions() const {
    return countryPreferences().mixedFractions();
  }
  bool mixedFractionsAreEnabled() const override {
    return countryPreferences().mixedFractions() ==
           Poincare::Preferences::MixedFractions::Enabled;
  }
  CountryPreferences::RegressionApp regressionAppVariant() const {
    return countryPreferences().regressionAppVariant();
  }
  CountryPreferences::GraphTemplatesLayout graphTemplatesLayout() const {
    return countryPreferences().graphTemplatesLayout();
  }
  Poincare::Preferences::LogarithmBasePosition logarithmBasePosition()
      const override {
    return countryPreferences().logarithmBasePosition();
  }
  Escher::LayoutPreferences::LogarithmKeyEvent logarithmKeyEvent()
      const override {
    return countryPreferences().logarithmKeyEvent();
  }
  Poincare::Preferences::ParabolaParameter parabolaParameter() const override {
    return countryPreferences().parabolaParameter();
  }
  CountryPreferences::SolverDoubleRootName solverDoubleRootName() const {
    return countryPreferences().solverDoubleRootName();
  }
  CountryPreferences::GraphTemplateDefault graphTemplateDefault() const {
    return countryPreferences().graphTemplateDefault();
  }
  CountryPreferences::StepAdjustmentWarning stepAdjustmentWarning() const {
    return countryPreferences().stepAdjustmentWarning();
  }
  int sequencesInitialRank() const;
  // NOTE: this in only used in Scandium
  constexpr Poincare::Preferences::TranslateBuiltins translateBuiltins()
      const override {
    return Poincare::Preferences::TranslateBuiltins::No;
  }
  void setTranslateBuiltins(Poincare::Preferences::TranslateBuiltins) override {
    OMG::unreachable();
  };

  const char* openIntervalChar(bool left) const {
    /* This should be done by country instead of language. However, some
     * messages have intervals. The toolbox would have to contain buffers
     * instead of messages to apply the correct interval. We decided that for
     * now that it's a language preference only. */
    return I18n::translate(left ? I18n::Message::OpenLeftInterval
                                : I18n::Message::OpenRightInterval);
  }

  bool showPopUp() const { return s_data->m_combinedPreferences.showPopUp; }
  void setShowPopUp(bool showPopUp) {
    s_data->m_combinedPreferences.showPopUp = showPopUp;
  }

  constexpr static int NumberOfBrightnessStates = 12;
  int brightnessLevel() const { return s_data->m_brightnessLevel; }
  void setBrightnessLevel(int brightnessLevel);

  int dimmingTime() const { return s_data->m_dimmingTime; }
  void setDimmingTime(int dimmingTime) { s_data->m_dimmingTime = dimmingTime; }

  KDFont::Size font() const { return s_data->m_combinedPreferences.font; }
  void setFont(KDFont::Size font) { s_data->m_combinedPreferences.font = font; }

  enum class EditionMode : bool {
    Edition2D = 0,
    Edition1D = 1,
  };
  EditionMode editionMode() const {
    return s_data->m_combinedPreferences.editionMode;
  }
  bool linearMode() const override {
    return editionMode() == EditionMode::Edition1D;
  }
  void setEditionMode(EditionMode editionMode) {
    s_data->m_combinedPreferences.editionMode = editionMode;
  }

  // In milliseconds
  constexpr static uint32_t k_defaultDimmingTime = 30 * 1000;

  Poincare::Preferences::CalculationPreferences calculationPreferences() const {
    return s_data->m_calculationPreferences;
  }
  Poincare::AngleUnit angleUnit() const {
    return s_data->m_calculationPreferences.angleUnit;
  }
  void setAngleUnit(Poincare::AngleUnit angleUnit) {
    s_data->m_calculationPreferences.angleUnit = angleUnit;
  }
  Poincare::Preferences::PrintFloatMode displayMode() const override {
    return s_data->m_calculationPreferences.displayMode;
  }
  void setDisplayMode(Poincare::Preferences::PrintFloatMode displayMode) {
    s_data->m_calculationPreferences.displayMode = displayMode;
  }
  Poincare::ComplexFormat complexFormat() const {
    return s_data->m_calculationPreferences.complexFormat;
  }
  void setComplexFormat(Poincare::ComplexFormat complexFormat) {
    s_data->m_calculationPreferences.complexFormat = complexFormat;
  }
  uint8_t numberOfSignificantDigits() const {
    return s_data->m_calculationPreferences.numberOfSignificantDigits;
  }
  void setNumberOfSignificantDigits(uint8_t numberOfSignificantDigits) {
    s_data->m_calculationPreferences.numberOfSignificantDigits =
        numberOfSignificantDigits;
  }
  uint32_t mathPreferencesCheckSum() const {
    return (static_cast<uint32_t>(complexFormat()) << 8) +
           static_cast<uint32_t>(angleUnit());
  }

  static GlobalPreferences GlobalPreferencesInstance;

 private:
  constexpr static uint8_t k_version = 2;

#if __EMSCRIPTEN__
  using BrightnessType = emscripten_align1_int;
  using DimmingTimeType = emscripten_align1_int;
#else
  using BrightnessType = int;
  using DimmingTimeType = uint32_t;
#endif

  constexpr static BrightnessType k_defaultBrightnessLevel =
      Ion::Backlight::MaxBrightness;
  constexpr static I18n::Language k_defaultLanguage = I18n::Language::EN;  //
  constexpr static bool k_defaultShowPopUp = true;                         //
  constexpr static KDFont::Size k_defaultFont = KDFont::Size::Large;       //
  constexpr static EditionMode k_defaultEditionMode = EditionMode::Edition2D;

  /* GlobalPreferences is a singleton, hence the private constructor. The unique
   * instance can be accessed through the
   * GlobalPreferences::SharedGlobalPreferences() pointer. */
  GlobalPreferences() = default;

  const CountryPreferences& countryPreferences() const {
    return I18n::CountryPreferencesArray[static_cast<uint8_t>(
        s_data->m_country)];
  }

  /* There should already have been an error when processing an empty
   * EPSILON_I18N flag */
  static_assert(I18n::NumberOfLanguages > 0,
                "I18n::NumberOfLanguages is not superior to 0");
  /* There should already have been an error when processing an empty
   * EPSILON_COUNTRIES flag */
  static_assert(I18n::NumberOfCountries > 0,
                "I18n::NumberOfCountries is not superior to 0");

  CODE_GUARD(
      combined_preferences, 1870460707,  //
      struct __attribute__((packed)) CombinedPreferences {
        bool showPopUp : 1;
        KDFont::Size font : 1;
        EditionMode editionMode : 1;
        uint8_t padding : OMG::BitHelper::numberOfBitsIn<uint8_t>() - 3;
        bool operator==(const CombinedPreferences&) const = default;
      };)
  constexpr static CombinedPreferences k_defaultCombinedPreferences = {
      k_defaultShowPopUp, k_defaultFont, k_defaultEditionMode, 0};

  /* Instead of storing [GlobalPreferences] to the Storage, we use a smaller
   * struct [GlobalPreferencesData], this is because [m_version] field need to
   * be located at byte 0 in the stored record for the website.
   * But the vtables of [GlobalPreferences] due to inheritance causes an
   * unwanted offset in the stored version number location */
  CODE_GUARD(
      global_preferences_data, 1321767116,  //
      struct __attribute__((packed)) GlobalPreferencesData {
        friend OMG::GlobalBox<GlobalPreferences>;
        friend Ion::Storage::FileSystem;
        constexpr static char k_recordName[] = "gp";

        uint8_t m_version = k_version;
        BrightnessType m_brightnessLevel = k_defaultBrightnessLevel;
        I18n::Language m_language = k_defaultLanguage;
        I18n::Country m_country = k_defaultCountry;
        CombinedPreferences m_combinedPreferences =
            k_defaultCombinedPreferences;
        Poincare::Preferences::CalculationPreferences m_calculationPreferences =
            Poincare::Preferences::k_defaultCalculationPreferences;
        DimmingTimeType m_dimmingTime = k_defaultDimmingTime;
        bool operator==(const GlobalPreferencesData&) const = default;
      };)

#if PLATFORM_DEVICE
  CODE_GUARD(
      global_preferences_size, 1681677831,  //
      static_assert(sizeof(Poincare::Preferences::CalculationPreferences) == 2);
      static_assert(sizeof(CombinedPreferences) == 1); static_assert(
          sizeof(GlobalPreferencesData) == 14,
          "Class GlobalPreferencesData changed size, might affect website");  //
  )
#endif
  static GlobalPreferencesData* s_data;

#if __EMSCRIPTEN
  /* GlobalPreferencesData lives in the Storage which does not enforce
   * alignment, so make sure Emscripten cannot attempt unaligned accesses. */
  static_assert(std::alignment_of<GlobalPreferencesData>() == 1);
#endif
};

// TODO?
// inline constexpr GlobalPreferences& SharedGlobalPreferences =
//     GlobalPreferences::GlobalPreferencesInstance;
