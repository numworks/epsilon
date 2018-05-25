#ifndef STATISTICS_BOX_VIEW_H
#define STATISTICS_BOX_VIEW_H

#include <escher.h>
#include "store.h"
#include "box_range.h"
#include "../constant.h"
#include "../shared/curve_view.h"

namespace Statistics {

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
  BoxView(Store * store, int series, Shared::BannerView * bannerView, Quantile * selectedQuantile, KDColor color);
  Quantile selectedQuantile() const { return *m_selectedQuantile; }
  bool selectQuantile(int selectedQuantile);
  int series() const { return m_series; }
  void setDisplayAxis(bool display) { m_displayAxis = display; }

  // CurveView
  void reload() override;

  // View
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  char * label(Axis axis, int index) const override;
  Store * m_store;
  BoxRange m_boxRange;
  char m_labels[k_maxNumberOfXLabels][Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  int m_series;
  Quantile * m_selectedQuantile;
  KDColor m_selectedHistogramColor;
  bool m_displayAxis;
};

}


#endif
