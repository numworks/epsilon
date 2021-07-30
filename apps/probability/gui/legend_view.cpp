#include "legend_view.h"

namespace Probability {

void LegendLabel::Icon::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillAntialiasedCircle(KDPointZero, k_diameter / 2, m_color, Palette::WallScreen);
}

LegendLabel::LegendLabel(const char * label, KDColor color) : m_icon(color) {
  m_labelView.setText(label);
  m_labelView.setBackgroundColor(Escher::Palette::WallScreen);
  m_labelView.setFont(KDFont::SmallFont);
}

KDSize LegendLabel::minimalSizeForOptimalDisplay() const {
  KDSize labelSize = m_labelView.minimalSizeForOptimalDisplay();
  return KDSize(k_diameter + k_marginBetween + labelSize.width(), labelSize.height() + k_offsetTop);
}

void LegendLabel::layoutSubviews(bool force) {
  m_icon.setFrame(KDRect(KDPoint(0, k_offsetTop), k_diameter, k_diameter), force);
  KDSize labelSize = m_labelView.minimalSizeForOptimalDisplay();
  m_labelView.setFrame(KDRect(k_diameter + k_marginBetween, 0, labelSize), force);
}

}  // namespace Probability
