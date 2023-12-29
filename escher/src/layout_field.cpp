#include <assert.h>
#include <escher/clipboard.h>
#include <escher/layout_field.h>
#include <escher/text_field.h>
#include <ion/events.h>
#include <ion/keyboard/layout_events.h>
#include <poincare/code_point_layout.h>
#include <poincare/expression.h>
#include <poincare/horizontal_layout.h>
#include <poincare/junior_layout.h>
#include <poincare/linear_layout_decoder.h>
#include <poincare/parametered_expression.h>
#include <poincare/parenthesis_layout.h>
#include <poincare/string_layout.h>
#include <poincare/symbol.h>
#include <poincare/xnt_helpers.h>
#include <poincare_junior/src/layout/app_helpers.h>
#include <poincare_junior/src/layout/layout_cursor.h>
#include <poincare_junior/src/layout/rack_layout.h>
#include <string.h>

#include <algorithm>
#include <array>

using namespace Poincare;

namespace Escher {

bool LayoutField::ContentView::setEditing(bool isEditing) {
  if (m_isEditing == isEditing) {
    return false;
  }
  // m_cursor.resetSelection();
  m_isEditing = isEditing;
  markWholeFrameAsDirty();
  bool layoutChanged = false;
  if (isEditing) {
    // layoutChanged = m_cursor.didEnterCurrentPosition();
  } else {
    // We're leaving the edition of the current layout
    // layoutChanged = m_cursor.didExitPosition();
  }
  layoutSubviews();
  markWholeFrameAsDirty();
  return layoutChanged;
}

void LayoutField::ContentView::clearLayout() {
  OLayout h = HorizontalLayout::Builder();
  JuniorLayout l = JuniorLayout::Juniorize(h);
  m_layoutView.setLayout(l);
  m_cursor =
      PoincareJ::LayoutBufferCursor(l, const_cast<PoincareJ::Tree *>(l.tree()));
}

KDSize LayoutField::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize evSize = m_layoutView.minimalSizeForOptimalDisplay();
  return KDSize(evSize.width() + TextCursorView::k_width, evSize.height());
}

void LayoutField::ContentView::copySelection(PoincareJ::Context *context,
                                             bool intoStoreMenu) {
  PoincareJ::LayoutSelection selection = m_cursor.selection();
  if (selection.isEmpty()) {
    if (intoStoreMenu) {
      App::app()->storeValue();
    }
    return;
  }
  constexpr size_t bufferSize = TextField::MaxBufferSize();
  char buffer[bufferSize];
  PoincareJ::Tree *t = selection.cloneSelection();
  Layout layoutToParse = JuniorLayout::Builder(t);
  t->removeTree();

  layoutToParse.serializeParsedExpression(buffer, bufferSize,
                                          /* TODO context */ nullptr);
  if (buffer[0] == 0) {
    layoutToParse.serializeForParsing(buffer, bufferSize);
  }
  if (buffer[0] == 0) {
    return;
  }
  if (intoStoreMenu) {
    App::app()->storeValue(buffer);
  } else {
    Clipboard::SharedClipboard()->store(buffer);
  }
}

View *LayoutField::ContentView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_layoutView;
  }
  return TextCursorView::CursorFieldView::subviewAtIndex(index);
}

void LayoutField::ContentView::layoutSubviews(bool force) {
  setChildFrame(&m_layoutView, bounds(), force);
  TextCursorView::CursorFieldView::layoutSubviews(force);
}

KDRect LayoutField::ContentView::cursorRect() const {
  KDPoint cursorTopLeftPosition = m_layoutView.drawingOrigin().translatedBy(
      m_cursor.cursorAbsoluteOrigin(font()));
  if (!m_isEditing) {
    /* We keep track of the cursor's position to prevent the input field from
     * scrolling to the beginning when switching to the history. This way,
     * when calling scrollToCursor after layoutCursorSubview, we don't lose
     * sight of the cursor. */
    return KDRect(cursorTopLeftPosition, KDSizeZero);
  }
  return KDRect(cursorTopLeftPosition, TextCursorView::k_width,
                m_cursor.cursorHeight(font()));
}

