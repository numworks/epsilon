#include "python_text_area.h"

namespace Code {

/* PythonTextArea */

void PythonTextArea::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
}

/* PythonTextArea::ContentView */

PythonTextArea::ContentView::ContentView(KDText::FontSize fontSize) :
  TextArea::ContentView(fontSize)
{
}

void PythonTextArea::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, m_backgroundColor);

  KDSize charSize = KDText::charSize(m_fontSize);

  // We want to draw even partially visible characters. So we need to round
  // down for the top left corner and up for the bottom right one.
  Text::Position topLeft(
    rect.x()/charSize.width(),
    rect.y()/charSize.height()
  );
  Text::Position bottomRight(
    rect.right()/charSize.width() + 1,
    rect.bottom()/charSize.height() + 1
  );

  int y = 0;
  size_t x = topLeft.column();

  for (Text::Line line : m_text) {
    if (y >= topLeft.line() && y <= bottomRight.line() && topLeft.column() < (int)line.length()) {
      //drawString(line.text(), 0, y*charHeight); // Naive version
      ctx->drawString(
        line.text() + topLeft.column(),
        KDPoint(x*charSize.width(), y*charSize.height()),
        m_fontSize,
        m_textColor,
        m_backgroundColor,
        min(line.length() - topLeft.column(), bottomRight.column() - topLeft.column())
      );
    }
    y++;
  }
}


}
