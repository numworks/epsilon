#include <escher/text_field.h>
#include <escher/text_input_helpers.h>
#include <escher/clipboard.h>
#include <escher/container.h>
#include <ion/events.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <ion/keyboard/layout_events.h>
#include <poincare/aliases_list.h>
#include <poincare/serialization_helper.h>
#include <poincare/derivative.h>
#include <poincare/integral.h>
#include <poincare/sum.h>
#include <poincare/product.h>
#include <poincare/symbol_abstract.h>
#include <ion/events.h>
#include <assert.h>
#include <algorithm>

namespace Escher {
static char s_draftTextBuffer[AbstractTextField::MaxBufferSize()];
static size_t s_currentDraftTextLength;

/* AbstractTextField::ContentView */

AbstractTextField::ContentView::ContentView(char * textBuffer, size_t textBufferSize, size_t draftTextBufferSize, KDFont::Size font, float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor) :
  TextInput::ContentView(font, horizontalAlignment, verticalAlignment),
  m_textBuffer(textBuffer),
  m_textBufferSize(textBufferSize),
  m_draftTextBufferSize(draftTextBufferSize),
  m_textColor(textColor),
  m_backgroundColor(backgroundColor),
  m_isEditing(false),
  m_useDraftBuffer(true)
{
  if (textBuffer == nullptr) {
    m_textBuffer = s_draftTextBuffer;
  }
  assert(m_draftTextBufferSize <= k_maxBufferSize);
  assert(m_textBufferSize <= k_maxBufferSize);
  reinitDraftTextBuffer();
}

void AbstractTextField::ContentView::setBackgroundColor(KDColor backgroundColor) {
  m_backgroundColor = backgroundColor;
  markRectAsDirty(bounds());
}

void AbstractTextField::ContentView::setTextColor(KDColor textColor) {
  m_textColor = textColor;
  markRectAsDirty(bounds());
}

void AbstractTextField::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor backgroundColor = m_backgroundColor;
  if (m_isEditing) {
    backgroundColor = KDColorWhite;
  }
  ctx->fillRect(bounds(), backgroundColor);
  if (selectionIsEmpty()) {
    ctx->drawString(text(), glyphFrameAtPosition(text(), text()).origin(), m_font, m_textColor, backgroundColor);
  } else {
    assert(m_isEditing);
    int selectionOffset = m_selectionStart - editedText();
    const char * textToDraw = text();
    // Draw the non selected text on the left of the selection
    ctx->drawString(textToDraw, glyphFrameAtPosition(text(), text()).origin(), m_font, m_textColor, backgroundColor, selectionOffset);
    int selectionLength = m_selectionEnd - m_selectionStart;
    textToDraw += selectionOffset;
    // Draw the selected text
    ctx->drawString(text() + selectionOffset, glyphFrameAtPosition(text(), textToDraw).origin(), m_font, m_textColor, Palette::Select, selectionLength);
    textToDraw += selectionLength;
    // Draw the non selected text on the right of the selection
    ctx->drawString(text() + selectionOffset + selectionLength, glyphFrameAtPosition(text(), textToDraw).origin(), m_font, m_textColor, backgroundColor);
  }
}

const char * AbstractTextField::ContentView::text() const {
  return const_cast<const char *>(m_isEditing && m_useDraftBuffer ? s_draftTextBuffer : m_textBuffer);
}

const char * AbstractTextField::ContentView::editedText() const {
  assert(m_useDraftBuffer || (m_textBuffer != nullptr && m_textBuffer != s_draftTextBuffer));
  return m_useDraftBuffer ? s_draftTextBuffer : m_textBuffer;
}

size_t AbstractTextField::ContentView::editedTextLength() const {
  return m_useDraftBuffer ? s_currentDraftTextLength : strlen(m_textBuffer);
}