LayoutField::ContentView::ContentView(KDGlyph::Format format)
    : m_cursor(), m_layoutView(&m_cursor, format), m_isEditing(false) {
  PoincareJ::RackLayout::layoutCursor = &m_cursor;
  clearLayout();
}

/* TODO: This buffer could probably be shared with some other temporary
 * space (for example the one from TextField). */
static char s_draftBuffer[AbstractTextField::MaxBufferSize()];

LayoutField::LayoutField(Responder *parentResponder,
                         LayoutFieldDelegate *delegate, KDGlyph::Format format)
    : EditableField(parentResponder, &m_contentView),
      m_contentView(format),
      m_delegate(delegate),
      m_inputViewMemoizedHeight(0),
      m_draftBuffer(s_draftBuffer),
      m_draftBufferSize(AbstractTextField::MaxBufferSize()) {
  setBackgroundColor(KDColorWhite);
}

void LayoutField::setDelegate(LayoutFieldDelegate *delegate) {
  m_delegate = delegate;
}

void LayoutField::setEditing(bool isEditing) {
  KDSize previousLayoutSize = m_contentView.minimalSizeForOptimalDisplay();
  if (m_contentView.setEditing(isEditing)) {
    reload(previousLayoutSize);
  }
}

void LayoutField::clearLayout() {
  // Replace the layout with an empty horizontal layout
  m_contentView.clearLayout();
  // Put the scroll to offset 0
  resetScroll();
  if (m_delegate) {
    m_delegate->layoutFieldDidChangeSize(this);
  }
}

void LayoutField::clearAndSetEditing(bool isEditing) {
  clearLayout();
  setEditing(isEditing);
}

void LayoutField::scrollToCursor() {
  KDRect cursorRect = m_contentView.cursorRect();
  KDCoordinate cursorBaseline = cursor()->cursorHeight(m_contentView.font());
  // cursor()->layout().baseline(m_contentView.font());
  scrollToBaselinedRect(cursorRect, cursorBaseline);
}

void LayoutField::setLayout(Poincare::OLayout newLayout) {
  m_contentView.clearLayout();
  KDSize previousSize = minimalSizeForOptimalDisplay();
  const_cast<LayoutView *>(m_contentView.layoutView())
      ->setLayout(JuniorLayout::Juniorize(newLayout.makeEditable()));
  putCursorOnOneSide(OMG::Direction::Right());
  reload(previousSize);
}

PoincareJ::Context *LayoutField::context() const {
  return nullptr;
  // return m_delegate ? m_delegate->context() : nullptr;
}

size_t LayoutField::dumpContent(char *buffer, size_t bufferSize,
                                int *cursorOffset, int *position) {
  assert(layoutHasNode());
  size_t size = layout().size();
  if (size > bufferSize) {
    buffer[0] = 0;
    size = 0;
    *cursorOffset = -1;
  } else {
    memcpy(buffer, reinterpret_cast<char *>(layout().node()), size);
#if 0
    *cursorOffset = reinterpret_cast<char *>(cursor()->layout().node()) -
                    reinterpret_cast<char *>(layout().node());
    *position = cursor()->position();
#endif
  }
  return size;
}

void LayoutField::setBackgroundColor(KDColor c) {
  ScrollableView::setBackgroundColor(c);
  m_contentView.setBackgroundColor(c);
}

bool LayoutField::prepareToEdit() {
  if (!isEditing()) {
    setEditing(true);
  }
  assert(isEditing());
  return true;
}

bool LayoutField::findXNT(char *buffer, size_t bufferSize, int xntIndex,
                          size_t *cycleSize) {
#if 0
  Layout layout = cursor()->layout();
  if (linearMode()) {
    assert(layout.isHorizontal());
    HorizontalLayout horizontalLayout = static_cast<HorizontalLayout &>(layout);
    LinearLayoutDecoder decoder(horizontalLayout, cursor()->position());
    return XNTHelpers::FindXNTSymbol1D(decoder, buffer, bufferSize, xntIndex,
                                       cycleSize);
  }
  return XNTHelpers::FindXNTSymbol2D(layout, buffer, bufferSize, xntIndex,
                                     cycleSize);
#else
  return false;
#endif
}

