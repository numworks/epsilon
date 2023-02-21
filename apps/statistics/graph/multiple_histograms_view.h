#ifndef STATISTICS_MULTIPLE_HISTOGRAMS_VIEW_H
#define STATISTICS_MULTIPLE_HISTOGRAMS_VIEW_H

#include "../store.h"
#include "histogram_banner_view.h"
#include "histogram_parameter_controller.h"
#include "histogram_view.h"
#include "multiple_data_view.h"

namespace Statistics {

class MultipleHistogramsView : public MultipleDataView {
 public:
  MultipleHistogramsView(Store* store, Shared::CurveViewRange* curveViewRange);
  // MultipleDataView
  HistogramBannerView* bannerView() override { return &m_bannerView; }
  HistogramView* plotViewForSeries(int series) override;

 private:
  void layoutSubviews(bool force = false) override;
  void changeDataViewSeriesSelection(int series, bool select) override;
  HistogramView m_histogramView1;
  HistogramView m_histogramView2;
  HistogramView m_histogramView3;
  HistogramBannerView m_bannerView;
};

}  // namespace Statistics

#endif
