#include <escher/scrollable_expression_view_with_cursor.h>
#include <escher/clipboard.h>
#include <escher/text_field.h>
#include <poincare/src/layout/matrix_layout.h>
#include <assert.h>

ScrollableExpressionViewWithCursor::ScrollableExpressionViewWithCursor(Responder * parentResponder, Poincare::ExpressionLayout * expressionLayout, ScrollableExpressionViewWithCursorDelegate * delegate) :
  ScrollableView(parentResponder, &m_expressionViewWithCursor, this),
  m_expressionViewWithCursor(expressionLayout),
  m_delegate(delegate)
{
}

bool ScrollableExpressionViewWithCursor::isEditing() const {
  return m_expressionViewWithCursor.isEditing();
}

void ScrollableExpressionViewWithCursor::setEditing(bool isEditing) {
  m_expressionViewWithCursor.setEditing(isEditing);
}

void ScrollableExpressionViewWithCursor::clearLayout() {
  delete m_expressionViewWithCursor.expressionView()->expressionLayout();
  Poincare::ExpressionLayout * newLayout = new Poincare::HorizontalLayout();
  m_expressionViewWithCursor.expressionView()->setExpressionLayout(newLayout);
  m_expressionViewWithCursor.cursor()->setPointedExpressionLayout(newLayout);
}

void ScrollableExpressionViewWithCursor::scrollToCursor() {
  scrollToContentRect(m_expressionViewWithCursor.cursorRect(), true);
}

Toolbox * ScrollableExpressionViewWithCursor::toolbox() {
  if (m_delegate) {
    return m_delegate->toolboxForScrollableExpressionViewWithCursor(this);
  }
  return nullptr;
}

bool ScrollableExpressionViewWithCursor::handleEvent(Ion::Events::Event event) {
  KDSize previousSize = minimalSizeForOptimalDisplay();
  bool shouldRecomputeLayout = false;
  bool didHandleEvent = false;
  if (privateHandleMoveEvent(event, &shouldRecomputeLayout)) {
    if (!shouldRecomputeLayout) {
      m_expressionViewWithCursor.cursorPositionChanged();
      scrollToCursor();
      return true;
    }
    didHandleEvent = true;
  } else if (privateHandleEvent(event)) {
    didHandleEvent = true;
  }
  if (didHandleEvent) {
    reload();
    KDSize newSize = minimalSizeForOptimalDisplay();
    if (m_delegate && previousSize.height() != newSize.height()) {
      m_delegate->scrollableExpressionViewWithCursorDidChangeSize(this);
      reload();
    }
    return true;
  }
  return false;
}

bool ScrollableExpressionViewWithCursor::scrollableExpressionViewWithCursorShouldFinishEditing(Ion::Events::Event event) {
  return m_delegate->scrollableExpressionViewWithCursorShouldFinishEditing(this, event);
}

KDSize ScrollableExpressionViewWithCursor::minimalSizeForOptimalDisplay() const {
  return m_expressionViewWithCursor.minimalSizeForOptimalDisplay();
}

bool ScrollableExpressionViewWithCursor::privateHandleMoveEvent(Ion::Events::Event event, bool * shouldRecomputeLayout) {
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

bool ScrollableExpressionViewWithCursor::privateHandleEvent(Ion::Events::Event event) {
  if (m_delegate && m_delegate->scrollableExpressionViewWithCursorDidReceiveEvent(this, event)) {
    return true;
  }
  if (Responder::handleEvent(event)) {
    /* The only event Responder handles is 'Toolbox' displaying. In that case,
     * the ScrollableExpressionViewWithCursor is forced into editing mode. */
    if (!isEditing()) {
      setEditing(true);
    }
    return true;
  }
  if (isEditing() && scrollableExpressionViewWithCursorShouldFinishEditing(event)) {
    setEditing(false);
    int bufferSize = TextField::maxBufferSize();
    char buffer[bufferSize];
    m_expressionViewWithCursor.expressionView()->expressionLayout()->writeTextInBuffer(buffer, bufferSize);
    if (m_delegate->scrollableExpressionViewWithCursorDidFinishEditing(this, buffer, event)) {
      delete m_expressionViewWithCursor.expressionView()->expressionLayout();
      Poincare::ExpressionLayout * newLayout = new Poincare::HorizontalLayout();
      m_expressionViewWithCursor.expressionView()->setExpressionLayout(newLayout);
      m_expressionViewWithCursor.cursor()->setPointedExpressionLayout(newLayout);
    }
    return true;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !isEditing()) {
    setEditing(true);
    m_expressionViewWithCursor.cursor()->setPointedExpressionLayout(m_expressionViewWithCursor.expressionView()->expressionLayout());
    m_expressionViewWithCursor.cursor()->setPosition(Poincare::ExpressionLayoutCursor::Position::Right);
    return true;
  }
  if (event == Ion::Events::Back && isEditing()) {
    setEditing(false);
    m_delegate->scrollableExpressionViewWithCursorDidAbortEditing(this);
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
  if (event == Ion::Events::Clear && isEditing()) {
    clearLayout();
    return true;
  }
  return false;
}

void ScrollableExpressionViewWithCursor::insertLayoutAtCursor(Poincare::ExpressionLayout * layout, Poincare::ExpressionLayout * pointedLayout) {
  if (layout == nullptr) {
    return;
  }
  KDSize previousSize = minimalSizeForOptimalDisplay();
  if (layout->isMatrix() && pointedLayout && pointedLayout->hasAncestor(layout)) {
    static_cast<Poincare::MatrixLayout *>(layout)->addGreySquares();
  }
  if (pointedLayout != nullptr) {
    m_expressionViewWithCursor.cursor()->addLayout(layout);
    m_expressionViewWithCursor.cursor()->setPointedExpressionLayout(pointedLayout);
    m_expressionViewWithCursor.cursor()->setPosition(Poincare::ExpressionLayoutCursor::Position::Right);
  } else {
    m_expressionViewWithCursor.cursor()->addLayoutAndMoveCursor(layout);
  }
  reload();
  KDSize newSize = minimalSizeForOptimalDisplay();
  if (m_delegate && previousSize.height() != newSize.height()) {
    m_delegate->scrollableExpressionViewWithCursorDidChangeSize(this);
  }
}

void ScrollableExpressionViewWithCursor::insertLayoutFromTextAtCursor(const char * text) {
  Poincare::Expression * expression = Poincare::Expression::parse(text);
  if (expression != nullptr) {
    Poincare::ExpressionLayout * layout = expression->createLayout();
    delete expression;
    insertLayoutAtCursor(layout, nullptr);
    reload();
    return;
  }
  m_expressionViewWithCursor.cursor()->insertText(text);
  reload();
}

void ScrollableExpressionViewWithCursor::reload() {
  m_expressionViewWithCursor.expressionView()->expressionLayout()->invalidAllSizesPositionsAndBaselines();
  m_expressionViewWithCursor.cursorPositionChanged();
  layoutSubviews();
  scrollToCursor();
  markRectAsDirty(bounds());
}
