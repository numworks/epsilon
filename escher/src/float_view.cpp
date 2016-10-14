#include <escher/float_view.h>

FloatView::FloatView() :
ChildlessView(),
m_float(Float(0.0f)),
m_backgroundColor(KDColorWhite)
{
}

void FloatView::setFloat(float f) {
  m_float = Float(f);
  m_float.convertFloatToText(m_buffer, 14, 7);
}

char * FloatView::buffer() {
  return m_buffer;
}

void FloatView::setBackgroundColor(KDColor backgroundColor) {
  m_backgroundColor = backgroundColor;
  markRectAsDirty(bounds());
}

void FloatView::drawRect(KDContext * ctx, KDRect rect) const {
  KDSize textSize = KDText::stringSize(m_buffer);
  KDPoint origin(m_frame.width() - textSize.width(), 0.5f*(m_frame.height() - textSize.height()));
  ctx->drawString(m_buffer, origin, KDColorBlack, m_backgroundColor);
}
