#include "editor_view.h"
#include <poincare.h>
#include <escher/app.h>

/* EditorView */

EditorView::EditorView(Responder * parentResponder) :
  Responder(parentResponder),
  View(),
  m_textArea(parentResponder),
  m_gutterView(KDText::FontSize::Large)
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
  app()->setFirstResponder(&m_textArea);
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

EditorView::GutterView::GutterView(KDText::FontSize fontSize) :
  View(),
  m_fontSize(fontSize),
  m_offset(0)
{
}

void EditorView::GutterView::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor textColor = KDColor::RGB24(0x919EA4);
  KDColor backgroundColor = KDColor::RGB24(0xE4E6E7);

  ctx->fillRect(rect, backgroundColor);

  KDSize charSize = KDText::charSize(m_fontSize);

  KDCoordinate firstLine = m_offset / charSize.height();
  KDCoordinate firstLinePixelOffset = m_offset - firstLine * charSize.height();

  char lineNumber[4];
  int numberOfLines = bounds().height() / charSize.height() + 1;
  for (int i=0; i<numberOfLines; i++) {
    Poincare::Integer line(i + firstLine + 1);
    line.writeTextInBuffer(lineNumber, 4);
    KDCoordinate leftPadding = (2 - strlen(lineNumber)) * charSize.width();
    ctx->drawString(
      lineNumber,
      KDPoint(k_margin + leftPadding, i*charSize.height() - firstLinePixelOffset),
      m_fontSize,
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
  return KDSize(2 * k_margin + numberOfChars * KDText::charSize(m_fontSize).width(), 0);
}
