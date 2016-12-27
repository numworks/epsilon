#ifndef STATISTICS_HISTOGRAM_VIEW_H
#define STATISTICS_HISTOGRAM_VIEW_H

#include <escher.h>
#include "data.h"
#include "banner_view.h"
#include "../constant.h"
#include "../curve_view.h"

namespace Statistics {

class HistogramView : public CurveView {
public:
  HistogramView(Data * m_data);
  void reload(float dirtyZoneCenter);
  bool selectedBins();
  void selectBins(bool selectedBins);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_bannerHeight = 30;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  char * label(Axis axis, int index) const override;
  float evaluateCurveAtAbscissa(Model * curve, float t) const override;
  Data * m_data;
  char m_labels[k_maxNumberOfXLabels][Constant::FloatBufferSizeInScientificMode];
  bool m_selectedBins;
  BannerView m_bannerView;
};

}


#endif
