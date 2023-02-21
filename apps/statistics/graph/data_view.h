#ifndef STATISTICS_DATA_VIEW_H
#define STATISTICS_DATA_VIEW_H

#include <apps/shared/banner_view.h>
#include <apps/shared/plot_view.h>
#include <escher/view.h>

#include "../store.h"

namespace Statistics {

class DataView : public Escher::View {
 public:
  constexpr static int k_defaultSelectedIndex = 0;

  DataView() : m_displayBanner(false) {}
  virtual Shared::AbstractPlotView* plotViewForSeries(int series) = 0;
  virtual void reload() = 0;
  void selectViewForSeries(int series);
  void deselectViewForSeries(int series);

  // Display
  void setDisplayBanner(bool display);

 protected:
  virtual void changeDataViewSeriesSelection(int series, bool select);
  KDRect bannerFrame() const;
  void layoutSubviews(bool force = false) override;
  virtual void layoutDataSubviews(bool force) = 0;
  virtual Shared::BannerView* bannerView() = 0;
  void layoutBanner(bool force);

  bool m_displayBanner;
};

}  // namespace Statistics

#endif
