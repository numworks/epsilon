#include "value_cell.h"

namespace Graph {

ValueCell::ValueCell() :
  EvenOddCell(),
  m_float(Float(0.0f))
{
}

void ValueCell::setFloat(float f) {
  m_float = Float(f);
  m_float.convertFloatToText(m_buffer, 14, 7);
}

void ValueCell::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddCell::drawRect(ctx, rect);
  // Write the content of the cell
  KDColor background = backgroundColor();
  KDSize textSize = KDText::stringSize(m_buffer);
  KDPoint origin(m_frame.width() - textSize.width(), 0.5f*(m_frame.height() - textSize.height()));
  ctx->drawString(m_buffer, origin, KDColorBlack, background);
}

}
