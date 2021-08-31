#include "histogram_banner_view.h"
#include "../constant.h"
#include <assert.h>

using namespace Escher;

namespace Statistics {

HistogramBannerView::HistogramBannerView() :
    m_intervalLegendView(Font(),
                         I18n::Message::Interval,
                         KDFont::ALIGN_RIGHT,
                         KDFont::ALIGN_CENTER,
                         TextColor(),
                         BackgroundColor()),
    m_intervalView(Font(),
                   KDFont::ALIGN_LEFT,
                   KDFont::ALIGN_CENTER,
                   TextColor(),
                   BackgroundColor()),
    m_labelledIntervalView(&m_intervalLegendView, &m_intervalView),
    m_sizeLegendView(Font(),
                     I18n::Message::Frequency,
                     KDFont::ALIGN_RIGHT,
                     KDFont::ALIGN_CENTER,
                     TextColor(),
                     BackgroundColor()),
    m_sizeView(Font(), KDFont::ALIGN_LEFT, KDFont::ALIGN_CENTER, TextColor(), BackgroundColor()),
    m_labelledSizeView(&m_sizeLegendView, &m_sizeView),
    m_frequencyLegendView(Font(),
                          I18n::Message::RelativeFrequency,
                          KDFont::ALIGN_RIGHT,
                          KDFont::ALIGN_CENTER,
                          TextColor(),
                          BackgroundColor()),
    m_frequencyView(Font(),
                    KDFont::ALIGN_LEFT,
                    KDFont::ALIGN_CENTER,
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
