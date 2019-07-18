#include "editor_view.h"
#include <poincare/integer.h>
#include <escher/app.h>

namespace Code {

/* EditorView */

static constexpr const KDFont * editorFont = KDFont::LargeFont;

EditorView::EditorView(Responder * parentResponder, App * pythonDelegate) :
  Responder(parentResponder),
  View(),
  m_textArea(parentResponder, pythonDelegate, editorFont),
  m_gutterView(editorFont)
{
  m_textArea.setScrollViewDelegate(this);
}

void EditorView::scrollViewDidChangeOffset(ScrollViewDataSource * scrollViewDataSource) {
  m_gutterView.setOffset(scrollViewDataSource->offset().y());
}

int EditorView::numberOfSubviews() const {
  return 2;
}

View * EditorView::subviewAtIndex(int index) {
  View * subviews[] = {&m_textArea, &m_gutterView};
  return subviews[index];
}

void EditorView::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_textArea);
}

void EditorView::layoutSubviews() {
  m_gutterView.setOffset(0);
  KDCoordinate gutterWidth = m_gutterView.minimalSizeForOptimalDisplay().width();
  m_gutterView.setFrame(KDRect(0, 0, gutterWidth, bounds().height()));

  m_textArea.setFrame(KDRect(
    gutterWidth,
    0,
    bounds().width()-gutterWidth,
    bounds().height()
  ));
}

/* EditorView::GutterView */

EditorView::GutterView::GutterView(const KDFont * font) :
  View(),
  m_font(font),
  m_offset(0)
{
}

void EditorView::GutterView::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor textColor = KDColor::RGB24(0x919EA4);
  KDColor backgroundColor = KDColor::RGB24(0xE4E6E7);

  ctx->fillRect(rect, backgroundColor);

  KDSize glyphSize = m_font->glyphSize();

  KDCoordinate firstLine = m_offset / glyphSize.height();
  KDCoordinate firstLinePixelOffset = m_offset - firstLine * glyphSize.height();

  char lineNumber[4];
  int numberOfLines = bounds().height() / glyphSize.height() + 1;
  for (int i=0; i<numberOfLines; i++) {
    Poincare::Integer line(i + firstLine + 1);
    line.serialize(lineNumber, 4);
    KDCoordinate leftPadding = (2 - strlen(lineNumber)) * glyphSize.width();
    ctx->drawString(
      lineNumber,
      KDPoint(k_margin + leftPadding, i*glyphSize.height() - firstLinePixelOffset),
      m_font,
      textColor,
      backgroundColor
    );
  }
}

void EditorView::GutterView::setOffset(KDCoordinate offset) {
  if (m_offset == offset) {
    return;
  }
  m_offset = offset;
  markRectAsDirty(bounds());
}


KDSize EditorView::GutterView::minimalSizeForOptimalDisplay() const {
  int numberOfChars = 2; // TODO: Could be computed
  return KDSize(2 * k_margin + numberOfChars * m_font->glyphSize().width(), 0);
}

}
