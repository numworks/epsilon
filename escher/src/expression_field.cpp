#include <escher/expression_field.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <algorithm>

constexpr KDCoordinate ExpressionField::k_maximalHeight;
constexpr KDCoordinate ExpressionField::k_minimalHeight;

ExpressionField::ExpressionField(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate) :
  Responder(parentResponder),
  View(),
  m_inputViewMemoizedHeight(0),
  m_textField(parentResponder, nullptr, k_textFieldBufferSize, k_textFieldBufferSize, inputEventHandlerDelegate, textFieldDelegate, KDFont::LargeFont, 0.0f, 0.5f, KDColorBlack, KDColorWhite),
  m_layoutField(parentResponder, inputEventHandlerDelegate, layoutFieldDelegate)
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

const char * ExpressionField::text() {
  if (!editionIsInTextField()) {
    m_layoutField.layout().serializeParsedExpression(m_textField.draftTextBuffer(), k_textFieldBufferSize, m_layoutField.context());
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
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_separatorThickness), Palette::GreyMiddle);
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
