#ifndef COUNTRY_PREFERENCES_H
#define COUNTRY_PREFERENCES_H

#include <poincare/preferences.h>

class CountryPreferences {
public:
  enum class AvailableExamModes : uint8_t {
    StandardOnly,
    All
  };

  enum class MethodForQuartiles : uint8_t {
    MedianOfSublist,
    CumulatedFrequency
  };

  enum class HomeAppsLayout : uint8_t {
    Default,
    HidePython,
  };

  constexpr CountryPreferences(AvailableExamModes availableExamModes, MethodForQuartiles methodForQuartiles, Poincare::Preferences::UnitFormat unitFormat, HomeAppsLayout homeAppsLayout) :
    m_availableExamModes(availableExamModes),
    m_methodForQuartiles(methodForQuartiles),
    m_unitFormat(unitFormat),
    m_homeAppsLayout(homeAppsLayout)
  {}

  constexpr AvailableExamModes availableExamModes() const { return m_availableExamModes; }
  constexpr MethodForQuartiles methodForQuartiles() const { return m_methodForQuartiles; }
  constexpr Poincare::Preferences::UnitFormat unitFormat() const { return m_unitFormat; }
  constexpr HomeAppsLayout homeAppsLayout() const { return m_homeAppsLayout; }

private:
  const AvailableExamModes m_availableExamModes;
  const MethodForQuartiles m_methodForQuartiles;
  const Poincare::Preferences::UnitFormat m_unitFormat;
  const HomeAppsLayout m_homeAppsLayout;
};

#endif
