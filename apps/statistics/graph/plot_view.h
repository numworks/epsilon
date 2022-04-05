#ifndef STATISTICS_PLOT_VIEW_H
#define STATISTICS_PLOT_VIEW_H

#include <apps/shared/curve_view.h>
#include <apps/shared/curve_view_cursor.h>
#include <escher/view.h>
#include "plot_curve_view.h"
#include "plot_banner_view.h"
#include "plot_range.h"
#include "multiple_data_view.h"
#include "../store.h"

namespace Statistics {

/* TODO : Add an intermediary class between MultipleDataView and View, as well
 *        as between MultipleDataViewController and ViewController +
 *        GraphButtonRowDelegate to avoid having to completely re-implement
 *        MultipleDataView's methods.
 *        PlotView should disappear since Shared::CurveView has all the tools
 *        to handle a banner and a range. */
class PlotView : public MultipleDataView {
public:
  PlotView(Store * store, PlotCurveView * plotCurveView, PlotRange * graphRange, PlotBannerView * bannerView) :
    MultipleDataView(store),
    m_plotCurveView(plotCurveView),
    m_graphRange(graphRange),
    m_bannerView(bannerView) {
  }

  // MultipleDataView
  int numberOfSubviews() const override { return 2; } // CurveView and Banner
  Escher::View * subviewAtIndex(int index) override;
  // All series are displayed in the same curve view
  Shared::CurveView * dataViewAtIndex(int index) override { return m_plotCurveView; }
  int seriesOfSubviewAtIndex(int index) override { return m_store->indexOfKthValidSeries(index); }
  PlotBannerView * bannerView() override { return m_bannerView; }
  void reload() override;

protected:
  void layoutDataSubviews(bool force) override;

  PlotCurveView * m_plotCurveView;
  PlotRange * m_graphRange;
  PlotBannerView * m_bannerView;
};

}  // namespace Statistics

#endif /* STATISTICS_PLOT_VIEW_H */
