#include "histogram_banner_view.h"
#include "apps/constant.h"
#include <assert.h>
#include <poincare.h>

namespace Statistics {

HistogramBannerView::HistogramBannerView() :
  m_intervalLegendView(KDText::FontSize::Small, &I18n::Common::Interval, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_intervalView(KDText::FontSize::Small, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sizeLegendView(KDText::FontSize::Small, &I18n::Common::Size, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_sizeView(KDText::FontSize::Small, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_frequencyLegendView(KDText::FontSize::Small, &I18n::Common::Frequency, 1.0f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_frequencyView(KDText::FontSize::Small, 0.0f, 0.5f, KDColorBlack, Palette::GreyMiddle)
{
}

int HistogramBannerView::numberOfSubviews() const {
  return 6;
}

TextView * HistogramBannerView::textViewAtIndex(int i) const {
  const TextView * textViews[6] = {&m_intervalLegendView, &m_intervalView, &m_sizeLegendView, &m_sizeView, &m_frequencyLegendView, &m_frequencyView};
  return (TextView *)textViews[i];
}

MessageTextView * HistogramBannerView::messageTextViewAtIndex(int index) const {
  const MessageTextView * textViews[6] = {&m_intervalLegendView, nullptr, &m_sizeLegendView, nullptr, &m_frequencyLegendView, nullptr};
  return (MessageTextView *)textViews[index];
}

}
