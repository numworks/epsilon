#ifndef STATISTICS_MULTIPLE_BOXES_VIEW_H
#define STATISTICS_MULTIPLE_BOXES_VIEW_H

#include <escher.h>
#include "store.h"
#include "box_axis_view.h"
#include "box_banner_view.h"
#include "box_view.h"
#include "multiple_data_view.h"

namespace Statistics {

class BoxController;

class MultipleBoxesView : public MultipleDataView {
public:
  MultipleBoxesView(Store * store, BoxView::Quantile * selectedQuantile);
  // MultipleDataView
  int seriesOfSubviewAtIndex(int index) override;
  BoxBannerView * bannerView() override { return &m_bannerView; }
  BoxView * dataViewAtIndex(int index) override;
  void layoutDataSubviews(bool force) override;
  void reload() override;

  // View
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;

private:
  static constexpr KDCoordinate k_axisViewHeight = 21;
  BoxView m_boxView1;
  BoxView m_boxView2;
  BoxView m_boxView3;
  BoxAxisView m_axisView;
  BoxBannerView m_bannerView;
};

}

#endif
