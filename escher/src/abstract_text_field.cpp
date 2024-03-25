#include <assert.h>
#include <escher/abstract_text_field.h>
#include <escher/clipboard.h>
#include <escher/container.h>
#include <escher/text_input_helpers.h>
#include <ion/events.h>
#include <ion/keyboard/layout_events.h>
#include <ion/unicode/utf8_helper.h>
#include <poincare/aliases_list.h>
#include <poincare/parametered_expression.h>
#include <poincare/serialization_helper.h>
#include <poincare/symbol_abstract.h>
#include <poincare_junior/src/layout/xnt.h>

#include <algorithm>

using namespace Poincare;

namespace Escher {

/* Draft text used when editing, in order to save previous text
 * in case edition fails or abort.*/
static char s_draftTextBuffer[AbstractTextField::MaxBufferSize()];

/* AbstractTextField::ContentView */

AbstractTextField::ContentView::ContentView(char *textBuffer,
                                            size_t textBufferSize,
                                            KDGlyph::Format format)
    : TextInput::ContentView(format),
      m_textBuffer(textBuffer),
      m_textBufferSize(textBufferSize),
      m_isEditing(false),
      m_isStalled(false) {
  if (textBuffer == nullptr) {
    m_textBuffer = s_draftTextBuffer;
  }
  assert(m_textBufferSize <= k_maxBufferSize);
  reinitDraftTextBuffer();
}

void AbstractTextField::ContentView::setBackgroundColor(
    KDColor backgroundColor) {
  m_format.style.backgroundColor = backgroundColor;
  markWholeFrameAsDirty();
}

void AbstractTextField::ContentView::setTextColor(KDColor textColor) {
  m_format.style.glyphColor = textColor;
  markWholeFrameAsDirty();
}

void AbstractTextField::ContentView::drawRect(KDContext *ctx,
                                              KDRect rect) const {
  KDColor backgroundColor = m_format.style.backgroundColor;
  if (m_isEditing) {
    backgroundColor = KDColorWhite;
  }
  ctx->fillRect(bounds(), backgroundColor);
  KDGlyph::Style glyphStyle{.glyphColor = m_format.style.glyphColor,
                            .backgroundColor = backgroundColor,
                            .font = m_format.style.font};
  if (selectionIsEmpty()) {
    ctx->drawString(text(), glyphFrameAtPosition(text(), text()).origin(),
                    glyphStyle);
  } else {
    assert(m_isEditing);
    int selectionOffset = selectionLeft() - draftText();
    const char *textToDraw = text();
    // Draw the non selected text on the left of the selection
    ctx->drawString(textToDraw, glyphFrameAtPosition(text(), text()).origin(),
                    glyphStyle, selectionOffset);
    int selectionLength = selectionRight() - selectionLeft();
    textToDraw += selectionOffset;
    // Draw the selected text
    ctx->drawString(text() + selectionOffset,
                    glyphFrameAtPosition(text(), textToDraw).origin(),
                    {.glyphColor = m_format.style.glyphColor,
                     .backgroundColor = Palette::Select,
                     .font = m_format.style.font},
                    selectionLength);
    textToDraw += selectionLength;
    // Draw the non selected text on the right of the selection
    ctx->drawString(text() + selectionOffset + selectionLength,
                    glyphFrameAtPosition(text(), textToDraw).origin(),
                    glyphStyle);
  }
}

const char *AbstractTextField::ContentView::text() const {
  return const_cast<const char *>(m_isEditing ? s_draftTextBuffer
                                              : m_textBuffer);
}

const char *AbstractTextField::ContentView::draftText() const {
  return s_draftTextBuffer;
}

void AbstractTextField::ContentView::setText(const char *text) {
  size_t textRealLength = strlen(text);
  size_t maxBufferSize = m_isEditing ? draftTextBufferSize() : m_textBufferSize;
  char *buffer = const_cast<char *>(this->text());
  if (textRealLength > maxBufferSize - 1) {
    /* The text was too long to be copied
     * TODO Maybe add a warning for the user? */
    buffer[0] = 0;
    return;
  }
  // Copy the text
  strlcpy(buffer, text, maxBufferSize);
  /* Replace System parentheses (used to keep layout tree structure) by normal
   * parentheses */
  SerializationHelper::ReplaceSystemParenthesesAndBracesByUserParentheses(
      buffer);
  markWholeFrameAsDirty();
}

void AbstractTextField::ContentView::setEditing(bool isEditing) {
  if (m_isEditing == isEditing) {
    return;
  }
  if (!isEditing) {
    m_isStalled = false;
  }
  resetSelection();
  m_isEditing = isEditing;
  const char *bufferStart = draftText();
  const char *bufferEnd = draftTextEnd();
  if (m_cursorLocation < bufferStart || m_cursorLocation > bufferEnd) {
    m_cursorLocation = bufferEnd;
  }
  markWholeFrameAsDirty();
  layoutSubviews();
}

void AbstractTextField::ContentView::stallOrStopEditing() {
  if (!m_isStalled) {
    setEditing(false);
  }
}

void AbstractTextField::ContentView::reinitDraftTextBuffer() {
  /* We first need to clear the buffer, otherwise setCursorLocation might do
   * various operations on a buffer with maybe non-initialized content, such as
   * stringSize, etc. Those operation might be perilous on non-UTF8 content. */
  s_draftTextBuffer[0] = 0;
  setCursorLocation(draftText());
}

bool AbstractTextField::ContentView::insertTextAtLocation(const char *text,
                                                          char *location,
                                                          int textLen) {
  assert(m_isEditing);

  char *buffer = const_cast<char *>(draftText());
  size_t editedLength = draftTextLength();

  size_t textLength = textLen < 0 ? strlen(text) : (size_t)textLen;
  // TODO when paste fails because of a too big message, create a pop-up
  if (editedLength + textLength >= draftTextBufferSize() || textLength == 0) {
    return false;
  }

  memmove(location + textLength, location,
          (buffer + editedLength + 1) - location);

  /* Caution: One byte will be overridden by the null-terminating char of
   * strlcpy */
  size_t copySize = std::min(
      textLength + 1,
      static_cast<size_t>((buffer + draftTextBufferSize()) - location));
  char *overridenByteLocation = location + copySize - 1;
  char overridenByte = *overridenByteLocation;
  strlcpy(location, text, copySize);
  *overridenByteLocation = overridenByte;

  reloadRectFromPosition(m_format.horizontalAlignment == 0.0f ? location
                                                              : buffer);
  return true;
}

KDSize AbstractTextField::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize stringSize = KDFont::Font(m_format.style.font)->stringSize(text());
  assert(stringSize.height() == KDFont::GlyphHeight(m_format.style.font));
  if (m_isEditing) {
    return KDSize(stringSize.width() + TextCursorView::k_width,
                  stringSize.height());
  }
  return stringSize;
}

