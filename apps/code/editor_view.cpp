#include "editor_view.h"

#include <apps/global_preferences.h>
#include <escher/app.h>
#include <escher/palette.h>
#include <poincare/integer.h>

using namespace Escher;

namespace Code {

/* EditorView */

EditorView::EditorView(Responder* parentResponder, App* pythonDelegate)
    : Responder(parentResponder),
      View(),
      m_textArea(parentResponder, pythonDelegate,
                 GlobalPreferences::sharedGlobalPreferences->font()),
      m_gutterView(GlobalPreferences::sharedGlobalPreferences->font()) {
  m_textArea.setScrollViewDelegate(this);
}

bool EditorView::isAutocompleting() const {
  return m_textArea.isAutocompleting();
}

void EditorView::resetSelection() { m_textArea.resetSelection(); }

void EditorView::scrollViewDidChangeOffset(
    ScrollViewDataSource* scrollViewDataSource) {
  m_gutterView.setOffset(scrollViewDataSource->offset().y());
}

View* EditorView::subviewAtIndex(int index) {
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
  m_gutterView.setOffset(0);
  KDCoordinate gutterWidth =
      m_gutterView.minimalSizeForOptimalDisplay().width();
  setChildFrame(&m_gutterView, KDRect(0, 0, gutterWidth, bounds().height()),
                force);

  setChildFrame(
      &m_textArea,
      KDRect(gutterWidth, 0, bounds().width() - gutterWidth, bounds().height()),
      force);
}

/* EditorView::GutterView */

void EditorView::GutterView::drawRect(KDContext* ctx, KDRect rect) const {
  KDColor textColor = Palette::BlueishGray;
  KDColor backgroundColor = KDColor::RGB24(0xE4E6E7);

  ctx->fillRect(rect, backgroundColor);

  KDSize glyphSize = KDFont::GlyphSize(m_font);

  KDCoordinate firstLine = m_offset / glyphSize.height();
  KDCoordinate firstLinePixelOffset = m_offset - firstLine * glyphSize.height();

  char lineNumber[k_lineNumberCharLength];
  int numberOfLines = bounds().height() / glyphSize.height() + 1;
  for (int i = 0; i < numberOfLines; i++) {
    // Only the first two digits are displayed
    int lineNumberValue = (i + firstLine + 1) % 100;
    Poincare::Integer line(lineNumberValue);
    if (firstLine < 10 || lineNumberValue >= 10) {
      line.serialize(lineNumber, k_lineNumberCharLength);
    } else {
      // Add a leading "0"
      lineNumber[0] = '0';
      line.serialize(lineNumber + 1, k_lineNumberCharLength - 1);
    }
    KDCoordinate leftPadding = (2 - strlen(lineNumber)) * glyphSize.width();
    ctx->drawString(lineNumber,
                    KDPoint(k_margin + leftPadding,
                            i * glyphSize.height() - firstLinePixelOffset),
                    {.glyphColor = textColor,
                     .backgroundColor = backgroundColor,
                     .font = m_font});
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
  int numberOfChars = 2;  // TODO: Could be computed
  return KDSize(2 * k_margin + numberOfChars * KDFont::GlyphWidth(m_font), 0);
}

}  // namespace Code
