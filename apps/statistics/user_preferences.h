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
    m_displayOutliers(GlobalPreferences::sharedGlobalPreferences()->outliersStatus() == CountryPreferences::OutlierDefaultVisibility::Displayed),
    m_curveDrawnOver(false),
    m_curveMu(3.0),
    m_curveSigma(1.0)
    {}

  bool displayCumulatedFrequencies(int series) { return m_displayCumulatedFrequencies[series]; }
  void setDisplayCumulatedFrequencies(int series, bool value) { m_displayCumulatedFrequencies[series] = value; }

  double barWidth() { return m_barWidth; }
  void setBarWidth(double value) { m_barWidth = value; }

  double firstDrawnBarAbscissa() { return m_firstDrawnBarAbscissa; }
  void setFirstDrawnBarAbscissa(double value) { m_firstDrawnBarAbscissa = value; }

  bool displayOutliers() { return m_displayOutliers; }
  void setDisplayOutliers(bool value) { m_displayOutliers = value; }

  bool drawCurveOverHistogram() { return m_curveDrawnOver; }
  void setCurveOverHistogram(bool value) { m_curveDrawnOver = value; }

  double normalCurveOverHistogramMu() { return m_curveMu; }
  void setNormalCurveOverHistogramMu(double value) { m_curveMu = value; }

  double normalCurveOverHistogramSigma() { return m_curveSigma; }
  void setNormalCurveOverHistogramSigma(double value) { m_curveSigma = value; }

private:
  // Cumulated frequencies column display
  bool m_displayCumulatedFrequencies[Shared::DoublePairStore::k_numberOfSeries];
  // Graph preferences
  double m_barWidth;
  double m_firstDrawnBarAbscissa;
  bool m_displayOutliers;
  bool m_curveDrawnOver;
  double m_curveMu;
  double m_curveSigma;
};

}

#endif