void AbstractTextField::ContentView::setText(const char * text) {
  size_t textRealLength = strlen(text);
  size_t maxBufferSize = editionBufferSize();
  char * buffer = const_cast<char *>(this->text());
  if (textRealLength > maxBufferSize - 1) {
    // The text was too long to be copied
    // TODO Maybe add a warning for the user?
    buffer[0] = 0;
    return;
  }
  int textLength = std::min(textRealLength, maxBufferSize - 1);
  // Copy the text
  strlcpy(buffer, text, maxBufferSize);
  // Replace System parentheses (used to keep layout tree structure) by normal parentheses
  Poincare::SerializationHelper::ReplaceSystemParenthesesAndBracesByUserParentheses(buffer);
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
  const char * buffer = editedText();
  size_t textLength = strlen(buffer);
  if (buffer == const_cast<const char *>(s_draftTextBuffer)) {
    s_currentDraftTextLength = textLength;
  }
  if (m_cursorLocation < buffer
      || m_cursorLocation > buffer + textLength)
  {
    m_cursorLocation = buffer + textLength;
  }
  markRectAsDirty(bounds());
  layoutSubviews();
}

void AbstractTextField::ContentView::reinitDraftTextBuffer() {
  /* We first need to clear the buffer, otherwise setCursorLocation might do
   * various operations on a buffer with maybe non-initialized content, such as
   * stringSize, etc. Those operation might be perilous on non-UTF8 content. */
  if (m_useDraftBuffer) {
    s_draftTextBuffer[0] = 0;
    s_currentDraftTextLength = 0;
  } else {
    m_textBuffer[0] = 0;
  }
  setCursorLocation(editedText());
}

bool AbstractTextField::ContentView::insertTextAtLocation(const char * text, char * location, int textLen) {
  assert(m_isEditing);

  char * buffer = const_cast<char *>(editedText());
  size_t editedLength = editedTextLength();

  size_t textLength = textLen < 0 ? strlen(text) : (size_t)textLen;
  // TODO when paste fails because of a too big message, create a pop-up
  if (editedLength + textLength >= editionBufferSize() || textLength == 0) {
    return false;
  }

  memmove(location + textLength, location, (buffer + editedLength + 1) - location);

  // Caution! One byte will be overridden by the null-terminating char of strlcpy
  size_t copySize = std::min(textLength + 1, static_cast<size_t>((buffer + editionBufferSize()) - location));
  char * overridenByteLocation = location + copySize - 1;
  char overridenByte = *overridenByteLocation;
  strlcpy(location, text, copySize);
  *overridenByteLocation = overridenByte;
  if (buffer == s_draftTextBuffer) {
    s_currentDraftTextLength += copySize-1; // Do no count the null-termination
  }

  reloadRectFromPosition(m_horizontalAlignment == 0.0f ? location : buffer);
  return true;
}

KDSize AbstractTextField::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize stringSize = KDFont::Font(m_font)->stringSize(text());
  assert(stringSize.height() == KDFont::GlyphHeight(m_font));
  if (m_isEditing) {
    return KDSize(stringSize.width() + m_cursorView.minimalSizeForOptimalDisplay().width(), stringSize.height());
  }
  return stringSize;
}

