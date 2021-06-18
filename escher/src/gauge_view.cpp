#include <escher/gauge_view.h>
#include <escher/palette.h>

const uint8_t gaugeIndicatorMask[GaugeView::k_indicatorDiameter][GaugeView::k_indicatorDiameter] = {
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

GaugeView::GaugeView() :
  m_level(1),
  m_backgroundColor(Palette::ListCellBackground)
{
}

float GaugeView::level() {
  return m_level;
}

void GaugeView::setLevel(float level) {
  if (m_level != level) {
    level = level < 0 ? 0 : level;
    level = level > 1 ? 1 : level;
    m_level = level;
    markRectAsDirty(bounds());
  }
}

void GaugeView::setBackgroundColor(KDColor color) {
  if (m_backgroundColor != color) {
    m_backgroundColor = color;
    markRectAsDirty(bounds());
  }
}

void GaugeView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), m_backgroundColor);
  /* Draw the gauge centered vertically on all the width available */
  KDCoordinate width = bounds().width()-k_indicatorDiameter;
  KDCoordinate height =  bounds().height();
  KDColor gaugeIndicatorWorkingBuffer[GaugeView::k_indicatorDiameter*GaugeView::k_indicatorDiameter];

  ctx->fillRect(KDRect(k_indicatorDiameter/2, (height-k_thickness)/2, width*m_level, k_thickness), Palette::ControlEnabled);
  ctx->fillRect(KDRect(k_indicatorDiameter/2+width*m_level, (height-k_thickness)/2, width*(1.0f-m_level), k_thickness), Palette::ControlDisabled);
  KDRect frame(width*m_level, (height-k_indicatorDiameter)/2, k_indicatorDiameter, k_indicatorDiameter);
  ctx->blendRectWithMask(frame, Palette::Control, (const uint8_t *)gaugeIndicatorMask, gaugeIndicatorWorkingBuffer);
}

KDSize GaugeView::minimalSizeForOptimalDisplay() const {
  return KDSize(12*k_indicatorDiameter, k_indicatorDiameter);
}
