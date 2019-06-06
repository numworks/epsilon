#include <escher/layout_field.h>
#include <escher/clipboard.h>
#include <escher/text_field.h>
#include <poincare/expression.h>
#include <poincare/horizontal_layout.h>
#include <assert.h>
#include <string.h>

using namespace Poincare;

static inline KDCoordinate minCoordinate(KDCoordinate x, KDCoordinate y) { return x < y ? x : y; }

LayoutField::ContentView::ContentView() :
  m_cursor(),
  m_expressionView(0.0f, 0.5f, KDColorBlack, KDColorWhite),
  m_cursorView(),
  m_isEditing(false)
{
  clearLayout();
}

void LayoutField::ContentView::setEditing(bool isEditing) {
  m_isEditing = isEditing;
  markRectAsDirty(bounds());
  layoutSubviews();
}

void LayoutField::ContentView::clearLayout() {
  HorizontalLayout h = HorizontalLayout::Builder();
  if (m_expressionView.setLayout(h)) {
    m_cursor.setLayout(h);
  }
}

KDSize LayoutField::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize evSize = m_expressionView.minimalSizeForOptimalDisplay();
  return KDSize(evSize.width() + Poincare::LayoutCursor::k_cursorWidth, evSize.height());
}

View * LayoutField::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  View * m_views[] = {&m_expressionView, &m_cursorView};
  return m_views[index];
}

void LayoutField::ContentView::layoutSubviews() {
  m_expressionView.setFrame(bounds());
  layoutCursorSubview();
}

void LayoutField::ContentView::layoutCursorSubview() {
  if (!m_isEditing) {
    m_cursorView.setFrame(KDRectZero);
    return;
  }
  KDPoint expressionViewOrigin = m_expressionView.absoluteDrawingOrigin();
  Layout pointedLayoutR = m_cursor.layoutReference();
  LayoutCursor::Position cursorPosition = m_cursor.position();
  LayoutCursor eqCursor = pointedLayoutR.equivalentCursor(&m_cursor);
  if (eqCursor.isDefined() && pointedLayoutR.hasChild(eqCursor.layoutReference())) {
    pointedLayoutR = eqCursor.layoutReference();
    cursorPosition = eqCursor.position();
  }
  KDPoint cursoredExpressionViewOrigin = pointedLayoutR.absoluteOrigin();
  KDCoordinate cursorX = expressionViewOrigin.x() + cursoredExpressionViewOrigin.x();
  if (cursorPosition == LayoutCursor::Position::Right) {
    cursorX += pointedLayoutR.layoutSize().width();
  }
  KDPoint cursorTopLeftPosition(cursorX, expressionViewOrigin.y() + cursoredExpressionViewOrigin.y() + pointedLayoutR.baseline() - m_cursor.baseline());
  m_cursorView.setFrame(KDRect(cursorTopLeftPosition, LayoutCursor::k_cursorWidth, m_cursor.cursorHeight()));
}

CodePoint LayoutField::XNTCodePoint(CodePoint defaultXNTCodePoint) {
  CodePoint xnt = m_contentView.cursor()->layoutReference().XNTCodePoint();
  if (xnt != UCodePointNull) {
    return xnt;
  }
  return defaultXNTCodePoint;
}

void LayoutField::reload(KDSize previousSize) {
  layout().invalidAllSizesPositionsAndBaselines();
  KDSize newSize = minimalSizeForOptimalDisplay();
  if (m_delegate && previousSize.height() != newSize.height()) {
    m_delegate->layoutFieldDidChangeSize(this);
  }
  m_contentView.cursorPositionChanged();
  scrollToCursor();
  markRectAsDirty(bounds());
}