bool AbstractTextField::ContentView::removePreviousGlyph() {
  assert(m_isEditing);

  char *buffer = const_cast<char *>(draftText());

  if (m_format.horizontalAlignment > 0.0f) {
    /* Reload the view. If we do it later, the text beins supposedly shorter, we
     * will not clean the first char. */
    reloadRectFromPosition(buffer);
  }
  // Remove the glyph if possible
  int removedLength = UTF8Helper::RemovePreviousGlyph(
      buffer, const_cast<char *>(cursorLocation()));
  if (removedLength == 0) {
    assert(cursorLocation() == buffer);
    return false;
  }

  // Set the cursor location and reload the view
  assert(cursorLocation() - removedLength >= buffer);
  setCursorLocation(cursorLocation() - removedLength);
  if (m_format.horizontalAlignment == 0.0f) {
    reloadRectFromPosition(cursorLocation());
  }
  layoutSubviews();
  return true;
}

bool AbstractTextField::ContentView::removeEndOfLine() {
  assert(m_isEditing);
  char *buffer = const_cast<char *>(draftText());
  size_t lengthToCursor = (size_t)(cursorLocation() - buffer);
  if (draftTextLength() == lengthToCursor) {
    return false;
  }
  reloadRectFromPosition(m_format.horizontalAlignment == 0.0f ? cursorLocation()
                                                              : buffer);
  *(const_cast<char *>(cursorLocation())) = 0;
  layoutSubviews();
  return true;
}

void AbstractTextField::ContentView::willModifyTextBuffer() {
  assert(m_isEditing);
  /* This method should be called when the buffer is modified outside the
   * content view, for instance from the textfield directly. */
  reloadRectFromPosition(draftText());
}

