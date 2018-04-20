#include <escher/expression_layout_field.h>
#include <escher/clipboard.h>
#include <escher/text_field.h>
#include <poincare/src/layout/matrix_layout.h>
#include <poincare/expression_layout_cursor.h>
#include <assert.h>

ExpressionLayoutField::ExpressionLayoutField(Responder * parentResponder, Poincare::ExpressionLayout * expressionLayout, ExpressionLayoutFieldDelegate * delegate) :
  ScrollableView(parentResponder, &m_expressionViewWithCursor, this),
  m_expressionViewWithCursor(expressionLayout),
  m_delegate(delegate)
{
}

bool ExpressionLayoutField::isEditing() const {
  return m_expressionViewWithCursor.isEditing();
}

void ExpressionLayoutField::setEditing(bool isEditing) {
  m_expressionViewWithCursor.setEditing(isEditing);
}

void ExpressionLayoutField::clearLayout() {
  delete m_expressionViewWithCursor.expressionView()->expressionLayout();
  Poincare::ExpressionLayout * newLayout = new Poincare::HorizontalLayout();
  m_expressionViewWithCursor.expressionView()->setExpressionLayout(newLayout);
  m_expressionViewWithCursor.cursor()->setPointedExpressionLayout(newLayout);
}

void ExpressionLayoutField::scrollToCursor() {
  scrollToContentRect(m_expressionViewWithCursor.cursorRect(), true);
}

Toolbox * ExpressionLayoutField::toolbox() {
  if (m_delegate) {
    return m_delegate->toolboxForExpressionLayoutField(this);
  }
  return nullptr;
}

bool ExpressionLayoutField::handleEvent(Ion::Events::Event event) {
  KDSize previousSize = minimalSizeForOptimalDisplay();
  bool didHandleEvent = false;
  bool shouldRecomputeLayout = m_expressionViewWithCursor.cursor()->showEmptyLayoutIfNeeded();
  bool moveEventChangedLayout = false;
  if (privateHandleMoveEvent(event, &moveEventChangedLayout)) {
    shouldRecomputeLayout = shouldRecomputeLayout || moveEventChangedLayout;
    didHandleEvent = true;
  } else if (privateHandleEvent(event)) {
    shouldRecomputeLayout = true;
    didHandleEvent = true;
  }
  if (didHandleEvent) {
    if (!isEditing()) {
      setEditing(true);
    }
    shouldRecomputeLayout = m_expressionViewWithCursor.cursor()->hideEmptyLayoutIfNeeded() || shouldRecomputeLayout;
    if (!shouldRecomputeLayout) {
      m_expressionViewWithCursor.cursorPositionChanged();
      scrollToCursor();
      return true;
    }
    reload();
    KDSize newSize = minimalSizeForOptimalDisplay();
    if (m_delegate && previousSize.height() != newSize.height()) {
      m_delegate->expressionLayoutFieldDidChangeSize(this);
      reload();
    }
    return true;
  }
  return false;
}

bool ExpressionLayoutField::expressionLayoutFieldShouldFinishEditing(Ion::Events::Event event) {
  return m_delegate->expressionLayoutFieldShouldFinishEditing(this, event);
}

KDSize ExpressionLayoutField::minimalSizeForOptimalDisplay() const {
  return m_expressionViewWithCursor.minimalSizeForOptimalDisplay();
}

bool ExpressionLayoutField::privateHandleMoveEvent(Ion::Events::Event event, bool * shouldRecomputeLayout) {
  Poincare::ExpressionLayoutCursor result;
  if (event == Ion::Events::Left) {
    result = m_expressionViewWithCursor.cursor()->cursorOnLeft(shouldRecomputeLayout);
  } else if (event == Ion::Events::Right) {
    result =  m_expressionViewWithCursor.cursor()->cursorOnRight(shouldRecomputeLayout);
  } else if (event == Ion::Events::Up) {
    result = m_expressionViewWithCursor.cursor()->cursorAbove(shouldRecomputeLayout);
  } else if (event == Ion::Events::Down) {
    result = m_expressionViewWithCursor.cursor()->cursorUnder(shouldRecomputeLayout);
  } else if (event == Ion::Events::ShiftLeft) {
    result.setPointedExpressionLayout(m_expressionViewWithCursor.expressionView()->expressionLayout());
    result.setPosition(Poincare::ExpressionLayoutCursor::Position::Left);
  } else if (event == Ion::Events::ShiftRight) {
    result.setPointedExpressionLayout(m_expressionViewWithCursor.expressionView()->expressionLayout());
    result.setPosition(Poincare::ExpressionLayoutCursor::Position::Right);
  }
  if (result.isDefined()) {
    m_expressionViewWithCursor.setCursor(result);
    return true;
  }
  return false;
}

