#include "console_line_cell.h"
#include "console_controller.h"
#include <kandinsky/point.h>
#include <kandinsky/coordinate.h>
#include <apps/i18n.h>

namespace Code {

ConsoleLineCell::ConsoleLineCell() :
  HighlightCell(),
  m_line()
{
}

void ConsoleLineCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor textBackgroundColor = isHighlighted() ? Palette::Select : KDColorWhite;
  ctx->fillRect(bounds(), KDColorWhite);
  if (m_line.type() == ConsoleLine::Type::Command) {
    ctx->drawString(I18n::translate(I18n::Message::ConsolePrompt), KDPointZero, ConsoleController::k_fontSize);
    KDCoordinate chevronsWidth = KDText::stringSize(I18n::translate(I18n::Message::ConsolePrompt), ConsoleController::k_fontSize).width();
    ctx->drawString(m_line.text(), KDPoint(chevronsWidth, KDCoordinate(0)), ConsoleController::k_fontSize, KDColorBlack, textBackgroundColor);
  } else {
    ctx->drawString(m_line.text(), KDPointZero, ConsoleController::k_fontSize, KDColorBlack, textBackgroundColor);
  }
}

void ConsoleLineCell::setLine(ConsoleLine line) {
  m_line = line;
  reloadCell();
}

}
