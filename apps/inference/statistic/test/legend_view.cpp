#include "legend_view.h"

#include <escher/palette.h>

#include <algorithm>

using namespace Escher;

namespace Inference {

void LegendView::DotView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillAntialiasedCircle(KDPointZero, k_diameter / 2, KDColorOrange,
                             k_backgroundColor);
}

void LegendView::StripedDotView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillCircleWithStripes(KDPointZero, k_diameter / 2, Palette::PurpleBright,
                             k_backgroundColor, 3);
}

LegendView::LegendView()
    : m_pValueLabel(I18n::Message::PValue,
                    {{.glyphColor = Palette::GrayVeryDark,
                      .backgroundColor = k_backgroundColor,
                      .font = KDFont::Size::Small},
                     .verticalAlignment = KDGlyph::k_alignTop}),
      m_alphaLabel(I18n::Message::GreekAlpha,
                   {{.glyphColor = Palette::GrayVeryDark,
                     .backgroundColor = k_backgroundColor,
                     .font = KDFont::Size::Small},
                    .verticalAlignment = KDGlyph::k_alignTop}) {}

View* LegendView::subviewAtIndex(int i) {
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
  return KDSize(k_diameter + k_marginBetween + pLabelSize.width(),
                pLabelSize.height() + aLabelSize.height() + k_offsetTop);
}

void LegendView::layoutSubviews(bool force) {
  setChildFrame(&m_pValueIcon, KDRect(0, k_offsetTop, k_diameter, k_diameter),
                force);
  KDSize pLabelSize = m_pValueLabel.minimalSizeForOptimalDisplay();
  setChildFrame(&m_pValueLabel,
                KDRect(k_diameter + k_marginBetween, 0, pLabelSize), force);

  assert(k_offsetTop + k_diameter <= pLabelSize.height());
  KDCoordinate secondRowY = pLabelSize.height();
  setChildFrame(&m_alphaIcon,
                KDRect(0, secondRowY + k_offsetTop, k_diameter, k_diameter),
                force);
  KDSize aLabelSize = m_alphaLabel.minimalSizeForOptimalDisplay();
  setChildFrame(&m_alphaLabel,
                KDRect(k_diameter + k_marginBetween, secondRowY, aLabelSize),
                force);
}

}  // namespace Inference
