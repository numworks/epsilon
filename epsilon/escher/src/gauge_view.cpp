#include <escher/gauge_view.h>
#include <escher/menu_cell.h>
#include <escher/theme/color_picker.h>
#include <escher/theme/components.h>
#include <escher/theme/highlight.h>

namespace Escher {

const uint8_t gaugeIndicatorMask
    [GaugeView::k_indicatorDiameter][GaugeView::k_indicatorDiameter] = {
        {0xFF, 0xFF, 0xE1, 0x0C, 0x00, 0x00, 0x0C, 0xE1, 0xFF, 0xFF},
        {0xFF, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0xFF},
        {0xE1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE1},
        {0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C},
        {0xE1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE1},
        {0xFF, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0xFF},
        {0xFF, 0xFF, 0xE1, 0x0C, 0x00, 0x00, 0x0C, 0xE1, 0xFF, 0xFF},
};

void GaugeView::setLevel(float level) {
  if (m_level != level) {
    level = level < 0 ? 0 : level;
    level = level > 1 ? 1 : level;
    m_level = level;
    markWholeFrameAsDirty();
  }
}

void GaugeView::setHighlighted(bool highlighted) {
  if (m_isHighlighted != highlighted) {
    m_isHighlighted = highlighted;
    markWholeFrameAsDirty();
  }
}

void GaugeView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(bounds(), ColorPicker::Resolve(
                              Highlight(k_defaultBackground, m_isHighlighted)));
  /* Draw the gauge centered vertically on all the width available */
  KDCoordinate width = bounds().width() - k_indicatorDiameter;
  KDCoordinate height = bounds().height();
  KDColor gaugeIndicatorWorkingBuffer[GaugeView::k_indicatorDiameter *
                                      GaugeView::k_indicatorDiameter];

  ctx->fillRect(KDRect(k_indicatorDiameter / 2, (height - k_thickness) / 2,
                       width * m_level, k_thickness),
                ColorPicker::Resolve(Highlight(ThemeComponent::TokenMainColor,
                                               m_isHighlighted)));
  ctx->fillRect(
      KDRect(k_indicatorDiameter / 2 + width * m_level,
             (height - k_thickness) / 2, width * (1.0f - m_level), k_thickness),
      ColorPicker::Resolve(
          Highlight(ThemeComponent::TokenInactive, m_isHighlighted)));
  KDRect frame(width * m_level, (height - k_indicatorDiameter) / 2,
               k_indicatorDiameter, k_indicatorDiameter);
  ctx->blendRectWithMask(
      frame, ColorPicker::Resolve(ThemeComponent::TokenMainColor),
      (const uint8_t*)gaugeIndicatorMask, gaugeIndicatorWorkingBuffer);
}

KDSize GaugeView::minimalSizeForOptimalDisplay() const {
  return KDSize(12 * k_indicatorDiameter, k_indicatorDiameter);
}

}  // namespace Escher
