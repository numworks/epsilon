#ifndef STATISTICS_MULTIPLE_HISTOGRAMS_VIEW_H
#define STATISTICS_MULTIPLE_HISTOGRAMS_VIEW_H

#include <escher.h>
#include "store.h"
#include "histogram_view.h"
#include "histogram_banner_view.h"
#include "histogram_parameter_controller.h"
#include "multiple_data_view.h"
#include "../shared/ok_view.h"

namespace Statistics {

class MultipleHistogramsView : public MultipleDataView {
public:
  MultipleHistogramsView(HistogramController * controller, Store * store);
  // MultipleDataView
  int seriesOfSubviewAtIndex(int index) override;
  HistogramBannerView * bannerView() override { return &m_bannerView; }
  HistogramView * dataViewAtIndex(int index) override;
private:
  void layoutSubviews(bool force = false) override;
  void changeDataViewSelection(int index, bool select) override;
  HistogramView m_histogramView1;
  HistogramView m_histogramView2;
  HistogramView m_histogramView3;
  HistogramBannerView m_bannerView;
  Shared::OkView m_okView;
};

}

#endif
