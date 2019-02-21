#include "histogram_banner_view.h"
#include "../constant.h"
#include <assert.h>

namespace Statistics {

HistogramBannerView::HistogramBannerView() :
  m_intervalLegendView(KDFont::SmallFont, I18n::Message::Interval, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_intervalView(KDFont::SmallFont, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sizeLegendView(KDFont::SmallFont, I18n::Message::Size, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sizeView(KDFont::SmallFont, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_frequencyLegendView(KDFont::SmallFont, I18n::Message::Frequency, 1.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_frequencyView(KDFont::SmallFont, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle)
{
}

View * HistogramBannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  View * subviews[] = {&m_intervalLegendView, &m_intervalView, &m_sizeLegendView, &m_sizeView, &m_frequencyLegendView, &m_frequencyView};
  return subviews[index];
}

}
