#include <escher/editable_expression_view.h>
#include <assert.h>

EditableExpressionView::EditableExpressionView(Responder * parentResponder, Poincare::ExpressionLayout * expressionLayout, EditableExpressionViewDelegate * delegate) :
  ScrollableView(parentResponder, &m_expressionViewWithCursor, this),
  m_expressionViewWithCursor(expressionLayout),
  m_delegate(delegate)
{
}

bool EditableExpressionView::isEditing() const {
  return m_expressionViewWithCursor.isEditing();
}

void EditableExpressionView::setEditing(bool isEditing) {
  m_expressionViewWithCursor.setEditing(isEditing);
}

void EditableExpressionView::scrollToCursor() {
  scrollToContentRect(m_expressionViewWithCursor.cursorRect(), true);
}

Toolbox * EditableExpressionView::toolbox() {
  if (m_delegate) {
    return m_delegate->toolboxForEditableExpressionView(this);
  }
  return nullptr;
}

bool EditableExpressionView::handleEvent(Ion::Events::Event event) {
  if (privateHandleEvent(event)) {
    cursorOrLayoutChanged();
    return true;
  }
  return false;
}

bool EditableExpressionView::editableExpressionViewShouldFinishEditing(Ion::Events::Event event) {
  return m_delegate->editableExpressionViewShouldFinishEditing(this, event);
}

KDSize EditableExpressionView::minimalSizeForOptimalDisplay() const {
  return m_expressionViewWithCursor.minimalSizeForOptimalDisplay();
}

bool EditableExpressionView::privateHandleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    return m_expressionViewWithCursor.cursor()->moveLeft();
  }
  if (event == Ion::Events::Right) {
    return m_expressionViewWithCursor.cursor()->moveRight();
  }
  if (event == Ion::Events::Up) {
    return m_expressionViewWithCursor.cursor()->moveUp();
  }
  if (event == Ion::Events::Down) {
    return m_expressionViewWithCursor.cursor()->moveDown();
  }
  if (event == Ion::Events::ShiftLeft) {
    m_expressionViewWithCursor.cursor()->setPointedExpressionLayout(m_expressionViewWithCursor.expressionView()->expressionLayout());
    m_expressionViewWithCursor.cursor()->setPosition(Poincare::ExpressionLayoutCursor::Position::Left);
    return true;
  }
  if (event == Ion::Events::ShiftRight) {
    m_expressionViewWithCursor.cursor()->setPointedExpressionLayout(m_expressionViewWithCursor.expressionView()->expressionLayout());
    m_expressionViewWithCursor.cursor()->setPosition(Poincare::ExpressionLayoutCursor::Position::Right);
    return true;
  }
  if (event == Ion::Events::Division) {
    m_expressionViewWithCursor.cursor()->addFractionLayoutAndCollapseBrothers();
    return true;
  }
  if (event == Ion::Events::XNT) {
    m_expressionViewWithCursor.cursor()->addXNTCharLayout();
    return true;
  }
  if (event == Ion::Events::Exp) {
    m_expressionViewWithCursor.cursor()->addEmptyExponentialLayout();
    return true;
  }
  if (event == Ion::Events::Log) {
    m_expressionViewWithCursor.cursor()->addEmptyLogarithmLayout();
    return true;
  }
  if (event == Ion::Events::Power) {
    m_expressionViewWithCursor.cursor()->addEmptyPowerLayout();
    return true;
  }
  if (event == Ion::Events::Sqrt) {
    m_expressionViewWithCursor.cursor()->addEmptySquareRootLayout();
    return true;
  }
  if (event == Ion::Events::Square) {
    m_expressionViewWithCursor.cursor()->addEmptySquarePowerLayout();
    return true;
  }
  if (event.hasText()) {
    const char * textToInsert = event.text();
    if (textToInsert[1] == 0) {
      if (textToInsert[0] == Ion::Charset::MultiplicationSign) {
        const char middleDotString[] = {Ion::Charset::MiddleDot, 0};
        m_expressionViewWithCursor.cursor()->insertText(middleDotString);
        return true;
      }
      if (textToInsert[0] == '[' || textToInsert[0] == ']') {
        m_expressionViewWithCursor.cursor()->addEmptyMatrixLayout();
        return true;
      }
    }
    m_expressionViewWithCursor.cursor()->insertText(textToInsert);
    return true;
  }
  if (event == Ion::Events::Backspace) {
    m_expressionViewWithCursor.cursor()->performBackspace();
    return true;
  }
  return false;
}

void EditableExpressionView::insertLayoutAtCursor(Poincare::ExpressionLayout * layout, Poincare::ExpressionLayout * pointedLayout) {
  if (layout == nullptr) {
    return;
  }
  m_expressionViewWithCursor.cursor()->addLayout(layout);
  m_expressionViewWithCursor.cursor()->setPointedExpressionLayout(pointedLayout);
  m_expressionViewWithCursor.cursor()->setPosition(Poincare::ExpressionLayoutCursor::Position::Right);
  m_expressionViewWithCursor.expressionView()->expressionLayout()->invalidAllSizesPositionsAndBaselines();
  cursorOrLayoutChanged();
}

void EditableExpressionView::cursorOrLayoutChanged() {
  m_expressionViewWithCursor.expressionView()->expressionLayout()->invalidAllSizesPositionsAndBaselines();
  layoutSubviews();
}
