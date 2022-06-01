#include "legend_view.h"
#include <escher/palette.h>

using namespace Escher;

namespace Inference {

void LegendView::DotView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillAntialiasedCircle(KDPointZero, k_diameter / 2, m_color, Palette::WallScreen);
}

LegendView::LegendView() :
  m_pValueLabel(KDFont::SmallFont, I18n::Message::PValue, 0.0, 0.0, Palette::GrayVeryDark, Palette::WallScreen),
  m_pValueIcon(KDColorOrange)
{}

View * LegendView::subviewAtIndex(int i) {
  switch (i) {
  case 0:
    return &m_pValueLabel;
  default:
    assert(i == 1);
    return &m_pValueIcon;
  }
}

KDSize LegendView::minimalSizeForOptimalDisplay() const {
  KDSize labelSize = m_pValueLabel.minimalSizeForOptimalDisplay();
  return KDSize(k_diameter + k_marginBetween + labelSize.width(), labelSize.height() + k_offsetTop);
}

void LegendView::layoutSubviews(bool force) {
  m_pValueIcon.setFrame(KDRect(KDPoint(0, k_offsetTop), k_diameter, k_diameter), force);
  KDSize labelSize = m_pValueLabel.minimalSizeForOptimalDisplay();
  m_pValueLabel.setFrame(KDRect(k_diameter + k_marginBetween, 0, labelSize), force);
}

}  // namespace Inference
