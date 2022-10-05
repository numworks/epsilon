#include <escher/expression_field.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <algorithm>

namespace Escher {

constexpr KDCoordinate ExpressionField::k_maximalHeight;
constexpr KDCoordinate ExpressionField::k_minimalHeight;

ExpressionField::ExpressionField(Responder * parentResponder,
                                 InputEventHandlerDelegate * inputEventHandlerDelegate,
                                 TextFieldDelegate * textFieldDelegate,
                                 LayoutFieldDelegate * layoutFieldDelegate) :
    Responder(parentResponder),
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
}

KDSize ExpressionField::minimalSizeForOptimalDisplay() const {
  return KDSize(0, inputViewHeight());
}

bool ExpressionField::editionIsInTextField() const {
  return Poincare::Preferences::sharedPreferences()->editionMode() == Poincare::Preferences::EditionMode::Edition1D;
}

bool ExpressionField::isEmpty() const {
  return editionIsInTextField() ? (m_textField.draftTextLength() == 0) : !m_layoutField.hasText();
}

bool ExpressionField::inputViewHeightDidChange() {
  KDCoordinate newHeight = inputViewHeight();
  bool didChange = m_inputViewMemoizedHeight != newHeight;
  m_inputViewMemoizedHeight = newHeight;
  return didChange;
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

size_t ExpressionField::moveCursorAndDumpContent(char * buffer, size_t bufferSize) {
  size_t size;
  size_t returnValue;
  char * currentContent;
  if (editionIsInTextField()) {
    size = strlen(m_textField.draftTextBuffer()) + 1;
    m_textField.setCursorLocation(m_textField.cursorLocation() + size - 1);
    currentContent = m_textField.draftTextBuffer();
    /* We take advantage of the fact that draftTextBuffer is null terminated to
     * use a size of 0 as a shorthand for "The buffer contains raw text instead
     * of layouts", since the size of a layout is at least 32 (the size of an
     * empty horizontal layout). This way, we can detect when the edition mode
     * has changed and invalidate the data.*/
    returnValue = 0;
  } else {
    /* moveCursorAndDumpContent will be called whenever Calculation exits,
     * even when an exception occurs. We thus need to check the validity of the
     * layout we are dumping (m_layoutField.contentView.expressionView.layout).
     * However, attempting to get a handle on a layout that has been erased
     * will crash the program. We need the check to be performed on the
     * original object in expressionView. */
    if (!m_layoutField.layoutHasNode()) {
      buffer[0] = 0;
      return 0;
    }
    m_layoutField.putCursorRightOfLayout();
    size = m_layoutField.layout().size();
    currentContent = reinterpret_cast<char *>(m_layoutField.layout().node());
    returnValue = size;
  }
  if (size > bufferSize - 1) {
    buffer[0] = 0;
    return 0;
  }
  memcpy(buffer, currentContent, size);
  return returnValue;
}

void ExpressionField::putCursorLeftOfField() {
  if (editionIsInTextField()) {
    m_textField.setCursorLocation(m_textField.text());
  } else {
    m_layoutField.putCursorLeftOfLayout();
  }
}

void ExpressionField::restoreContent(const char * buffer, size_t size) {
  if (editionIsInTextField()) {
    if (size != 0 || buffer[0] == 0) {
      /* A size other than 0 means the buffer contains Layout information
       * (instead of raw text) that we don't want to restore. This is most
       * likely because the edition mode has been changed between use. */
      return;
    }
    setText(buffer);
    return;
  }
  if (size == 0) {
    return;
  }
  m_layoutField.setLayout(Poincare::Layout::LayoutFromAddress(buffer, size));
}

}
