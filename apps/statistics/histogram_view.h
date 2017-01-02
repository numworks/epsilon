#ifndef STATISTICS_HISTOGRAM_VIEW_H
#define STATISTICS_HISTOGRAM_VIEW_H

#include <escher.h>
#include "data.h"
#include "histogram_banner_view.h"
#include "../constant.h"
#include "../curve_view_with_banner.h"

namespace Statistics {

class HistogramView : public CurveViewWithBanner {
public:
  HistogramView(Data * m_data);
  void reloadSelection() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  char * label(Axis axis, int index) const override;
  BannerView * bannerView() override;
  float evaluateModelWithParameter(Model * curve, float t) const override;
  Data * m_data;
  char m_labels[k_maxNumberOfXLabels][Constant::FloatBufferSizeInScientificMode];
  HistogramBannerView m_bannerView;
};

}


#endif
