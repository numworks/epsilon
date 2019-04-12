#include <escher/expression_field.h>
#include <poincare/preferences.h>
#include <assert.h>

static inline KDCoordinate minCoordinate(KDCoordinate x, KDCoordinate y) { return x < y ? x : y; }
static inline KDCoordinate maxCoordinate(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

ExpressionField::ExpressionField(Responder * parentResponder, char * textBuffer, int textBufferLength, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate) :
  Responder(parentResponder),
  View(),
  m_textField(parentResponder, textBuffer, textBuffer, textBufferLength, inputEventHandlerDelegate, textFieldDelegate, false, KDFont::LargeFont, 0.0f, 0.5f, KDColorBlack, KDColorWhite),
  m_layoutField(parentResponder, inputEventHandlerDelegate, layoutFieldDelegate),
  m_textBuffer(textBuffer),
  m_textBufferLength(textBufferLength)
{
  // Initialize text field
  m_textField.setMargins(0, k_horizontalMargin, 0, k_horizontalMargin);
  m_textField.setBackgroundColor(KDColorWhite);
  // Initialize layout field
  m_layoutField.setMargins(k_verticalMargin, k_horizontalMargin, k_verticalMargin, k_horizontalMargin);
  m_layoutField.setBackgroundColor(KDColorWhite);
}

void ExpressionField::setEditing(bool isEditing, bool reinitDraftBuffer) {
  if (editionIsInTextField()) {
    m_textField.setEditing(isEditing, reinitDraftBuffer);
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

const char * ExpressionField::text() {
  if (!editionIsInTextField()) {
    m_layoutField.layout().serializeParsedExpression(m_textBuffer, m_textBufferLength);
  }
  return m_textBuffer;
}

void ExpressionField::setText(const char * text) {
  if (editionIsInTextField()) {
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

void ExpressionField::layoutSubviews() {
  KDRect inputViewFrame(0, k_separatorThickness, bounds().width(), bounds().height() - k_separatorThickness);
  if (editionIsInTextField()) {
    m_textField.setFrame(inputViewFrame);
    m_layoutField.setFrame(KDRectZero);
    return;
  }
  m_layoutField.setFrame(inputViewFrame);
  m_textField.setFrame(KDRectZero);
}

void ExpressionField::reload() {
  layoutSubviews();
  markRectAsDirty(bounds());
}

void ExpressionField::drawRect(KDContext * ctx, KDRect rect) const {
  // Draw the separator
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_separatorThickness), Palette::GreyMiddle);
}

bool ExpressionField::handleEvent(Ion::Events::Event event) {
  return editionIsInTextField() ? m_textField.handleEvent(event) : m_layoutField.handleEvent(event);
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

bool ExpressionField::heightIsMaximal() const {
  return inputViewHeight() == k_separatorThickness + maximalHeight();
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
    + (editionIsInTextField() ? k_textFieldHeight :
        minCoordinate(maximalHeight(),
          maxCoordinate(k_textFieldHeight, m_layoutField.minimalSizeForOptimalDisplay().height())));
}

KDCoordinate ExpressionField::maximalHeight() const {
  return 0.6*Ion::Display::Height;
}
