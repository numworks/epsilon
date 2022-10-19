#include <escher/expression_field.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <algorithm>

using namespace Poincare;

namespace Escher {

ExpressionField::ExpressionField(Responder * parentResponder,
                                 InputEventHandlerDelegate * inputEventHandlerDelegate,
                                 TextFieldDelegate * textFieldDelegate,
                                 LayoutFieldDelegate * layoutFieldDelegate,
                                 float horizontalAlignment,
                                 float verticalAlignment) :
    WithBlinkingTextCursor<Responder>(parentResponder),
    View(),
    m_textField(parentResponder,
                nullptr,
                k_textFieldBufferSize,
                k_textFieldBufferSize,
                inputEventHandlerDelegate,
                textFieldDelegate,
                KDFont::Size::Large,
                horizontalAlignment,
                verticalAlignment,
                KDColorBlack,
                KDColorWhite),
    m_layoutField(parentResponder,
                  inputEventHandlerDelegate,
                  layoutFieldDelegate,
                  KDFont::Size::Large,
                  horizontalAlignment,
                  verticalAlignment),
    m_inputViewMemoizedHeight(0) {
  // Initialize text field
  m_textField.setBackgroundColor(KDColorWhite);
  // Initialize layout field
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

void ExpressionField::layoutSubviewsInRect(KDRect rect, bool force) {
  if (editionIsInTextField()) {
    m_textField.setFrame(rect, force);
    m_layoutField.setFrame(KDRectZero, force);
    return;
  }
  m_layoutField.setFrame(rect, force);
  m_textField.setFrame(KDRectZero, force);
}

void ExpressionField::layoutSubviews(bool force) {
  layoutSubviewsInRect(bounds(), force);
}

bool ExpressionField::handleEvent(Ion::Events::Event event) {
  return editionIsInTextField() ? m_textField.handleEvent(event) : m_layoutField.handleEvent(event);
}

void ExpressionField::didBecomeFirstResponder() {
  m_inputViewMemoizedHeight = inputViewHeight();
  WithBlinkingTextCursor<Responder>::didBecomeFirstResponder();
}

KDSize ExpressionField::minimalSizeForOptimalDisplay() const {
  // return KDSize(1000, inputViewHeight());
  return KDSize(editionIsInTextField() ? m_textField.minimalSizeForOptimalDisplay().width() : m_layoutField.minimalSizeForOptimalDisplay().width(), inputViewHeight());
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
  return editionIsInTextField() ?  m_textField.minimalSizeForOptimalDisplay().height() : m_layoutField.minimalSizeForOptimalDisplay().height();
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
