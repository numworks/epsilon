#include "zoom_hint_view.h"

using namespace Escher;

namespace Inference {

ZoomHintView::ZoomHintView()
{
  I18n::Message messages[k_numberOfLegends] = {I18n::Message::ToZoom, I18n::Message::Or};
  for (int i = 0; i < k_numberOfLegends; i++) {
    m_legends[i].setFont(KDFont::SmallFont);
    m_legends[i].setMessage(messages[i]);
    m_legends[i].setBackgroundColor(BackgroundColor());
    m_legends[i].setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
  }
  KeyView::Type tokenTypes[k_numberOfTokens] = {KeyView::Type::Plus, KeyView::Type::Minus};
  for (int i = 0; i < k_numberOfTokens ; i++) {
    m_legendPictograms[i].setType(tokenTypes[i]);
  }
}

void ZoomHintView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight), BackgroundColor());
}

int ZoomHintView::numberOfSubviews() const {
  return k_numberOfLegends+k_numberOfTokens;
}

View * ZoomHintView::subviewAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfTokens+k_numberOfLegends);
  if (index < k_numberOfLegends) {
    return &m_legends[index];
  }
  return &m_legendPictograms[index-k_numberOfLegends];
}

void ZoomHintView::layoutSubviews(bool force) {
  KDCoordinate legendWidth[k_numberOfLegends];
  KDCoordinate totalLegendWidth = 0;
  for (int i = 0; i < k_numberOfLegends; i++) {
    legendWidth[i] = m_legends[i].minimalSizeForOptimalDisplay().width();
    totalLegendWidth += legendWidth[i];
  }
  KDCoordinate spacing = (bounds().width() - totalLegendWidth - k_tokenWidth * k_numberOfTokens) / 2;
  KDCoordinate height = bounds().height();

  KDCoordinate x = spacing;
  m_legends[0].setFrame(KDRect(x, 0, legendWidth[0], height), force);
  x += legendWidth[0];
  m_legendPictograms[0].setFrame(KDRect(x, 0, k_tokenWidth, height), force);
  x += k_tokenWidth;
  m_legends[1].setFrame(KDRect(x, 0, legendWidth[1], height), force);
  x += legendWidth[1];
  m_legendPictograms[1].setFrame(KDRect(x, 0, k_tokenWidth, height), force);
}

}
