#ifndef COUNTRY_PREFERENCES_H
#define COUNTRY_PREFERENCES_H

#include <poincare/preferences.h>

class CountryPreferences {
public:
  enum class AvailableExamModes : uint8_t {
    StandardOnly,
    StandardAndDutch,
    PressToTestOnly,
    PortugueseOnly,
    All
  };

  enum class MethodForQuartiles : bool {
    MedianOfSublist,
    CumulatedFrequency
  };

  enum class OutlierDefaultVisibility : bool {
    Displayed,
    Hidden
  };

  enum class HomeAppsLayout : uint8_t {
    Default,
    Variant1,
    Variant2,
    Variant3,
    Variant4
  };

  enum class DiscriminantSymbol : bool {
    Delta,
    D
  };

  enum class HistogramsOffset : bool {
    None,
    OnIntegerValues
  };

  enum class MeanSymbol : bool {
    Default,
    WithMu,
  };

  enum class YPredictedSymbol : bool {
    Y,
    YHat
  };

  enum class StatsRowsLayout : uint8_t {
    Default,
    Variant1,
  };

  constexpr CountryPreferences(AvailableExamModes availableExamModes, MethodForQuartiles methodForQuartiles, OutlierDefaultVisibility outliersStatus, Poincare::Preferences::UnitFormat unitFormat, HomeAppsLayout homeAppsLayout, DiscriminantSymbol discriminantSymbol, HistogramsOffset histogramOffset, MeanSymbol meanSymbol, YPredictedSymbol yPredictedSymbol, StatsRowsLayout statsRowsLayout, Poincare::Preferences::CombinatoricSymbols combinatoricSymbols) :
    m_availableExamModes(availableExamModes),
    m_homeAppsLayout(homeAppsLayout),
    m_unitFormat(unitFormat),
    m_methodForQuartiles(methodForQuartiles),
    m_outliersStatus(outliersStatus),
    m_discriminantSymbol(discriminantSymbol),
    m_histogramOffset(histogramOffset),
    m_meanSymbol(meanSymbol),
    m_yPredictedSymbol(yPredictedSymbol),
    m_statsRowsLayout(statsRowsLayout),
    m_combinatoricSymbols(combinatoricSymbols)
  {}

  constexpr AvailableExamModes availableExamModes() const { return m_availableExamModes; }
  constexpr MethodForQuartiles methodForQuartiles() const { return m_methodForQuartiles; }
  constexpr OutlierDefaultVisibility outliersStatus() const { return m_outliersStatus; }
  constexpr Poincare::Preferences::UnitFormat unitFormat() const { return m_unitFormat; }
  constexpr HomeAppsLayout homeAppsLayout() const { return m_homeAppsLayout; }
  constexpr HistogramsOffset histogramOffset() const { return m_histogramOffset; }
  constexpr const char * discriminantSymbol() const { return m_discriminantSymbol == DiscriminantSymbol::Delta ? "Δ" : "D"; }
  constexpr MeanSymbol meanSymbol() const { return m_meanSymbol; }
  constexpr const char * yPredictedSymbol() const { return m_yPredictedSymbol == YPredictedSymbol::Y ? "y" : "y\xCC\x82"; }
  constexpr StatsRowsLayout statsRowsLayout() const { return m_statsRowsLayout; }
  constexpr Poincare::Preferences::CombinatoricSymbols combinatoricSymbols() const { return m_combinatoricSymbols; }

private:
  const AvailableExamModes m_availableExamModes;
  const HomeAppsLayout m_homeAppsLayout;
  const Poincare::Preferences::UnitFormat m_unitFormat;
  const MethodForQuartiles m_methodForQuartiles;
  const OutlierDefaultVisibility m_outliersStatus;
  const DiscriminantSymbol m_discriminantSymbol;
  const HistogramsOffset m_histogramOffset;
  const MeanSymbol m_meanSymbol;
  const YPredictedSymbol m_yPredictedSymbol;
  const StatsRowsLayout m_statsRowsLayout;
  const Poincare::Preferences::CombinatoricSymbols m_combinatoricSymbols;
};

#endif
