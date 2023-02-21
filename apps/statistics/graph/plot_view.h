#ifndef STATISTICS_PLOT_VIEW_H
#define STATISTICS_PLOT_VIEW_H

#include <apps/shared/curve_view_cursor.h>
#include <escher/view.h>

#include "../store.h"
#include "multiple_data_view.h"
#include "plot_banner_view.h"
#include "plot_curve_view.h"
#include "plot_range.h"

namespace Statistics {

/* PlotView and PlotCurveView could be merged in a single class, but it would
 * create a diamond structure because the merged class should inherit from
 * Shared::LabeledCurveView and public DataView, which inherit both from View.
 */
class PlotView : public DataView {
 public:
  PlotView(PlotCurveView* plotCurveView, PlotRange* graphRange,
           PlotBannerView* bannerView)
      : DataView(),
        m_plotCurveView(plotCurveView),
        m_graphRange(graphRange),
        m_bannerView(bannerView) {}

  // DataView
  int numberOfSubviews() const override { return 2; }  // CurveView and Banner
  Escher::View* subviewAtIndex(int index) override;
  // All series are displayed in the same curve view
  Shared::AbstractPlotView* plotViewForSeries(int series) override {
    return m_plotCurveView;
  }
  PlotBannerView* bannerView() override { return m_bannerView; }
  void reload() override;

 protected:
  void layoutDataSubviews(bool force) override;

  PlotCurveView* m_plotCurveView;
  PlotRange* m_graphRange;
  PlotBannerView* m_bannerView;
};

}  // namespace Statistics

#endif
