#include <escher/color_view.h>
#include <escher/palette.h>


ColorView::ColorView() :
  m_selectedColor(0),
  m_backgroundColor(KDColorWhite)
{
}

KDColor ColorView::color() {
  return Palette::DataColor[m_selectedColor];
}

int ColorView::colorIndex() {
  return m_selectedColor;
}

void ColorView::setColor(int color) {
  if (m_selectedColor != color) {
    color = color < 0 ? 0 : color;
    color = color >= m_numColors ? m_numColors - 1 : color;
    m_selectedColor = color;
    markRectAsDirty(bounds());
  }
}

void ColorView::setBackgroundColor(KDColor color) {
  if (m_backgroundColor != color) {
    m_backgroundColor = color;
    markRectAsDirty(bounds());
  }
}

void ColorView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), m_backgroundColor);
  
  int scrol = k_colorBoxSize * (m_selectedColor - 2);
  scrol = scrol < 0 ? 0 : scrol;
  scrol = k_colorBoxSize * m_numColors - scrol < bounds().width() ? k_colorBoxSize * m_numColors - bounds().width() : scrol; 
  
  for (int i=0; i < m_numColors; i++) {
    if (i == m_selectedColor) {
      ctx->fillRect(KDRect(k_colorBoxSize * i - scrol, ((bounds().height() - k_colorBoxSize - 4) / 2) - 2, k_colorBoxSize, k_colorBoxSize), Palette::SelectDark);
    }
    ctx->fillRect(KDRect(k_colorBoxSize * i - scrol + 2, (bounds().height() - k_colorBoxSize - 4) / 2, k_colorBoxSize-4, k_colorBoxSize-4), Palette::DataColor[i]);
  }
}

KDSize ColorView::minimalSizeForOptimalDisplay() const {
  return KDSize(m_numColors * k_colorBoxSize, k_colorBoxSize);
}