void LayoutField::removePreviousXNT() {
  assert(cursor()->selection().isEmpty());
  // XNT is Cycling, remove the last inserted character
  cursor()->performBackspace();
}

void LayoutField::putCursorOnOneSide(OMG::HorizontalDirection side) {
  m_contentView.cursor()->setLayout(m_contentView.cursor()->rootNode(), side);
}

void LayoutField::reload(KDSize previousSize) {
  KDSize newSize = minimalSizeForOptimalDisplay();
  if (m_delegate && previousSize.height() != newSize.height()) {
    m_delegate->layoutFieldDidChangeSize(this);
  }
  m_contentView.cursorPositionChanged();
  scrollToCursor();
  markWholeFrameAsDirty();
}

using LayoutInsertionMethod =
    void (PoincareJ::LayoutBufferCursor::*)(PoincareJ::Context *context);

bool LayoutField::insertText(const char *text, bool indentation,
                             bool forceCursorRightOfText) {
  /* The text here can be:
   * - the result of a key pressed, such as "," or "cos(•)"
   * - the text added after a toolbox selection
   * - the result of a copy-paste. */

  if (text[0] == 0) {
    // The text is empty
    return true;
  }

  int currentNumberOfLayouts = m_contentView.layoutView()->numberOfLayouts();
  if (currentNumberOfLayouts >= k_maxNumberOfLayouts - 6) {
    /* We add -6 because in some cases (Ion::Events::Division,
     * Ion::Events::Exp...) we let the layout cursor handle the layout insertion
     * and these events may add at most 6 layouts (e.g *10^). */
    return false;
  }

  PoincareJ::LayoutBufferCursor *cursor = this->cursor();
  // Handle special cases
  constexpr Ion::Events::Event specialEvents[] = {
      Ion::Events::Division, Ion::Events::Exp,    Ion::Events::Power,
      Ion::Events::Sqrt,     Ion::Events::Square, Ion::Events::EE};
  constexpr LayoutInsertionMethod handleSpecialEvents[] = {
      &PoincareJ::LayoutBufferCursor::addFractionLayoutAndCollapseSiblings,
      &PoincareJ::LayoutBufferCursor::addEmptyExponentialLayout,
      &PoincareJ::LayoutBufferCursor::addEmptyPowerLayout,
      &PoincareJ::LayoutBufferCursor::addEmptySquareRootLayout,
      &PoincareJ::LayoutBufferCursor::addEmptySquarePowerLayout,
      &PoincareJ::LayoutBufferCursor::addEmptyTenPowerLayout};
  constexpr int numberOfSpecialEvents = std::size(specialEvents);
  static_assert(numberOfSpecialEvents == std::size(handleSpecialEvents),
                "Wrong number of layout insertion methods");
  if (!linearMode()) {
    char buffer[Ion::Events::EventData::k_maxDataSize] = {0};
    for (int i = 0; i < numberOfSpecialEvents; i++) {
      Ion::Events::copyText(static_cast<uint8_t>(specialEvents[i]), buffer,
                            Ion::Events::EventData::k_maxDataSize);
      if (strcmp(text, buffer) == 0) {
        (cursor->*handleSpecialEvents[i])(context());
        return true;
      }
    }
    if ((strcmp(text, "[") == 0) || (strcmp(text, "]") == 0)) {
      cursor->addEmptyMatrixLayout(context());
      return true;
    }
  }
  // Single keys are not parsed to avoid changing " or g to _" or _g
  Expression resultExpression = UTF8Helper::StringGlyphLength(text) > 1
                                    ? Expression::Parse(text, nullptr)
                                    : Expression();
  // If first inserted character was empty, cursor must be left of layout
  bool forceCursorLeftOfText =
      !forceCursorRightOfText && text[0] == UCodePointEmpty;
  if (linearMode() || resultExpression.isUninitialized()) {
    // The text is not parsable (for instance, ",") and is added char by char.
    KDSize previousLayoutSize = minimalSizeForOptimalDisplay();
    cursor->insertText(text, context(), forceCursorRightOfText,
                       forceCursorLeftOfText, linearMode());
    reload(previousLayoutSize);
    return true;
  }
  // The text is parsable, we create its layout an insert it.
  OLayout resultLayout = resultExpression.createLayout(
      Poincare::Preferences::SharedPreferences()->displayMode(),
      Poincare::PrintFloat::k_maxNumberOfSignificantDigits,
      App::app() ? App::app()->localContext() : nullptr, true);
  if (currentNumberOfLayouts + resultLayout.numberOfDescendants(true) >=
      k_maxNumberOfLayouts) {
    return false;
  }
  // Do not enter parentheses of expression that take no argument like random()
  forceCursorRightOfText =
      forceCursorRightOfText || resultExpression.numberOfChildren() == 0;

  /* If the inserted layout is of the form "function()", we want to make
   * the right parenthesis temporary to insert "function(".
   * This is to make the "(" key consistent with the functions keys like cos
   * and the functions inserted from the toolbox and the varbox.
   *
   * This is not done if forceCursorRightOfText, namely because the copy-pasted
   * layouts should not be altered, and more generally because it means that
   * the cursor won't go inside the parenthesis but rather right of it, so
   * the parenthesis should be kept pemanent on the right.
   *
   * This is done here and not before calling "insertText" for
   * multiple reasons:
   *   - In layout_events.cpp, we do not want to change the text of the Cos
   *     event since it should still output "cos()" in 1D fields.
   *   - Modifying the text inserted by the toolbox (by removing the right
   *     parenthesis) before calling this function could make the text
   *     unparsable.
   *   - It ensures that this behaviour is consistent across every ways of
   *     inserting functions.
   * If this is too generic though (for example if the inserted layout is
   * "3+cos(4)", we probably do not want to make the parenthesis temporary,
   * but this case never occurs for now), the resultExpression could
   * be analyzed to know if the parenthesis should be made temporary or not.
   * */
  if (!forceCursorRightOfText) {
    PoincareJ::AppHelpers::MakeRightMostParenthesisTemporary(
        static_cast<JuniorLayout &>(resultLayout).tree());
  }

  insertLayoutAtCursor(resultLayout, forceCursorRightOfText,
                       forceCursorLeftOfText);
  return true;
}

