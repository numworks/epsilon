#include "histogram_banner_view.h"
#include "../constant.h"
#include <assert.h>

namespace Statistics {

HistogramBannerView::HistogramBannerView() :
  m_intervalLegendView(Font(), I18n::Message::Interval, 0.0f, 0.5f, TextColor(), BackgroundColor()),
  m_intervalView(Font(), 0.0f, 0.5f, TextColor(), BackgroundColor()),
  m_sizeLegendView(Font(), I18n::Message::Size, 0.0f, 0.5f, TextColor(), BackgroundColor()),
  m_sizeView(Font(), 0.0f, 0.5f, TextColor(), BackgroundColor()),
  m_frequencyLegendView(Font(), I18n::Message::Frequency, 0.0f, 0.5f, TextColor(), BackgroundColor()),
  m_frequencyView(Font(), 0.0f, 0.5f, TextColor(), BackgroundColor())
{
}

View * HistogramBannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  View * subviews[] = {&m_intervalLegendView, &m_intervalView, &m_sizeLegendView, &m_sizeView, &m_frequencyLegendView, &m_frequencyView};
  return subviews[index];
}

}
