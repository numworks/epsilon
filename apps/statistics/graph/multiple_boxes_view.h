#ifndef STATISTICS_MULTIPLE_BOXES_VIEW_H
#define STATISTICS_MULTIPLE_BOXES_VIEW_H

#include "../store.h"
#include "box_axis_view.h"
#include "box_banner_view.h"
#include "box_view.h"
#include "multiple_data_view.h"

namespace Statistics {

class BoxController;

class MultipleBoxesView : public MultipleDataView {
public:
  MultipleBoxesView(Store * store, int * selectedBoxCalculation);
  // MultipleDataView
  int seriesOfSubviewAtIndex(int index) override;
  BoxBannerView * bannerView() override { return &m_bannerView; }
  BoxView * dataViewAtIndex(int index) override;
  void layoutDataSubviews(bool force) override;
  void reload() override;

  // View
  int numberOfSubviews() const override;
  Escher::View * subviewAtIndex(int index) override;

private:
  static constexpr KDCoordinate k_axisViewHeight = 21;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  BoxView m_boxView1;
  BoxView m_boxView2;
  BoxView m_boxView3;
  BoxAxisView m_axisView;
  BoxBannerView m_bannerView;
};

}

#endif
