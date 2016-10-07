#include "value_cell.h"
#include "float_to_string.h"

ValueCell::ValueCell() :
  EvenOddCell()
{
}

void ValueCell::setFloat(float f) {
  m_float = f;
  FloatToString::convertFloatToText(f, m_buffer, 10);
}

void ValueCell::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddCell::drawRect(ctx, rect);
  // Write the content of the cell
  KDColor background = backgroundColor();
  KDSize textSize = KDText::stringSize(m_buffer);
  KDPoint origin(m_frame.width() - textSize.width(), 0.5f*(m_frame.height() - textSize.height()));
  ctx->drawString(m_buffer, origin, KDColorBlack, background);
}
