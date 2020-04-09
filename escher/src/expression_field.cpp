#include <escher/expression_field.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <algorithm>
#include <new>

ExpressionField::ExpressionField(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate) :
  Responder(parentResponder),
  View(),
  m_inputViewMemoizedHeight(0),
  m_usingTextField(shouldUseTextField())
{
  if (usingTextField()) {
    // Construct and initialize text field
    new (&m_textField) TextField(parentResponder, nullptr, k_textFieldBufferSize, k_textFieldBufferSize, inputEventHandlerDelegate, textFieldDelegate, KDFont::LargeFont, 0.0f, 0.5f, KDColorBlack, KDColorWhite);
    m_textField.setMargins(0, k_horizontalMargin, 0, k_horizontalMargin);
    m_textField.setBackgroundColor(KDColorWhite);
  } else {
    // Construct and initialize layout field
    new (&m_layoutField) LayoutField(parentResponder, inputEventHandlerDelegate, layoutFieldDelegate);
    m_layoutField.setMargins(k_verticalMargin, k_horizontalMargin, k_verticalMargin, k_horizontalMargin);
    m_layoutField.setBackgroundColor(KDColorWhite);
  }
}

ExpressionField::~ExpressionField() {
  if (usingTextField()) {
    m_textField.~TextField();
  } else {
    m_layoutField.~LayoutField();
  }
}

void ExpressionField::setEditing(bool isEditing, bool reinitDraftBuffer) {
  if (usingTextField()) {
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

bool ExpressionField::isEditing() {
  return field<EditableField>()->isEditing();
}

const char * ExpressionField::text() {
  if (usingTextField()) {
    m_layoutField.layout().serializeParsedExpression(m_textBuffer, k_textFieldBufferSize, m_layoutField.context());
    return m_textBuffer;
  } else {
    return m_textField.draftTextBuffer();
  }
}

void ExpressionField::setText(const char * text) {
  if (usingTextField()) {
    m_textField.reinitDraftTextBuffer();
    m_textField.setText(text);
  } else {
    m_layoutField.clearLayout();
    m_layoutField.handleEventWithText(text, false, true);
  }
}

View * ExpressionField::subviewAtIndex(int index) {
  assert(index == 0);
  return field<View>(); // We know the field is also a view
}

void ExpressionField::layoutSubviews(bool force) {
  KDRect inputViewFrame(0, k_separatorThickness, bounds().width(), bounds().height() - k_separatorThickness);
  field<View>()->setFrame(inputViewFrame, force);
}

void ExpressionField::drawRect(KDContext * ctx, KDRect rect) const {
  // Draw the separator
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_separatorThickness), Palette::GreyMiddle);
}

bool ExpressionField::handleEvent(Ion::Events::Event event) {
  return field<Responder>()->handleEvent(event);
}

void ExpressionField::didBecomeFirstResponder() {
  m_inputViewMemoizedHeight = inputViewHeight();
}

KDSize ExpressionField::minimalSizeForOptimalDisplay() const {
  return KDSize(0, inputViewHeight());
}

bool ExpressionField::shouldUseTextField() const {
  return Poincare::Preferences::sharedPreferences()->editionMode() == Poincare::Preferences::EditionMode::Edition1D;
}

bool ExpressionField::isEmpty() const {
  return usingTextField() ? (m_textField.draftTextLength() == 0) : !m_layoutField.hasText();
}

bool ExpressionField::inputViewHeightDidChange() {
  KDCoordinate newHeight = inputViewHeight();
  bool didChange = m_inputViewMemoizedHeight != newHeight;
  m_inputViewMemoizedHeight = newHeight;
  return didChange;
}

bool ExpressionField::handleEventWithText(const char * text, bool indentation, bool forceCursorRightOfText) {
  return field<EditableField>()->handleEventWithText(text, indentation, forceCursorRightOfText);
}

KDCoordinate ExpressionField::inputViewHeight() const {
  return k_separatorThickness
    + (usingTextField() ? k_minimalHeight :
        std::min(k_maximalHeight,
          std::max(k_minimalHeight, m_layoutField.minimalSizeForOptimalDisplay().height())));
}
