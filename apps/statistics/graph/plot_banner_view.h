#ifndef STATISTICS_PLOT_BANNER_VIEW_H
#define STATISTICS_PLOT_BANNER_VIEW_H

#include <apps/shared/banner_view.h>
#include <escher/buffer_text_view.h>
#include <escher/view.h>

namespace Statistics {

class PlotBannerView : public Shared::BannerView {
public:
  PlotBannerView();
  Escher::BufferTextView * seriesName() { return &m_seriesName; }
  Escher::BufferTextView * value() { return &m_value; }
  Escher::BufferTextView * result() { return &m_result; }
private:
  constexpr static int k_numberOfSubviews = 3;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  Escher::View * subviewAtIndex(int index) override;
  Escher::BufferTextView m_seriesName;
  Escher::BufferTextView m_value;
  Escher::BufferTextView m_result;
};

}

#endif
