#include <escher/layout_field.h>
#include <escher/clipboard.h>
#include <escher/text_field.h>
#include <ion/keyboard/layout_events.h>
#include <poincare/expression.h>
#include <poincare/empty_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/code_point_layout.h>
#include <ion/events.h>
#include <assert.h>
#include <string.h>
#include <algorithm>

using namespace Poincare;

namespace Escher {

LayoutField::ContentView::ContentView(KDFont::Size font) :
    m_expressionView(KDContext::k_alignLeft,
                     KDContext::k_alignCenter,
                     KDColorBlack,
                     KDColorWhite,
                     font,
                     &m_selectionStart,
                     &m_selectionEnd),
    m_isEditing(false) {
  clearLayout();
}

bool LayoutField::ContentView::setEditing(bool isEditing) {
  m_isEditing = isEditing;
  markRectAsDirty(bounds());
  if (isEditing) {
    /* showEmptyLayoutIfNeeded is done in LayoutField::handleEvent, so no need
     * to do it here. */
    if (m_cursor.hideEmptyLayoutIfNeeded()) {
      m_expressionView.layout().invalidAllSizesPositionsAndBaselines();
      return true;
    }
  } else {
    // We're leaving the edition of the current layout
    useInsertionCursor();
  }
  layoutSubviews();
  markRectAsDirty(bounds());
  return false;
}

void LayoutField::ContentView::useInsertionCursor() {
  if (m_insertionCursor.isDefined()) {
    m_cursor.layout().removeGraySquaresFromAllGridAncestors();
    m_cursor = m_insertionCursor;
    m_cursor.layout().addGraySquaresToAllGridAncestors();
  }
}

void LayoutField::ContentView::clearLayout() {
  HorizontalLayout h = HorizontalLayout::Builder();
  if (m_expressionView.setLayout(h)) {
    resetSelection();
  }
  m_cursor.setLayout(h);
}

KDSize LayoutField::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize evSize = m_expressionView.minimalSizeForOptimalDisplay();
  return KDSize(evSize.width() + Poincare::LayoutCursor::k_cursorWidth, evSize.height());
}

bool IsBefore(Layout& l1, Layout& l2, bool strict) {
  char * node1 = reinterpret_cast<char *>(l1.node());
  char * node2 = reinterpret_cast<char *>(l2.node());
  return strict ? (node1 < node2) : (node1 <= node2);
}

