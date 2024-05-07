#include "editor_view.h"

#include <apps/global_preferences.h>
#include <escher/palette.h>
#include <poincare/print_int.h>

#include "app.h"

using namespace Escher;

namespace Code {

/* EditorView */

EditorView::EditorView(Responder* parentResponder, App* pythonDelegate)
    : Responder(parentResponder),
      View(),
      m_textArea(parentResponder, pythonDelegate,
                 GlobalPreferences::SharedGlobalPreferences()->font()),
      m_gutterView(GlobalPreferences::SharedGlobalPreferences()->font()) {
  m_textArea.setScrollViewDataSourceDelegate(this);
}

bool EditorView::isAutocompleting() const {
  return m_textArea.isAutocompleting();
}

void EditorView::resetSelection() { m_textArea.resetSelection(); }

void EditorView::removeAutocompletionText() {
  if (m_textArea.isAutocompleting()) {
    m_textArea.removeAutocompletionText();
  }
}

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
  App::app()->setFirstResponder(&m_textArea);
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

  char buffer[k_lineNumberCharLength + 1];
  int numberOfLines = bounds().height() / glyphSize.height() + 1;
  for (int i = 0; i < numberOfLines; i++) {
    // Only the first two digits are displayed
    int lineNumberValue = (i + firstLine + 1) % 100;
    int length =
        (firstLine < 10 ? Poincare::PrintInt::Left : Poincare::PrintInt::Right)(
            lineNumberValue, buffer, k_lineNumberCharLength);
    assert(length <= k_lineNumberCharLength);
    buffer[length] = 0;
    KDCoordinate leftPadding = (2 - strlen(buffer)) * glyphSize.width();
    ctx->drawString(buffer,
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
  markWholeFrameAsDirty();
}

KDSize EditorView::GutterView::minimalSizeForOptimalDisplay() const {
  int numberOfChars = 2;  // TODO: Could be computed
  return KDSize(2 * k_margin + numberOfChars * KDFont::GlyphWidth(m_font), 0);
}

}  // namespace Code
