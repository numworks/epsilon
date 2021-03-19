#include "histogram_banner_view.h"
#include "../constant.h"
#include <assert.h>

using namespace Escher;

namespace Statistics {

HistogramBannerView::HistogramBannerView() :
  m_intervalLegendView(Font(), I18n::Message::Interval, 1.0f, 0.5f, TextColor(), BackgroundColor()),
  m_intervalView(Font(), 0.0f, 0.5f, TextColor(), BackgroundColor()),
  m_labelledIntervalView(&m_intervalLegendView, &m_intervalView),
  m_sizeLegendView(Font(), I18n::Message::Frequency, 1.0f, 0.5f, TextColor(), BackgroundColor()),
  m_sizeView(Font(), 0.0f, 0.5f, TextColor(), BackgroundColor()),
  m_labelledSizeView(&m_sizeLegendView, &m_sizeView),
  m_frequencyLegendView(Font(), I18n::Message::RelativeFrequency, 1.0f, 0.5f, TextColor(), BackgroundColor()),
  m_frequencyView(Font(), 0.0f, 0.5f, TextColor(), BackgroundColor()),
  m_labelledFrequencyView(&m_frequencyLegendView, &m_frequencyView)
{
}

View * HistogramBannerView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  View * subviews[] = {&m_labelledIntervalView, &m_labelledSizeView, &m_labelledFrequencyView};
  return subviews[index];
}

}
