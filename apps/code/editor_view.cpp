#include "editor_view.h"
#include <apps/global_preferences.h>
#include <poincare/integer.h>
#include <escher/app.h>
#include <poincare/preferences.h>

namespace Code {

/* EditorView */

EditorView::EditorView(Responder * parentResponder, App * pythonDelegate) :
  Responder(parentResponder),
  View(),
  m_textArea(parentResponder, pythonDelegate, GlobalPreferences::sharedGlobalPreferences()->font()),
  m_gutterView(GlobalPreferences::sharedGlobalPreferences()->font())
{
  m_textArea.setScrollViewDelegate(this);
}

bool EditorView::isAutocompleting() const {
  return m_textArea.isAutocompleting();
}

void EditorView::resetSelection() {
  m_textArea.resetSelection();
}

void EditorView::scrollViewDidChangeOffset(ScrollViewDataSource * scrollViewDataSource) {
  if (m_gutterView.setOffsetAndNeedResize(scrollViewDataSource->offset().y())) {
    internalLayoutSubviews(true);
  }
}

View * EditorView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_textArea;
  }
  assert(index == 1);
  return &m_gutterView;
}

void EditorView::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_textArea);
}

void EditorView::layoutSubviews(bool force) {
  m_gutterView.setOffsetAndNeedResize(0); // Whatever the return is, we layout the editor view
  internalLayoutSubviews(force);
}

void EditorView::internalLayoutSubviews(bool force) {
  KDCoordinate gutterWidth = m_gutterView.computeWidth();
  m_gutterView.setFrame(KDRect(0, 0, gutterWidth, bounds().height()), force);

  m_textArea.setFrame(KDRect(
        gutterWidth,
        0,
        bounds().width()-gutterWidth,
        bounds().height()),
      force);
}

/* EditorView::GutterView */

void EditorView::GutterView::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor textColor = Palette::PrimaryText;
  KDColor backgroundColor = Palette::CodeGutterViewBackground;

  ctx->fillRect(rect, backgroundColor);

  KDSize glyphSize = m_font->glyphSize();

  KDCoordinate firstLine = m_offset / glyphSize.height();
  KDCoordinate firstLinePixelOffset = m_offset - firstLine * glyphSize.height();

  char lineNumberBuffer[m_numberOfDigits + 1];
  int numberOfLines = bounds().height() / glyphSize.height() + 1;
  for (int i=0; i<numberOfLines; i++) {
    int lineNumberValue = (i + firstLine + 1);
    Poincare::Integer line(lineNumberValue);

    int lineDigits = computeNumberOfDigitsFor(lineNumberValue);

    for (int j=0; j < m_numberOfDigits - lineDigits; j++) {
      lineNumberBuffer[j] = ' ';
    }

    line.serialize(lineNumberBuffer + (m_numberOfDigits - lineDigits), m_numberOfDigits + 1);

    ctx->drawString(
      lineNumberBuffer,
      KDPoint(k_margin, i*glyphSize.height() - firstLinePixelOffset),
      m_font,
      textColor,
      backgroundColor
    );
  }
}

bool EditorView::GutterView::setOffsetAndNeedResize(KDCoordinate offset) {
  if (m_offset == offset) {
    return false;
  }
  m_offset = offset;

  int numberOfDigits = computeMaxNumberOfDigits();
  if (numberOfDigits != m_numberOfDigits) {
    m_numberOfDigits = numberOfDigits;
    return true;
  }

  markRectAsDirty(bounds());
  return false;
}

int EditorView::GutterView::computeWidth() {
  return 2 * k_margin + (m_numberOfDigits) * m_font->glyphSize().width();
}

int EditorView::GutterView::computeMaxNumberOfDigits() {
  return computeNumberOfDigitsFor((bounds().height() / m_font->glyphSize().height() + 1) + (m_offset / m_font->glyphSize().height()));
}

int EditorView::GutterView::computeNumberOfDigitsFor(int value) {
  int digits = 1;
  while (value >= pow(10, digits)) {
    digits++;
  }
  return digits;
}

}
