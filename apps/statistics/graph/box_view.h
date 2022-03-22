#ifndef STATISTICS_BOX_VIEW_H
#define STATISTICS_BOX_VIEW_H

#include "../store.h"
#include "box_range.h"
#include <apps/constant.h>
#include <apps/shared/curve_view.h>

namespace Statistics {

class BoxController;

class BoxView : public Shared::CurveView {
public:
  enum class Quantile : int {
    None = -1,
    Min = 0,
    FirstQuartile = 1,
    Median = 2,
    ThirdQuartile = 3,
    Max = 4
  };
  BoxView(Store * store, int series, Shared::BannerView * bannerView, Quantile * selectedQuantile, KDColor color, KDColor lightColor);
  Quantile selectedQuantile() const { return *m_selectedQuantile; }
  bool selectQuantile(int selectedQuantile);
  int series() const { return m_series; }
  void reloadQuantile();

  // CurveView
  void reload(bool resetInterrupted = false, bool force = false) override;

  // View
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  static constexpr KDCoordinate k_quantileBarWidth = 2;
  static constexpr KDCoordinate k_twoBoxesHeight = 40;
  static constexpr KDCoordinate k_threeBoxesHeight = 27;

  KDCoordinate boxHeight() const { return m_store->numberOfValidSeries() > 2 ? k_threeBoxesHeight: k_twoBoxesHeight; }
  KDCoordinate boxLowerBoundPixel() const;
  KDCoordinate boxUpperBoundPixel() const;
  Store * m_store;
  BoxRange m_boxRange;
  int m_series;
  Quantile * m_selectedQuantile;
  KDColor m_selectedHistogramColor;
  KDColor m_selectedHistogramLightColor;
};

}


#endif