void LayoutField::ContentView::addSelection(Layout addedLayout) {
  KDRect rectBefore = selectionRect();
  if (selectionIsEmpty()) {
    /*
     *  ----------  -> +++ is the previous previous selection
     *     (   )    -> added selection
     *  ---+++++--  -> next selection
     * */
    m_selectionStart = addedLayout;
    m_selectionEnd = addedLayout;
  } else if (IsBefore(m_selectionEnd, addedLayout, true)) {
    /*
     *  +++-------  -> +++ is the previous previous selection
     *       (   )  -> added selection
     *  ++++++++++  -> next selection
     *  */
    if (addedLayout.parent() == m_selectionStart) {
      /* The previous selected layout is an horizontal layout and we remove one
       * of its children. */
      assert(m_selectionStart == m_selectionEnd
          && m_selectionStart.type() == LayoutNode::Type::HorizontalLayout);
      m_selectionStart = m_selectionStart.childAtIndex(0);
      m_selectionEnd = m_selectionEnd.childAtIndex(m_selectionEnd.numberOfChildren() - 1);
      addSelection(addedLayout);
      return;
    }
    /* The previous selected layouts and the new added selection are all
     * children of a same horizontal layout. */
    assert(m_selectionStart.parent() == m_selectionEnd.parent()
        && m_selectionStart.parent() == addedLayout.parent()
        && m_selectionStart.parent().type() == LayoutNode::Type::HorizontalLayout);
    m_selectionEnd = addedLayout;
  } else if (IsBefore(addedLayout, m_selectionStart, true)) {
    /*
     *  -------+++  -> +++ is the previous previous selection
     *  (   )       -> added selection
     *  ++++++++++  -> next selection
     * */
    if (addedLayout.type() == LayoutNode::Type::HorizontalLayout
        && m_selectionStart.parent() == addedLayout)
    {
      /* The selection was from the first to the last child of an horizontal
       * layout, we add this horizontal layout -> the selection is now empty. */
      assert(m_selectionEnd.parent() == addedLayout);
      assert(addedLayout.childAtIndex(0) == m_selectionStart);
      assert(addedLayout.childAtIndex(addedLayout.numberOfChildren() - 1) == m_selectionEnd);
      m_selectionStart = Layout();
      m_selectionEnd = Layout();
    } else {
      if (m_selectionStart.hasAncestor(addedLayout, true)) {
        // We are selecting a layout containing the current selection
        m_selectionEnd = addedLayout;
      }
      m_selectionStart = addedLayout;
    }
  } else {
    bool sameEnd = m_selectionEnd == addedLayout;
    bool sameStart = m_selectionStart == addedLayout;
    if (sameStart && sameEnd) {
      /*
       *  -----+++++  -> +++ is the previous previous selection
       *       (   )  -> added selection
       *  ----------  -> next selection
       * */
      m_selectionStart = Layout();
      m_selectionEnd = Layout();
    } else {
      assert(sameStart || sameEnd);
      /*
       *  ++++++++++  -> +++ is the previous previous selection
       *  (   )       -> added selection if sameStart
       *       (   )  -> added selection if sameEnd
       *  +++++-----  -> next selection
       *  The previous selected layouts and the new "added" selection are all
       *  children of a same horizontal layout. */
      Layout horizontalParent = m_selectionStart.parent();
      assert(!horizontalParent.isUninitialized()
          && horizontalParent == m_selectionEnd.parent()
          && horizontalParent == addedLayout.parent()
          && horizontalParent.type() == LayoutNode::Type::HorizontalLayout
          && ((sameEnd && horizontalParent.indexOfChild(m_selectionEnd) > 0)
            || (sameStart && horizontalParent.indexOfChild(m_selectionStart) < horizontalParent.numberOfChildren())));
      if (sameStart) {
        m_selectionStart = horizontalParent.childAtIndex(horizontalParent.indexOfChild(m_selectionStart) + 1);
      } else {
        m_selectionEnd = horizontalParent.childAtIndex(horizontalParent.indexOfChild(m_selectionEnd) - 1);
      }
    }
  }

  KDRect rectAfter = selectionRect();
  // We need to update the background color for selected/unselected layouts
  markRectAsDirty(rectBefore.unionedWith(rectAfter));
}

bool LayoutField::ContentView::resetSelection() {
  if (selectionIsEmpty()) {
    return false;
  }
  m_selectionStart = Layout();
  m_selectionEnd = Layout();
  return true;
}

void LayoutField::ContentView::copySelection(Context * context, bool intoStoreMenu) {
  if (selectionIsEmpty()) {
    if (intoStoreMenu) {
      Container::activeApp()->storeValue();
    }
    return;
  }
  constexpr int bufferSize = TextField::maxBufferSize();
  char buffer[bufferSize];

  if (m_selectionStart == m_selectionEnd) {
    m_selectionStart.serializeParsedExpression(buffer, bufferSize, context);
    if (buffer[0] == 0) {
      int offset = 0;
      if (m_selectionStart.type() == LayoutNode::Type::VerticalOffsetLayout) {
        assert(bufferSize > 1);
        buffer[offset++] = UCodePointEmpty;
      }
      m_selectionStart.serializeForParsing(buffer + offset, bufferSize - offset);
    }
  } else {
    Layout selectionParent = m_selectionStart.parent();
    assert(!selectionParent.isUninitialized());
    assert(selectionParent.type() == LayoutNode::Type::HorizontalLayout);
    int firstIndex = selectionParent.indexOfChild(m_selectionStart);
    int lastIndex = selectionParent.indexOfChild(m_selectionEnd);
    static_cast<HorizontalLayout&>(selectionParent).serializeChildren(firstIndex, lastIndex, buffer, bufferSize);
  }
  if (buffer[0] != 0) {
    if (intoStoreMenu) {
      Container::activeApp()->storeValue(buffer);
    } else {
      Clipboard::sharedClipboard()->store(buffer);
    }
  }
}

bool LayoutField::ContentView::selectionIsEmpty() const {
  assert(!m_selectionStart.isUninitialized() || m_selectionEnd.isUninitialized());
  assert(!m_selectionEnd.isUninitialized() || m_selectionStart.isUninitialized());
  return m_selectionStart.isUninitialized();
}