void LayoutField::didBecomeFirstResponder() {
  m_inputViewMemoizedHeight = inputViewHeight();
  TextCursorView::WithBlinkingCursor<
      ScrollableView<ScrollView::NoDecorator>>::didBecomeFirstResponder();
  scrollToCursor();
}

KDSize LayoutField::minimalSizeForOptimalDisplay() const {
  return KDSize(ScrollView::minimalSizeForOptimalDisplay().width(),
                inputViewHeight());
}

const char *LayoutField::text() {
  layout().serializeForParsing(m_draftBuffer, m_draftBufferSize);
  return m_draftBuffer;
}

void LayoutField::setText(const char *text) {
  clearLayout();
  insertText(text, false, true);
}

bool LayoutField::inputViewHeightDidChange() {
  KDCoordinate newHeight = inputViewHeight();
  bool didChange = m_inputViewMemoizedHeight != newHeight;
  m_inputViewMemoizedHeight = newHeight;
  return didChange;
}

void LayoutField::reload() {
  if (!linearMode()) {
    // Currently used only for its baseline effect, useless in linearMode
    reload(KDSizeZero);
  }
}

void LayoutField::restoreContent(const char *buffer, size_t size,
                                 int *cursorOffset, int *position) {
  if (size == 0) {
    return;
  }
#if 0
  setLayout(OLayout::LayoutFromAddress(buffer, size));
  if (*cursorOffset != -1) {
    const LayoutNode *cursorNode = reinterpret_cast<const LayoutNode *>(
        reinterpret_cast<char *>(layout().node()) + *cursorOffset);
    LayoutCursor restoredCursor = LayoutCursor(OLayout(cursorNode));
    restoredCursor.safeSetPosition(*position);
    *cursor() = restoredCursor;
  }
#endif
}