bool ExpressionLayoutField::privateHandleEvent(Ion::Events::Event event) {
  if (m_delegate && m_delegate->expressionLayoutFieldDidReceiveEvent(this, event)) {
    return true;
  }
  if (Responder::handleEvent(event)) {
    /* The only event Responder handles is 'Toolbox' displaying. In that case,
     * the ExpressionLayoutField is forced into editing mode. */
    if (!isEditing()) {
      setEditing(true);
    }
    return true;
  }
  if (isEditing() && expressionLayoutFieldShouldFinishEditing(event)) {
    setEditing(false);
    int bufferSize = TextField::maxBufferSize();
    char buffer[bufferSize];
    m_expressionViewWithCursor.expressionView()->expressionLayout()->writeTextInBuffer(buffer, bufferSize);
    if (m_delegate->expressionLayoutFieldDidFinishEditing(this, buffer, event)) {
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
    m_delegate->expressionLayoutFieldDidAbortEditing(this);
    return true;
  }
  if (event == Ion::Events::Division) {
    m_expressionViewWithCursor.cursor()->addFractionLayoutAndCollapseSiblings();
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

void ExpressionLayoutField::insertLayoutAtCursor(Poincare::ExpressionLayout * layout, Poincare::ExpressionLayout * pointedLayout) {
  if (layout == nullptr) {
    return;
  }
  m_expressionViewWithCursor.cursor()->showEmptyLayoutIfNeeded();
  KDSize previousSize = minimalSizeForOptimalDisplay();
  if (layout->isMatrix() && pointedLayout && pointedLayout->hasAncestor(layout)) {
    static_cast<Poincare::MatrixLayout *>(layout)->addGreySquares();
  }
  bool layoutWillBeMerged = layout->isHorizontal();
  m_expressionViewWithCursor.cursor()->addLayoutAndMoveCursor(layout);
  if (pointedLayout != nullptr) {
    m_expressionViewWithCursor.cursor()->setPointedExpressionLayout(pointedLayout);
    m_expressionViewWithCursor.cursor()->setPosition(Poincare::ExpressionLayoutCursor::Position::Right);
  } else if (!layoutWillBeMerged) {
    m_expressionViewWithCursor.cursor()->setPointedExpressionLayout(layout->layoutToPointWhenInserting());
    m_expressionViewWithCursor.cursor()->setPosition(Poincare::ExpressionLayoutCursor::Position::Right);
  }
  m_expressionViewWithCursor.cursor()->hideEmptyLayoutIfNeeded();
  reload();
  KDSize newSize = minimalSizeForOptimalDisplay();
  if (m_delegate && previousSize.height() != newSize.height()) {
    m_delegate->expressionLayoutFieldDidChangeSize(this);
  }
}

void ExpressionLayoutField::insertLayoutFromTextAtCursor(const char * text) {
  Poincare::Expression * expression = Poincare::Expression::parse(text);
  if (expression != nullptr) {
    Poincare::ExpressionLayout * layout = expression->createLayout();
    delete expression;
    m_expressionViewWithCursor.cursor()->showEmptyLayoutIfNeeded();
    insertLayoutAtCursor(layout, nullptr);
    m_expressionViewWithCursor.cursor()->hideEmptyLayoutIfNeeded();
    reload();
    return;
  }
  m_expressionViewWithCursor.cursor()->showEmptyLayoutIfNeeded();
  m_expressionViewWithCursor.cursor()->insertText(text);
  m_expressionViewWithCursor.cursor()->hideEmptyLayoutIfNeeded();
  reload();
}

void ExpressionLayoutField::reload() {
  m_expressionViewWithCursor.expressionView()->expressionLayout()->invalidAllSizesPositionsAndBaselines();
  layoutSubviews();
  m_delegate->expressionLayoutFieldDidChangeSize(this);
  scrollToCursor();
  m_expressionViewWithCursor.cursorPositionChanged();
  markRectAsDirty(bounds());
}
