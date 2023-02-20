#ifndef STATISTICS_MULTIPLE_BOXES_VIEW_H
#define STATISTICS_MULTIPLE_BOXES_VIEW_H

#include "../store.h"
#include "box_axis_view.h"
#include "box_banner_view.h"
#include "box_view.h"
#include "multiple_data_view.h"

namespace Statistics {

class MultipleBoxesView : public MultipleDataView {
public:
  MultipleBoxesView(Store * store, int * selectedBoxCalculation);
  // MultipleDataView
  BoxBannerView * bannerView() override { return &m_bannerView; }
  BoxView * plotViewForSeries(int series) override;
  void layoutDataSubviews(bool force) override;
  void reload() override;
  bool moveSelectionHorizontally(int series, OMG::NewHorizontalDirection direction);

  // View
  int numberOfSubviews() const override;
  Escher::View * subviewAtIndex(int index) override;

private:
  constexpr static KDCoordinate TopToFirstBoxMargin(int numberOfSeries) { return numberOfSeries == 1 ? 48 : 14; }
  constexpr static KDCoordinate BoxToBoxMargin(int numberOfSeries) { return numberOfSeries == 3 ? 12 : 24; }
  constexpr static KDCoordinate k_axisViewHeight = 21;

  void drawRect(KDContext * ctx, KDRect rect) const override;
  void changeDataViewSeriesSelection(int series, bool select) override;
  BoxView m_boxView1;
  BoxView m_boxView2;
  BoxView m_boxView3;
  BoxAxisView m_axisView;
  BoxBannerView m_bannerView;
};

}

#endif
