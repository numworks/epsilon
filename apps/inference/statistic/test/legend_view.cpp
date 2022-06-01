#include "legend_view.h"
#include <escher/palette.h>
#include <algorithm>

using namespace Escher;

namespace Inference {

void LegendView::DotView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillAntialiasedCircle(KDPointZero, k_diameter / 2, m_color, Palette::WallScreen);
}

LegendView::LegendView() :
  m_pValueLabel(KDFont::SmallFont, I18n::Message::PValue, 0.0, 0.0, Palette::GrayVeryDark, Palette::WallScreen),
  m_alphaLabel(KDFont::SmallFont, I18n::Message::GreekAlpha, 0.0, 0.0, Palette::GrayVeryDark, Palette::WallScreen),
  m_pValueIcon(KDColorOrange),
  m_alphaIcon(Palette::GrayVeryDark)
{}

View * LegendView::subviewAtIndex(int i) {
  switch (i) {
  case 0:
    return &m_pValueLabel;
  case 1:
    return &m_alphaLabel;
  case 2:
    return &m_pValueIcon;
  default:
    assert(i == 3);
    return &m_alphaIcon;
  }
}

KDSize LegendView::minimalSizeForOptimalDisplay() const {
  KDSize pLabelSize = m_pValueLabel.minimalSizeForOptimalDisplay();
  KDSize aLabelSize = m_alphaLabel.minimalSizeForOptimalDisplay();
  assert(aLabelSize.width() <= pLabelSize.width());
  return KDSize(k_diameter + k_marginBetween + pLabelSize.width(), pLabelSize.height() + aLabelSize.height() + k_offsetTop);
}

void LegendView::layoutSubviews(bool force) {
  m_pValueIcon.setFrame(KDRect(0, k_offsetTop, k_diameter, k_diameter), force);
  KDSize pLabelSize = m_pValueLabel.minimalSizeForOptimalDisplay();
  m_pValueLabel.setFrame(KDRect(k_diameter + k_marginBetween, 0, pLabelSize), force);

  assert(k_offsetTop + k_diameter <= pLabelSize.height());
  KDCoordinate secondRowY = pLabelSize.height();
  m_alphaIcon.setFrame(KDRect(0, secondRowY + k_offsetTop, k_diameter, k_diameter), force);
  KDSize aLabelSize = m_alphaLabel.minimalSizeForOptimalDisplay();
  m_alphaLabel.setFrame(KDRect(k_diameter + k_marginBetween, secondRowY, aLabelSize), force);
}

}  // namespace Inference