void AbstractTextField::ContentView::didModifyTextBuffer() {
  /* This method should be called when the buffer is modified outside the
   * content view, for instance from the textfield directly. */
  layoutSubviews();
}

size_t AbstractTextField::dumpContent(char *buffer, size_t bufferSize,
                                      int *cursorOffset) {
  size_t size = draftTextLength() + 1;
  if (size > bufferSize) {
    buffer[0] = 0;
    *cursorOffset = -1;
  } else {
    memcpy(buffer, draftText(), size);
    *cursorOffset = cursorLocation() - draftText();
  }
  return 0;
}

size_t AbstractTextField::ContentView::deleteSelection() {
  assert(!selectionIsEmpty());
  assert(m_isEditing);
  size_t removedLength = selectionRight() - selectionLeft();
  char *buffer = const_cast<char *>(draftText());
  strlcpy(const_cast<char *>(selectionLeft()), selectionRight(),
          draftTextBufferSize() - (selectionLeft() - buffer));
  // We cannot call resetSelection() because m_selectionEnd is invalid.
  m_selectionStart = nullptr;
  return removedLength;
}

KDRect AbstractTextField::ContentView::cursorRect() const {
  return m_isEditing ? TextInput::ContentView::cursorRect() : KDRectZero;
}

KDRect AbstractTextField::ContentView::glyphFrameAtPosition(
    const char *buffer, const char *position) const {
  assert(buffer != nullptr && position != nullptr);
  assert(position >= buffer);
  KDSize glyphSize = KDFont::GlyphSize(m_format.style.font);
  KDCoordinate cursorWidth = TextCursorView::k_width;
  KDCoordinate horizontalOffset =
      m_format.horizontalAlignment == 0.0f
          ? 0.0f
          : m_format.horizontalAlignment *
                (bounds().width() -
                 KDFont::Font(m_format.style.font)->stringSize(buffer).width() -
                 cursorWidth);
  return KDRect(
      horizontalOffset + KDFont::Font(m_format.style.font)
                             ->stringSizeUntil(buffer, position)
                             .width(),
      m_format.verticalAlignment * (bounds().height() - glyphSize.height()),
      glyphSize);
}

/* AbstractTextField */

AbstractTextField::AbstractTextField(Responder *parentResponder,
                                     View *contentView,
                                     TextFieldDelegate *delegate)
    : TextInput(parentResponder, contentView), m_delegate(delegate) {}

void AbstractTextField::setBackgroundColor(KDColor backgroundColor) {
  ScrollView::setBackgroundColor(backgroundColor);
  contentView()->setBackgroundColor(backgroundColor);
}

void AbstractTextField::setText(const char *text) {
  if (contentView()->isStalled()) {
    return;
  }
  resetScroll();
  contentView()->setText(text);
  layoutSubviews();
  if (this->text() == draftText()) {
    /* Set the cursor location here and not in ContentView::setText so that
     * TextInput::willSetCursorLocation is called. */
    setCursorLocation(draftText() + strlen(text));
  }
}

bool AbstractTextField::prepareToEdit() {
  if (!isEditable()) {
    return false;
  }
  if (!isEditing()) {
    reinitDraftTextBuffer();
    setEditing(true);
    if (m_delegate) {
      m_delegate->textFieldDidStartEditing(this);
    }
  }
  assert(text() == draftText());
  assert(isEditing());
  return true;
}

bool AbstractTextField::findXNT(char *buffer, size_t bufferSize, int xntIndex,
                                size_t *cycleSize) {
  UTF8Decoder decoder(text(), cursorLocation());
  return PoincareJ::FindXNTSymbol1D(decoder, buffer, bufferSize, xntIndex,
                                    cycleSize);
}

void AbstractTextField::removePreviousXNT() {
  assert(selectionIsEmpty());
  // Since XNT is cycling on simple glyphs, remove the last inserted one
  bool success = removePreviousGlyph();
  assert(success);
  (void)success;  // Silence compilation warnings
}

void AbstractTextField::willResignFirstResponder() {
  contentView()->stallOrStopEditing();
  TextInput::willResignFirstResponder();
}

bool AbstractTextField::handleEvent(Ion::Events::Event event) {
  assert(!contentView()->isStalled());
  bool textDidChange;
  if (m_delegate) {
    m_delegate->updateRepetitionIndexes(this, event);
  }
  bool didHandleEvent = privateHandleEvent(event, &textDidChange);
  if (didHandleEvent && textDidChange && m_delegate) {
    m_delegate->textFieldDidHandleEvent(this);
  }
  return didHandleEvent;
}