bool AbstractTextField::ContentView::removePreviousGlyph() {
  assert(m_isEditing);

  char * buffer = const_cast<char *>(editedText());

  if (m_horizontalAlignment > 0.0f) {
    /* Reload the view. If we do it later, the text beins supposedly shorter, we
     * will not clean the first char. */
    reloadRectFromPosition(buffer);
  }
  // Remove the glyph if possible
  int removedLength = UTF8Helper::RemovePreviousGlyph(buffer, const_cast<char *>(cursorLocation()));
  if (removedLength == 0) {
    assert(cursorLocation() == buffer);
    return false;
  }

  if (buffer == s_draftTextBuffer) {
    // Update the draft buffer length
    s_currentDraftTextLength-= removedLength;
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
  char * buffer = const_cast<char *>(editedText());
  size_t lengthToCursor = (size_t)(cursorLocation() - buffer);
  if (editedTextLength() == lengthToCursor) {
    return false;
  }
  reloadRectFromPosition(m_horizontalAlignment == 0.0f ? cursorLocation() : buffer);
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
  if (m_useDraftBuffer) {
    s_currentDraftTextLength = strlen(s_draftTextBuffer);
  }
  layoutSubviews();
}

void AbstractTextField::ContentView::setEditionBuffer(char * buffer, size_t bufferSize) {
  m_textBuffer = buffer;
  m_textBufferSize = bufferSize;
  m_useDraftBuffer = false;
}

size_t AbstractTextField::ContentView::deleteSelection() {
  assert(!selectionIsEmpty());
  assert(m_isEditing);
  size_t removedLength = m_selectionEnd - m_selectionStart;
  char * buffer = const_cast<char *>(editedText());
  strlcpy(const_cast<char *>(m_selectionStart), m_selectionEnd, editionBufferSize() - (m_selectionStart - buffer));
  /* We cannot call resetSelection() because m_selectionStart and m_selectionEnd
   * are invalid */
  m_selectionStart = nullptr;
  m_selectionEnd = nullptr;
  if (buffer == s_draftTextBuffer) {
    assert(removedLength <= s_currentDraftTextLength);
    s_currentDraftTextLength -= removedLength;
  }
  return removedLength;
}

void AbstractTextField::ContentView::layoutSubviews(bool force) {
  if (!m_isEditing) {
    m_cursorView.setFrame(KDRectZero, force);
    return;
  }
  TextInput::ContentView::layoutSubviews(force);
}

size_t AbstractTextField::ContentView::editionBufferSize() const {
  return m_useDraftBuffer ? m_draftTextBufferSize : m_textBufferSize;
}

KDRect AbstractTextField::ContentView::glyphFrameAtPosition(const char * buffer, const char * position) const {
  assert(buffer != nullptr && position != nullptr);
  assert(position >= buffer);
  KDSize glyphSize = KDFont::GlyphSize(m_font);
  KDCoordinate cursorWidth = m_cursorView.minimalSizeForOptimalDisplay().width();
  KDCoordinate horizontalOffset = m_horizontalAlignment == 0.0f ? 0.0f :
    m_horizontalAlignment * (m_frame.width() - KDFont::Font(m_font)->stringSize(buffer).width() - cursorWidth);
  return KDRect(
      horizontalOffset + KDFont::Font(m_font)->stringSizeUntil(buffer, position).width(),
      m_verticalAlignment * (m_frame.height() - glyphSize.height()),
      glyphSize);
}

/* AbstractTextField */

AbstractTextField::AbstractTextField(Responder * parentResponder, View * contentView, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate) :
  TextInput(parentResponder, contentView),
  EditableField(inputEventHandlerDelegate),
  m_delegate(delegate)
{}

void AbstractTextField::setBackgroundColor(KDColor backgroundColor) {
  ScrollView::setBackgroundColor(backgroundColor);
  contentView()->setBackgroundColor(backgroundColor);
}

void AbstractTextField::setTextColor(KDColor textColor) {
  contentView()->setTextColor(textColor);
}

void AbstractTextField::setDelegates(InputEventHandlerDelegate * inputEventHandlerDelegate,
                             TextFieldDelegate * delegate) {
  m_inputEventHandlerDelegate = inputEventHandlerDelegate;
  m_delegate = delegate;
}

void AbstractTextField::setInputEventHandlerDelegate(
    InputEventHandlerDelegate * inputEventHandlerDelegate) {
  m_inputEventHandlerDelegate = inputEventHandlerDelegate;
}

bool AbstractTextField::isEditing() const {
  return nonEditableContentView()->isEditing();
}

size_t AbstractTextField::draftTextLength() const {
  assert(isEditing());
  return nonEditableContentView()->editedTextLength();
}

void AbstractTextField::setText(const char * text) {
  reloadScroll();
  contentView()->setText(text);
  if (contentView()->text() == contentView()->editedText()) {
    /* Set the cursor location here and not in ContentView::setText so that
     * TextInput::willSetCursorLocation is called. */
    setCursorLocation(contentView()->editedText()+strlen(text));
  }
}

bool AbstractTextField::privateHandleEvent(Ion::Events::Event event) {
  // Handle Toolbox or Var event
  if (handleBoxEvent(event)) {
    return true;
  }
  if (isEditing() && shouldFinishEditing(event)) {
    /* If textFieldDidFinishEditing displays a pop-up (because of an unvalid
     * text for instance), the text field will call willResignFirstResponder.
     * This will call textFieldDidAbortEditing if the textfield is still editing,
     * which we do not want, as we are not really aborting edition, just
     * displaying a pop-up before returning to edition.
     * We thus set editing to false. */
    setEditing(false);
    /* We avoid copying the edited text into the other text buffer in case
     * textFieldDidFinishEditing fails - we then want to be able to abort
     * edition and find the old text back. Anyway, if textFieldDidFinishEditing
     * does not fail, it will save the editedText in a model and reload the
     * content of the textfield buffer using the very same model - that has
     * been updated. */
    if (m_delegate->textFieldDidFinishEditing(this, contentView()->editedText(), event)) {
      // Clean draft text for next use
      reinitDraftTextBuffer();
      resetSelection();
      /* We allow overscroll to avoid calling layoutSubviews twice because the
       * content might have changed. */
      reloadScroll(true);
      return true;
    }
    setEditing(true);
    return true;
  }
  /* If a move event was not caught before, we handle it here to avoid bubbling
   * the event up. */
  if (isEditing()
      && (event == Ion::Events::Up
        || event == Ion::Events::Down
        || event == Ion::Events::Left
        || event == Ion::Events::Right))
  {
    return true;
  }
  if (event == Ion::Events::Backspace && isEditing()) {
    if (contentView()->selectionIsEmpty()) {
      return removePreviousGlyph();
    }
    deleteSelection();
    return true;
  }
  if (event == Ion::Events::Back && isEditing()) {
    reinitDraftTextBuffer();
    resetSelection();
    setEditing(false);
    m_delegate->textFieldDidAbortEditing(this);
    reloadScroll(true);
    return true;
  }
  if (event == Ion::Events::Clear && isEditing()) {
    if (!contentView()->selectionIsEmpty()) {
      deleteSelection();
    } else if (!removeEndOfLine()) {
      removeWholeText();
    }
    return true;
  }
  if (event == Ion::Events::Copy || event == Ion::Events::Cut) {
    if (storeInClipboard() && event == Ion::Events::Cut) {
      if (!contentView()->selectionIsEmpty()) {
        deleteSelection();
      } else {
        removeWholeText();
      }
    }
    return true;
  }
  if (event == Ion::Events::Sto) {
    return handleStoreEvent();
  }
  return false;
}

void AbstractTextField::removePreviousGlyphIfRepetition(bool defaultXNTHasChanged) {
  if (!defaultXNTHasChanged && Ion::Events::repetitionFactor() > 0 && isEditing() && contentView()->selectionIsEmpty()) {
    // Since XNT is cycling on simple glyphs, remove the last inserted one
    bool success = removePreviousGlyph();
    assert(success);
    (void) success; // Silence compilation warnings
    /* TODO: Handle cycling with non-default layouts and Fix issues with
     * repetition over a syntax error dismissal */
  }
}

// TODO : Handle cycling with non-default layouts.
size_t AbstractTextField::insertXNTChars(CodePoint defaultXNTCodePoint, char * buffer, size_t bufferLength) {
  /* If cursor is in one of the following functions, and everything before the
   * cursor is correctly nested, the default XNTCodePoint will be improved.
   * These functions all have the following structure :
   * functionName(argument, variable, additonalOutOfContextFields ...)
   * If the cursor is in an argument field, and the variable is well nested and
   * defined, the variable will be inserted into the given buffer. Otherwise,
   * the (improved or not) defaultXNTCodePoint is inserted. */
  constexpr static struct { Poincare::AliasesList aliasesList; char xnt; } sFunctions[] = {
    { Poincare::Derivative::s_functionHelper.aliasesList(), Poincare::Derivative::k_defaultXNTChar },
    { Poincare::Integral::s_functionHelper.aliasesList(), Poincare::Integral::k_defaultXNTChar },
    { Poincare::Product::s_functionHelper.aliasesList(), Poincare::Product::k_defaultXNTChar },
    { Poincare::Sum::s_functionHelper.aliasesList(), Poincare::Sum::k_defaultXNTChar }
  };
  if (!isEditing()) {
    reinitDraftTextBuffer();
    setEditing(true);
    m_delegate->textFieldDidStartEditing(this);
  }
  bool defaultXNTHasChanged = false;
  const char * text = this->text();
  assert(text == contentView()->editedText());
  const char * locationOfCursor = cursorLocation();
  // Step 1 : Identify the function the cursor is in
  UTF8Decoder functionDecoder(text, locationOfCursor);
  const char * location = functionDecoder.stringPosition();
  CodePoint c = UCodePointUnknown;
  // Analyze glyphs on the left of the cursor
  if (location > text) {
    c = functionDecoder.previousCodePoint();
    location = functionDecoder.stringPosition();
  }
  int functionLevel = 0;
  int cursorLevel = 0;
  bool cursorInVariableField = false;
  bool functionFound = false;
  while (location > text && !functionFound) {
    switch (c) {
      case '(':
        // Check if we are skipping to the next matching '('.
        if (functionLevel > 0) {
          functionLevel--;
          break;
        }
        // Skip over whitespace.
        while (location > text && functionDecoder.previousCodePoint() == ' ') {
          location = functionDecoder.stringPosition();
        }
        // Move back right before the last non whitespace code-point
        functionDecoder.nextCodePoint();
        location = functionDecoder.stringPosition();
        // Identify one of the functions
        for (size_t i = 0; i < sizeof(sFunctions)/sizeof(sFunctions[0]); i++) {
          const char * name = sFunctions[i].aliasesList.mainAlias();
          size_t length = strlen(name);
          if ((location >= text + length) && memcmp(&text[(location - text) - length], name, length) == 0) {
            functionFound = true;
            // Update default code point
            defaultXNTCodePoint = CodePoint(sFunctions[i].xnt);
            defaultXNTHasChanged = true;
          }
        }
        if (!functionFound) {
          // No function found, reset search parameters
          cursorInVariableField = false;
          cursorLevel += 1;
        }
        break;
      case ',':
        if (functionLevel == 0) {
          if (cursorInVariableField) {
            // Cursor is out of context, skip to the next matching '('
            functionLevel ++;
            cursorLevel ++;
          }
          // Update cursor's position status
          cursorInVariableField = !cursorInVariableField;
        }
        break;
      case ')':
        // Skip to the next matching '('.
        functionLevel ++;
        break;
    }
    c = functionDecoder.previousCodePoint();
    location = functionDecoder.stringPosition();
  }

  /* Step 2 : Locate variable text */
  if (functionFound && !cursorInVariableField) {
    const char * parameterText;
    size_t parameterLength = bufferLength + 1;
    if (Poincare::ParameteredExpression::ParameterText(locationOfCursor, &parameterText, &parameterLength) && bufferLength >= parameterLength) {
      memcpy(buffer, parameterText, parameterLength);
      return parameterLength;
    }
  }
  removePreviousGlyphIfRepetition(defaultXNTHasChanged);
  return UTF8Decoder::CodePointToChars(defaultXNTCodePoint, buffer, bufferLength);
}

bool AbstractTextField::addXNTCodePoint(CodePoint xnt) {
  constexpr int bufferSize = Poincare::SymbolAbstract::k_maxNameSize;
  char buffer[bufferSize];
  size_t length = insertXNTChars(xnt, buffer, bufferSize - 1);

  assert(length < bufferSize);
  buffer[length] = 0;
  return handleEventWithText(buffer, false, true);
}

bool AbstractTextField::handleEvent(Ion::Events::Event event) {
  assert(m_delegate != nullptr);
  size_t previousTextLength = strlen(text());
  bool didHandleEvent = false;
  if (privateHandleMoveEvent(event)) {
    didHandleEvent = true;
  } else if (privateHandleSelectEvent(event)) {
    didHandleEvent = true;
  } else if (m_delegate->textFieldDidReceiveEvent(this, event)) {
    return true;
  } else if (event == Ion::Events::Paste) {
    return handleEventWithText(Clipboard::SharedClipboard()->storedText(), false, true);
  } else if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !isEditing()) {
    const char * previousText = contentView()->text();
    setEditing(true);
    m_delegate->textFieldDidStartEditing(this);
    setText(previousText);
    didHandleEvent = true;
  } else {
    char buffer[Ion::Events::EventData::k_maxDataSize] = {0};
    size_t eventTextLength = Ion::Events::copyText(static_cast<uint8_t>(event), buffer, Ion::Events::EventData::k_maxDataSize);
    if (eventTextLength > 0) {
      return handleEventWithText(buffer);
    }
  }
  if (!didHandleEvent) {
    didHandleEvent = privateHandleEvent(event);
  }

  // Here, text only changed if something was deleted
  return m_delegate->textFieldDidHandleEvent(this, didHandleEvent, strlen(text()) != previousTextLength);
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
  const char * draftBuffer = contentView()->editedText();
  if (event == Ion::Events::Left || event == Ion::Events::Right) {
    if (!contentView()->selectionIsEmpty()) {
      resetSelection();
      return true;
    }
    if (event == Ion::Events::Left && cursorLocation() > draftBuffer) {
      return TextInput::moveCursorLeft();
    }
    if (event == Ion::Events::Right && cursorLocation() < draftBuffer + draftTextLength()) {
      return TextInput::moveCursorRight();
    }
  }
  return false;
}

