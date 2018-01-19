#include <escher/editable_expression_view.h>
#include <escher/clipboard.h>
#include <escher/text_field.h>
#include <poincare/src/layout/matrix_layout.h>
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
  KDSize previousSize = minimalSizeForOptimalDisplay();
  bool shouldRecomputeLayout = false;
  if (privateHandleMoveEvent(event, &shouldRecomputeLayout)) {
    if (!shouldRecomputeLayout) {
      m_expressionViewWithCursor.cursorPositionChanged();
      scrollToCursor();
      return true;
    }
    reload();
    KDSize newSize = minimalSizeForOptimalDisplay();
    if (m_delegate && previousSize.height() != newSize.height()) {
      m_delegate->editableExpressionViewDidChangeSize(this);
      reload();
    }
    return true;
  }
  if (privateHandleEvent(event)) {
    reload();
    KDSize newSize = minimalSizeForOptimalDisplay();
    if (m_delegate && previousSize.height() != newSize.height()) {
      m_delegate->editableExpressionViewDidChangeSize(this);
      reload();
    }
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

bool EditableExpressionView::privateHandleMoveEvent(Ion::Events::Event event, bool * shouldRecomputeLayout) {
  if (event == Ion::Events::Left) {
    return m_expressionViewWithCursor.cursor()->moveLeft(shouldRecomputeLayout);
  }
  if (event == Ion::Events::Right) {
    return m_expressionViewWithCursor.cursor()->moveRight(shouldRecomputeLayout);
  }
  if (event == Ion::Events::Up) {
    return m_expressionViewWithCursor.cursor()->moveUp(shouldRecomputeLayout);
  }
  if (event == Ion::Events::Down) {
    return m_expressionViewWithCursor.cursor()->moveDown(shouldRecomputeLayout);
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
  return false;
}

bool EditableExpressionView::privateHandleEvent(Ion::Events::Event event) {
  if (m_delegate && m_delegate->editableExpressionViewDidReceiveEvent(this, event)) {
    return true;
  }
  if (Responder::handleEvent(event)) {
    /* The only event Responder handles is 'Toolbox' displaying. In that case,
     * the EditableExpressionView is forced into editing mode. */
    if (!isEditing()) {
      setEditing(true);
    }
    return true;
  }
  if (isEditing() && editableExpressionViewShouldFinishEditing(event)) {
    setEditing(false);
    int bufferSize = TextField::maxBufferSize();
    char buffer[bufferSize];
    m_expressionViewWithCursor.expressionView()->expressionLayout()->writeTextInBuffer(buffer, bufferSize);
    if (m_delegate->editableExpressionViewDidFinishEditing(this, buffer, event)) {
      delete m_expressionViewWithCursor.expressionView()->expressionLayout();
      Poincare::ExpressionLayout * newLayout = new Poincare::HorizontalLayout();
      m_expressionViewWithCursor.expressionView()->setExpressionLayout(newLayout);
      m_expressionViewWithCursor.cursor()->setPointedExpressionLayout(newLayout);
    }
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
  if (event == Ion::Events::Paste) {
    if (!isEditing()) {
      setEditing(true);
    }
    insertLayoutFromTextAtCursor(Clipboard::sharedClipboard()->storedText());
    return true;
  }
  return false;
}

void EditableExpressionView::insertLayoutAtCursor(Poincare::ExpressionLayout * layout, Poincare::ExpressionLayout * pointedLayout) {
  if (layout == nullptr) {
    return;
  }
  KDSize previousSize = minimalSizeForOptimalDisplay();
  m_expressionViewWithCursor.cursor()->addLayout(layout);
  if (layout->isMatrix() && pointedLayout->hasAncestor(layout)) {
    static_cast<Poincare::MatrixLayout *>(layout)->addGreySquares();
  }
  m_expressionViewWithCursor.cursor()->setPointedExpressionLayout(pointedLayout);
  m_expressionViewWithCursor.cursor()->setPosition(Poincare::ExpressionLayoutCursor::Position::Right);
  reload();
  KDSize newSize = minimalSizeForOptimalDisplay();
  if (m_delegate && previousSize.height() != newSize.height()) {
    m_delegate->editableExpressionViewDidChangeSize(this);
  }
}

void EditableExpressionView::insertLayoutFromTextAtCursor(const char * text) {
  Poincare::Expression * expression = Poincare::Expression::parse(text);
  if (expression != nullptr) {
    Poincare::ExpressionLayout * layout = expression->createLayout();
    delete expression;
    insertLayoutAtCursor(layout, layout);
    reload();
    return;
  }
  m_expressionViewWithCursor.cursor()->insertText(text);
  reload();
}

void EditableExpressionView::reload() {
  m_expressionViewWithCursor.expressionView()->expressionLayout()->invalidAllSizesPositionsAndBaselines();
  m_expressionViewWithCursor.cursorPositionChanged();
  layoutSubviews();
  scrollToCursor();
  markRectAsDirty(bounds());
}
