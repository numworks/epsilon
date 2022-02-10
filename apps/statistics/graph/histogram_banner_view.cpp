#include "histogram_banner_view.h"
#include <apps/constant.h>
#include <assert.h>

using namespace Escher;

namespace Statistics {

HistogramBannerView::HistogramBannerView() :
    m_intervalLegendView(Font(),
                         I18n::Message::Interval,
                         KDContext::k_alignRight,
                         KDContext::k_alignCenter,
                         TextColor(),
                         BackgroundColor()),
    m_intervalView(Font(),
                   KDContext::k_alignLeft,
                   KDContext::k_alignCenter,
                   TextColor(),
                   BackgroundColor()),
    m_labelledIntervalView(&m_intervalLegendView, &m_intervalView),
    m_sizeLegendView(Font(),
                     I18n::Message::Frequency,
                     KDContext::k_alignRight,
                     KDContext::k_alignCenter,
                     TextColor(),
                     BackgroundColor()),
    m_sizeView(Font(), KDContext::k_alignLeft, KDContext::k_alignCenter, TextColor(), BackgroundColor()),
    m_labelledSizeView(&m_sizeLegendView, &m_sizeView),
    m_frequencyLegendView(Font(),
                          I18n::Message::RelativeFrequency,
                          KDContext::k_alignRight,
                          KDContext::k_alignCenter,
                          TextColor(),
                          BackgroundColor()),
    m_frequencyView(Font(),
                    KDContext::k_alignLeft,
                    KDContext::k_alignCenter,
                    TextColor(),
                    BackgroundColor()),
    m_labelledFrequencyView(&m_frequencyLegendView, &m_frequencyView) {
}

View * HistogramBannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  View * subviews[] = {&m_labelledIntervalView, &m_labelledSizeView, &m_labelledFrequencyView};
  return subviews[index];
}

bool HistogramBannerView::lineBreakBeforeSubview(View * subview) const {
  return subview == &m_labelledSizeView
      || Shared::BannerView::lineBreakBeforeSubview(subview);
}

}