bool AbstractTextField::privateHandleEvent(Ion::Events::Event event,
                                           bool *textDidChange) {
  assert(!contentView()->isStalled());
  size_t previousTextLength = strlen(text());
  *textDidChange = false;

  // Handle move and selection
  if (handleMoveEvent(event) || handleSelectEvent(event)) {
    return true;
  }

  // Notify delegate
  if (m_delegate) {
    if (m_delegate->textFieldDidReceiveEvent(this, event)) {
      return true;
    }
    if (isEditing() && m_delegate->textFieldShouldFinishEditing(this, event)) {
      setEditing(false);
      if (m_delegate->textFieldDidFinishEditing(this, event)) {
        /* Text has been updated with draft text. We can clean the draft text
         * for next use. */
        reinitDraftTextBuffer();
        resetScroll();
      } else {
        setEditing(true);
      }
      *textDidChange = previousTextLength != strlen(text());
      return true;
    }
  }

  /* If move event was not caught neither by handleMoveEvent nor by
   * layoutFieldShouldFinishEditing, we handle it here to avoid bubbling the
   * event up. */
  if (event.isMoveEvent() && isEditing()) {
    return true;
  }

  // Handle sto
  if (event == Ion::Events::Sto && handleStoreEvent()) {
    return true;
  }

  // Handle copy
  if (event == Ion::Events::Copy) {
    storeInClipboard();
    return true;
  }

  if (!isEditable()) {
    return false;
  }
  // From now on, we only consider editable fields

  // Handle boxes
  if (privateHandleBoxEvent(event)) {
    return true;
  }

  // Handle cut
  if (event == Ion::Events::Cut) {
    if (storeInClipboard() && isEditing()) {
      assert(!selectionIsEmpty());
      deleteSelection();
      *textDidChange = true;
    }
    return true;
  }

  // Handle paste
  if (event == Ion::Events::Paste) {
    bool didHandleEvent =
        insertText(Clipboard::SharedClipboard()->storedText(), false, true);
    *textDidChange = didHandleEvent;
    return didHandleEvent;
  }

  // Enter edition
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !isEditing()) {
    const char *previousText = text();
    prepareToEdit();
    setText(previousText);
    *textDidChange = true;
    return true;
  }

  // Handle back
  if (event == Ion::Events::Back && isEditing()) {
    reinitDraftTextBuffer();
    setEditing(false);
    if (m_delegate) {
      m_delegate->textFieldDidAbortEditing(this);
    }
    resetScroll();
    *textDidChange = previousTextLength != strlen(text());
    return true;
  }

  // Handle backspace
  if (event == Ion::Events::Backspace && isEditing()) {
    if (selectionIsEmpty()) {
      return (*textDidChange = removePreviousGlyph());
    }
    deleteSelection();
    *textDidChange = true;
    return true;
  }

  // Handle clear
  if (event == Ion::Events::Clear && isEditing()) {
    if (!selectionIsEmpty()) {
      deleteSelection();
    } else if (!removeEndOfLine()) {
      removeWholeText();
    }
    *textDidChange = true;
    return true;
  }

  // Handle special events with text
  constexpr size_t bufferSize = Ion::Events::EventData::k_maxDataSize;
  char buffer[bufferSize] = {0};
  if (getTextFromEvent(event, buffer, bufferSize) > 0) {
    bool didHandleEvent = insertText(buffer);
    *textDidChange = didHandleEvent;
    return didHandleEvent;
  }

  return false;
}

size_t AbstractTextField::getTextFromEvent(Ion::Events::Event event,
                                           char *buffer, size_t bufferSize) {
  if (event == Ion::Events::DoubleQuotes && m_delegate &&
      m_delegate->shouldInsertSingleQuoteInsteadOfDoubleQuotes(this)) {
    return SerializationHelper::CodePoint(buffer, bufferSize, '\'');
  }
  return Ion::Events::copyText(static_cast<uint8_t>(event), buffer, bufferSize);
}

void AbstractTextField::scrollToCursor() {
  if (!isEditing()) {
    return;
  }
  return TextInput::scrollToCursor();
}

