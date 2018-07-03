#include <escher/layout_field.h>
#include <apps/i18n.h>
#include <escher/clipboard.h>
#include <escher/text_field.h>
#include <poincare/expression.h>
#include <poincare/expression_layout_cursor.h>
//#include <poincare/src/layout/matrix_layout.h> //TODO
#include <assert.h>
#include <string.h>

using namespace Poincare;

void LayoutField::ContentView::layoutCursorSubview() {
  if (!m_isEditing) {
    m_cursorView.setFrame(KDRectZero);
    return;
  }
  KDPoint expressionViewOrigin = m_expressionView.absoluteDrawingOrigin();
  LayoutRef pointedLayoutR = m_cursor.layoutReference();
  LayoutCursor::Position cursorPosition = m_cursor.position();
  LayoutCursor eqCursor = pointedLayoutR.equivalentCursor(&m_cursor);
  if (pointedLayoutR.hasChild(eqCursor.layoutReference())) {
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

void LayoutField::reload() {
  KDSize previousSize = minimalSizeForOptimalDisplay();
  layoutRef().invalidAllSizesPositionsAndBaselines();
  KDSize newSize = minimalSizeForOptimalDisplay();
  if (m_delegate && previousSize.height() != newSize.height()) {
    m_delegate->layoutFieldDidChangeSize(this);
  }
  m_contentView.cursorPositionChanged();
  scrollToCursor();
  markRectAsDirty(bounds());
}

bool LayoutField::handleEventWithText(const char * text, bool indentation, bool forceCursorRightOfText) {
  if (text[0] == 0) {
    // The text is empty
    return true;
  }

  int currentNumberOfLayouts = m_contentView.expressionView()->numberOfLayouts();
  if (currentNumberOfLayouts >= k_maxNumberOfLayouts - 6) {
    /* We add -6 because in some cases (Ion::Events::Division,
     * Ion::Events::Exp...) we let the layout cursor handle the layout insertion
     * and these events may add at most 6 layouts (e.g *10^•). */
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
    Expression * resultExpression = Expression::parse(text);
    if (resultExpression == nullptr) {
      m_contentView.cursor()->insertText(text);
      return true;
    }
    LayoutRef resultLayoutRef = resultExpression->createLayout();
    delete resultExpression;
    if (currentNumberOfLayouts + resultLayoutRef.numberOfDescendants(true) >= k_maxNumberOfLayouts) {
      return true;
    }
    // Find the pointed layout.
    LayoutRef pointedLayoutRef(nullptr);
    if (strcmp(text, I18n::translate(I18n::Message::RandomCommandWithArg)) == 0) {
      /* Special case: if the text is "random()", the cursor should not be set
       * inside the parentheses. */
      pointedLayoutRef = resultLayoutRef;
    } else if (resultLayoutRef.isHorizontal()) {
      /* If the layout is horizontal, pick the first open parenthesis. For now,
       * all horizontal layouts in MathToolbox have parentheses. */
      for (LayoutRef l : resultLayoutRef.directChildren()) {
        if (l.isLeftParenthesis()) {
          pointedLayoutRef = l;
          break;
        }
      }
    }
    /* Insert the layout. If pointedLayout is nullptr, the cursor will be on the
     * right of the inserted layout. */
    insertLayoutAtCursor(resultLayoutRef, pointedLayoutRef, forceCursorRightOfText);
  }
  return true;
}

bool LayoutField::handleEvent(Ion::Events::Event event) {
  bool didHandleEvent = false;
  bool shouldRecomputeLayout = m_contentView.cursor()->showEmptyLayoutIfNeeded();
  bool moveEventChangedLayout = false;
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
      reload();
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
  if (Responder::handleEvent(event)) {
    /* The only event Responder handles is 'Toolbox' displaying. In that case,
     * the ExpressionLayoutField is forced into editing mode. */
    if (!isEditing()) {
      setEditing(true);
    }
    return true;
  }
  if (isEditing() && m_delegate->layoutFieldShouldFinishEditing(this, event)) { //TODO use class method?
    setEditing(false);
    if (m_delegate->layoutFieldDidFinishEditing(this, layoutRef(), event)) {
      clearLayout();
    }
    return true;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !isEditing()) {
    setEditing(true);
    m_contentView.cursor()->setPLayoutReference(layoutRef());
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
    result = m_contentView.cursor()->cursorOnLeft(shouldRecomputeLayout);
  } else if (event == Ion::Events::Right) {
    result =  m_contentView.cursor()->cursorOnRight(shouldRecomputeLayout);
  } else if (event == Ion::Events::Up) {
    result = m_contentView.cursor()->cursorAbove(shouldRecomputeLayout);
  } else if (event == Ion::Events::Down) {
    result = m_contentView.cursor()->cursorUnder(shouldRecomputeLayout);
  } else if (event == Ion::Events::ShiftLeft) {
    *shouldRecomputeLayout = true;
    if (m_contentView.cursor()->layoutReference()->removeGreySquaresFromAllMatrixAncestors()) {
      *shouldRecomputeLayout = true;
    }
    result.setPointedLayoutRef(layoutRef());
    result.setPosition(LayoutCursor::Position::Left);
  } else if (event == Ion::Events::ShiftRight) {
    if (m_contentView.cursor()->layoutReference()->removeGreySquaresFromAllMatrixAncestors()) {
      *shouldRecomputeLayout = true;
    }
    result.setPointedLayoutRef(layoutRef());
    result.setPosition(LayoutCursor::Position::Right);
  }
  if (result.isDefined()) {
    m_contentView.setCursor(result);
    return true;
  }
  return false;
}

void LayoutField::scrollRightOfLayout(LayoutRef layoutR) {
  KDRect layoutRect(layout.absoluteOrigin().translatedBy(m_contentView.expressionView()->drawingOrigin()), layout.size());
  scrollToBaselinedRect(layoutRect, layout.baseline());
}

void LayoutField::scrollToBaselinedRect(KDRect rect, KDCoordinate baseline) {
  scrollToContentRect(rect, true);
  // Show the rect area around its baseline
  KDCoordinate underBaseline = rect.height() - baseline;
  KDCoordinate minAroundBaseline = min(baseline, underBaseline);
  minAroundBaseline = min(minAroundBaseline, bounds().height() / 2);
  KDRect balancedRect(rect.x(), rect.y() + baseline - minAroundBaseline, rect.width(), 2 * minAroundBaseline);
  scrollToContentRect(balancedRect, true);
}

void LayoutField::insertLayoutAtCursor(LayoutRef layoutR, LayoutRef pointedLayoutR, bool forceCursorRightOfLayout) {
  if (!layoutR.isDefined()) {
    return;
  }
  m_contentView.cursor()->showEmptyLayoutIfNeeded();
  bool layoutWillBeMerged = layoutR->isHorizontal();
  LayoutRef lastMergedLayoutChild = layoutWillBeMerged ? layout.child(layout.numberOfChildren()-1) : nullptr;
  m_contentView.cursor()->addLayoutAndMoveCursor(layout);
  if (!forceCursorRightOfLayout) {
    if (pointedLayoutR.isDefined() && (pointedLayout != layout || !layoutWillBeMerged)) {
      m_contentView.cursor()->setPointedLayoutRef(pointedLayoutR);
      m_contentView.cursor()->setPosition(LayoutCursor::Position::Right);
    } else if (!layoutWillBeMerged) {
      m_contentView.cursor()->setPointedLayoutRef(layout.layoutToPointWhenInserting());
      m_contentView.cursor()->setPosition(LayoutCursor::Position::Right);
    }
  } else if (!layoutWillBeMerged) {
    m_contentView.cursor()->setPointedLayoutRef(layout);
    m_contentView.cursor()->setPosition(LayoutCursor::Position::Right);
  }
  m_contentView.cursor()->layoutReference().addGreySquaresToAllMatrixAncestors();
  m_contentView.cursor()->hideEmptyLayoutIfNeeded();
  reload();
  if (!layoutWillBeMerged) {
    scrollRightOfLayout(layout);
  } else {
    assert(lastMergedLayoutChild != nullptr);
    scrollRightOfLayout(lastMergedLayoutChild);
  }
  scrollToCursor();
}