void LayoutField::ContentView::deleteSelection() {
  if (selectionIsEmpty()) {
    return;
  }
  Layout selectionParent = m_selectionStart.parent();

  /* If the selected layout is the upmost layout, it must be an horizontal
   * layout. Empty it. */
  if (selectionParent.isUninitialized()) {
    assert(m_selectionStart == m_selectionEnd);
    assert(m_selectionStart.type() == LayoutNode::Type::HorizontalLayout);
    clearLayout();
  } else {
    assert(selectionParent == m_selectionEnd.parent());
    // Remove the selected children or replace it with an empty layout.
    if (selectionParent.type() == LayoutNode::Type::HorizontalLayout) {
      int firstIndex = selectionParent.indexOfChild(m_selectionStart);
      int lastIndex = m_selectionStart == m_selectionEnd ? firstIndex : selectionParent.indexOfChild(m_selectionEnd);
      for (int i = lastIndex; i >= firstIndex; i--) {
        static_cast<HorizontalLayout&>(selectionParent).removeChildAtIndex(i, &m_cursor, false);
      }
    } else {
      // Only one child can be selected
      assert(m_selectionStart == m_selectionEnd);
      selectionParent.replaceChildWithEmpty(m_selectionStart, &m_cursor);
    }
  }
  resetSelection();
}

void LayoutField::ContentView::updateInsertionCursor() {
  if (!m_insertionCursor.isDefined()) {
    Layout l = m_cursor.layout();
    if (l.type() == LayoutNode::Type::EmptyLayout && static_cast<EmptyLayout &>(l).color() == EmptyLayoutNode::Color::Gray) {
      // Don't set m_insertionCursor pointing to a layout which might disappear
      return;
    }
    m_insertionCursor = m_cursor;
  }
}

View * LayoutField::ContentView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  View * m_views[] = {&m_expressionView, &m_cursorView};
  return m_views[index];
}

void LayoutField::ContentView::layoutSubviews(bool force) {
  m_expressionView.setFrame(bounds(), force);
  layoutCursorSubview(force);
}

void LayoutField::ContentView::layoutCursorSubview(bool force) {
  if (!m_isEditing) {
    /* We keep track of the cursor's position to prevent the input field from
     * scrolling to the beginning when switching to the history. This way,
     * when calling scrollToCursor after layoutCursorSubview, we don't lose
     * sight of the cursor. */
    m_cursorView.setFrame(KDRect(cursorRect().x(), cursorRect().y(), 0, 0), force);
    return;
  }
  KDPoint expressionViewOrigin = m_expressionView.absoluteDrawingOrigin();
  Layout pointedLayoutR = m_cursor.layout();
  LayoutCursor::Position cursorPosition = m_cursor.position();
  LayoutCursor eqCursor = pointedLayoutR.equivalentCursor(&m_cursor);
  if (eqCursor.isDefined() && pointedLayoutR.hasChild(eqCursor.layout())) {
    pointedLayoutR = eqCursor.layout();
    cursorPosition = eqCursor.position();
  }
  KDPoint cursoredExpressionViewOrigin = pointedLayoutR.absoluteOrigin(font());
  KDCoordinate cursorX = expressionViewOrigin.x() + cursoredExpressionViewOrigin.x();
  if (cursorPosition == LayoutCursor::Position::Right) {
    cursorX += pointedLayoutR.layoutSize(font()).width();
  }
  if (selectionIsEmpty()) {
    KDPoint cursorTopLeftPosition(cursorX, expressionViewOrigin.y() + cursoredExpressionViewOrigin.y() + pointedLayoutR.baseline(font()) - m_cursor.baselineWithoutSelection(font()));
    m_cursorView.setFrame(KDRect(cursorTopLeftPosition, LayoutCursor::k_cursorWidth, m_cursor.cursorHeightWithoutSelection(font())), force);
  } else {
    KDRect cursorRect = selectionRect();
    KDPoint cursorTopLeftPosition(cursorX, expressionViewOrigin.y() + cursorRect.y());
    m_cursorView.setFrame(KDRect(cursorTopLeftPosition, LayoutCursor::k_cursorWidth, cursorRect.height()), force);
  }
}

