#include <escher/layout_field.h>
#include <escher/clipboard.h>
#include <escher/text_field.h>
#include <ion/keyboard/layout_events.h>
#include <poincare/expression.h>
#include <poincare/horizontal_layout.h>
#include <poincare/code_point_layout.h>
#include <ion/events.h>
#include <assert.h>
#include <string.h>
#include <algorithm>

using namespace Poincare;

namespace Escher {

LayoutField::ContentView::ContentView(KDFont::Size font) :
    m_expressionView(&m_cursor,
                     KDContext::k_alignLeft,
                     KDContext::k_alignCenter,
                     KDColorBlack,
                     KDColorWhite,
                     font),
    m_isEditing(false) {
  clearLayout();
}

bool LayoutField::ContentView::setEditing(bool isEditing) {
  if (m_isEditing == isEditing) {
    return false;
  }
  m_isEditing = isEditing;
  markRectAsDirty(bounds());
  bool layoutChanged = false;
  if (isEditing) {
    layoutChanged = m_cursor.didEnterCurrentPosition();
  } else {
    // We're leaving the edition of the current layout
    layoutChanged = m_cursor.didExitPosition();
  }
  layoutSubviews();
  markRectAsDirty(bounds());
  return layoutChanged;
}

void LayoutField::ContentView::clearLayout() {
  HorizontalLayout h = HorizontalLayout::Builder();
  m_expressionView.setLayout(h);
  m_cursor = LayoutCursor(h);
}

KDSize LayoutField::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize evSize = m_expressionView.minimalSizeForOptimalDisplay();
  return KDSize(evSize.width() + Poincare::LayoutCursor::k_cursorWidth, evSize.height());
}

void LayoutField::ContentView::copySelection(Context * context, bool intoStoreMenu) {
  LayoutSelection selection = m_cursor.selection().clone();
  if (selection.isEmpty()) {
    if (intoStoreMenu) {
      Container::activeApp()->storeValue();
    }
    return;
  }
  constexpr int bufferSize = TextField::MaxBufferSize();
  char buffer[bufferSize];
  Layout layoutToParse;
  if (selection.layout().isHorizontal()) {
    layoutToParse = HorizontalLayout::Builder();
    for (int i = selection.leftPosition(); i < selection.rightPosition(); i++) {
      static_cast<HorizontalLayout&>(layoutToParse).addChildAtIndexInPlace(selection.layout().childAtIndex(i), i - selection.leftPosition(), i - selection.leftPosition());
    }
  } else {
    layoutToParse = selection.layout();
  }

  layoutToParse.serializeParsedExpression(buffer, bufferSize, context);
  if (buffer[0] == 0) {
    layoutToParse.serializeForParsing(buffer, bufferSize);
  }
  if (buffer[0] == 0) {
    return;
  }
  if (intoStoreMenu) {
    Container::activeApp()->storeValue(buffer);
  } else {
    Clipboard::SharedClipboard()->store(buffer);
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
  KDPoint cursorTopLeftPosition =
    m_expressionView.absoluteDrawingOrigin()
      .translatedBy(m_cursor.cursorAbsoluteOrigin(font()));
  m_cursorView.setFrame(KDRect(cursorTopLeftPosition, LayoutCursor::k_cursorWidth, m_cursor.cursorHeight(font())), force);
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
  putCursorOnOneSide(OMG::Direction::Right());
  reload(previousSize);
}

Context * LayoutField::context() const {
  return (m_delegate != nullptr) ? m_delegate->context() : nullptr;
}

size_t LayoutField::dumpContent(char * buffer, size_t bufferSize, int * cursorOffset, int * position) {
  assert(layoutHasNode());
  size_t size = layout().size();
  if (size > bufferSize) {
    buffer[0] = 0;
    size = 0;
    *cursorOffset = -1;
  } else {
    memcpy(buffer, reinterpret_cast<char *>(layout().node()), size);
    *cursorOffset = reinterpret_cast<char *>(cursor()->layout().node()) - reinterpret_cast<char *>(layout().node());
    *position = cursor()->position();
  }
  return size;
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
    if (Ion::Events::repetitionFactor() > 0 && isEditing()) {
      assert(m_contentView.cursor()->selection().isEmpty());
      // XNT is Cycling, remove the last inserted character
      m_contentView.cursor()->performBackspace();
    }
  }

  // Do not insert layout if it has too many descendants
  if (m_contentView.expressionView()->numberOfLayouts() + xnt.numberOfDescendants(true) >= k_maxNumberOfLayouts) {
    return true;
  }
  // TODO: Insert text if linearMode
  insertLayoutAtCursor(xnt, true);
  return true;
}

