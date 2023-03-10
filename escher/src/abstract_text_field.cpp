#include <assert.h>
#include <escher/abstract_text_field.h>
#include <escher/clipboard.h>
#include <escher/container.h>
#include <escher/text_input_helpers.h>
#include <ion/events.h>
#include <ion/keyboard/layout_events.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <poincare/aliases_list.h>
#include <poincare/parametered_expression.h>
#include <poincare/serialization_helper.h>
#include <poincare/symbol_abstract.h>
#include <poincare/xnt_helpers.h>

#include <algorithm>

namespace Escher {
static char s_draftTextBuffer[AbstractTextField::MaxBufferSize()];
static size_t s_currentDraftTextLength;

/* AbstractTextField::ContentView */

AbstractTextField::ContentView::ContentView(
    char *textBuffer, size_t textBufferSize, KDFont::Size font,
    float horizontalAlignment, float verticalAlignment, KDColor textColor,
    KDColor backgroundColor)
    : TextInput::ContentView(font, horizontalAlignment, verticalAlignment),
      m_textBuffer(textBuffer),
      m_textBufferSize(textBufferSize),
      m_textColor(textColor),
      m_backgroundColor(backgroundColor),
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
  m_backgroundColor = backgroundColor;
  markRectAsDirty(bounds());
}

void AbstractTextField::ContentView::setTextColor(KDColor textColor) {
  m_textColor = textColor;
  markRectAsDirty(bounds());
}