KDRect LayoutField::ContentView::selectionRect() const {
  if (selectionIsEmpty()) {
    return KDRectZero;
  }
  if (m_selectionStart == m_selectionEnd) {
    return KDRect(m_selectionStart.absoluteOrigin(font()), m_selectionStart.layoutSize(font()));
  }
  Layout selectionParent = m_selectionStart.parent();
  assert(m_selectionEnd.parent() == selectionParent);
  assert(selectionParent.type() == LayoutNode::Type::HorizontalLayout);
  KDRect selectionRectInParent = static_cast<HorizontalLayout &>(selectionParent).relativeSelectionRect(&m_selectionStart, &m_selectionEnd, font());
  return selectionRectInParent.translatedBy(selectionParent.absoluteOrigin(font()));
}

void LayoutField::setEditing(bool isEditing) {
  KDSize previousLayoutSize = m_contentView.minimalSizeForOptimalDisplay();
  if (m_contentView.setEditing(isEditing)) {
    reload(previousLayoutSize);
  }
}

void LayoutField::clearLayout() {
  m_contentView.clearLayout(); // Replace the layout with an empty horizontal layout
  reloadScroll(); // Put the scroll to offset 0
}

void LayoutField::setLayout(Poincare::Layout newLayout) {
  m_contentView.clearLayout();
  KDSize previousSize = minimalSizeForOptimalDisplay();
  const_cast<ExpressionView *>(m_contentView.expressionView())->setLayout(newLayout.makeEditable());
  putCursorRightOfLayout();
  reload(previousSize);
}

Context * LayoutField::context() const {
  return (m_delegate != nullptr) ? m_delegate->context() : nullptr;
}

bool LayoutField::addXNTCodePoint(CodePoint defaultXNTCodePoint) {
  if (!isEditing()) {
    setEditing(true);
  }
  /* TODO : Cycle default XNT and local XNT layouts in parametered expressions
   * such as derivative, sum, integral or layouts. */
  // Query bottom-most layout
  Layout xnt = m_contentView.cursor()->layout().XNTLayout();
  if (xnt.isUninitialized()) {
    xnt = CodePointLayout::Builder(defaultXNTCodePoint);
    if (Ion::Events::repetitionFactor() > 0 && isEditing() && m_contentView.selectionIsEmpty()) {
      // XNT is Cycling, remove the last inserted character
      m_contentView.cursor()->performBackspace();
    }
  }

  // Delete the selected layouts if needed
  deleteSelection();
  // Do not insert layout if it has too many descendants
  if (m_contentView.expressionView()->numberOfLayouts() + xnt.numberOfDescendants(true) >= k_maxNumberOfLayouts) {
    return true;
  }
  // No need to provide an expression because cursor is forced right of text.
  insertLayoutAtCursor(xnt, Poincare::Expression(), true);
  return true;
}

void LayoutField::putCursorRightOfLayout() {
  m_contentView.cursor()->showEmptyLayoutIfNeeded();
  m_contentView.cursor()->layout().removeGraySquaresFromAllGridAncestors();
  m_contentView.setCursor(LayoutCursor(m_contentView.expressionView()->layout(), LayoutCursor::Position::Right));
}

