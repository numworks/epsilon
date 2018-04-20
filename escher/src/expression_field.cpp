#include <escher/expression_field.h>
#include <poincare/preferences.h>
#include <assert.h>

ExpressionField::ExpressionField(Responder * parentResponder, char * textBuffer, int textBufferLength, Poincare::ExpressionLayout * layout, TextFieldDelegate * textFieldDelegate, ExpressionLayoutFieldDelegate * expressionLayoutFieldDelegate) :
  Responder(parentResponder),
  View(),
  m_textField(parentResponder, textBuffer, textBuffer, textBufferLength, textFieldDelegate, false),
  m_expressionLayoutField(parentResponder, layout, expressionLayoutFieldDelegate),
  m_textBuffer(textBuffer),
  m_textBufferLength(textBufferLength)
{
}

void ExpressionField::setEditing(bool isEditing, bool reinitDraftBuffer) {
  if (editionIsInTextField()) {
    m_textField.setEditing(isEditing, reinitDraftBuffer);
  } else {
    if (reinitDraftBuffer) {
      m_expressionLayoutField.clearLayout();
    }
    m_expressionLayoutField.setEditing(isEditing);
  }
}

bool ExpressionField::isEditing() const {
  return editionIsInTextField() ? m_textField.isEditing() : m_expressionLayoutField.isEditing();
}

const char * ExpressionField::text() {
  if (!editionIsInTextField()) {
    m_expressionLayoutField.writeTextInBuffer(m_textBuffer, m_textBufferLength);
  }
  return m_textBuffer;
}

void ExpressionField::setText(const char * text) {
  if (editionIsInTextField()) {
    m_textField.setText(text);
    return;
  }
  m_expressionLayoutField.clearLayout();
  if (strlen(text) > 0) {
    m_expressionLayoutField.insertLayoutFromTextAtCursor(text);
  }
}

void ExpressionField::insertText(const char * text) {
  if (editionIsInTextField()) {
    m_textField.handleEventWithText(text);
  } else {
    m_expressionLayoutField.setEditing(true);
    m_expressionLayoutField.insertLayoutFromTextAtCursor(text);
  }
}

View * ExpressionField::subviewAtIndex(int index) {
  assert(index == 0);
  if (editionIsInTextField()) {
    return &m_textField;
  }
  return &m_expressionLayoutField;
}

void ExpressionField::layoutSubviews() {
  KDRect inputViewFrame(k_leftMargin, k_separatorThickness, bounds().width() - k_leftMargin, bounds().height() - k_separatorThickness);
  if (editionIsInTextField()) {
    m_textField.setFrame(inputViewFrame);
    m_expressionLayoutField.setFrame(KDRectZero);
    return;
  }
  m_expressionLayoutField.setFrame(inputViewFrame);
  m_textField.setFrame(KDRectZero);
}

void ExpressionField::reload() {
  layoutSubviews();
  markRectAsDirty(bounds());
}

void ExpressionField::drawRect(KDContext * ctx, KDRect rect) const {
  // Draw the separator
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_separatorThickness), Palette::GreyMiddle);
  // Color the left margin
  ctx->fillRect(KDRect(0, k_separatorThickness, k_leftMargin, bounds().height() - k_separatorThickness), m_textField.backgroundColor());
  if (!editionIsInTextField()) {
    // Color the upper margin
    ctx->fillRect(KDRect(0, k_separatorThickness, bounds().width(), k_verticalExpressionViewMargin), m_textField.backgroundColor());
  }
}

bool ExpressionField::handleEvent(Ion::Events::Event event) {
  return editionIsInTextField() ? m_textField.handleEvent(event) : m_expressionLayoutField.handleEvent(event);
}

KDSize ExpressionField::minimalSizeForOptimalDisplay() const {
  return KDSize(0, inputViewHeight());
}

bool ExpressionField::editionIsInTextField() const {
  return Poincare::Preferences::sharedPreferences()->editionMode() == Poincare::Preferences::EditionMode::Edition1D;
}

bool ExpressionField::isEmpty() const {
  return editionIsInTextField() ? (m_textField.draftTextLength() == 0) : m_expressionLayoutField.hasText();
}

bool ExpressionField::heightIsMaximal() const {
  return inputViewHeight() == k_separatorThickness + k_verticalExpressionViewMargin + maximalHeight();
}

KDCoordinate ExpressionField::inputViewHeight() const {
  if (editionIsInTextField()) {
    return k_separatorThickness + k_textFieldHeight;
  }
  return k_separatorThickness
    + k_verticalExpressionViewMargin
    + min(maximalHeight(),
        max(k_textFieldHeight,
          m_expressionLayoutField.minimalSizeForOptimalDisplay().height()
          + k_verticalExpressionViewMargin));
}

KDCoordinate ExpressionField::maximalHeight() const {
  return 0.6*Ion::Display::Height;
}