void LayoutField::putCursorOnOneSide(OMG::HorizontalDirection side) {
  LayoutCursor previousCursor = *m_contentView.cursor();
  m_contentView.setCursor(LayoutCursor(m_contentView.expressionView()->layout(), side));
  m_contentView.cursor()->didEnterCurrentPosition(previousCursor);
}

void LayoutField::reload(KDSize previousSize) {
  KDSize newSize = minimalSizeForOptimalDisplay();
  if (m_delegate && previousSize.height() != newSize.height()) {
    m_delegate->layoutFieldDidChangeSize(this);
  }
  m_contentView.cursorPositionChanged();
  scrollToCursor();
  markRectAsDirty(bounds());
}

typedef void (Poincare::LayoutCursor::*AddLayoutPointer)(Context * context);

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
    return false;
  }

  Poincare::LayoutCursor * cursor = m_contentView.cursor();
  // Handle special cases
  Ion::Events::Event specialEvents[] = {Ion::Events::Division, Ion::Events::Exp, Ion::Events::Power, Ion::Events::Sqrt, Ion::Events::Square, Ion::Events::EE};
  AddLayoutPointer handleSpecialEvents[] = {&Poincare::LayoutCursor::addFractionLayoutAndCollapseSiblings, &Poincare::LayoutCursor::addEmptyExponentialLayout,  &Poincare::LayoutCursor::addEmptyPowerLayout,  &Poincare::LayoutCursor::addEmptySquareRootLayout, &Poincare::LayoutCursor::addEmptySquarePowerLayout, &Poincare::LayoutCursor::addEmptyTenPowerLayout};
  int numberOfSpecialEvents = sizeof(specialEvents)/sizeof(Ion::Events::Event);
  assert(numberOfSpecialEvents == sizeof(handleSpecialEvents)/sizeof(AddLayoutPointer));
  if (!linearMode()) {
    char buffer[Ion::Events::EventData::k_maxDataSize] = {0};
    for (int i = 0; i < numberOfSpecialEvents; i++) {
      Ion::Events::copyText(static_cast<uint8_t>(specialEvents[i]), buffer, Ion::Events::EventData::k_maxDataSize);
      if (strcmp(text, buffer) == 0) {
        (cursor->*handleSpecialEvents[i])(delegateContext());
        return true;
      }
    }
  }
  if ((strcmp(text, "[") == 0) || (strcmp(text, "]") == 0)) {
    cursor->addEmptyMatrixLayout(delegateContext());
    return true;
  }
  // Single keys are not parsed to avoid changing " or g to _" or _g
  Expression resultExpression = UTF8Helper::StringGlyphLength(text) > 1 ? Expression::Parse(text, nullptr) : Expression();
  // If first inserted character was empty, cursor must be left of layout
  bool forceCursorLeftOfText = !forceCursorRightOfText && text[0] == UCodePointEmpty;
  if (linearMode() || resultExpression.isUninitialized()) {
    // The text is not parsable (for instance, ",") and is added char by char.
    KDSize previousLayoutSize = minimalSizeForOptimalDisplay();
    cursor->insertText(text, delegateContext(), forceCursorRightOfText, forceCursorLeftOfText, linearMode());
    reload(previousLayoutSize);
    return true;
  }
  // The text is parsable, we create its layout an insert it.
  Layout resultLayout = resultExpression.createLayout(
    Poincare::Preferences::sharedPreferences->displayMode(),
    Poincare::PrintFloat::k_numberOfStoredSignificantDigits,
    Container::activeApp() ? Container::activeApp()->localContext() : nullptr,
    true);
  if (currentNumberOfLayouts + resultLayout.numberOfDescendants(true) >= k_maxNumberOfLayouts) {
    return false;
  }
  // Do not enter parentheses of expression that take no argument like random()
  forceCursorRightOfText = forceCursorRightOfText || resultExpression.numberOfChildren() == 0;
  insertLayoutAtCursor(resultLayout, forceCursorRightOfText, forceCursorLeftOfText);
  return true;
}

