#include "editor_view.h"
#include <apps/global_preferences.h>
#include <poincare/integer.h>
#include <escher/app.h>
#include <poincare/preferences.h>

namespace Code {

/* EditorView */

constexpr char Code::EditorView::k_eol;

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
  m_gutterView.setOffset(scrollViewDataSource->offset().y());
  if (m_gutterView.isEditorReloadNeeded()) {
    redrawSubviews();
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
  m_gutterView.setOffset(0);
  KDCoordinate gutterWidth = m_gutterView.widthComputed().width();
  m_gutterView.setFrame(KDRect(0, 0, gutterWidth, bounds().height()), force);

  m_textArea.setFrame(KDRect(
        gutterWidth,
        0,
        bounds().width()-gutterWidth,
        bounds().height()),
      force);
  m_gutterView.loadMaxDigits();
}

void EditorView::redrawSubviews() {
  KDCoordinate gutterWidth = m_gutterView.widthComputed().width();
  m_gutterView.setFrame(KDRect(0, 0, gutterWidth, bounds().height()), true);
  m_textArea.setFrame(KDRect(
        gutterWidth,
        0,
        bounds().width()-gutterWidth,
        bounds().height()),
      true);
  markRectAsDirty(bounds());
}

/* EditorView::GutterView */

void EditorView::GutterView::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor textColor = Palette::PrimaryText;
  KDColor backgroundColor = Palette::CodeGutterViewBackground;

  ctx->fillRect(rect, backgroundColor);

  KDSize glyphSize = m_font->glyphSize();

  KDCoordinate firstLine = m_offset / glyphSize.height();
  KDCoordinate firstLinePixelOffset = m_offset - firstLine * glyphSize.height();

  char lineNumber[m_digits];
  int numberOfLines = bounds().height() / glyphSize.height() + 1;
  for (int i=0; i<numberOfLines; i++) {
    int lineNumberValue = (i + firstLine + 1);
    Poincare::Integer line(lineNumberValue);

    int lineDigits = getDigits(lineNumberValue);

    for (int j=0; j < (m_digits - lineDigits - 1); j++) {
      lineNumber[j] = '0';
    }

    line.serialize(lineNumber + (m_digits-lineDigits - 1), lineDigits + 1);

    KDCoordinate leftPadding = (m_digits - strlen(lineNumber) - 1) * glyphSize.width();
    ctx->drawString(
            lineNumber,
            KDPoint(k_margin + leftPadding, i*glyphSize.height() - firstLinePixelOffset),
            m_font,
            textColor,
            backgroundColor
    );
  }
}

void EditorView::GutterView::loadMaxDigits() {
  m_digits = getDigits((bounds().height() / m_font->glyphSize().height() + 1) + (m_offset / m_font->glyphSize().height())) + 1;
}

void EditorView::GutterView::setOffset(KDCoordinate offset) {
  if (m_offset == offset) {
    return;
  }
  m_offset = offset;
  m_previousDigits = m_digits;
  loadMaxDigits();
  markRectAsDirty(bounds());
}

KDSize EditorView::GutterView::widthComputed() {
  return KDSize(2 * k_margin + (m_digits - 1) * m_font->glyphSize().width(), 0);
}

int EditorView::GutterView::getDigits(int value) {
  int digits = 0;
  while (value >= pow(10, digits)) {digits++;}
  return digits;
}

bool EditorView::GutterView::isEditorReloadNeeded() {
  return m_previousDigits != m_digits;
}

}