void LayoutField::setTextEditionBuffer(char *buffer, size_t bufferSize) {
  m_draftBuffer = buffer;
  m_draftBufferSize = bufferSize;
}

KDCoordinate LayoutField::inputViewHeight() const {
  return std::max(k_minimalHeight,
                  ScrollView::minimalSizeForOptimalDisplay().height());
}

bool LayoutField::handleEventWithText(const char *text, bool indentation,
                                      bool forceCursorRightOfText) {
  KDSize previousSize = minimalSizeForOptimalDisplay();
  bool didHandle = insertText(text, indentation, forceCursorRightOfText);
  return didHandleEvent(didHandle, didHandle, true, previousSize);
}

bool LayoutField::handleEvent(Ion::Events::Event event) {
  if (m_delegate) {
    m_delegate->updateRepetitionIndexes(this, event);
  }
  KDSize previousSize = minimalSizeForOptimalDisplay();
  bool layoutDidChange;
  bool shouldUpdateCursor;
  bool didHandle =
      privateHandleEvent(event, &layoutDidChange, &shouldUpdateCursor);
  return didHandleEvent(didHandle, layoutDidChange, shouldUpdateCursor,
                        previousSize);
}

bool LayoutField::privateHandleEvent(Ion::Events::Event event,
                                     bool *layoutDidChange,
                                     bool *shouldUpdateCursor) {
  *layoutDidChange = false;
  *shouldUpdateCursor = true;

  // Handle move and selection
  if (handleMoveEvent(event, layoutDidChange)) {
    prepareToEdit();
    return true;
  }

  // Notify delegate
  if (m_delegate) {
    if (m_delegate->layoutFieldDidReceiveEvent(this, event)) {
      return true;
    }
    if (isEditing() &&
        m_delegate->layoutFieldShouldFinishEditing(this, event)) {
      setEditing(false);
#if TODO_PCJ
      cursor()->beautifyLeft(context());
#endif
      if (!m_delegate->layoutFieldDidFinishEditing(this, event)) {
        prepareToEdit();
      }
      return true;
    }
  }

  // Handle events that have text
  constexpr size_t bufferSize = Ion::Events::EventData::k_maxDataSize;
  char buffer[bufferSize] = {0};
  if (getTextFromEvent(event, buffer, bufferSize) > 0) {
    prepareToEdit();
    bool didHandleEvent = insertText(buffer);
    *layoutDidChange = didHandleEvent;
    return didHandleEvent;
  }

  /* If move event was not caught neither by handleMoveEvent nor by
   * layoutFieldShouldFinishEditing, we handle it here to avoid bubbling the
   * event up. */
  if (event.isMoveEvent() && isEditing()) {
    return true;
  }

  // Handle sto
  if (event == Ion::Events::Sto && isEditing()) {
    handleStoreEvent();
    *shouldUpdateCursor = false;
    return true;
  }

  // Handle boxes
  if (privateHandleBoxEvent(event)) {
    prepareToEdit();
    *shouldUpdateCursor = false;
    return true;
  }

  // Handle copy, cut
  if ((event == Ion::Events::Copy || event == Ion::Events::Cut) &&
      isEditing()) {
    m_contentView.copySelection(context(), false);
    if (event == Ion::Events::Cut && !cursor()->selection().isEmpty()) {
      cursor()->performBackspace();
      *layoutDidChange = true;
    }
    return true;
  }

  // Handle paste
  if (event == Ion::Events::Paste) {
    prepareToEdit();
    bool didHandleEvent =
        insertText(Clipboard::SharedClipboard()->storedText(), false, true);
    *layoutDidChange = didHandleEvent;
    return didHandleEvent;
  }

  // Enter edition
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !isEditing()) {
    prepareToEdit();
    return true;
  }

  // Handle back
  if (event == Ion::Events::Back && isEditing()) {
    clearAndSetEditing(false);
    if (m_delegate) {
      m_delegate->layoutFieldDidAbortEditing(this);
    }
    return true;
  }

  // Handle backspace
  if (event == Ion::Events::Backspace) {
    prepareToEdit();
    cursor()->performBackspace();
    *layoutDidChange = true;
    return true;
  }

  // Handle clear
  if (event == Ion::Events::Clear && isEditing()) {
    clearLayout();
    *layoutDidChange = true;
    return true;
  }

  *shouldUpdateCursor = false;
  return false;
}

