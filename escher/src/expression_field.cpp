#include <escher/expression_field.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <algorithm>

using namespace Poincare;

namespace Escher {

constexpr KDCoordinate ExpressionField::k_maximalHeight;
constexpr KDCoordinate ExpressionField::k_minimalHeight;

ExpressionField::ExpressionField(Responder * parentResponder,
                                 InputEventHandlerDelegate * inputEventHandlerDelegate,
                                 TextFieldDelegate * textFieldDelegate,
                                 LayoutFieldDelegate * layoutFieldDelegate) :
    WithBlinkingTextCursor<Responder>(parentResponder),
    View(),
    m_textField(parentResponder,
                nullptr,
                k_textFieldBufferSize,
                k_textFieldBufferSize,
                inputEventHandlerDelegate,
                textFieldDelegate,
                KDFont::Size::Large,
                KDContext::k_alignLeft,
                KDContext::k_alignCenter,
                KDColorBlack,
                KDColorWhite),
    m_layoutField(parentResponder, inputEventHandlerDelegate, layoutFieldDelegate),
    m_inputViewMemoizedHeight(0) {
  // Initialize text field
  m_textField.setMargins(0, k_horizontalMargin, 0, k_horizontalMargin);
  m_textField.setBackgroundColor(KDColorWhite);
  // Initialize layout field
  m_layoutField.setMargins(k_verticalMargin, k_horizontalMargin, k_verticalMargin, k_horizontalMargin);
  m_layoutField.setBackgroundColor(KDColorWhite);
}

void ExpressionField::setEditing(bool isEditing, bool reinitDraftBuffer) {
  if (editionIsInTextField()) {
    if (reinitDraftBuffer) {
      m_textField.reinitDraftTextBuffer();
    }
    m_textField.setEditing(isEditing);
  } else {
    if (reinitDraftBuffer) {
      m_layoutField.clearLayout();
    }
    m_layoutField.setEditing(isEditing);
  }
}

bool ExpressionField::isEditing() const {
  return editionIsInTextField() ? m_textField.isEditing() : m_layoutField.isEditing();
}

void ExpressionField::setBackgroundColor(KDColor backgroundColor) {
  editionIsInTextField() ? m_textField.setBackgroundColor(backgroundColor) : m_layoutField.setBackgroundColor(backgroundColor);
}

const char * ExpressionField::text() {
  if (!editionIsInTextField()) {
    m_layoutField.layout().serializeForParsing(m_textField.draftTextBuffer(), k_textFieldBufferSize);
  }
  return m_textField.draftTextBuffer();
}

void ExpressionField::setText(const char * text) {
  if (editionIsInTextField()) {
    m_textField.reinitDraftTextBuffer();
    m_textField.setText(text);
  } else {
    m_layoutField.clearLayout();
    m_layoutField.handleEventWithText(text, false, true);
  }
}

View * ExpressionField::subviewAtIndex(int index) {
  assert(index == 0);
  if (editionIsInTextField()) {
    return &m_textField;
  }
  return &m_layoutField;
}

void ExpressionField::layoutSubviews(bool force) {
  KDRect inputViewFrame(0, k_separatorThickness, bounds().width(), bounds().height() - k_separatorThickness);
  if (editionIsInTextField()) {
    m_textField.setFrame(inputViewFrame, force);
    m_layoutField.setFrame(KDRectZero, force);
    return;
  }
  m_layoutField.setFrame(inputViewFrame, force);
  m_textField.setFrame(KDRectZero, force);
}

void ExpressionField::drawRect(KDContext * ctx, KDRect rect) const {
  // Draw the separator
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_separatorThickness), Palette::GrayMiddle);
}

bool ExpressionField::handleEvent(Ion::Events::Event event) {
  return editionIsInTextField() ? m_textField.handleEvent(event) : m_layoutField.handleEvent(event);
}

void ExpressionField::didBecomeFirstResponder() {
  m_inputViewMemoizedHeight = inputViewHeight();
  WithBlinkingTextCursor<Responder>::didBecomeFirstResponder();
}

KDSize ExpressionField::minimalSizeForOptimalDisplay() const {
  return KDSize(0, inputViewHeight());
}

bool ExpressionField::editionIsInTextField() const {
  return false;
}

bool ExpressionField::isEmpty() const {
  return editionIsInTextField() ? (m_textField.draftTextLength() == 0) : m_layoutField.isEmpty();
}

bool ExpressionField::inputViewHeightDidChange() {
  KDCoordinate newHeight = inputViewHeight();
  bool didChange = m_inputViewMemoizedHeight != newHeight;
  m_inputViewMemoizedHeight = newHeight;
  return didChange;
}

void ExpressionField::reload() {
  if (!editionIsInTextField()) {
    m_layoutField.reload(KDSizeZero);
  }
  // Currently used only for its baseline effect, useless on textField
}

bool ExpressionField::handleEventWithText(const char * text, bool indentation, bool forceCursorRightOfText) {
  if (editionIsInTextField()) {
    return m_textField.handleEventWithText(text, indentation, forceCursorRightOfText);
  } else {
    return m_layoutField.handleEventWithText(text, indentation, forceCursorRightOfText);
  }
}

KDCoordinate ExpressionField::inputViewHeight() const {
  return k_separatorThickness
    + (editionIsInTextField() ? k_minimalHeight :
        std::min(k_maximalHeight,
          std::max(k_minimalHeight, m_layoutField.minimalSizeForOptimalDisplay().height())));
}

size_t ExpressionField::dumpContent(char * buffer, size_t bufferSize, int * cursorOffset, int * position) {
  return editionIsInTextField() ? m_textField.dumpContent(buffer, bufferSize, cursorOffset) : m_layoutField.dumpContent(buffer, bufferSize, cursorOffset, position);
}

void ExpressionField::putCursorLeftOfField() {
  if (editionIsInTextField()) {
    m_textField.setCursorLocation(m_textField.text());
  } else {
    m_layoutField.putCursorOnOneSide(OMG::Direction::Left());
  }
}

void ExpressionField::restoreContent(const char * buffer, size_t size, int * cursorOffset, int * position) {
  if (editionIsInTextField()) {
    if (size != 0 || buffer[0] == 0) {
      /* A size other than 0 means the buffer contains Layout information
       * (instead of raw text) that we don't want to restore. This is most
       * likely because the edition mode has been changed between use. */
      return;
    }
    setText(buffer);
    if (*cursorOffset != -1) {
      m_textField.setCursorLocation(m_textField.draftTextBuffer() + *cursorOffset);
    }
    return;
  }
  if (size == 0) {
    return;
  }
  m_layoutField.setLayout(Layout::LayoutFromAddress(buffer, size));
  if (*cursorOffset != -1) {
    const LayoutNode * cursorNode = reinterpret_cast<const LayoutNode *>(reinterpret_cast<char *>(m_layoutField.layout().node()) + *cursorOffset);
    LayoutCursor restoredCursor = LayoutCursor(Layout(cursorNode));
    restoredCursor.setPosition(*position);
    *m_layoutField.cursor() = restoredCursor;
  }
}

}
