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
    assert(1 <= numberOfSeries && numberOfSeries <= k_numberOfBoxViews);
    return numberOfSeries == 1 ? 48 : 14;
  }
  constexpr static KDCoordinate BoxToBoxMargin(int numberOfSeries) {
    assert(2 <= numberOfSeries && numberOfSeries <= k_numberOfBoxViews);
    switch (numberOfSeries) {
      case 2:
        return 24;
      case 3:
        return 12;
      default:
        return 10;
    }
  }

  constexpr static KDCoordinate k_axisViewHeight = 21;

  void drawRect(KDContext* ctx, KDRect rect) const override;
  void changeDataViewSeriesSelection(int series, bool select) override;

  /* TODO: it would be nice to use an std::array<BoxView,
   * Store::k_numberOfSeries> here. However BoxView (and the View parent
   * object) have their default constructor and their move assignment operator
   * deleted, so there is no easy way to achieve that. */
  BoxView m_boxView1;
  BoxView m_boxView2;
  BoxView m_boxView3;
  BoxView m_boxView4;
  BoxView m_boxView5;
  BoxView m_boxView6;

  static constexpr size_t k_numberOfBoxViews = 6;
  static_assert(k_numberOfBoxViews == Store::k_numberOfSeries);

  BoxAxisView m_axisView;
  BoxBannerView m_bannerView;
};

}  // namespace Statistics

#endif