bool LayoutField::handleEventWithText(const char * text, bool indentation, bool forceCursorRightOfText) {
  /* The text here can be:
   * - the result of a key pressed, such as "," or "cos(•)"
   * - the text added after a toolbox selection
   * - the result of a copy-paste. */
  if (text[0] == 0) {
    // The text is empty
    return true;
  }

  int currentNumberOfLayouts = m_contentView.expressionView()->numberOfLayouts();
  if (currentNumberOfLayouts >= k_maxNumberOfLayouts - 6) {
    /* We add -6 because in some cases (Ion::Events::Division,
     * Ion::Events::Exp...) we let the layout cursor handle the layout insertion
     * and these events may add at most 6 layouts (e.g *10^). */
    return true;
  }

  // Handle special cases
  if (strcmp(text, Ion::Events::Division.text()) == 0) {
    m_contentView.cursor()->addFractionLayoutAndCollapseSiblings();
  } else if (strcmp(text, Ion::Events::Exp.text()) == 0) {
    m_contentView.cursor()->addEmptyExponentialLayout();
  } else if (strcmp(text, Ion::Events::Power.text()) == 0) {
    m_contentView.cursor()->addEmptyPowerLayout();
  } else if (strcmp(text, Ion::Events::Sqrt.text()) == 0) {
    m_contentView.cursor()->addEmptySquareRootLayout();
  } else if (strcmp(text, Ion::Events::Square.text()) == 0) {
    m_contentView.cursor()->addEmptySquarePowerLayout();
  } else if (strcmp(text, Ion::Events::EE.text()) == 0) {
    m_contentView.cursor()->addEmptyTenPowerLayout();
  } else if ((strcmp(text, "[") == 0) || (strcmp(text, "]") == 0)) {
    m_contentView.cursor()->addEmptyMatrixLayout();
  } else {
    Expression resultExpression = Expression::Parse(text);
    if (resultExpression.isUninitialized()) {
      // The text is not parsable (for instance, ",") and is added char by char.
      KDSize previousLayoutSize = minimalSizeForOptimalDisplay();
      m_contentView.cursor()->insertText(text);
      reload(previousLayoutSize);
      return true;
    }
    // The text is parsable, we create its layout an insert it.
    Layout resultLayout = resultExpression.createLayout(Poincare::Preferences::sharedPreferences()->displayMode(), Poincare::PrintFloat::k_numberOfStoredSignificantDigits);
    if (currentNumberOfLayouts + resultLayout.numberOfDescendants(true) >= k_maxNumberOfLayouts) {
      return true;
    }
    insertLayoutAtCursor(resultLayout, resultExpression, forceCursorRightOfText);
  }
  return true;
}

bool LayoutField::handleEvent(Ion::Events::Event event) {
  bool didHandleEvent = false;
  bool shouldRecomputeLayout = m_contentView.cursor()->showEmptyLayoutIfNeeded();
  bool moveEventChangedLayout = false;
  KDSize previousSize = minimalSizeForOptimalDisplay();
  if (privateHandleMoveEvent(event, &moveEventChangedLayout)) {
    if (!isEditing()) {
      setEditing(true);
    }
    shouldRecomputeLayout = shouldRecomputeLayout || moveEventChangedLayout;
    didHandleEvent = true;
  } else if (privateHandleEvent(event)) {
    shouldRecomputeLayout = true;
    didHandleEvent = true;
  }
  if (didHandleEvent) {
    shouldRecomputeLayout = m_contentView.cursor()->hideEmptyLayoutIfNeeded() || shouldRecomputeLayout;
    if (!shouldRecomputeLayout) {
      m_contentView.cursorPositionChanged();
      scrollToCursor();
    } else {
      reload(previousSize);
    }
    return true;
  }
  m_contentView.cursor()->hideEmptyLayoutIfNeeded();
  return false;
}

bool LayoutField::privateHandleEvent(Ion::Events::Event event) {
  if (m_delegate && m_delegate->layoutFieldDidReceiveEvent(this, event)) {
    return true;
  }
  if (handleBoxEvent(app(), event)) {
    if (!isEditing()) {
      setEditing(true);
    }
    return true;
  }
  if (isEditing() && m_delegate->layoutFieldShouldFinishEditing(this, event)) { //TODO use class method?
    setEditing(false);
    if (m_delegate->layoutFieldDidFinishEditing(this, layout(), event)) {
      // Reinit layout for next use
      clearLayout();
    } else {
      setEditing(true);
    }
    return true;
  }
  /* if move event was not caught neither by privateHandleMoveEvent nor by
   * layoutFieldShouldFinishEditing, we handle it here to avoid bubbling the
   * event up. */
  if ((event == Ion::Events::Up || event == Ion::Events::Down || event == Ion::Events::Left || event == Ion::Events::Right) && isEditing()) {
    return true;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !isEditing()) {
    setEditing(true);
    m_contentView.cursor()->setLayout(layout());
    m_contentView.cursor()->setPosition(LayoutCursor::Position::Right);
    return true;
  }
  if (event == Ion::Events::Back && isEditing()) {
    clearLayout();
    setEditing(false);
    m_delegate->layoutFieldDidAbortEditing(this);
    return true;
  }
  if (event.hasText() || event == Ion::Events::Paste || event == Ion::Events::Backspace) {
    if (!isEditing()) {
      setEditing(true);
    }
    if (event.hasText()) {
      handleEventWithText(event.text());
    } else if (event == Ion::Events::Paste) {
      handleEventWithText(Clipboard::sharedClipboard()->storedText(), false, true);
    } else {
      assert(event == Ion::Events::Backspace);
      m_contentView.cursor()->performBackspace();
    }
    return true;
  }
  if (event == Ion::Events::Clear && isEditing()) {
    clearLayout();
    return true;
  }
  return false;
}