bool AbstractTextField::privateHandleSelectEvent(Ion::Events::Event event) {
  if (!isEditing()) {
    return false;
  }
  if (event == Ion::Events::ShiftLeft || event == Ion::Events::ShiftRight || event == Ion::Events::ShiftUp || event == Ion::Events::ShiftDown) {
    selectLeftRight(event == Ion::Events::ShiftLeft || event == Ion::Events::ShiftUp, event == Ion::Events::ShiftUp || event == Ion::Events::ShiftDown);
    return true;
  }
  return false;
}

bool AbstractTextField::handleEventWithText(const char * eventText, bool indentation, bool forceCursorRightOfText) {
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
      bool complete = UTF8Helper::CopyAndRemoveCodePoints(buffer, bufferSize, eventText, c, 2);
      /* If the text is too long to be stored in buffer, we do not insert any
       * text. This behaviour is consistent with 'insertTextAtLocation'
       * behaviour. */
      if (!complete) {
        return false;
      }
    }
    // Replace System parentheses (used to keep layout tree structure) by normal parentheses
    Poincare::SerializationHelper::ReplaceSystemParenthesesAndBracesByUserParentheses(buffer);

    if (insertTextAtLocation(buffer, const_cast<char *>(cursorLocation()))) {
      /* The cursor position depends on the text as we sometimes want to position
       * the cursor at the end of the text and sometimes after the first
       * parenthesis. */
      const char * nextCursorLocation = cursorLocation();
      if (forceCursorRightOfText) {
        nextCursorLocation+= strlen(buffer);
      } else {
        nextCursorLocation+= TextInputHelpers::CursorIndexInCommand(eventText);
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
    const char * start = nonEditableContentView()->selectionStart();
    Clipboard::SharedClipboard()->store(start, nonEditableContentView()->selectionEnd() - start);
    return true;
  }
  return false;
}

bool AbstractTextField::handleStoreEvent() {
  if (!isEditing()) {
    Container::activeApp()->storeValue(text());
    return true;
  }
  if (!nonEditableContentView()->selectionIsEmpty()) {
    const char * start = nonEditableContentView()->selectionStart();
    static_assert(TextField::MaxBufferSize() == Escher::Clipboard::k_bufferSize);
    char buffer[Escher::Clipboard::k_bufferSize];
    strlcpy(buffer, start, nonEditableContentView()->selectionEnd() - start + 1);
    Container::activeApp()->storeValue(buffer);
  } else {
    Container::activeApp()->storeValue();
  }
  return true;
}

/* TextField */

TextField::TextField(Responder * parentResponder, char * textBuffer, size_t textBufferSize, size_t draftTextBufferSize,
    InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate, KDFont::Size font,
    float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor) :
  AbstractTextField(parentResponder, &m_contentView, inputEventHandlerDelegate, delegate),
  m_contentView(textBuffer, textBufferSize, draftTextBufferSize, font, horizontalAlignment, verticalAlignment, textColor, backgroundColor)
{}

}