bool LayoutField::shouldFinishEditing(Ion::Events::Event event) {
  if (m_delegate->layoutFieldShouldFinishEditing(this, event)) {
    m_contentView.cursor()->stopSelecting();
    return true;
  }
  return false;
}

bool LayoutField::handleEvent(Ion::Events::Event event) {
  KDSize previousSize = minimalSizeForOptimalDisplay();
  bool shouldRedrawLayout = false;
  bool didHandleEvent = false;
  if (privateHandleMoveEvent(event, &shouldRedrawLayout)) {
    if (!isEditing()) {
      setEditing(true);
    }
    didHandleEvent = true;
  } else if (privateHandleEvent(event, &shouldRedrawLayout)) {
    didHandleEvent = true;
  }
  if (!shouldRedrawLayout) {
    m_contentView.cursorPositionChanged();
    scrollToCursor();
  } else {
    reload(previousSize);
  }
  return m_delegate ? m_delegate->layoutFieldDidHandleEvent(this, didHandleEvent, shouldRedrawLayout) : didHandleEvent;
}

bool LayoutField::privateHandleEvent(Ion::Events::Event event, bool * shouldRedrawLayout) {
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
    } else {
      setEditing(true);
    }
    return true;
  }
  /* if move event was not caught neither by privateHandleMoveEvent nor by
   * layoutFieldShouldFinishEditing, we handle it here to avoid bubbling the
   * event up. */
  if (event.isMoveEvent() && isEditing()) {
    return true;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !isEditing()) {
    setEditing(true);
    return true;
  }
  if (event == Ion::Events::Back && isEditing()) {
    clearLayout();
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
      handleEventWithText(Clipboard::SharedClipboard()->storedText(), false, true);
    } else {
      assert(event == Ion::Events::Backspace);
      m_contentView.cursor()->performBackspace();
    }
    *shouldRedrawLayout = true;
    return true;
  }
  if ((event == Ion::Events::Copy || event == Ion::Events::Cut || event == Ion::Events::Sto) && isEditing()) {
    m_contentView.copySelection(context(), event == Ion::Events::Sto);
    if (event == Ion::Events::Cut && !m_contentView.cursor()->selection().isEmpty()) {
      m_contentView.cursor()->performBackspace();
      *shouldRedrawLayout = true;
    }
    return true;
  }
  if (event == Ion::Events::Clear && isEditing()) {
    clearLayout();
    return true;
  }
  return false;
}

bool LayoutField::handleStoreEvent() {
  m_contentView.copySelection(context(), true);
  return true;
}

bool LayoutField::privateHandleMoveEvent(Ion::Events::Event event, bool * shouldRedrawLayout) {
  bool isMoveEvent = event.isMoveEvent();
  bool isSelectionEvent = event.isSelectionEvent();
  if (!isMoveEvent && !isSelectionEvent) {
    return false;
  }
  return m_contentView.cursor()->moveMultipleSteps(OMG::Direction(event), Ion::Events::longPressFactor(), isSelectionEvent, shouldRedrawLayout);
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

void LayoutField::insertLayoutAtCursor(Layout layout, bool forceCursorRightOfLayout, bool forceCursorLeftOfLayout) {
  assert(!linearMode());
  if (layout.isUninitialized()) {
    return;
  }
  layout = layout.makeEditable();
  KDSize previousSize = minimalSizeForOptimalDisplay();
  m_contentView.cursor()->insertLayoutAtCursor(layout, delegateContext(), forceCursorRightOfLayout, forceCursorLeftOfLayout);

  // Reload
  reload(previousSize);
  scrollToCursor();
}

}