bool AbstractTextField::handleMoveEvent(Ion::Events::Event event) {
  if (!isEditing()) {
    return false;
  }
  if (event == Ion::Events::Left || event == Ion::Events::Right) {
    if (!selectionIsEmpty()) {
      resetSelection();
      return true;
    }
    if (event == Ion::Events::Left && cursorLocation() > draftText()) {
      return TextInput::moveCursorLeft();
    }
    if (event == Ion::Events::Right && cursorLocation() < draftTextEnd()) {
      return TextInput::moveCursorRight();
    }
  }
  return false;
}

bool AbstractTextField::handleSelectEvent(Ion::Events::Event event) {
  if (!isEditing()) {
    return false;
  }
  if (event.isSelectionEvent()) {
    selectLeftRight(
        (event == Ion::Events::ShiftLeft || event == Ion::Events::ShiftUp)
            ? OMG::Direction::Left()
            : OMG::Direction::Right(),
        event == Ion::Events::ShiftUp || event == Ion::Events::ShiftDown);
    return true;
  }
  return false;
}

bool AbstractTextField::insertText(const char *eventText, bool indentation,
                                   bool forceCursorRightOfText) {
  prepareToEdit();
  assert(isEditing());

  // Delete the selected text if needed
  if (!selectionIsEmpty()) {
    deleteSelection();
  }

  if (eventText[0] != 0) {
    // Remove the Empty code points
    constexpr size_t bufferSize = AbstractTextField::MaxBufferSize();
    char buffer[bufferSize];
    {
      CodePoint c[] = {UCodePointEmpty, '\n'};
      bool complete = UTF8Helper::CopyAndRemoveCodePoints(buffer, bufferSize,
                                                          eventText, c, 2);
      /* If the text is too long to be stored in buffer, we do not insert any
       * text. This behaviour is consistent with 'insertTextAtLocation'
       * behaviour. */
      if (!complete) {
        return false;
      }
    }
    /* Replace System parentheses (used to keep layout tree structure) by normal
     * parentheses */
    SerializationHelper::ReplaceSystemParenthesesAndBracesByUserParentheses(
        buffer);

    if (insertTextAtLocation(buffer, const_cast<char *>(cursorLocation()))) {
      /* The cursor position depends on the text as we sometimes want to
       * position the cursor at the end of the text and sometimes after the
       * first parenthesis. */
      const char *nextCursorLocation = cursorLocation();
      if (forceCursorRightOfText) {
        nextCursorLocation += strlen(buffer);
      } else {
        nextCursorLocation += TextInputHelpers::CursorIndexInCommand(eventText);
      }
      setCursorLocation(nextCursorLocation);
    }
  }
  return true;
}

bool AbstractTextField::handleEventWithText(const char *eventText,
                                            bool indentation,
                                            bool forceCursorRightOfText) {
  /* TODO: this method should not exist, we should only have
   * insertText and always call handleEvent. */
  if (insertText(eventText, indentation, forceCursorRightOfText)) {
    if (m_delegate) {
      m_delegate->textFieldDidHandleEvent(this);
    }
    return true;
  }
  return false;
}

void AbstractTextField::removeWholeText() {
  assert(isEditable());
  reinitDraftTextBuffer();
  resetSelection();
  markWholeFrameAsDirty();
  layoutSubviews();
  resetScroll();
}

bool AbstractTextField::storeInClipboard() const {
  if (!isEditing()) {
    Clipboard::SharedClipboard()->store(text());
    return true;
  } else if (!selectionIsEmpty()) {
    const char *start = nonEditableContentView()->selectionLeft();
    Clipboard::SharedClipboard()->store(
        start, nonEditableContentView()->selectionRight() - start);
    return true;
  }
  return false;
}

bool AbstractTextField::handleStoreEvent() {
  if (!isEditing() && m_delegate && m_delegate->textFieldIsStorable(this)) {
    App::app()->storeValue(text());
  } else if (isEditing() && !selectionIsEmpty()) {
    const char *start = nonEditableContentView()->selectionLeft();
    static_assert(TextField::MaxBufferSize() ==
                  Escher::Clipboard::k_bufferSize);
    char buffer[Escher::Clipboard::k_bufferSize];
    strlcpy(buffer, start,
            nonEditableContentView()->selectionRight() - start + 1);
    App::app()->storeValue(buffer);
  } else {
    App::app()->storeValue();
  }
  return true;
}

}  // namespace Escher
