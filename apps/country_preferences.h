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

  constexpr CountryPreferences(AvailableExamModes availableExamModes, MethodForQuartiles methodForQuartiles, OutlierDefaultVisibility outliersStatus, Poincare::Preferences::UnitFormat unitFormat, HomeAppsLayout homeAppsLayout, DiscriminantSymbol discriminantSymbol) :
    m_availableExamModes(availableExamModes),
    m_homeAppsLayout(homeAppsLayout),
    m_unitFormat(unitFormat),
    m_methodForQuartiles(methodForQuartiles),
    m_outliersStatus(outliersStatus),
    m_discriminantSymbol(discriminantSymbol)
  {}

  constexpr AvailableExamModes availableExamModes() const { return m_availableExamModes; }
  constexpr MethodForQuartiles methodForQuartiles() const { return m_methodForQuartiles; }
  constexpr OutlierDefaultVisibility outliersStatus() const { return m_outliersStatus; }
  constexpr Poincare::Preferences::UnitFormat unitFormat() const { return m_unitFormat; }
  constexpr HomeAppsLayout homeAppsLayout() const { return m_homeAppsLayout; }
  constexpr const char * discriminantSymbol() const { return m_discriminantSymbol == DiscriminantSymbol::Delta ? "Δ" : "D"; }

private:
  const AvailableExamModes m_availableExamModes;
  const HomeAppsLayout m_homeAppsLayout;
  const Poincare::Preferences::UnitFormat m_unitFormat;
  const MethodForQuartiles m_methodForQuartiles;
  const OutlierDefaultVisibility m_outliersStatus;
  const DiscriminantSymbol m_discriminantSymbol;
};

#endif
