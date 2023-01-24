#ifndef STATISTICS_USER_PREFERENCES_H
#define STATISTICS_USER_PREFERENCES_H

#include <apps/shared/double_pair_store.h>
#include <apps/shared/double_pair_store_preferences.h>
#include <apps/global_preferences.h>

namespace Statistics {

// These preferences are memoized in snapshot.

class UserPreferences : public Shared::DoublePairStorePreferences {
public:
  UserPreferences() :
    m_displayCumulatedFrequencies{false, false, false},
    m_barWidth(1.0),
    m_firstDrawnBarAbscissa(0.0),
    m_displayOutliers(GlobalPreferences::sharedGlobalPreferences->outliersStatus() == CountryPreferences::OutlierDefaultVisibility::Displayed)
    {}

  bool displayCumulatedFrequencies(int series) { return m_displayCumulatedFrequencies[series]; }
  void setDisplayCumulatedFrequencies(int series, bool value) { m_displayCumulatedFrequencies[series] = value; }

  double barWidth() { return m_barWidth; }
  void setBarWidth(double value) { m_barWidth = value; }

  double firstDrawnBarAbscissa() { return m_firstDrawnBarAbscissa; }
  void setFirstDrawnBarAbscissa(double value) { m_firstDrawnBarAbscissa = value; }

  bool displayOutliers() { return m_displayOutliers; }
  void setDisplayOutliers(bool value) { m_displayOutliers = value; }

private:
  // Cumulated frequencies column display
  bool m_displayCumulatedFrequencies[Shared::DoublePairStore::k_numberOfSeries];
  // Graph preferences
  double m_barWidth;
  double m_firstDrawnBarAbscissa;
  bool m_displayOutliers;
};

}

#endif
