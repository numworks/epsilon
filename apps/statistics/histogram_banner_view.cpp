#include "histogram_banner_view.h"
#include "../constant.h"
#include <assert.h>
#include <poincare.h>

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

TextView * HistogramBannerView::textViewAtIndex(int i) const {
  const TextView * textViews[k_numberOfSubviews] = {&m_intervalLegendView, &m_intervalView, &m_sizeLegendView, &m_sizeView, &m_frequencyLegendView, &m_frequencyView};
  return (TextView *)textViews[i];
}

MessageTextView * HistogramBannerView::messageTextViewAtIndex(int index) const {
  const MessageTextView * textViews[k_numberOfSubviews] = {&m_intervalLegendView, nullptr, &m_sizeLegendView, nullptr, &m_frequencyLegendView, nullptr};
  return (MessageTextView *)textViews[index];
}

}