void LayoutField::putCursorLeftOfLayout() {
  m_contentView.cursor()->showEmptyLayoutIfNeeded();
  m_contentView.cursor()->layout().removeGraySquaresFromAllGridAncestors();
  m_contentView.setCursor(LayoutCursor(m_contentView.expressionView()->layout(), LayoutCursor::Position::Left));
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

typedef void (Poincare::LayoutCursor::*AddLayoutPointer)();

bool LayoutField::handleEventWithText(const char * text, bool indentation, bool forceCursorRightOfText) {
  /* The text here can be:
   * - the result of a key pressed, such as "," or "cos(•)"
   * - the text added after a toolbox selection
   * - the result of a copy-paste. */

  /* This routing can be called even if no actual underlying event has been
   * dispatched on the LayoutField. For instance, when someone wants to insert
   * text in the field from the outside. In this scenario, let's make sure the
   * insertionCursor is invalidated. */

  /* TODO : dirty code : Reloads are handled in different places (handleEvent(),
   *        insertLayoutAtCursor() or here) and can be called several times
   *        depending on text source (events, copy-paste, or toolbox). */
  m_contentView.invalidateInsertionCursor();

  // Delete the selected layouts if needed
  deleteSelection();

  if (text[0] == 0) {
    // The text is empty
    return true;
  }

  int currentNumberOfLayouts = m_contentView.expressionView()->numberOfLayouts();
  if (currentNumberOfLayouts >= k_maxNumberOfLayouts - 6) {
    /* We add -6 because in some cases (Ion::Events::Division,
     * Ion::Events::Exp...) we let the layout cursor handle the layout insertion
     * and these events may add at most 6 layouts (e.g *10^). */
    return false;
  }

  Poincare::LayoutCursor * cursor = m_contentView.cursor();
  // Handle special cases
  Ion::Events::Event specialEvents[] = {Ion::Events::Division, Ion::Events::Exp, Ion::Events::Power, Ion::Events::Sqrt, Ion::Events::Square, Ion::Events::EE};
  AddLayoutPointer handleSpecialEvents[] = {&Poincare::LayoutCursor::addFractionLayoutAndCollapseSiblings, &Poincare::LayoutCursor::addEmptyExponentialLayout,  &Poincare::LayoutCursor::addEmptyPowerLayout,  &Poincare::LayoutCursor::addEmptySquareRootLayout, &Poincare::LayoutCursor::addEmptySquarePowerLayout, &Poincare::LayoutCursor::addEmptyTenPowerLayout};
  int numberOfSpecialEvents = sizeof(specialEvents)/sizeof(Ion::Events::Event);
  assert(numberOfSpecialEvents == sizeof(handleSpecialEvents)/sizeof(AddLayoutPointer));
  char buffer[Ion::Events::EventData::k_maxDataSize] = {0};
  for (int i = 0; i < numberOfSpecialEvents; i++) {
    Ion::Events::copyText(static_cast<uint8_t>(specialEvents[i]), buffer, Ion::Events::EventData::k_maxDataSize);
    if (strcmp(text, buffer) == 0) {
      (cursor->*handleSpecialEvents[i])();
      return true;
    }
  }
  if ((strcmp(text, "[") == 0) || (strcmp(text, "]") == 0)) {
    cursor->addEmptyMatrixLayout();
    return true;
  }
  // Single keys are not parsed to avoid changing " or g to _" or _g
  // TODO: currently using char is sufficient but utf8 would be proper
  Expression resultExpression = (text[0] != 0 && text[1] != 0) ? Expression::Parse(text, nullptr) : Expression();
  // If first inserted character was empty, cursor must be left of layout
  bool forceCursorLeftOfText = !forceCursorRightOfText && text[0] == UCodePointEmpty;
  if (resultExpression.isUninitialized()) {
    // The text is not parsable (for instance, ",") and is added char by char.
    KDSize previousLayoutSize = minimalSizeForOptimalDisplay();
    cursor->insertText(text, forceCursorRightOfText, forceCursorLeftOfText);
    reload(previousLayoutSize);
    return true;
  }
  // The text is parsable, we create its layout an insert it.
  Layout resultLayout = resultExpression.createLayout(
    Poincare::Preferences::sharedPreferences()->displayMode(),
    Poincare::PrintFloat::k_numberOfStoredSignificantDigits,
    Container::activeApp() ? Container::activeApp()->localContext() : nullptr,
    true);
  if (currentNumberOfLayouts + resultLayout.numberOfDescendants(true) >= k_maxNumberOfLayouts) {
    return false;
  }
  insertLayoutAtCursor(resultLayout, resultExpression, forceCursorRightOfText, forceCursorLeftOfText);
  return true;
}

bool LayoutField::shouldFinishEditing(Ion::Events::Event event) {
  if (m_delegate->layoutFieldShouldFinishEditing(this, event)) {
    resetSelection();
    return true;
  }
  return false;
}

bool LayoutField::handleEvent(Ion::Events::Event event) {
  bool didHandleEvent = false;
  KDSize previousSize = minimalSizeForOptimalDisplay();
  bool shouldRecomputeLayout = m_contentView.cursor()->showEmptyLayoutIfNeeded();
  bool moveEventChangedLayout = false;
  if (!eventShouldUpdateInsertionCursor(event)) {
    m_contentView.invalidateInsertionCursor();
  }
  if (privateHandleMoveEvent(event, &moveEventChangedLayout)) {
    if (!isEditing()) {
      setEditing(true);
    }
    shouldRecomputeLayout = shouldRecomputeLayout || moveEventChangedLayout;
    didHandleEvent = true;
  } else if (privateHandleSelectionEvent(event, &shouldRecomputeLayout)) {
    didHandleEvent = true;
    // Handle matrices
    if (!m_contentView.selectionIsEmpty()) {
      bool removedSquares = false;
      Layout * selectStart = m_contentView.selectionStart();
      Layout * selectEnd = m_contentView.selectionEnd();
      if (*selectStart != *selectEnd) {
        Layout p = selectStart->parent();
        assert(p == selectEnd->parent());
        assert(p.type() == LayoutNode::Type::HorizontalLayout);
        removedSquares = p.removeGraySquaresFromAllGridChildren();
      } else {
        removedSquares = selectStart->removeGraySquaresFromAllGridChildren();
      }
      shouldRecomputeLayout = m_contentView.cursor()->layout().removeGraySquaresFromAllGridChildren() || removedSquares || shouldRecomputeLayout;
    }
  } else if (privateHandleEvent(event)) {
    shouldRecomputeLayout = true;
    didHandleEvent = true;
  }
  /* Hide empty layout only if the layout is being edited, otherwise the cursor
   * is not visible so any empty layout should be visible. */
  bool didHideLayouts = isEditing() && m_contentView.cursor()->hideEmptyLayoutIfNeeded();
  if (!didHandleEvent) {
    return false;
  }
  shouldRecomputeLayout = didHideLayouts || shouldRecomputeLayout;
  if (!shouldRecomputeLayout) {
    m_contentView.cursorPositionChanged();
    scrollToCursor();
  } else {
    reload(previousSize);
  }
  return true;
}

void LayoutField::deleteSelection() {
  m_contentView.deleteSelection();
}

#define static_assert_immediately_follows(a, b) static_assert( \
  static_cast<uint8_t>(a) + 1 == static_cast<uint8_t>(b), \
  "Ordering error" \
)

