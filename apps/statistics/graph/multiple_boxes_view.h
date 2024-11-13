#ifndef STATISTICS_MULTIPLE_BOXES_VIEW_H
#define STATISTICS_MULTIPLE_BOXES_VIEW_H

#include "../store.h"
#include "box_axis_view.h"
#include "box_banner_view.h"
#include "box_view.h"
#include "data_view_controller.h"
#include "multiple_data_view.h"

namespace Statistics {

class MultipleBoxesView : public MultipleDataView {
 public:
  MultipleBoxesView(Store* store, DataViewController* dataViewController);
  // MultipleDataView
  BoxBannerView* bannerView() override { return &m_bannerView; }
  BoxView* plotViewForSeries(int series) override;
  void layoutDataSubviews(bool force) override;
  void reload() override;
  bool moveSelectionHorizontally(int series,
                                 OMG::HorizontalDirection direction);

  // View
  int numberOfSubviews() const override;
  Escher::View* subviewAtIndex(int index) override;

 private:
  constexpr static KDCoordinate TopToFirstBoxMargin(int numberOfSeries) {
    assert(1 <= numberOfSeries && numberOfSeries <= Store::k_numberOfSeries);
    return numberOfSeries == 1 ? k_topToFirstBoxMarginOneSeries
                               : k_topToFirstBoxMarginMultipleSeries;
  }
  constexpr static KDCoordinate BoxToBoxMargin(int numberOfSeries) {
    assert(1 <= numberOfSeries && numberOfSeries <= k_boxToBoxMargins.size());
    return k_boxToBoxMargins[numberOfSeries - 1];
  }

  constexpr static bool isBoxMarginValid() {
    for (std::size_t numberOfSeries = 1;
         numberOfSeries < Store::k_numberOfSeries; numberOfSeries++) {
      if (MultipleBoxesView::BoxToBoxMargin(numberOfSeries) <
          BoxPlotPolicy::BoxVerticalMargin()) {
        return false;
      }
    }
    return true;
  }

  constexpr static KDCoordinate k_axisViewHeight = 21;

  static constexpr KDCoordinate k_topToFirstBoxMarginOneSeries = 48;
  static constexpr KDCoordinate k_topToFirstBoxMarginMultipleSeries = 14;
  static constexpr std::array<KDCoordinate, Store::k_numberOfSeries>
      k_boxToBoxMargins = {24, 24, 12, 10, 10, 10};

  void drawRect(KDContext* ctx, KDRect rect) const override;
  void changeDataViewSeriesSelection(int series, bool select) override;

  std::array<BoxView, Store::k_numberOfSeries> m_boxViews;

  BoxAxisView m_axisView;
  BoxBannerView m_bannerView;
};

}  // namespace Statistics

#endif
