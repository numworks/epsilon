#include <escher/editable_expression_view.h>
#include <poincare/preferences.h>
#include <assert.h>

EditableExpressionView::EditableExpressionView(Responder * parentResponder, TextFieldDelegate * textFieldDelegate, ScrollableExpressionViewWithCursorDelegate * scrollableExpressionViewWithCursorDelegate) :
  Responder(parentResponder),
  View(),
  m_textField(parentResponder, m_textBody, m_textBody, k_bufferLength, textFieldDelegate, false),
  m_scrollableExpressionViewWithCursor(parentResponder, new Poincare::HorizontalLayout(), scrollableExpressionViewWithCursorDelegate)
{
  m_textBody[0] = 0;
}

void EditableExpressionView::setEditing(bool isEditing, bool reinitDraftBuffer) {
  if (editionIsInTextField()) {
    m_textField.setEditing(isEditing, reinitDraftBuffer);
  }
  if (reinitDraftBuffer) {
    m_scrollableExpressionViewWithCursor.clearLayout();
  }
  m_scrollableExpressionViewWithCursor.setEditing(isEditing);
}

bool EditableExpressionView::isEditing() const {
  return editionIsInTextField() ? m_textField.isEditing() : m_scrollableExpressionViewWithCursor.isEditing();
}

const char * EditableExpressionView::text() {
  if (!editionIsInTextField()) {
    m_scrollableExpressionViewWithCursor.expressionViewWithCursor()->expressionView()->expressionLayout()->writeTextInBuffer(m_textBody, k_bufferLength);
  }
  return m_textBody;
}

void EditableExpressionView::setText(const char * text) {
  if (editionIsInTextField()) {
    m_textField.setText(text);
    return;
  }
  m_scrollableExpressionViewWithCursor.clearLayout();
  if (strlen(text) > 0) {
    m_scrollableExpressionViewWithCursor.insertLayoutFromTextAtCursor(text);
  }
}

void EditableExpressionView::insertText(const char * text) {
  if (editionIsInTextField()) {
    m_textField.handleEventWithText(text);
  } else {
    m_scrollableExpressionViewWithCursor.setEditing(true);
    m_scrollableExpressionViewWithCursor.insertLayoutFromTextAtCursor(text);
  }
}

View * EditableExpressionView::subviewAtIndex(int index) {
  assert(index == 0);
  if (editionIsInTextField()) {
    return &m_textField;
  }
  return &m_scrollableExpressionViewWithCursor;
}

void EditableExpressionView::layoutSubviews() {
  KDRect inputViewFrame(k_leftMargin, k_separatorThickness, bounds().width() - k_leftMargin, bounds().height() - k_separatorThickness);
  if (editionIsInTextField()) {
    m_textField.setFrame(inputViewFrame);
    m_scrollableExpressionViewWithCursor.setFrame(KDRectZero);
    return;
  }
  m_scrollableExpressionViewWithCursor.setFrame(inputViewFrame);
  m_textField.setFrame(KDRectZero);
}

void EditableExpressionView::reload() {
  layoutSubviews();
  markRectAsDirty(bounds());
}

void EditableExpressionView::drawRect(KDContext * ctx, KDRect rect) const {
  // Draw the separator
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_separatorThickness), Palette::GreyMiddle);
  // Color the left margin
  ctx->fillRect(KDRect(0, k_separatorThickness, k_leftMargin, bounds().height() - k_separatorThickness), m_textField.backgroundColor());
  if (!editionIsInTextField()) {
    // Color the upper margin
    ctx->fillRect(KDRect(0, k_separatorThickness, bounds().width(), k_verticalExpressionViewMargin), m_textField.backgroundColor());
  }
}

bool EditableExpressionView::handleEvent(Ion::Events::Event event) {
  return editionIsInTextField() ? m_textField.handleEvent(event) : m_scrollableExpressionViewWithCursor.handleEvent(event);
}

KDSize EditableExpressionView::minimalSizeForOptimalDisplay() const {
  return KDSize(0, inputViewHeight());
}

bool EditableExpressionView::editionIsInTextField() const {
  return Poincare::Preferences::sharedPreferences()->editionMode() == Poincare::Preferences::EditionMode::Edition1D;
}

bool EditableExpressionView::isEmpty() const {
  if (editionIsInTextField()) {
    return m_textField.draftTextLength() == 0;
  }
  Poincare::ExpressionLayout * layout =  const_cast<ScrollableExpressionViewWithCursor *>(&m_scrollableExpressionViewWithCursor)->expressionViewWithCursor()->expressionView()->expressionLayout();
  return !layout->hasText();
}

bool EditableExpressionView::heightIsMaximal() const {
  return inputViewHeight() == k_separatorThickness + k_verticalExpressionViewMargin + maximalHeight();
}

KDCoordinate EditableExpressionView::inputViewHeight() const {
  if (editionIsInTextField()) {
    return k_separatorThickness + k_textFieldHeight;
  }
  return k_separatorThickness
    + k_verticalExpressionViewMargin
    + min(maximalHeight(),
        max(k_textFieldHeight,
          m_scrollableExpressionViewWithCursor.minimalSizeForOptimalDisplay().height()
          + k_verticalExpressionViewMargin));
}

KDCoordinate EditableExpressionView::maximalHeight() const {
  return 0.6*Ion::Display::Height;
}