#define static_assert_sequential(a, b, c, d) \
  static_assert_immediately_follows(a, b); \
  static_assert_immediately_follows(b, c); \
  static_assert_immediately_follows(c, d);


static_assert_sequential(
  Ion::Events::Left,
  Ion::Events::Up,
  Ion::Events::Down,
  Ion::Events::Right
);

static inline bool IsMoveEvent(Ion::Events::Event event) {
  return
    static_cast<uint8_t>(event) >= static_cast<uint8_t>(Ion::Events::Left) &&
    static_cast<uint8_t>(event) <= static_cast<uint8_t>(Ion::Events::Right);
}

bool LayoutField::privateHandleEvent(Ion::Events::Event event) {
  if (m_delegate && m_delegate->layoutFieldDidReceiveEvent(this, event)) {
    return true;
  }
  if (handleBoxEvent(event)) {
    if (!isEditing()) {
      setEditing(true);
    }
    return true;
  }
  if (isEditing() && m_delegate && m_delegate->layoutFieldShouldFinishEditing(this, event)) { //TODO use class method?
    setEditing(false);
    if (m_delegate->layoutFieldDidFinishEditing(this, layout(), event)) {
      // Reinit layout for next use
      clearLayout();
      resetSelection();
    } else {
      setEditing(true);
    }
    return true;
  }
  /* if move event was not caught neither by privateHandleMoveEvent nor by
   * layoutFieldShouldFinishEditing, we handle it here to avoid bubbling the
   * event up. */
  if (IsMoveEvent(event) && isEditing()) {
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
    resetSelection();
    setEditing(false);
    m_delegate->layoutFieldDidAbortEditing(this);
    return true;
  }
  char buffer[Ion::Events::EventData::k_maxDataSize] = {0};
  size_t eventTextLength = Ion::Events::copyText(static_cast<uint8_t>(event), buffer, Ion::Events::EventData::k_maxDataSize);
  if (eventTextLength > 0 || event == Ion::Events::Paste || event == Ion::Events::Backspace) {
    if (!isEditing()) {
      setEditing(true);
    }
    if (eventTextLength > 0) {
      handleEventWithText(buffer);
    } else if (event == Ion::Events::Paste) {
      handleEventWithText(Clipboard::sharedClipboard()->storedText(), false, true);
    } else {
      assert(event == Ion::Events::Backspace);
      if (!m_contentView.selectionIsEmpty()) {
        deleteSelection();
      } else {
        m_contentView.cursor()->performBackspace();
      }
    }
    return true;
  }
  if ((event == Ion::Events::Copy || event == Ion::Events::Cut || event == Ion::Events::Sto) && isEditing()) {
    m_contentView.copySelection(context(), event == Ion::Events::Sto);
    if (event == Ion::Events::Cut) {
      m_contentView.deleteSelection();
    }
    return true;
  }
  if (event == Ion::Events::Clear && isEditing()) {
    clearLayout();
    resetSelection();
    return true;
  }
  return false;
}