size_t LayoutField::getTextFromEvent(Ion::Events::Event event, char *buffer,
                                     size_t bufferSize) {
  if (event == Ion::Events::Log &&
      Poincare::Preferences::SharedPreferences()->logarithmKeyEvent() ==
          Poincare::Preferences::LogarithmKeyEvent::WithBaseTen) {
    constexpr const char *k_logWithBase10 = "log(\x11,10)";
    return strlcpy(buffer, k_logWithBase10, bufferSize);
  }
  if (event == Ion::Events::Sto && m_delegate &&
      m_delegate->shouldInsertTextForStoEvent(this)) {
    return strlcpy(buffer, "→", bufferSize);
  }
  if (event == Ion::Events::Ans && m_delegate &&
      m_delegate->shouldInsertTextForAnsEvent(this)) {
    return strlcpy(buffer, Symbol::k_ansAliases.mainAlias(), bufferSize);
  }
  if (event == Ion::Events::DoubleQuotes && m_delegate &&
      m_delegate->shouldInsertSingleQuoteInsteadOfDoubleQuotes(this)) {
    return SerializationHelper::CodePoint(buffer, bufferSize, '\'');
  }
  return Ion::Events::copyText(static_cast<uint8_t>(event), buffer, bufferSize);
}

bool LayoutField::handleStoreEvent() {
  m_contentView.copySelection(context(), true);
  return true;
}

bool LayoutField::handleMoveEvent(Ion::Events::Event event,
                                  bool *layoutDidChange) {
  bool isMoveEvent = event.isMoveEvent();
  bool isSelectionEvent = event.isSelectionEvent();
  if (!isMoveEvent && !isSelectionEvent) {
    return false;
  }
  return cursor()->moveMultipleSteps(
      OMG::Direction(event), Ion::Events::longPressFactor(), isSelectionEvent,
      layoutDidChange, context());
}

bool LayoutField::didHandleEvent(bool didHandleEvent, bool layoutDidChange,
                                 bool shouldUpdateCursor, KDSize previousSize) {
  if (!layoutDidChange) {
    if (shouldUpdateCursor) {
      m_contentView.cursorPositionChanged();
      scrollToCursor();
    }
  } else {
    reload(previousSize);
  }
  if (didHandleEvent && m_delegate) {
    m_delegate->layoutFieldDidHandleEvent(this);
  }
  return didHandleEvent;
}

void LayoutField::scrollToBaselinedRect(KDRect rect, KDCoordinate baseline) {
  // Show the rect area around its baseline
  KDCoordinate underBaseline = rect.bottom() - baseline;
  KDCoordinate minAroundBaseline = std::min(baseline, underBaseline);
  assert(!SumOverflowsKDCoordinate(rect.y(), baseline));
  minAroundBaseline =
      std::min<KDCoordinate>(minAroundBaseline, bounds().height() / 2);
  KDRect balancedRect(rect.x(), rect.y() + baseline - minAroundBaseline,
                      rect.width(), 2 * minAroundBaseline);
  scrollToContentRect(balancedRect);
}

void LayoutField::insertLayoutAtCursor(JuniorLayout layout,
                                       bool forceCursorRightOfLayout,
                                       bool forceCursorLeftOfLayout) {
  if (layout.isUninitialized()) {
    return;
  }
  layout = layout.makeEditable();
  KDSize previousSize = minimalSizeForOptimalDisplay();
  cursor()->insertLayout(layout.tree(), context(), forceCursorRightOfLayout,
                         forceCursorLeftOfLayout);

  // Reload
  reload(previousSize);
  scrollToCursor();
}

}  // namespace Escher
