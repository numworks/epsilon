#include "legend_view.h"

#include <escher/palette.h>

namespace Inference {

void LegendView::DotView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillAntialiasedCircle(KDPointZero, k_diameter / 2, m_color, Escher::Palette::WallScreen);
}

LegendView::LegendView(const char * label, KDColor color) : m_icon(color) {
  m_labelView.setText(label);
  m_labelView.setBackgroundColor(Escher::Palette::WallScreen);
  m_labelView.setFont(KDFont::SmallFont);
  m_labelView.setTextColor(Escher::Palette::GrayVeryDark);
}

KDSize LegendView::minimalSizeForOptimalDisplay() const {
  KDSize labelSize = m_labelView.minimalSizeForOptimalDisplay();
  return KDSize(k_diameter + k_marginBetween + labelSize.width(), labelSize.height() + k_offsetTop);
}

void LegendView::layoutSubviews(bool force) {
  m_icon.setFrame(KDRect(KDPoint(0, k_offsetTop), k_diameter, k_diameter), force);
  KDSize labelSize = m_labelView.minimalSizeForOptimalDisplay();
  m_labelView.setFrame(KDRect(k_diameter + k_marginBetween, 0, labelSize), force);
}

}  // namespace Inference