bool LayoutField::handleStoreEvent() {
  m_contentView.copySelection(context(), true);
  return true;
}

static_assert_sequential(
  LayoutCursor::Direction::Left,
  LayoutCursor::Direction::Up,
  LayoutCursor::Direction::Down,
  LayoutCursor::Direction::Right
);


static inline LayoutCursor::Direction DirectionForMoveEvent(Ion::Events::Event event) {
  assert(IsMoveEvent(event));
  return static_cast<LayoutCursor::Direction>(
    static_cast<uint8_t>(LayoutCursor::Direction::Left) +
    static_cast<uint8_t>(event) - static_cast<uint8_t>(Ion::Events::Left)
  );
}

bool LayoutField::privateHandleMoveEvent(Ion::Events::Event event, bool * shouldRecomputeLayout) {
  if (!IsMoveEvent(event)) {
    return false;
  }
  if (resetSelection()) {
    *shouldRecomputeLayout = true;
    return true;
  }
  LayoutCursor result;
  int step = Ion::Events::longPressFactor();
  result = m_contentView.cursor()->cursorAtDirection(DirectionForMoveEvent(event), shouldRecomputeLayout, false, step);
  if (result.isDefined()) {
    if (eventShouldUpdateInsertionCursor(event)) {
      m_contentView.updateInsertionCursor();
    }
    m_contentView.setCursor(result);
    return true;
  }
  return false;
}

static_assert_sequential(
  Ion::Events::ShiftLeft,
  Ion::Events::ShiftUp,
  Ion::Events::ShiftDown,
  Ion::Events::ShiftRight
);

static inline bool IsSelectionEvent(Ion::Events::Event event) {
  return
    static_cast<uint8_t>(event) >= static_cast<uint8_t>(Ion::Events::ShiftLeft) &&
    static_cast<uint8_t>(event) <= static_cast<uint8_t>(Ion::Events::ShiftRight);
}

static inline LayoutCursor::Direction DirectionForSelectionEvent(Ion::Events::Event event) {
  assert(IsSelectionEvent(event));
  return static_cast<LayoutCursor::Direction>(
    static_cast<uint8_t>(LayoutCursor::Direction::Left) +
    static_cast<uint8_t>(event) - static_cast<uint8_t>(Ion::Events::ShiftLeft)
  );
}

bool LayoutField::privateHandleSelectionEvent(Ion::Events::Event event, bool * shouldRecomputeLayout) {
  if (!IsSelectionEvent(event)) {
    return false;
  }
  int step = Ion::Events::longPressFactor();
  // Selection is handled one step at a time. Repeat selection for each step.
  for (int i = 0; i < step; ++i) {
    Layout addedSelection;
    LayoutCursor result = m_contentView.cursor()->selectAtDirection(
      DirectionForSelectionEvent(event),
      shouldRecomputeLayout,
      &addedSelection
    );
    if (addedSelection.isUninitialized()) {
      // Successful event if at least one step succeeded.
      return i > 0;
    }
    /* TODO : addSelection() is built so that it should be called steps by steps
     *        It could be reworked to handle selection with steps > 1 and match
     *        text_input's implementation */
    m_contentView.addSelection(addedSelection);
    assert(result.isDefined());
    m_contentView.setCursor(result);
  }
  return true;
}

void LayoutField::scrollRightOfLayout(Layout layoutR) {
  KDRect layoutRect(layoutR.absoluteOrigin(m_contentView.font()).translatedBy(m_contentView.expressionView()->drawingOrigin()), layoutR.layoutSize(m_contentView.font()));
  scrollToBaselinedRect(layoutRect, layoutR.baseline(m_contentView.font()));
}

