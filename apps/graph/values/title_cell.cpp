#include "title_cell.h"
#include "../function_store.h"

TitleCell::TitleCell() :
  EvenOddCell(),
  m_text(nullptr)
{
}

void TitleCell::setText(const char * title) {
  m_text = (char *)title;
}

void TitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddCell::drawRect(ctx, rect);
  // Write the content of the cell
  KDColor background = backgroundColor();
  KDSize textSize = KDText::stringSize(m_text);
  KDPoint origin(0.5f*(m_frame.width() - textSize.width()), 0.5f*(m_frame.height() - textSize.height()));
  ctx->drawString(m_text, origin, KDColorBlack, background);
  ctx->drawString(Graph::Function::Parameter, origin.translatedBy(KDPoint(textSize.width(), 0)), KDColorBlack, background);
}
