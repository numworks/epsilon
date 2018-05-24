#ifndef STATISTICS_MULTIPLE_HISTOGRAMS_VIEW_H
#define STATISTICS_MULTIPLE_HISTOGRAMS_VIEW_H

#include <escher.h>
#include "store.h"
#include "histogram_view.h"
#include "histogram_banner_view.h"
#include "histogram_parameter_controller.h"
#include "../shared/ok_view.h"

namespace Statistics {

class MultipleHistogramsView : public View {
public:
  MultipleHistogramsView(HistogramController * controller, Store * store);
  void reload();
  HistogramView * histogramViewAtIndex(int index);
  int seriesOfSubviewAtIndex(int index);
  int indexOfSubviewAtSeries(int series);
  HistogramBannerView * bannerView() { return &m_bannerView; }
  void setDisplayBanner(bool display) { m_displayBanner = display; }
  void selectHistogram(int index);
  void deselectHistogram(int index);
  // View
  void drawRect(KDContext * ctx, KDRect rect) const override;
  int numberOfSubviews() const override;
private:
  KDRect bannerFrame() const;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void changeHistogramSelection(int index, bool select);
  HistogramView m_histogramView1;
  HistogramView m_histogramView2;
  HistogramView m_histogramView3;
  HistogramBannerView m_bannerView;
  Shared::OkView m_okView;
  bool m_displayBanner;
  Store * m_store;
};

}

#endif