void LayoutField::scrollToBaselinedRect(KDRect rect, KDCoordinate baseline) {
  scrollToContentRect(rect, true);
  // Show the rect area around its baseline
  KDCoordinate underBaseline = rect.height() - baseline;
  KDCoordinate minAroundBaseline = std::min(baseline, underBaseline);
  minAroundBaseline = std::min<KDCoordinate>(minAroundBaseline, bounds().height() / 2);
  KDRect balancedRect(rect.x(), rect.y() + baseline - minAroundBaseline, rect.width(), 2 * minAroundBaseline);
  scrollToContentRect(balancedRect, true);
}

void LayoutField::insertLayoutAtCursor(Layout layoutR, Poincare::Expression correspondingExpression, bool forceCursorRightOfLayout, bool forceCursorLeftOfText) {
  if (layoutR.isUninitialized()) {
    return;
  }
  layoutR = layoutR.makeEditable();
  KDSize previousSize = minimalSizeForOptimalDisplay();
  Poincare::LayoutCursor * cursor = m_contentView.cursor();
  // Handle empty layouts
  cursor->showEmptyLayoutIfNeeded();

  bool layoutWillBeMerged = layoutR.type() == LayoutNode::Type::HorizontalLayout;
  Layout lastMergedLayoutChild = (layoutWillBeMerged && layoutR.numberOfChildren() > 0) ? layoutR.childAtIndex(layoutR.numberOfChildren()-1) : Layout();

  // If the layout will be merged, find now where the cursor will point
  Layout cursorMergedLayout = Layout();
  if (layoutWillBeMerged) {
    if (forceCursorRightOfLayout) {
      cursorMergedLayout = lastMergedLayoutChild;
    } else if (forceCursorLeftOfText) {
      // First Merged Layout Child
      cursorMergedLayout = layoutR.numberOfChildren() > 0 ? layoutR.childAtIndex(0) : Layout();
    } else {
      assert(!correspondingExpression.isUninitialized());
      cursorMergedLayout = layoutR.layoutToPointWhenInserting(&correspondingExpression, &forceCursorLeftOfText);
      if (cursorMergedLayout == layoutR) {
        /* LayoutR will not be inserted in the layout, so point to its last
         * child instead. It is visually equivalent. */
        cursorMergedLayout = lastMergedLayoutChild;
      }
    }
  }

  // Add the layout. This puts the cursor at the right of the added layout
  cursor->addLayoutAndMoveCursor(layoutR);

  /* Move the cursor if needed.
   *
   * If forceCursorRightOfLayout is true and the layout has been merged, there
   * is no need to move the cursor because it already points to the right of the
   * added layouts.
   *
   * If the layout to point to has been merged, only its children have been
   * inserted in the layout. We already computed where the cursor should point,
   * because we cannot compute this now that the children are merged in between
   * another layout's children.
   *
   * For other cases, move the cursor to the layout indicated by
   * layoutToPointWhenInserting. This pointed layout cannot be computed before
   * adding layoutR, because addLayoutAndMoveCursor might have changed layoutR's
   * children.
   * For instance, if we add an absolute value with an empty child left of a 0,
   * the empty child is deleted and the 0 is collapsed into the absolute value.
   * Sketch of the situation, ' being the cursor:
   *  Initial layout:   '0
   *  "abs(x)" pressed in the toolbox => |•| is added, • being an empty layout
   *  Final layout: |0'|
   *
   * Fortunately, merged layouts' children are not modified by the merge, so it
   * is ok to compute their pointed layout before adding them.
   * */

  if (!forceCursorRightOfLayout) {
    if (!layoutWillBeMerged && !forceCursorLeftOfText) {
      assert(cursorMergedLayout.isUninitialized());
      assert(!correspondingExpression.isUninitialized());
      cursorMergedLayout = layoutR.layoutToPointWhenInserting(&correspondingExpression, &forceCursorLeftOfText);
    }
    assert(!cursorMergedLayout.isUninitialized());
    m_contentView.cursor()->setLayout(cursorMergedLayout);
    // If forceCursorLeftOfText, position cursor left of first merged layout.
    m_contentView.cursor()->setPosition(forceCursorLeftOfText ? LayoutCursor::Position::Left : LayoutCursor::Position::Right);
  } else if (!layoutWillBeMerged) {
    m_contentView.cursor()->setLayout(layoutR);
    m_contentView.cursor()->setPosition(LayoutCursor::Position::Right);
  }

  // Handle matrices
  cursor->layout().addGraySquaresToAllGridAncestors();

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

}
