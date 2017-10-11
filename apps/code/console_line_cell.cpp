#include "console_line_cell.h"

namespace Code {

ConsoleLineCell::ConsoleLineCell() :
  HighlightCell(),
  m_line()
{
}

void ConsoleLineCell::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorRed);
  ctx->drawString(m_line.text(), KDPointZero);
}

void ConsoleLineCell::setLine(ConsoleLine line) {
  m_line = line;
  reloadCell();
}

}
