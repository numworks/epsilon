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
  BoxView(Store * store, Shared::BannerView * bannerView, Quantile * selectedQuantile);
  void reload() override;
  Quantile selectedQuantile();
  bool selectQuantile(int selectedQuantile);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  char * label(Axis axis, int index) const override;
  Store * m_store;
  BoxRange m_boxRange;
  char m_labels[k_maxNumberOfXLabels][Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  Quantile * m_selectedQuantile;
};

}


#endif