bool LayoutField::privateHandleMoveEvent(Ion::Events::Event event, bool * shouldRecomputeLayout) {
  LayoutCursor result;
  if (event == Ion::Events::Left) {
    result = m_contentView.cursor()->cursorAtDirection(LayoutCursor::MoveDirection::Left, shouldRecomputeLayout);
  } else if (event == Ion::Events::Right) {
    result = m_contentView.cursor()->cursorAtDirection(LayoutCursor::MoveDirection::Right, shouldRecomputeLayout);
  } else if (event == Ion::Events::Up) {
    result = m_contentView.cursor()->cursorAtDirection(LayoutCursor::MoveDirection::Up, shouldRecomputeLayout);
  } else if (event == Ion::Events::Down) {
    result = m_contentView.cursor()->cursorAtDirection(LayoutCursor::MoveDirection::Down, shouldRecomputeLayout);
  } else if (event == Ion::Events::ShiftLeft) {
    *shouldRecomputeLayout = true;
    if (m_contentView.cursor()->layoutReference().removeGreySquaresFromAllMatrixAncestors()) {
      *shouldRecomputeLayout = true;
    }
    result.setLayout(layout());
    result.setPosition(LayoutCursor::Position::Left);
  } else if (event == Ion::Events::ShiftRight) {
    if (m_contentView.cursor()->layoutReference().removeGreySquaresFromAllMatrixAncestors()) {
      *shouldRecomputeLayout = true;
    }
    result.setLayout(layout());
    result.setPosition(LayoutCursor::Position::Right);
  }
  if (result.isDefined()) {
    m_contentView.setCursor(result);
    return true;
  }
  return false;
}

void LayoutField::scrollRightOfLayout(Layout layoutR) {
  KDRect layoutRect(layoutR.absoluteOrigin().translatedBy(m_contentView.expressionView()->drawingOrigin()), layoutR.layoutSize());
  scrollToBaselinedRect(layoutRect, layoutR.baseline());
}

void LayoutField::scrollToBaselinedRect(KDRect rect, KDCoordinate baseline) {
  scrollToContentRect(rect, true);
  // Show the rect area around its baseline
  KDCoordinate underBaseline = rect.height() - baseline;
  KDCoordinate minAroundBaseline = minCoordinate(baseline, underBaseline);
  minAroundBaseline = minCoordinate(minAroundBaseline, bounds().height() / 2);
  KDRect balancedRect(rect.x(), rect.y() + baseline - minAroundBaseline, rect.width(), 2 * minAroundBaseline);
  scrollToContentRect(balancedRect, true);
}

void LayoutField::insertLayoutAtCursor(Layout layoutR, Poincare::Expression correspondingExpression, bool forceCursorRightOfLayout) {
  if (layoutR.isUninitialized()) {
    return;
  }

  KDSize previousSize = minimalSizeForOptimalDisplay();
  Poincare::LayoutCursor * cursor = m_contentView.cursor();

  // Handle empty layouts
  cursor->showEmptyLayoutIfNeeded();

  bool layoutWillBeMerged = layoutR.type() == LayoutNode::Type::HorizontalLayout;
  Layout lastMergedLayoutChild = layoutWillBeMerged ? layoutR.childAtIndex(layoutR.numberOfChildren()-1) : Layout();

  // Find the layout where the cursor will point
  assert(!correspondingExpression.isUninitialized());
  Layout cursorLayout = forceCursorRightOfLayout ? layoutR : layoutR.layoutToPointWhenInserting(&correspondingExpression);
  assert(!cursorLayout.isUninitialized());

  // Add the layout. This puts the cursor at the right of the added layout
  cursor->addLayoutAndMoveCursor(layoutR);

  /* Move the cursor if needed.
   * If the layout to point to has been merged, it means that only its children
   * have been inserted in the layout, so we must not move the cursor to the
   * parent. In this case, addLayoutAndMoveCursor made the cursor point to the
   * last merged child, which is what is wanted.
   * For other cases, move the cursor to the computed layout. */
  if (!(layoutWillBeMerged && cursorLayout == layoutR)) {
    cursor->setLayout(cursorLayout);
    cursor->setPosition(LayoutCursor::Position::Right);
  }

  // Handle matrices
  cursor->layoutReference().addGreySquaresToAllMatrixAncestors();

  // Handle empty layouts
  cursor->hideEmptyLayoutIfNeeded();

  // Reload
  reload(previousSize);
  if (!layoutWillBeMerged) {
    scrollRightOfLayout(layoutR);
  } else {
    assert(!lastMergedLayoutChild.isUninitialized());
    scrollRightOfLayout(lastMergedLayoutChild);
  }
  scrollToCursor();
}
