#include <escher/solid_text_area.h>

void SolidTextArea::ContentView::clearRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, m_backgroundColor);
}

void SolidTextArea::ContentView::drawLine(KDContext * ctx, int line, const char * text, size_t length, int fromColumn, int toColumn) const {
  drawStringAt(
    ctx,
    line,
    fromColumn,
    text + fromColumn,
    min(length - fromColumn, toColumn - fromColumn),
    m_textColor,
    m_backgroundColor
  );
}