void AbstractTextField::ContentView::drawRect(KDContext *ctx,
                                              KDRect rect) const {
  KDColor backgroundColor = m_backgroundColor;
  if (m_isEditing) {
    backgroundColor = KDColorWhite;
  }
  ctx->fillRect(bounds(), backgroundColor);
  KDGlyph::Style glyphStyle{.glyphColor = m_textColor,
                            .backgroundColor = backgroundColor,
                            .font = m_font};
  if (selectionIsEmpty()) {
    ctx->drawString(text(), glyphFrameAtPosition(text(), text()).origin(),
                    glyphStyle);
  } else {
    assert(m_isEditing);
    int selectionOffset = selectionLeft() - editedText();
    const char *textToDraw = text();
    // Draw the non selected text on the left of the selection
    ctx->drawString(textToDraw, glyphFrameAtPosition(text(), text()).origin(),
                    glyphStyle, selectionOffset);
    int selectionLength = selectionRight() - selectionLeft();
    textToDraw += selectionOffset;
    // Draw the selected text
    ctx->drawString(text() + selectionOffset,
                    glyphFrameAtPosition(text(), textToDraw).origin(),
                    KDGlyph::Style{.glyphColor = m_textColor,
                                   .backgroundColor = Palette::Select,
                                   .font = m_font},
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

const char *AbstractTextField::ContentView::editedText() const {
  return s_draftTextBuffer;
}

size_t AbstractTextField::ContentView::editedTextLength() const {
  return s_currentDraftTextLength;
}

void AbstractTextField::ContentView::setText(const char *text) {
  size_t textRealLength = strlen(text);
  size_t maxBufferSize = m_isEditing ? draftTextBufferSize() : m_textBufferSize;
  char *buffer = const_cast<char *>(this->text());
  if (textRealLength > maxBufferSize - 1) {
    // The text was too long to be copied
    // TODO Maybe add a warning for the user?
    buffer[0] = 0;
    return;
  }
  int textLength = std::min(textRealLength, maxBufferSize - 1);
  // Copy the text
  strlcpy(buffer, text, maxBufferSize);
  // Replace System parentheses (used to keep layout tree structure) by normal
  // parentheses
  Poincare::SerializationHelper::
      ReplaceSystemParenthesesAndBracesByUserParentheses(buffer);
  // Update the draft text length
  if (buffer == s_draftTextBuffer) {
    s_currentDraftTextLength = textLength;
  }
  markRectAsDirty(bounds());
}

void AbstractTextField::ContentView::setEditing(bool isEditing) {
  if (m_isEditing == isEditing) {
    return;
  }
  resetSelection();
  m_isEditing = isEditing;
  const char *buffer = editedText();
  size_t textLength = strlen(buffer);
  if (buffer == const_cast<const char *>(s_draftTextBuffer)) {
    s_currentDraftTextLength = textLength;
  }
  if (m_cursorLocation < buffer || m_cursorLocation > buffer + textLength) {
    m_cursorLocation = buffer + textLength;
  }
  markRectAsDirty(bounds());
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
  s_currentDraftTextLength = 0;
  setCursorLocation(editedText());
}

bool AbstractTextField::ContentView::insertTextAtLocation(const char *text,
                                                          char *location,
                                                          int textLen) {
  assert(m_isEditing);

  char *buffer = const_cast<char *>(editedText());
  size_t editedLength = editedTextLength();

  size_t textLength = textLen < 0 ? strlen(text) : (size_t)textLen;
  // TODO when paste fails because of a too big message, create a pop-up
  if (editedLength + textLength >= draftTextBufferSize() || textLength == 0) {
    return false;
  }

  memmove(location + textLength, location,
          (buffer + editedLength + 1) - location);

  // Caution! One byte will be overridden by the null-terminating char of
  // strlcpy
  size_t copySize = std::min(
      textLength + 1,
      static_cast<size_t>((buffer + draftTextBufferSize()) - location));
  char *overridenByteLocation = location + copySize - 1;
  char overridenByte = *overridenByteLocation;
  strlcpy(location, text, copySize);
  *overridenByteLocation = overridenByte;
  if (buffer == s_draftTextBuffer) {
    s_currentDraftTextLength +=
        copySize - 1;  // Do no count the null-termination
  }

  reloadRectFromPosition(m_horizontalAlignment == 0.0f ? location : buffer);
  return true;
}

KDSize AbstractTextField::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize stringSize = KDFont::Font(m_font)->stringSize(text());
  assert(stringSize.height() == KDFont::GlyphHeight(m_font));
  if (m_isEditing) {
    return KDSize(stringSize.width() +
                      m_cursorView.minimalSizeForOptimalDisplay().width(),
                  stringSize.height());
  }
  return stringSize;
}

bool AbstractTextField::ContentView::removePreviousGlyph() {
  assert(m_isEditing);

  char *buffer = const_cast<char *>(editedText());

  if (m_horizontalAlignment > 0.0f) {
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

  if (buffer == s_draftTextBuffer) {
    // Update the draft buffer length
    s_currentDraftTextLength -= removedLength;
    assert(s_draftTextBuffer[s_currentDraftTextLength] == 0);
  }

  // Set the cursor location and reload the view
  assert(cursorLocation() - removedLength >= buffer);
  setCursorLocation(cursorLocation() - removedLength);
  if (m_horizontalAlignment == 0.0f) {
    reloadRectFromPosition(cursorLocation());
  }
  layoutSubviews();
  return true;
}

bool AbstractTextField::ContentView::removeEndOfLine() {
  assert(m_isEditing);
  char *buffer = const_cast<char *>(editedText());
  size_t lengthToCursor = (size_t)(cursorLocation() - buffer);
  if (editedTextLength() == lengthToCursor) {
    return false;
  }
  reloadRectFromPosition(m_horizontalAlignment == 0.0f ? cursorLocation()
                                                       : buffer);
  if (buffer == s_draftTextBuffer) {
    s_currentDraftTextLength = lengthToCursor;
  }
  *(const_cast<char *>(cursorLocation())) = 0;
  layoutSubviews();
  return true;
}

void AbstractTextField::ContentView::willModifyTextBuffer() {
  assert(m_isEditing);
  /* This method should be called when the buffer is modified outside the
   * content view, for instance from the textfield directly. */
  reloadRectFromPosition(editedText());
}

void AbstractTextField::ContentView::didModifyTextBuffer() {
  /* This method should be called when the buffer is modified outside the
   * content view, for instance from the textfield directly. */
  s_currentDraftTextLength = strlen(s_draftTextBuffer);
  layoutSubviews();
}

size_t AbstractTextField::dumpContent(char *buffer, size_t bufferSize,
                                      int *cursorOffset) {
  size_t size = draftTextLength() + 1;
  if (size > bufferSize) {
    buffer[0] = 0;
    *cursorOffset = -1;
  } else {
    memcpy(buffer, draftTextBuffer(), size);
    *cursorOffset = cursorLocation() - draftTextBuffer();
  }
  return 0;
}

size_t AbstractTextField::ContentView::deleteSelection() {
  assert(!selectionIsEmpty());
  assert(m_isEditing);
  size_t removedLength = selectionRight() - selectionLeft();
  char *buffer = const_cast<char *>(editedText());
  strlcpy(const_cast<char *>(selectionLeft()), selectionRight(),
          draftTextBufferSize() - (selectionLeft() - buffer));
  // We cannot call resetSelection() because m_selectionEnd is invalid.
  m_selectionStart = nullptr;
  if (buffer == s_draftTextBuffer) {
    assert(removedLength <= s_currentDraftTextLength);
    s_currentDraftTextLength -= removedLength;
  }
  return removedLength;
}

void AbstractTextField::ContentView::layoutSubviews(bool force) {
  if (!m_isEditing) {
    setChildFrame(&m_cursorView, KDRectZero, force);
    return;
  }
  TextInput::ContentView::layoutSubviews(force);
}

KDRect AbstractTextField::ContentView::glyphFrameAtPosition(
    const char *buffer, const char *position) const {
  assert(buffer != nullptr && position != nullptr);
  assert(position >= buffer);
  KDSize glyphSize = KDFont::GlyphSize(m_font);
  KDCoordinate cursorWidth =
      m_cursorView.minimalSizeForOptimalDisplay().width();
  KDCoordinate horizontalOffset =
      m_horizontalAlignment == 0.0f
          ? 0.0f
          : m_horizontalAlignment *
                (bounds().width() -
                 KDFont::Font(m_font)->stringSize(buffer).width() -
                 cursorWidth);
  return KDRect(
      horizontalOffset +
          KDFont::Font(m_font)->stringSizeUntil(buffer, position).width(),
      m_verticalAlignment * (bounds().height() - glyphSize.height()),
      glyphSize);
}

/* AbstractTextField */

AbstractTextField::AbstractTextField(
    Responder *parentResponder, View *contentView,
    InputEventHandlerDelegate *inputEventHandlerDelegate,
    TextFieldDelegate *delegate)
    : TextInput(parentResponder, contentView),
      EditableField(inputEventHandlerDelegate),
      m_delegate(delegate) {}

void AbstractTextField::setBackgroundColor(KDColor backgroundColor) {
  ScrollView::setBackgroundColor(backgroundColor);
  contentView()->setBackgroundColor(backgroundColor);
}

void AbstractTextField::setTextColor(KDColor textColor) {
  contentView()->setTextColor(textColor);
}

void AbstractTextField::setDelegates(
    InputEventHandlerDelegate *inputEventHandlerDelegate,
    TextFieldDelegate *delegate) {
  m_inputEventHandlerDelegate = inputEventHandlerDelegate;
  m_delegate = delegate;
}

void AbstractTextField::setInputEventHandlerDelegate(
    InputEventHandlerDelegate *inputEventHandlerDelegate) {
  m_inputEventHandlerDelegate = inputEventHandlerDelegate;
}

bool AbstractTextField::isEditing() const {
  return nonEditableContentView()->isEditing();
}

size_t AbstractTextField::draftTextLength() const {
  assert(isEditing());
  return nonEditableContentView()->editedTextLength();
}

void AbstractTextField::setText(const char *text) {
  reloadScroll();
  contentView()->setText(text);
  if (contentView()->text() == contentView()->editedText()) {
    /* Set the cursor location here and not in ContentView::setText so that
     * TextInput::willSetCursorLocation is called. */
    setCursorLocation(contentView()->editedText() + strlen(text));
  }
}

bool AbstractTextField::privateHandleEventWhileEditing(
    Ion::Events::Event event) {
  assert(isEditing());
  if (shouldFinishEditing(event)) {
    /* If textFieldDidFinishEditing displays a pop-up (because of an unvalid
     * text for instance), the text field will call willResignFirstResponder.
     * This will call textFieldDidAbortEditing if the textfield is still
     * editing, which we do not want, as we are not really aborting edition,
     * just displaying a pop-up before returning to edition. We thus set editing
     * to false. */
    setEditing(false);
    /* We avoid copying the edited text into the other text buffer in case
     * textFieldDidFinishEditing fails - we then want to be able to abort
     * edition and find the old text back. Anyway, if textFieldDidFinishEditing
     * does not fail, it will save the editedText in a model and reload the
     * content of the textfield buffer using the very same model - that has
     * been updated. */
    if (m_delegate->textFieldDidFinishEditing(this, contentView()->editedText(),
                                              event)) {
      // Clean draft text for next use
      reinitDraftTextBuffer();
      resetSelection();
      reloadScroll();
      return true;
    }
    setEditing(true);
    return true;
  }

  /* If a move event was not caught before, we handle it here to avoid bubbling
   * the event up. */
  if (event == Ion::Events::Up || event == Ion::Events::Down ||
      event == Ion::Events::Left || event == Ion::Events::Right) {
    return true;
  }

  if (event == Ion::Events::Backspace) {
    if (contentView()->selectionIsEmpty()) {
      return removePreviousGlyph();
    }
    deleteSelection();
    return true;
  }

  if (event == Ion::Events::Back) {
    reinitDraftTextBuffer();
    resetSelection();
    setEditing(false);
    m_delegate->textFieldDidAbortEditing(this);
    reloadScroll();
    return true;
  }

  if (event == Ion::Events::Clear) {
    if (!contentView()->selectionIsEmpty()) {
      deleteSelection();
    } else if (!removeEndOfLine()) {
      removeWholeText();
    }
    return true;
  }

  return false;
}

void AbstractTextField::removePreviousGlyphIfRepetition(
    bool defaultXNTHasChanged) {
  if (!defaultXNTHasChanged && Ion::Events::repetitionFactor() > 0 &&
      isEditing() && contentView()->selectionIsEmpty()) {
    // Since XNT is cycling on simple glyphs, remove the last inserted one
    bool success = removePreviousGlyph();
    assert(success);
    (void)success;  // Silence compilation warnings
    /* TODO: Handle cycling with non-default layouts and Fix issues with
     * repetition over a syntax error dismissal */
  }
}

// TODO : Handle cycling with non-default layouts.
size_t AbstractTextField::insertXNTChars(CodePoint defaultXNTCodePoint,
                                         char *buffer, size_t bufferLength) {
  if (!isEditing()) {
    reinitDraftTextBuffer();
    setEditing(true);
    m_delegate->textFieldDidStartEditing(this);
  }
  assert(text() == contentView()->editedText());
  UTF8Decoder decoder(text(), cursorLocation());
  bool defaultXNTHasChanged = false;
  if (Poincare::FindXNTSymbol(decoder, &defaultXNTHasChanged,
                              &defaultXNTCodePoint)) {
    const char *parameterText;
    size_t parameterLength = bufferLength + 1;
    decoder.unsafeSetPosition(reinterpret_cast<size_t>(cursorLocation()));
    if (Poincare::ParameteredExpression::ParameterText(
            decoder.stringPosition(), &parameterText, &parameterLength) &&
        bufferLength >= parameterLength) {
      memcpy(buffer, parameterText, parameterLength);
      return parameterLength;
    }
  }
  removePreviousGlyphIfRepetition(defaultXNTHasChanged);
  return UTF8Decoder::CodePointToChars(defaultXNTCodePoint, buffer,
                                       bufferLength);
}

bool AbstractTextField::addXNTCodePoint(CodePoint xnt) {
  constexpr int bufferSize = Poincare::SymbolAbstractNode::k_maxNameSize;
  char buffer[bufferSize];
  size_t length = insertXNTChars(xnt, buffer, bufferSize - 1);

  assert(length < bufferSize);
  buffer[length] = 0;
  return handleEventWithText(buffer, false, true);
}

void AbstractTextField::willResignFirstResponder() {
  contentView()->stallOrStopEditing();
  TextInput::willResignFirstResponder();
}

bool AbstractTextField::handleEvent(Ion::Events::Event event) {
  assert(m_delegate != nullptr);
  assert(!contentView()->isStalled());
  size_t previousTextLength = strlen(text());
  bool fieldIsEditable = m_delegate->textFieldIsEditable(this);
  bool didHandleEvent = false;

  // Handle move and selection
  if (privateHandleMoveEvent(event) || privateHandleSelectEvent(event)) {
    didHandleEvent = true;
    goto notify_delegate_after_handle_event;
  }

  // Notify delegate
  if (m_delegate->textFieldDidReceiveEvent(this, event)) {
    return true;
  }

  // Handle copy, cut and paste
  if (event == Ion::Events::Copy ||
      (event == Ion::Events::Cut && fieldIsEditable)) {
    if (storeInClipboard() && event == Ion::Events::Cut) {
      if (!contentView()->selectionIsEmpty()) {
        deleteSelection();
      } else {
        removeWholeText();
      }
    }
    didHandleEvent = true;
    goto notify_delegate_after_handle_event;
  }

  if (fieldIsEditable && event == Ion::Events::Paste) {
    return handleEventWithText(Clipboard::SharedClipboard()->storedText(),
                               false, true);
  }

  if (fieldIsEditable &&
      (event == Ion::Events::OK || event == Ion::Events::EXE) && !isEditing()) {
    const char *previousText = contentView()->text();
    setEditing(true);
    m_delegate->textFieldDidStartEditing(this);
    setText(previousText);
    didHandleEvent = true;
    goto notify_delegate_after_handle_event;
  }

  if ((event == Ion::Events::Sto && handleStoreEvent()) ||
      (fieldIsEditable &&
       ((isEditing() && privateHandleEventWhileEditing(event)) ||
        handleBoxEvent(event)))) {
    didHandleEvent = true;
    goto notify_delegate_after_handle_event;
  }

  if (fieldIsEditable) {
    char buffer[Ion::Events::EventData::k_maxDataSize] = {0};
    size_t eventTextLength =
        Ion::Events::copyText(static_cast<uint8_t>(event), buffer,
                              Ion::Events::EventData::k_maxDataSize);
    if (eventTextLength > 0) {
      return handleEventWithText(buffer);
    }
  }

notify_delegate_after_handle_event:
  /* Only the strlen are compared because this method does not reaches this
   * point if the text was modified, only if it was not altered or if
   * text was deleted. */
  return m_delegate->textFieldDidHandleEvent(
      this, didHandleEvent, strlen(text()) != previousTextLength);
}

void AbstractTextField::scrollToCursor() {
  if (!isEditing()) {
    return;
  }
  return TextInput::scrollToCursor();
}

bool AbstractTextField::shouldFinishEditing(Ion::Events::Event event) {
  if (m_delegate->textFieldShouldFinishEditing(this, event)) {
    resetSelection();
    return true;
  }
  return false;
}

bool AbstractTextField::privateHandleMoveEvent(Ion::Events::Event event) {
  if (!isEditing()) {
    return false;
  }
  const char *draftBuffer = contentView()->editedText();
  if (event == Ion::Events::Left || event == Ion::Events::Right) {
    if (!contentView()->selectionIsEmpty()) {
      resetSelection();
      return true;
    }
    if (event == Ion::Events::Left && cursorLocation() > draftBuffer) {
      return TextInput::moveCursorLeft();
    }
    if (event == Ion::Events::Right &&
        cursorLocation() < draftBuffer + draftTextLength()) {
      return TextInput::moveCursorRight();
    }
  }
  return false;
}

bool AbstractTextField::privateHandleSelectEvent(Ion::Events::Event event) {
  if (!isEditing()) {
    return false;
  }
  if (event == Ion::Events::ShiftLeft || event == Ion::Events::ShiftRight ||
      event == Ion::Events::ShiftUp || event == Ion::Events::ShiftDown) {
    selectLeftRight(
        (event == Ion::Events::ShiftLeft || event == Ion::Events::ShiftUp)
            ? OMG::Direction::Left()
            : OMG::Direction::Right(),
        event == Ion::Events::ShiftUp || event == Ion::Events::ShiftDown);
    return true;
  }
  return false;
}

bool AbstractTextField::handleEventWithText(const char *eventText,
                                            bool indentation,
                                            bool forceCursorRightOfText) {
  if (!isEditing()) {
    reinitDraftTextBuffer();
    setEditing(true);
    m_delegate->textFieldDidStartEditing(this);
  }

  assert(isEditing());

  // Delete the selected text if needed
  if (!contentView()->selectionIsEmpty()) {
    deleteSelection();
  }

  if (eventText[0] != 0) {
    // Remove the Empty code points
    constexpr int bufferSize = AbstractTextField::MaxBufferSize();
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
    // Replace System parentheses (used to keep layout tree structure) by normal
    // parentheses
    Poincare::SerializationHelper::
        ReplaceSystemParenthesesAndBracesByUserParentheses(buffer);

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
  return m_delegate->textFieldDidHandleEvent(this, true, true);
}

void AbstractTextField::removeWholeText() {
  reinitDraftTextBuffer();
  resetSelection();
  markRectAsDirty(bounds());
  layoutSubviews();
  reloadScroll();
}

bool AbstractTextField::storeInClipboard() const {
  if (!isEditing()) {
    Clipboard::SharedClipboard()->store(text());
    return true;
  } else if (!nonEditableContentView()->selectionIsEmpty()) {
    const char *start = nonEditableContentView()->selectionLeft();
    Clipboard::SharedClipboard()->store(
        start, nonEditableContentView()->selectionRight() - start);
    return true;
  }
  return false;
}

bool AbstractTextField::handleStoreEvent() {
  if (!isEditing() && m_delegate->textFieldIsStorable(this)) {
    Container::activeApp()->storeValue(text());
  } else if (isEditing() && !nonEditableContentView()->selectionIsEmpty()) {
    const char *start = nonEditableContentView()->selectionLeft();
    static_assert(TextField::MaxBufferSize() ==
                  Escher::Clipboard::k_bufferSize);
    char buffer[Escher::Clipboard::k_bufferSize];
    strlcpy(buffer, start,
            nonEditableContentView()->selectionRight() - start + 1);
    Container::activeApp()->storeValue(buffer);
  } else {
    Container::activeApp()->storeValue();
  }
  return true;
}

}  // namespace Escher
